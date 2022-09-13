

//----------------------------------------------------------------
void parseUserId(String payload)
{
  DebugTln("parseUserId() ..");

  SpiRamJsonDocument filter(500);
  filter["email"] = true;
  filter["id"]    = true;
  filter["type"]  = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(2000);

  //deserializeJson(doc, payload);
  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  if (checkDeserializationError(__FUNCTION__, doc, err)) return;

  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");
  strlcpy(spotify.userId, doc["id"].as<const char *>(), sizeof(spotify.userId));
  DebugTf("User Id [%s]\r\n", spotify.userId);

} //  parseUserId()


//----------------------------------------------------------------
bool parseDevices(String payload)
{
  //DebugTln("parseDevices() ..");
  memset(aDevices, 0, sizeof(aDevices));
  numDevices = 0;

  SpiRamJsonDocument filter(500);
  filter["devices"][0]["id"] = true;
  filter["devices"][0]["is_active"] = true;
  filter["devices"][0]["name"] = true;
  filter["devices"][0]["volume_percent"] = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(5000);
  DebugTf("Psram Free [%d]bytes [after]\r\n", ESP.getFreePsram() );

  //DeserializationError err = deserializeJson(doc, payload);
  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  if (checkDeserializationError(__FUNCTION__, doc, err)) return false;

  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");

  JsonArray devices = doc["devices"];

  //DebugTf("Found [%d] devices\r\n", devices.size());
  //DebugFlush();

  int i=0;
  for (JsonObject device : devices)
  {
    snprintf(aDevices[i].deviceId,   sizeof(aDevices[i].deviceId),   "%s", device["id"].as<const char *>() );
    snprintf(aDevices[i].deviceName, sizeof(aDevices[i].deviceName), "%s", device["name"].as<const char *>() );
    aDevices[i].deviceVolume = device["volume_percent"].as<int>();
    if (device["is_active"].as<bool>()) actDeviceNum = i;
    /*
        Serial.printf("[%2d] [%-20.20s] [%2d%%] [%s]\r\n", i
                                                  , aDevices[i].deviceName
                                                  , aDevices[i].deviceVolume
                                                  , aDevices[i].deviceId);
    */
    i++;
    if (i>_MAX_DEVICES) break;
  }
  numDevices = i;
  if (numDevices <= 0) return false;

  //DebugTf("Sort [%d] Devices by Name\r\n", numDevices);
  qsort(aDevices, numDevices, sizeof(aDevices[0]), sortByDeviceName);

  //-- "false" do not show, "true" show devices
  if (false)
  {
    for(int i=0; i<numDevices; i++)
    {
      Serial.printf("[%2d] [%-20.20s] [%2d%%] [%s]\r\n", i
                    , aDevices[i].deviceName
                    , aDevices[i].deviceVolume
                    , aDevices[i].deviceId);
    }
  }

  return true;

} //  parseDevices()


//----------------------------------------------------------------
int parseMyPlaylists(String payload)
{
  char tmpName[_ALBUM_NAME_LEN];
  int n, i = inUriStore;

  DebugTln("parseMyPlaylists() ..");

  if (inUriStore > _MAX_URISTORE)
  {
    DebugTf("uriStore is full (%d playlists). Bailout\r\n", inUriStore);
    return inUriStore;
  }

  SpiRamJsonDocument filter(1000);
  filter["items"][0]["name"] = true;
  filter["items"][0]["type"] = true;
  filter["items"][0]["uri"]  = true;
  filter["items"][0]["tracks"]["total"]  = true;
  filter["total"]  = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(10000);

  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  //DeserializationError err = deserializeJson(doc, payload);
  if (checkDeserializationError(__FUNCTION__, doc, err)) return false;

  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");

  JsonArray playlists = doc["items"];

  DebugTf("Found [%d] playlists\r\n", playlists.size());
  DebugFlush();

  for (JsonObject playlist : playlists)
  {
    strlcpy(tmpName, playlist["name"], sizeof(tmpName));
    //DebugTf("playlistName [%s] ..", tmpName);
    n = searchPlaylistByName(tmpName);
    //-- found playlist not already in Store
    if (n < 0)
    {
      snprintf(uriStore[i].nfcTag,  sizeof(uriStore[i].nfcTag), "-");
      snprintf(uriStore[i].artist,  sizeof(uriStore[i].artist), "%s", playlist["name"].as<const char *>() );
      snprintf(uriStore[i].album,   sizeof(uriStore[i].album),  "%s", playlist["name"].as<const char *>() );
      snprintf(uriStore[i].playlistName, sizeof(uriStore[i].playlistName), "%s", playlist["name"].as<const char *>() );
      snprintf(uriStore[i].playlistUri, sizeof(uriStore[i].playlistUri),  "%s", playlist["uri"].as<const char *>() );
      uriStore[i].tracks = playlist["tracks"]["total"].as<int>();
      //Debugf("Add: [%2d] [%-20.20s] ([%d] tracks) [%s]\r\n", (i+1)
      //                                        , uriStore[i].playlistName
      //                                        , uriStore[i].tracks
      //                                        , uriStore[i].nfcTag);
      i++;
      if (i>_MAX_URISTORE) break;
    }
    else //-- already in Store, so update URI & number of tracks!
    {
      snprintf(uriStore[n].playlistUri, sizeof(uriStore[n].playlistUri), "%s", playlist["uri"].as<const char *>() );
      uriStore[n].tracks = playlist["tracks"]["total"].as<int>();
    }
  }

  if (i <= 1) return 0;

  //qsort(uriStore, i, sizeof(uriStore[0]), sortByPlaylist);

  return i;

} //  parseMyPlaylists()


//----------------------------------------------------------------
bool parsePlaylists(String payload, const char *query, char *playlistId, int pidLen)
{
  Debugf("parsePlaylistItems(%s) ..", playlistId);

  DebugTf("First: get playlistId [%s]\r\n", query);
  SpiRamJsonDocument filter(500);
  filter["items"][0]["name"] = true;
  filter["items"][0]["id"]   = true;
  filter["items"][0]["type"] = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(10000);

  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  //DeserializationError err = deserializeJson(doc, payload);
  if(checkDeserializationError(__FUNCTION__, doc, err)) return false;

  DebugTln("==================================");
  serializeJsonPretty(doc, Serial);
  Debugln();
  DebugTln("==================================");

  JsonArray playlists = doc["items"];
  DebugTf("Found [%d] playlists\r\n", playlists.size());
  DebugFlush();

  int i=0;
  for (JsonObject playlist : playlists)
  {
    if (strcmp(query, playlist["name"].as<const char *>()) == 0)
    {
      Serial.printf("Found: [%-20.20s] -> id[%s]\r\n", playlist["name"].as<const char *>()
                    , playlist["id"].as<const char *>());
      snprintf(playlistId, pidLen, playlist["id"].as<const char *>());
      break;
    }
  }
  if (strlen(playlistId) < 1)
  {
    DebugTf("Error: no playlist [%s] found!\r\n", query);
    return false;
  }
  DebugTf("Ok, found playlistId for [%s] -> [%s]\r\n", query, playlistId);
  return true;

} //  parsePlaylists()


//----------------------------------------------------------------
int parsePlaylistItems(String payload, const char *playlistId)
{
  char trackUri[50] = {};
  int  totalTracks = 0;
  char tracksToRemove[600] = {};
  char startBracket[3] = "{";

  Debugf("parsePlaylistItems(%s) ..", playlistId);

  SpiRamJsonDocument filter(500);
  //filter["items"][0]["track"]["artists"][0]["name"] = true;
  filter["items"][0]["track"]["uri"] = true;
  filter["items"][0]["track"]["name"] = true;
  filter["items"][0]["track"]["track_number"] = true;
  filter["total"] = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(10000);

  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  //DeserializationError err = deserializeJson(doc, payload);
  if(checkDeserializationError(__FUNCTION__, doc, err)) return false;

  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");

  JsonArray tracks = doc["items"];

  DebugTf("Found [%d] tracks\r\n", tracks.size());
  DebugFlush();

  strlcat(tracksToRemove, "{\"tracks\":[", sizeof(tracksToRemove));
  for (JsonObject track : tracks)
  {
    snprintf(trackUri, sizeof(trackUri), "%s", track["track"]["uri"].as<const char *>());
    //Serial.printf("Found: [%2d] [%-20.20s] -> uri[%s]\r\n"
    //                                            , track["track"]["track_number"].as<int>()
    //                                            , track["track"]["name"].as<const char*>()
    //                                            , trackUri);
    strlcat(tracksToRemove, startBracket, sizeof(tracksToRemove));
    strlcat(tracksToRemove, "\"uri\":\"", sizeof(tracksToRemove));
    strlcat(tracksToRemove, trackUri, sizeof(tracksToRemove));
    strlcat(tracksToRemove, "\"}\n", sizeof(tracksToRemove));
    snprintf(startBracket, sizeof(startBracket), ",{");
  }
  strlcat(tracksToRemove, "]}\n", sizeof(tracksToRemove));
  //DebugTln("==delete uri's===============");
  //Debugln(tracksToRemove);
  spotify.deletePlaylistItems(playlistId, tracksToRemove);

  totalTracks = doc["total"].as<int>();

  return totalTracks;

} //  parsePlaylistItems()


//----------------------------------------------------------------
bool parsePlaybackState(String payload, bool doVerbose)
{
  //Serial.println("parsePlaybackState() ..");

  SpiRamJsonDocument filter(2000);

  filter["device"]["id"] = true;
  filter["device"]["name"] = true;
  filter["device"]["type"] = true;
  filter["device"]["volume_percent"] = true;
  filter["shuffle_state"] = true;
  filter["repeat_state"] = true;
  filter["context"]["uri"] = true;
  filter["context"]["type"] = true;
  filter["progress_ms"] = true;
  filter["item"]["album"]["artists"][0]["name"] = true;  // artistName
  filter["item"]["album"]["name"] = true;                // albumName
  filter["item"]["name"] = true;    // trackName
  filter["item"]["track_number"] = true;
  filter["item"]["type"] = true;
  filter["item"]["uri"] = true;
  filter["item"]["duration_ms"] = true;
  filter["is_playing"] = true;
  filter["error"]["status"] = true;
  filter["error"]["message"] = true;

  // Deserialize the document
  SpiRamJsonDocument doc(30000);

  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));

  if (checkDeserializationError(__FUNCTION__, doc, err))
  {
    DebugTln("some error found! return;");
    return false;
  }
  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");

  strlcpy(playbackState->deviceId,    doc["device"]["id"].as<const char *>(),   sizeof(playbackState->deviceId));
  strlcpy(playbackState->deviceName,  doc["device"]["name"].as<const char *>(), sizeof(playbackState->deviceName));
  strlcpy(playbackState->deviceType,  doc["device"]["type"].as<const char *>(), sizeof(playbackState->deviceType));
  strlcpy(playbackState->repeat, doc["repeat_state"].as<const char *>(),   sizeof(playbackState->repeat));
  strlcpy(playbackState->contextUri,  doc["context"]["uri"].as<const char *>(), sizeof(playbackState->contextUri));
  strlcpy(playbackState->contextType, doc["context"]["type"].as<const char *>(), sizeof(playbackState->contextType));
  strlcpy(playbackState->itemUri,     doc["item"]["uri"].as<const char *>(),    sizeof(playbackState->itemUri));
  strlcpy(playbackState->itemType,    doc["item"]["type"].as<const char *>(),   sizeof(playbackState->itemType));
  strlcpy(playbackState->albumName,   doc["item"]["album"]["name"].as<const char *>()
          , sizeof(playbackState->albumName));
  strlcpy(playbackState->itemName,    doc["item"]["name"].as<const char *>(),   sizeof(playbackState->itemName));
  strlcpy(playbackState->artistName,  doc["item"]["album"]["artists"][0]["name"].as<const char *>()
          ,   sizeof(playbackState->artistName));
  strlcpy(playbackState->nfcTag,      actNfcTag,   sizeof(playbackState->nfcTag));

  playbackState->deviceVolume = (doc["device"]["volume_percent"].as<int>()) + 1;
  playbackState->shuffle      = doc["shuffle_state"].as<bool>();
  playbackState->progress     = doc["progress_ms"].as<int>();
  playbackState->isPlaying    = doc["is_playing"].as<bool>();
  playbackState->itemDuration = doc["item"]["duration_ms"].as<int>();
  playbackState->trackNr      = doc["item"]["track_number"].as<int>();

  playbackState->checkProgress = millis() + (playbackState->itemDuration - playbackState->progress);

  if (strlen(actNfcTag) == 20)
        actMusic = searchPlaylistByNfcTag(actNfcTag);
  else  actMusic = searchPlaylistByUri(playbackState->contextUri);

  if (actMusic >= 0)
        strlcpy(playbackState->contextName, uriStore[actMusic].playlistName,   sizeof(playbackState->contextName));
  else  memset(playbackState->contextName, 0, sizeof(playbackState->contextName));

  setSystemDevice();

  //-- verbose? --
  if (doVerbose)
  {
    DebugTln("playbackState:");
    DebugTf("    device Id: [%s]\r\n",  playbackState->deviceId);
    DebugTf("  device Name: [%s]\r\n",  playbackState->deviceName);
    DebugTf("         Type: [%s]\r\n",  playbackState->deviceType);
    DebugTf("       Volume: [%d%%]\r\n", playbackState->deviceVolume);
    DebugTf("      Shuffle: [%s]\r\n",  playbackState->shuffle ?"true":"false");
    DebugTf("       Repeat: [%s]\r\n",  playbackState->repeat);
    DebugTf("    Track Nr.: [%d]\r\n",  playbackState->trackNr);
    DebugTf("   is Playing: [%s]\r\n",  playbackState->isPlaying?"true":"false");
    DebugTf("indx UriStore: [%d]\r\n",  actMusic);
    DebugTf("      NFC tag: [%s]\r\n",  playbackState->nfcTag);
    DebugTf(" context Name: [%s]\r\n",  playbackState->contextName);
    DebugTf("  context Uri: [%s]\r\n",  playbackState->contextUri);
    DebugTf(" context Type: [%s]\r\n",  playbackState->contextType);
    DebugTf("       Artist: [%s]\r\n",  playbackState->artistName);
    DebugTf("    item Name: [%s]\r\n",  playbackState->itemName);
    DebugTf("     item Uri: [%s]\r\n",  playbackState->itemUri);
    DebugTf("    item Type: [%s]\r\n",  playbackState->itemType);
    DebugTf("item Duration: [%d]\r\n",  playbackState->itemDuration);
    DebugTf("  Progress ms: [%d]\r\n",  playbackState->progress);
    DebugTf("   check Prgs: [%d]\r\n",  playbackState->checkProgress);
    Debugln();
  }

  RESET_TIMER(checkPlaybackStateTimer);

  return true;

} //  parsePlaybackState()



//----------------------------------------------------------------
bool checkDeserializationError(const char *inFunct, const JsonDocument &doc
                               , DeserializationError error)
{
  if (error)
  {
    digitalWrite(_ERROR_LED, HIGH);
    RESET_TIMER(errorLedOff);
    Debugf("[%s] *****************************************\r\n", inFunct);
    Debugf("[%s] memoryUsage [%d]bytes\r\n", inFunct, doc.memoryUsage());
    Debugf("[%s] *****************************************\r\n", inFunct);
    Debugf("[%s] **  Error parsing payload [", inFunct);
    Debug(error.f_str());
    Debugln("]**");
    Debugf("[%s] *****************************************\r\n", inFunct);
    runLoopFunctions();
    return true;
  }

  if (doc["error"]["status"].as<int>() != 0)
  {
    digitalWrite(_ERROR_LED, HIGH);
    digitalWrite(_GREEN_LED, LOW);
    RESET_TIMER(errorLedOff);
    DebugTf("[%s] Error[%d] - [%s]\r\n", inFunct, doc["error"]["status"].as<int>(), doc["error"]["message"].as<const char *>());
    if (doc["error"]["status"].as<int>() == 401)
    {
      // Refresh Spotify Auth token and Device ID
      DebugTln("fetchToken()..");
      int httpCode = spotify.fetchToken();
      DebugTf("httpCode is [%d]\r\n", httpCode);
      if (httpCode == 200)
      {
        spotifyAccessOK = true;
      }
      else
      {
        DebugTln("\r\n\n===================================================");
        DebugTln("Serious problem!!! Cannot obtain Refresh Token!!!");
        spotifyAccessOK = false;
        DebugTln("===================================================\r\n\n");
      }
    } //-- error == 401 ..

    runLoopFunctions();

    return true;  //-- true := Yes, there is an error

  } //-- there is an error

  //-- no error!
  return false;

} //  checkDeserializationError()

//============================================================
//== wrappers
//============================================================

//----------------------------------------------------------------------------
void setVolume(int newVolume)
{
  newVolume = (int)((int)(newVolume / 5) * 5);
  if (newVolume <   0) newVolume =   0;
  if (newVolume > 100) newVolume = 100;
  playbackState->deviceVolume = newVolume;
  spotify.setVolume(newVolume);

} //  setVolume()

//----------------------------------------------------------------------------
void setShuffleMode(bool newMode)
{
  spotify.shuffle(newMode);
  DebugTf("newMode [%s]\r\n", newMode ? "Shuffle ON":"Shuffle OFF");
  playbackState->shuffle = newMode;
  systemDevice.deviceShuffle = newMode;

  if (newMode)
  {
    digitalWrite(_SHUFFLE_LED, HIGH);
  }
  else
  {
    digitalWrite(_SHUFFLE_LED, LOW);
  }

} //  setShuffleMode()


//----------------------------------------------------------------------------
void nextTrack()
{
  DebugTln("next Track!");
  spotify.setDeviceId(aDevices[actDeviceNum].deviceId);
  spotify.next();
  parsePlaybackState(spotify.getPlaybackState(), true);

} //  nextTrack()


//----------------------------------------------------------------------------
void previousTrack()
{
  DebugTln("previous Track!");
  spotify.setDeviceId(aDevices[actDeviceNum].deviceId);
  spotify.previous();
  parsePlaybackState(spotify.getPlaybackState(), true);

} //  previousTrack()


//----------------------------------------------------------------------------
bool setSystemDevice()
{
  if (strncasecmp(playbackState->deviceName, systemDevice.deviceName, strlen(playbackState->deviceName)) != 0)
  {
    actDeviceNum  = searchPlayerByName(systemDevice.deviceName);
    strlcpy(spotify.deviceName, systemDevice.deviceName, sizeof(spotify.deviceName));
    spotify.setDeviceId(systemDevice.deviceId);
    DebugTf("     actDeviceNum [%d]\r\n", actDeviceNum);
    DebugTf("systemDevice Name [%s]\r\n", systemDevice.deviceName);
    DebugTf("  systemDevice Id [%s]\r\n", systemDevice.deviceId);
    return false;
  }

  //DebugTf("connected to [%s] (correct)!\r\n", systemDevice.deviceName);
  return true;

} //  setSystemDevice()


//------------------------------------------------------------
void playSpotifyUri()
{
  int code = spotify.play();
  switch (code)
  {
    case 404:
    {
      // device id changed, get new one
      spotify.getDevices();
      spotify.play();
      spotify.repeat();
      break;
    }
    case  -1: //-- 17-08-2022
    case 401:
    {
      // auth token expired, get new one
      spotify.fetchToken();
      spotify.play();
      spotify.repeat();
      break;
    }
    default:
    {
      spotify.repeat();
      break;
    }
  }
} //  playSpotifyUri()


/*eof*/
/*eof*/
