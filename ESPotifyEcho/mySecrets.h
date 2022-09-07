/*
  Get clientID and Client secret here:

  https://developer.spotify.com/dashboard/

  create an application

  Then: [Edit Settings]

  Application name
      What ever

  Application description
      Spotify Control by ESp32

  Website
      <empty>

  Where the user may obtain more information about this application (e.g. http://mysite.com).
      <empty>
      
  Redirect URI (importent!!!)
      https://getyourspotifyrefreshtoken.herokuapp.com/callback 

  [SAVE]
*/


// Spotify settings
const char* defaultDeviceName  = "your Echo";
//-- your Spotify account
const char* clientId     = "";
const char* clientSecret = "";
/*
Get refreshToken:
         https://getyourspotifyrefreshtoken.herokuapp.com/
*/
const char* refreshToken = "";

/*eof*/
