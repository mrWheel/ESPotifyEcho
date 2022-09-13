

//#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_VERBOSE

#define DO_DEBUG   0

#include <HTTPClient.h>
#include "spotifyClient.h"
#include <base64.h>

//-------------------------------------------------------------------
spotifyClient::spotifyClient() 
{
} //  spotifyClient()

//-------------------------------------------------------------------
void spotifyClient::begin( const char* clientId, const char* clientSecret
                         , const char* refreshToken, const char *newDeviceId) 
{
  strlcpy(this->clientId,     clientId,     sizeof(this->clientId));
  strlcpy(this->clientSecret, clientSecret, sizeof(this->clientSecret));
  strlcpy(this->refreshToken, refreshToken, sizeof(this->refreshToken));
  strlcpy(this->deviceId,     newDeviceId,  sizeof(this->deviceId));

  client.setCACert(digicert_root_ca);
}

//-------------------------------------------------------------------
int spotifyClient::fetchToken()
{
    HTTPClient http;

    String body = "grant_type=refresh_token&refresh_token=" + String(refreshToken);
    String authorizationRaw = String(clientId) + ":" + String(clientSecret);
    String authorization =  base64::encode(authorizationRaw);
    http.begin(client, "https://accounts.spotify.com/api/token"); 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("Authorization", "Basic " + authorization);

    int httpCode = http.POST(body);                                   
    if (httpCode > 0) { 
        String returnedPayload = http.getString();
        if ( httpCode == 200 )
        {
            accessToken = parseAccessToken("access_token", returnedPayload );
            if (DO_DEBUG) Serial.println("Got new access token");
        }
        else
        {
            Serial.printf("Failed to get new access token (reply[%d])\r\n", httpCode);
            Serial.println(returnedPayload);
        }
    }
    else {
        Serial.println("Failed to connect to https://accounts.spotify.com/api/token" );
    }
    http.end(); 
    return httpCode;
    
} //  fetchToken()

//-------------------------------------------------------------------
int spotifyClient::shuffle(bool doShuffle)
{
    if (DO_DEBUG) Serial.printf("shuffle(%d)\r\n",doShuffle);
    HttpResult result;
    if (doShuffle)
          result = CallAPI( "PUT"
                      , "https://api.spotify.com/v1/me/player/shuffle?state=true&device_id="
                      + String(deviceId), "" );
    else  result = CallAPI( "PUT"
                      , "https://api.spotify.com/v1/me/player/shuffle?state=false&device_id="
                      + String(deviceId), "" );
    return result.httpCode;
}

//-------------------------------------------------------------------
int spotifyClient::repeat()
{
    /*if (DO_DEBUG)*/ Serial.printf("repeat(\"context\") on [%s]\r\n", this->deviceId);
    HttpResult result = CallAPI( "PUT", "https://api.spotify.com/v1/me/player/repeat?state=context&device_id=" 
                                       + String(this->deviceId), "" );
    return result.httpCode;
}

//-------------------------------------------------------------------
int spotifyClient::next()
{
    if (DO_DEBUG) Serial.printf("[%s]next()\r\n", __FUNCTION__);
    HttpResult result = CallAPI( "POST", "https://api.spotify.com/v1/me/player/next?device_id=" + String(this->deviceId), "" );
    return result.httpCode;
}

//-------------------------------------------------------------------
int spotifyClient::previous()
{
    if (DO_DEBUG) Serial.printf("[%s]previous()\r\n", __FUNCTION__);
    HttpResult result = CallAPI( "POST", "https://api.spotify.com/v1/me/player/previous?device_id=" + String(deviceId), "" );
    return result.httpCode;
}

//-------------------------------------------------------------------
int spotifyClient::pause()
{
    if (DO_DEBUG) Serial.println("pause()");
    HttpResult result = CallAPI( "PUT", "https://api.spotify.com/v1/me/player/pause", "" );
    return result.httpCode;
}

//-------------------------------------------------------------------
int spotifyClient::play()
{
    if (DO_DEBUG) Serial.printf("[%s]play()\r\n", __FUNCTION__);
    String body = "{\"context_uri\":\"" + this->currentUri + "\"}";
    String url = "https://api.spotify.com/v1/me/player/play?device_id=" + String(this->deviceId);

    Serial.printf("Play on [%s]\r\n", this->deviceId);
    Serial.println(body);
    HttpResult result = CallAPI( "PUT", url, body );

    Serial.println(result.payload);
    
    return result.httpCode;
    
} //  play()

//-------------------------------------------------------------------
//context_uri
//string
//Optional. Spotify URI of the context to play. Valid contexts are albums, artists & playlists. {context_uri:"spotify:album:1Je1IMUlBXcx1Fz0WE7oPT"}

//uris
//array of strings
//Optional. A JSON array of the Spotify track URIs to play. For example: {"uris": ["spotify:track:4iV5W9uYEdYUVa79Axb7Rh", "spotify:track:1301WleyT98MSxVHPZCA6M"]}

//offset
//object
//Optional. Indicates from where in the context playback should start. Only available when context_uri corresponds to an album or playlist object "position" is zero based and can’t be negative. Example: "offset": {"position": 5} "uri" is a string representing the uri of the item to start at. Example: "offset": {"uri": "spotify:track:1301WleyT98MSxVHPZCA6M"}

//AdditionalProperties
//position_ms
//integer
//integer
int spotifyClient::play(const char* contextUri, const char* itemUri, int progress)
{
    if (DO_DEBUG) Serial.printf("[%s]play()\r\n", __FUNCTION__);
    
    this->currentUri = String(contextUri);
    
    String body = "{\"context_uri\":\"" + this->currentUri + "\",\"offset\":{\"uri\":\"";
           body += String(itemUri);
           body += "\"},\"position_ms\":";
           body += String(progress);
           body += "}";
    
    String url = "https://api.spotify.com/v1/me/player/play?device_id=" + String(this->deviceId);

    Serial.printf("Play on [%s]\r\n", this->deviceId);
    Serial.println(body);
    HttpResult result = CallAPI( "PUT", url, body );

    Serial.println(result.payload);
    
    return result.httpCode;
    
} //  play(int, int)


//-------------------------------------------------------------------
int spotifyClient::setDeviceId(char *playerId )
{
    if (DO_DEBUG) Serial.printf("[%s]SetDevice(%s)\r\n", __FUNCTION__, playerId);
    String body = "{\"device_ids\":[\"" + String(playerId) + "\"]}";
    String url = "https://api.spotify.com/v1/me/player";

    HttpResult result = CallAPI( "PUT", url, body );
    if (DO_DEBUG) Serial.printf("httpCode [%d]\r\n", result.httpCode);
    if (result.httpCode == 204)
    {
      strlcpy(this->deviceId, playerId, sizeof(this->deviceId));
    }

    return result.httpCode;
    
} //  setDeviceId()

//-------------------------------------------------------------------
void spotifyClient::setUserId(char *newUserId )
{
    if (DO_DEBUG) Serial.printf("[%s]setUserId(%s)\r\n", __FUNCTION__, newUserId);
    snprintf(this->userId, sizeof(this->userId), "%s", newUserId);
    
} //  setUserId()


//-------------------------------------------------------------------
String spotifyClient::search(String type, String query )
{
    if (DO_DEBUG) Serial.printf("[%s]search(%s,%s)\r\n", __FUNCTION__, type.c_str(), query.c_str());

    type.trim();
    query.trim();
    //query.replace(" ", "%20");
    
    String URI  = "https://api.spotify.com/v1/search?q=";
           URI += query;
           URI += "&type=";
           URI += type;
           
    //Serial.println(query);
    Serial.println(URI);
    
    HttpResult result = CallAPI( "GET", URI, "" );
    if (DO_DEBUG) Serial.println(result.payload);

    return result.payload;
    
} //  search()


//-------------------------------------------------------------------
int spotifyClient::setVolume(int volume_percent)
{
    //Serial.printf("setVolume(%d%%)\r\n", volume_percent);
    //deviceId.trim();
   
    String URI  = "https://api.spotify.com/v1/me/player/volume?volume_percent=";
           URI += String(volume_percent);
           URI += "&device_id=";
           URI += String(this->deviceId);

    //Serial.printf("URI=[%s]\r\n", URI.c_str());
    HttpResult result = CallAPI( "PUT", URI, "" );
    //Serial.printf("httpCode [%d]\r\n", result.httpCode);
    
    return result.httpCode;
    
} //  setVolume()


//-------------------------------------------------------------------
int spotifyClient::createPlaylist(String playlistName)
{
    /*if (DO_DEBUG)*/ Serial.printf("createPlaylist(%s)\r\n", playlistName);
    String  body  = "{\"name\": \"";
            body += playlistName;
            body += "\",\"description\": \"created by ESPotify\"";
            body += ",\"public\": false }";

    String  URI  = "https://api.spotify.com/v1/users/";
            URI += userId;
            URI += "/playlists";
            //URI += "\" --data \"";
            //URI += body;

    /*if (DO_DEBUG)*/ Serial.printf("URI=[%s]\r\n", URI.c_str());
    /*if (DO_DEBUG) Serial.printf("body=[%s]\r\n", body.c_str()); */
    HttpResult result = CallAPI( "POST", URI, body );
    if (DO_DEBUG) Serial.printf("httpCode [%d]\r\n", result.httpCode);
    Serial.println(result.payload);
    return result.httpCode;
     
} //  createPlaylist()


//-------------------------------------------------------------------
int spotifyClient::addToPlaylist(String trackUri)
{
    /*if (DO_DEBUG)*/ Serial.printf("addToPlaylist(%s)\r\n", trackUri.c_str());

    String  URI  = "https://api.spotify.com/v1/playlists/";
            URI += playlistId;
            URI += "/tracks?uris=";
            URI += trackUri;

    /*if (DO_DEBUG)*/ Serial.printf("URI=[%s]\r\n", URI.c_str());
    HttpResult result = CallAPI( "POST", URI, "" );
    if (DO_DEBUG) Serial.printf("httpCode [%d]\r\n", result.httpCode);
    Serial.println(result.payload);
    return result.httpCode;
     
} //  addToPlaylist()


//-------------------------------------------------------------------
int spotifyClient::deletePlaylistItems(const char* playlistId, const char* tracksToRemove)
{
  char httpRequest[600] {};
  
    /*if (DO_DEBUG)*/ Serial.printf("DeletePlaylist(%s)..\r\n", playlistId);
   
    String URI  = "https://api.spotify.com/v1/playlists/";
           URI += String(playlistId);
           URI += "/tracks";
    Serial.println(tracksToRemove);
    /*if (DO_DEBUG)*/ Serial.printf("URI=[%s]\r\n", URI.c_str());
    //HttpResult result = CallAPI( "DELETE", URI, "" );
    //if (DO_DEBUG) Serial.printf("httpCode [%d]\r\n", result.httpCode);
    //return result.httpCode;
    
    String authorization = "Bearer " + accessToken;
    char cAuth[400]={};
    snprintf(cAuth, sizeof(cAuth), authorization.c_str());
    Serial.printf("cAuth is [%d] chars\r\n", strlen(cAuth));

    client.flush();
    if (client.connect("api.spotify.com", 443))
    {
      client.setCACert(digicert_root_ca);
      /**
      strlcpy(httpRequest, "GET https://api.spotify.com/v1/me HTTP/1.1\n", sizeof(httpRequest));
      strlcat(httpRequest, "host: https://api.spotify.com\n", sizeof(httpRequest));
      strlcat(httpRequest, "Accept: application/json\n", sizeof(httpRequest));
      strlcat(httpRequest, "Content-Type: application/json\n", sizeof(httpRequest));
      strlcat(httpRequest, "Authorization: ", sizeof(httpRequest));
      strlcat(httpRequest, cAuth, sizeof(httpRequest));
      strlcat(httpRequest, "\n", sizeof(httpRequest));
      **/
      strlcpy(httpRequest, "GET /v1/me HTTP/2\n", sizeof(httpRequest));
      strlcat(httpRequest, "host: api.spotify.com\n", sizeof(httpRequest));
      strlcat(httpRequest, "Accept: application/json\n", sizeof(httpRequest));
      strlcat(httpRequest, "Content-Type: application/json\n", sizeof(httpRequest));
      strlcat(httpRequest, "Content-Length: 0\n", sizeof(httpRequest));
      strlcat(httpRequest, "Authorization: ", sizeof(httpRequest));
      strlcat(httpRequest, cAuth, sizeof(httpRequest));
      strlcat(httpRequest, "\n\n", sizeof(httpRequest));
    
      Serial.printf("httpRequest is [%d] chars\r\n", strlen(httpRequest));
      Serial.println(httpRequest);

      client.println(httpRequest);
    }
    
    //char buffer[1000] = {};
    int counter = 0;
    //delay(10);
    while (client.connected()) 
    {
      while (client.available()) 
      {
        Serial.print((char)client.read());
      }
    }
    Serial.println("\r\n!done!\r\n");
    
    client.stop();
    
    return 400;
     
} //  deletePlaylistItems()


//-------------------------------------------------------------------
String spotifyClient::getPlaylists(const char* playlistId)
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/playlists", "" );
     //if (DO_DEBUG)* Serial.println(result.payload);

     return result.payload;
     
} //  getPlaylists()


//-------------------------------------------------------------------
String spotifyClient::getPlaylistItems(const char* playlistId)
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/player/devices", "" );
     //if (DO_DEBUG) Serial.println(result.payload);
     if (DO_DEBUG) Serial.printf("Device ID: [%s]\r\n", this->deviceId);

     return result.payload;
     
} //  getPlaylistItems()


//-------------------------------------------------------------------
String spotifyClient::getDevices()
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/player/devices", "" );
     //if (DO_DEBUG) Serial.println(result.payload);
     //deviceId = GetDeviceId(result.payload);
     if (DO_DEBUG) Serial.printf("Device ID: [%s]\r\n", this->deviceId);

     return result.payload;
     
} //  getDevices()


//-------------------------------------------------------------------
String spotifyClient::getUserId()
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me", "" );
     //if (DO_DEBUG) Serial.println(result.payload);

     return result.payload;
     
} //  getUserId()


//-------------------------------------------------------------------
String spotifyClient::getPlaylists()
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/playlists", "" );
     //if (DO_DEBUG) Serial.println(result.payload);

     return result.payload;
     
} //  getPlaylists()


//-------------------------------------------------------------------
String spotifyClient::getArtistAlbums(String artistId, int limit, int offset)
{
     
    String URI  = "https://api.spotify.com/v1/artists/";
           URI += String(artistId);
           URI += "/albums?include_groups=album&";
           URI += limit;
           URI += "&offset=";
           URI += offset;

     if (DO_DEBUG) Serial.println(URI);
     HttpResult result = CallAPI( "GET", URI, "" );
     //if (DO_DEBUG) Serial.println(result.payload);

     return result.payload;
     
} //  getArtistAlbums()


//-------------------------------------------------------------------
String spotifyClient::getPlaybackState()
{
     HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/player", "" );

     return result.payload;

} //  getPlaybackState()


//-------------------------------------------------------------------
String spotifyClient::parseAccessToken(String key, String json )
{
    String retVal = "";    
    int index = json.indexOf(key);

    if ( index > 0 )
    {
        bool copy = false;
        for ( int i = index; i < json.length(); i++ )
        {
            if ( copy )
            {
                if ( json.charAt(i) == '"' || json.charAt(i) == ',')
                {
                    break;
                }
                else
                {
                    retVal += json.charAt(i);
                }
            }
            else if ( json.charAt(i) == ':')
            {
                copy = true;
                if ( json.charAt(i+1) == '"' )
                {
                    i++;
                }
            }
        }
    }
    return retVal;
}

//=====================================================================

//-------------------------------------------------------------------
String spotifyClient::getTracks(const char *type, String albumId, int limit, int offset)
{
  albumId.trim();
  
  String URI  = "https://api.spotify.com/v1/"+String(type)+"/";
         URI += albumId;
         URI += "/tracks";
         URI += "/?limit=";
         URI += limit;
         URI += "&offset=";
         URI += offset;
         
     HttpResult result = CallAPI( "GET", URI, "");
     //if (DO_DEBUG) Serial.println(result.payload);
     Serial.println("++++++++++++++++++++++++++");     
     Serial.printf("getTracks[%s/%s/tracks]\r\n", type, albumId.c_str());
     //Serial.println(result.payload);
     Serial.println("++++++++++++++++++++++++++");     
     return result.payload;
     
} //  getTracks()


//----------------------------------------------------------------------
String spotifyClient::getCurrentPlaying()
{
  HttpResult result = CallAPI( "GET", "https://api.spotify.com/v1/me/player/currently-playing", "");
  
  if (result.httpCode != 200)
  {
    Serial.printf("[%s()] Result.httpCode [%d]\r\n", __FUNCTION__, result.httpCode);
    result.payload = "{\"error\":{\"status\":"+String(result.httpCode)+",\"message\":\"Oeps\"}}";
  }
  //else
  //{
  //  result.payload = "";
  //}
  //Serial.printf("[%s()]\r\n%s\r\n", __FUNCTION__, result.payload.c_str());
  
  return result.payload;
  
} //  getCurrentPlaying()


//--------------------------------------------------------------------------
HttpResult spotifyClient::CallAPI( String method, String url, String body )
{
    HttpResult result;
    result.httpCode = 0;

    //Serial.println(url);
    //if (body.length() > 0) Serial.println(body);
    
    if (DO_DEBUG) Serial.printf("[%s][%s]\r\n", __FUNCTION__, url.c_str());
    if (DO_DEBUG) Serial.print( " returned: " );

    HTTPClient http;
    client.flush();
    http.begin(client, url); 

    String authorization = "Bearer " + accessToken;

    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", authorization);

    // address bug where Content-Length not added by HTTPClient is content length is zero
    if ( body.length() == 0 )
    {
         http.addHeader("Content-Length", String(0));
    }

    if ( method == "PUT" )
    {
        result.httpCode = http.PUT(body); 
    }
    else if ( method == "POST" )
    {
        result.httpCode = http.POST(body); 
    }
    else if ( method == "GET" )
    {
        result.httpCode = http.GET(); 
    }    
    else if ( method == "DELETE" )
    {
        result.httpCode = http.GET(); 
    }    

    if (result.httpCode > 0) 
    { 
        if (result.httpCode != 200 && result.httpCode > 400) 
        {
          if (DO_DEBUG) Serial.printf("[%s]\r\n",url.c_str());
          if (DO_DEBUG) Serial.printf("[%s]returned: [%d]\r\n", __FUNCTION__, result.httpCode );
          //if (DO_DEBUG) Serial.println(result.httpCode);
        }
        else
        {
          if (DO_DEBUG) Serial.println(result.httpCode);
        }
        if ( http.getSize() > 0 )
        {
            result.payload = http.getString();
        }
    }
    else 
    {
        Serial.printf("[%s][%d]Failed to connect to:\r\n[%s]\r\n", __FUNCTION__, result.httpCode, url.c_str());
        Serial.println(result.payload);
    }
    http.end(); 

    return result;
    
} //  CallAPI()
