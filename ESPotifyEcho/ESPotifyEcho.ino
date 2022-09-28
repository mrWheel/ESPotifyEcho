/*
  ESPotifyEcho    Rev. 1.0 (28-09-2022)

  This is code which I've created using “the evolutionary method”
  … which means “I don't know how it works” ;-)

  IDE settings:
    - Board           : "ESP32 Dev Module" [ESP32-WROVER-E]
    - Flash Size      : "4MB (32Mb)"
    - Partition Scheme: "Default 4MB with spiffs (1.2MB APP/1.5MB SPIFFS)"
    - Core Debug Level: "None" ??
    - PSRAM           : "Enabled"
    - Arduino Runs On : "Core 1"
    - Events Run On   : "Core 0"

  Coding Style  ( http://astyle.sourceforge.net/astyle.html#_Quick_Start )
   - Allman style (-A1)
   - tab 2 spaces (-s2)
   - Indent 'switch' blocks (-S)
   - Indent preprocessor blocks (-xW)
   - Indent multi-line preprocessor definitions ending with a backslash (-w)
   - Indent C++ comments beginning in column one (-Y)
   - Insert space padding after commas (-xg)
   - Attach a pointer or reference operator (-k3)

  use:  astyle -A1 -s2 -S -xW -w -Y -xg -k3 *.{ino|h}

  remove <filename>.orig afterwards
*/
#define _USE_UPDATE_SERVER

#define _HOSTNAME                 "ESPotify"
#define _SETTINGS_FILE            "/settings.json"
#define _DEVICE_FILE              "/device.json"
#define _URI_STORE_FILE           "/playlistStore.json"
#define _EEPROM_VERSION              1

#define _NFCTAG_LEN                 20
#define _WIFI_SSID_LEN              32
#define _WIFI_PASSWORD_LEN          64
#define _SPOTIFY_ID_LEN             35
#define _SPOTIFY_SECRET_LEN         35
#define _SPOTIFY_REFRESHTOKEN_LEN  200
#define _MAX_LITTLEFS_FILES         30
#define _URI_LEN                    40
#define _DEVICE_ID_LEN              60
#define _DEVICE_NAME_LEN            30
#define _DEVICE_TYPE_LEN            20
#define _PLAYLIST_NAME_LEN         100
#define _ARTIST_NAME_LEN           100
#define _ALBUM_NAME_LEN            100
#define _TRACK_NAME_LEN            100

#define _DEBUG
#define _MAX_DEVICES                20
#define _MAX_URISTORE               99
#define _URISTORE_REC_LEN          400

#define _WHITE_LED                  13 //-- WHT1 on Silkscreen Rev.1.0 PCB
#define _GREEN_LED                  14 //-- WHT2 on Silkscreen Rev.1.0 PCB
#define _ERROR_LED                  15
#define _SHUFFLE_LED                25
#define _B0_LED                     26
#define _B1_LED                     27

#define _PROX_BUTTONS                2

//#define _ALT_CAP_SWITCHES //-- normal is 0 & 1

#ifdef _ALT_CAP_SWITCHES
  #define _CAP_SW_UP                10 //-- UP must be the lowest number
  #define _CAP_SW_DOWN              11 //-- DOWN must be the larger number
#else
  #define _CAP_SW_UP                 0 //-- UP must be the lowest number
  #define _CAP_SW_DOWN               1 //-- DOWN must be the larger number
#endif


#define _MFRC522_RST                 4
#define _MFRC522_SS                  5     //-- label "SDA"
#define MFRC522_NO_TAG              -1
#define MFRC522_SAME_TAG             0
#define MFRC522_NEW_TAG              1
#define MFRC522_NO_ACTION            9
#define _NO_NFC_TAG                 "00000000000000000000"

#define _SPI_SCK                    18
#define _SPI_MISO                   19
#define _SPI_MOSI                   23

#define _I2C_SDA                    21
#define _I2C_SCL                    22

#include <Arduino.h>
//------ [TelnetStream@1.2.2]
#include <TelnetStream.h>
#include "Debug.h"
//------ [WiFi@1.2.7 WiFi@2.0.0] (esp32 core 2.0.3)
#include <WiFi.h>
//------ [WiFiManager@2.0.10-beta] https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>
#include "spotifyClient.h"
//------ [ArduinoJson@6.19.3]
#include <ArduinoJson.h>
#include "jsonPSram.h"
#include "checkPsram.h"
#include "timers.h"
//------ [EEPROM@2.0.0] (esp32 core 2.0.3)
#include <EEPROM.h>
//------ [LittleFS@2.0.0] - [FS@2.0.0] (esp32 core 2.0.3)
#include <LittleFS.h>
//------ [WebServer@2.0.0] (esp32 core 2.0.3)
#include <WebServer.h>
//------ [arduinoWebSockets-master@2.3.5]
#include <WebSocketsServer.h>  // Include Websocket Library
//------ [ESPmDNS@2.0.0] (esp32 core 2.0.3)
#include <ESPmDNS.h>
//------ [Wire@2.0.0]
#include <Wire.h>
//------ [Adafruit_MPR121@1.1.1]
#include <Adafruit_MPR121.h>
//------ [SPI@2.0.0] (esp32 core 2.0.3)
#include <SPI.h>
//------ [MFRC522@1.4.10]
#include <MFRC522.h>
#ifdef _USE_UPDATE_SERVER
  //#include <ModUpdateServer.h>   // https://github.com/mrWheel/ModUpdateServer
  //#include "updateServerHtml.h"
  #include <HTTPUpdateServer.h>
#endif


#ifndef _BV
  #define _BV(bit) (1 << (bit))
#endif

WebServer       httpServer(80);
#ifdef _USE_UPDATE_SERVER
  HTTPUpdateServer httpUpdater; //httpUpdater(true);
#endif

//--create instance for webSocket server on port"81"
WebSocketsServer webSocket = WebSocketsServer(81);

//-- You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();
//-- Create MFRC522 instance
MFRC522 mfrc522(_MFRC522_SS, _MFRC522_RST);


enum proxStates { PS_INIT, PS_PRESSED, PS_SHORT_RELEASE, PS_LONG_PRESS, PS_VERY_LONG_PRESS };

DECLARE_TIMER(checkPlaybackStateTimer, 10);   //  30 seconds
DECLARE_TIMER(checkRfidReader, 2);            //   2 second
DECLARE_TIMERm(errorLedOff, 1);               //   1 minuten
DECLARE_TIMERm(calibrateMPR121Timer, 10);     //  10 minuten
DECLARE_TIMERms(shuffleLedOff, 2000);         //   2 seconden
DECLARE_TIMERms(whiteLedsTimer, 100);         // 100 milliseconden
DECLARE_TIMERms(updateGuiTimer, 2000);        // 100 milliseconden

struct settingStruct
{
  char wifiSSID[_WIFI_SSID_LEN];
  char wifiPassword[_WIFI_PASSWORD_LEN];
  char hostName[20];
  char spotifyClientId[_SPOTIFY_ID_LEN];
  char spotifyClientSecret[_SPOTIFY_SECRET_LEN];
  char spotifyRefreshToken[_SPOTIFY_REFRESHTOKEN_LEN];
};

struct deviceStruct
{
  char  deviceId[_DEVICE_ID_LEN];     // "8166929d-7509-4a7e-989f-cd3194692532_amzn_1"
  char  deviceName[_DEVICE_ID_LEN];
  int   deviceVolume;
  bool  deviceShuffle;
};

struct trackStruct
{
  char      playlistUri[_URI_LEN];
  char      trackName[_TRACK_NAME_LEN];
  char      albumName[_ALBUM_NAME_LEN];
  char      artistName[_ARTIST_NAME_LEN];
  uint32_t  duration_ms;
  uint32_t  progress_ms;
  uint32_t  checkProgress;
};

struct uriStoreStruct
{
  int       recNr;
  char      nfcTag[_NFCTAG_LEN+1];  //-- make room for '0'
  char      playlistName[_PLAYLIST_NAME_LEN];
  char      playlistUri[_URI_LEN];
  int       tracks;
  char      artist[_ARTIST_NAME_LEN];
  char      album[_ALBUM_NAME_LEN];
};

struct playbackStateStruct
{
  char      deviceId[_DEVICE_ID_LEN];
  char      deviceName[_DEVICE_ID_LEN];
  char      deviceType[_DEVICE_TYPE_LEN];
  int       deviceVolume;
  bool      shuffle;
  char      repeat[10];
  uint32_t  progress;
  uint32_t  checkProgress;
  bool      isPlaying;
  int       trackNr;
  char      artistName[_ARTIST_NAME_LEN];
  char      contextName[_PLAYLIST_NAME_LEN];
  char      contextUri[_URI_LEN];
  char      contextType[20];
  char      albumName[_ALBUM_NAME_LEN];
  char      itemName[_TRACK_NAME_LEN];
  char      itemUri[_URI_LEN];
  char      itemType[20];
  uint32_t  itemDuration;
  char      nfcTag[_NFCTAG_LEN+1];
};

struct eepromStruct
{
  int       goodState;
  char      playlistName[_PLAYLIST_NAME_LEN];
  char      playlistUri[_URI_LEN];
};

settingStruct       *settings;
uriStoreStruct      *uriStore;
int8_t              inUriStore = 0;
int8_t              actMusic = 0;
deviceStruct        *aDevices;
deviceStruct        systemDevice;
int8_t              numDevices = 0;
int8_t              actDeviceNum  = -1;

eepromStruct        eepromStore;
byte const          BUFFERSiZE = 176;
bool                firstLoop = true;
playbackStateStruct *playbackState;
bool                nfcTagPresent   = true;
bool                mfrc522Present  = false;
int8_t              mfrc522state, mfrc522prevState;
bool                spotifyAccessOK;
bool                mpr121Present = false;
bool                ledState = 0;
char                actNfcTag[_NFCTAG_LEN +1] = {0};
String              jsonString;

//---- prototypes --------------------------------------------
//void convertToJson(const settingStruct &src, JsonVariant dst);
//void convertFromJson(JsonVariantConst src, settingStruct &dst);

spotifyClient spotify = spotifyClient();
psramCheck    psram   = psramCheck();


//------------------------------------------------------------
bool connectWifi(const char *ssid, const char *password)
{
  WiFi.mode(WIFI_STA);

  int t = 0;
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    t++;
    if (t > 20) break;
  }
  if (t<20)
  {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("");
    if (WiFi.softAPdisconnect(true))
          Serial.println("WiFi Access Point disconnected and closed");
    else  Serial.println("Hm.. could not disconnect WiFi Access Point! (maybe there was none?)");

    return true;
  }
  //-- WiFiManager, Local intialization.
  //-- Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  //wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect(_HOSTNAME); // open ap

  if(!res)
  {
    Serial.println("Failed to connect");
    // ESP.restart();
    return false;
  }
  else
  {
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    snprintf(settings->wifiSSID,     sizeof(settings->wifiSSID),     "%s", wm.getWiFiSSID().c_str());
    snprintf(settings->wifiPassword, sizeof(settings->wifiPassword), "%s", wm.getWiFiPass().c_str());
    Serial.println("\r\n************* Now rebooting! *************");
    saveSettingsFile(_SETTINGS_FILE);
    delay(2000);
    ESP.restart();
    delay(3000);
  }
  return true;

} //  connectWifi()


//------------------------------------------------------------
void runLoopFunctions()
{
  menuLoop();

  if (mpr121Present)
  {
    mpr121UpdateStates();
    mpr121Handle();
  }
  httpServer.handleClient();
  //-- webSocket server methode that handles all Client
  webSocket.loop();


  pulseWhiteLeds();

  if (!spotifyAccessOK)
  {
    digitalWrite(_ERROR_LED, HIGH);
    digitalWrite(_GREEN_LED, LOW);
    return;
  }

} //  runLoopFunctions()


//------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  while(!Serial)
  {
    delay(10);
  }

  Serial.printf("\r\nTemporary Hostname [%s]\r\n", _HOSTNAME);
  Serial.flush();

  pinMode(_WHITE_LED,  OUTPUT);
  pinMode(_GREEN_LED,  OUTPUT);
  pinMode(_ERROR_LED,   OUTPUT);
  pinMode(_SHUFFLE_LED, OUTPUT);
  pinMode(_B0_LED,      OUTPUT);
  pinMode(_B1_LED,      OUTPUT);

  blinkAllLeds(10);

  //------------ claim PSRAM -------------------------------------------------------
  DebugTf("Psram Free [%d]bytes [after]\r\n", ESP.getFreePsram() );

  settings = (settingStruct *) ps_malloc( sizeof(settingStruct) );
  DebugTf("Claim [%d]bytes for settings\r\n", sizeof(settings) );
  memset(settings, 0, sizeof(settingStruct) );

  uriStore = (uriStoreStruct *) ps_malloc( sizeof(uriStoreStruct) * (_MAX_URISTORE+1) );
  DebugTf("Claim [%d]bytes for uriStore\r\n", (sizeof(uriStoreStruct) * (_MAX_URISTORE+1)) );
  memset(uriStore, 0, (sizeof(uriStoreStruct) * (_MAX_URISTORE+1)));

  aDevices = (deviceStruct *) ps_malloc( sizeof(deviceStruct) * _MAX_DEVICES );
  DebugTf("Claim [%d]bytes for aDevices\r\n", (sizeof(deviceStruct) * _MAX_DEVICES) );
  memset(aDevices, 0, (sizeof(deviceStruct) * _MAX_DEVICES));

  playbackState = (playbackStateStruct *) ps_malloc( sizeof(playbackStateStruct) );
  DebugTf("Claim [%d]bytes for playbackState\r\n", sizeof(playbackStateStruct) );
  memset(playbackState, 0, sizeof(playbackStateStruct));

  LittleFS.begin();
  loadSettings(_SETTINGS_FILE);
  Serial.printf("\r\nHostname [%s]\r\n", settings->hostName);
  Serial.flush();

  //printFile(_DEVICE_FILE);
  loadDeviceFile(_DEVICE_FILE);

  if (!connectWifi(settings->wifiSSID, settings->wifiPassword))
  {
    Serial.println("\r\nSERIOUS PROBLEM");
    Serial.println("NOT CONNECTED TO WIFI!");
  }

  Serial.printf("mDNS setup as [%s.local]\r\n", settings->hostName);
  if (MDNS.begin(settings->hostName))      // Start the mDNS responder for Hostname.local
        Serial.printf("mDNS responder started as [%s.local]\r\n", settings->hostName);
  else  Serial.println("Error setting up MDNS responder!\r\n");

  MDNS.addService("http", "tcp", 80);
  Serial.println();

  TelnetStream.begin();
  //-- empty buffer
  while(TelnetStream.available())
  {
    TelnetStream.read();
    delay(10);
  }

  spotify.begin( settings->spotifyClientId
                 , settings->spotifyClientSecret
                 , settings->spotifyRefreshToken
                 , systemDevice.deviceId);


  if (ESP.getPsramSize() > 0)
  {
    DebugTf("Psram size [%d]bytes\r\n", ESP.getPsramSize() );
    DebugTf("Psram Free [%d]bytes\r\n", ESP.getFreePsram() );
  }
  else  DebugTln("module does not have PSRAM");

  DebugTln();
  //-- initialize mpr121 module
  if (mpr121Init())
  {
    DebugTln("MPR121 found!");
    mpr121Present = true;
  }
  else
  {
    DebugTln("MPR121 not found, check wiring?");
    DebugTln("Continue without buttons..!");
    mpr121Present = false;
  }

  // Init SPI bus and MFRC522 for NFC reader
  SPI.begin();
  //-- Init MFRC522 board.
  mfrc522.PCD_Init();
  delay(20);
  //-- Show details of PCD - MFRC522 Card Reader details.
  DebugTln();
  mfrc522.PCD_DumpVersionToSerial();
  if (mfrc522.PCD_PerformSelfTest())
        mfrc522Present = true;
  else  mfrc522Present = false;
  DebugTln();

  // Refresh Spotify Auth token and Device ID
  DebugTln("fetchToken()..");
  int httpCode = spotify.fetchToken();
  DebugTf("httpCode is [%d] ", httpCode);
  if (httpCode == 200)
  {
    Debugln(" -> OK!");
    spotifyAccessOK = true;
  }
  else
  {
    DebugTln(" -> should be 200! Error!\r\n");
    Debugln("======================================================");
    Debugln("** Serious problem!!! Cannot obtain Refresh Token!! **");
    Debugln("======================================================");
    DebugTln("\r\n");
    spotifyAccessOK = false;
    blinkAllLeds(15);
  }

  psram.startPsram();
  if (spotifyAccessOK)
  {
    DebugTln("getUserId()..");
    parseUserId(spotify.getUserId());
  }
  psram.endPsram();

  if (spotifyAccessOK)
  {
    DebugTln("getDevices()..");
    if (!parseDevices(spotify.getDevices()) )
    {
      DebugTln("No devices found!");
      digitalWrite(_ERROR_LED, HIGH);
      digitalWrite(_GREEN_LED, LOW);
    }
  }
  else blinkAllLeds(5);

  DebugTf("Read EEPROM data (%d bytes)\r\n", sizeof(eepromStruct));
  EEPROM.begin((sizeof(eepromStruct)+100));
  if (!eepromRead())
  {
    DebugTln("Eeprom data NOT valid");
    actDeviceNum  = -1;
  }
  else
  {
    DebugTln("EEPROM data is valid!");
  }

  if (spotifyAccessOK)
  {
    actDeviceNum = searchPlayerByName(systemDevice.deviceName);
    DebugTf("     actDeviceNum [%d]\r\n", actDeviceNum);
    DebugTf("systemDevice Name [%s]\r\n", systemDevice.deviceName);
    DebugTf("  systemDevice Id [%s]\r\n", systemDevice.deviceId);
    strlcpy(spotify.deviceName, systemDevice.deviceName, sizeof(spotify.deviceName));
  }

  DebugTf("[%d] Player[%-20.20s] Volume[%d%%]\r\n", actDeviceNum
          , systemDevice.deviceName
          , systemDevice.deviceVolume);

  if (spotifyAccessOK)
  {
    DebugTln("Populate=========================================");
    inUriStore = populateUriStore();
    DebugTf("There are [%d] records in uriStore\r\n", inUriStore);
  }

  //-- set some static var's
  readNfcTag();
  readNfcTag();
  //-- now ready to read NFC tag
  readNfcTag();
  DebugTf("actNfcTag [%s]\r\n", actNfcTag);
  if (spotifyAccessOK) parsePlaybackState(spotify.getPlaybackState(), true);

  if (setSystemDevice())
        DebugTln("Ok, seems correct device is already selected!");
  else  DebugTln("Hm.. changed device (just to be sure)!");

  actMusic = -1;
  if (spotifyAccessOK)
  {
    actMusic = searchPlaylistByNfcTag(actNfcTag);

    if (actMusic >= 0)
    {
      if (strncmp(playbackState->contextUri, uriStore[actMusic].playlistUri, sizeof(uriStore[actMusic].playlistUri)) == 0 )
      {
        DebugTln("Ok, seems playlist is already playing!");
      }
      else
      {
        handleNewTag();
      }
    }
    else
    {
      handleNewTag();
    }

    DebugTln("spotify.repeat(context) ..");
    spotify.repeat();
  }

  if (ESP.getFreePsram() > 0)
    DebugTf("FreePSRAM [%d]bytes\r\n", ESP.getFreePsram() );
  else  DebugTln("module does not have PSRAM");

  httpServer.serveStatic("/",                 LittleFS, "/index.html");
  httpServer.serveStatic("/index",            LittleFS, "/index.html");
  httpServer.serveStatic("/index.html",       LittleFS, "/index.html");
  httpServer.serveStatic("/ESPotifyEcho.css", LittleFS, "/ESPotifyEcho.css");
  httpServer.serveStatic("/playlists",        LittleFS, "/playlists.html");
  httpServer.serveStatic("/playlists.html",   LittleFS, "/playlists.html");
  httpServer.serveStatic("/settings",         LittleFS, "/settings.html");
  httpServer.serveStatic("/settings.html",    LittleFS, "/settings.html");
  httpServer.serveStatic("/timerOn.png",      LittleFS, "/turningClock.gif");

  setupFSmanager();

  httpUpdater.setup(&httpServer);
  //-- Start server's
  httpServer.begin();
  //-- init the Websocketserver
  webSocket.begin();
  //-- init the webSocketEvent function when a websocket event occurs
  webSocket.onEvent(webSocketEvent);

  DebugTln("HTTP httpServer started");

  DebugTln();
  DebugTln("Done with setup()!\r\n");
  DebugFlush();

  RESET_TIMER(checkPlaybackStateTimer);

} //  setup()


//------------------------------------------------------------
void loop()
{
  runLoopFunctions();

  if (mfrc522Present && DUE(checkRfidReader))
  {
    if (mfrc522state != MFRC522_NO_ACTION)
    {
      mfrc522prevState = mfrc522state;
    }
    mfrc522state = readNfcTag();
    switch(mfrc522state)
    {
      case MFRC522_NO_TAG:
        if (mfrc522prevState != mfrc522state)
        {
          DebugTln("NO Card found!");
          handleNoTag();
        }
        break;
      case MFRC522_SAME_TAG:
        //DebugTln("SAME Card found!");
        handleSameTag();
        break;
      case MFRC522_NEW_TAG:
        DebugTln("NEW Card found!");
        handleNewTag();
        break;

    } //  .. state

  } //  due ..

  if (DUE(checkPlaybackStateTimer))
  {
    parsePlaybackState(spotify.getPlaybackState(), false);
    if (!showNowPlaying(false) )
    {
      playSpotifyUri();
      updateGuiPlayingInfo();
    }
  }

  if (mpr121Present && DUE(calibrateMPR121Timer))
  {
    mpr121Init();
  }

  if (DUE(errorLedOff))
  {
    digitalWrite(_ERROR_LED, LOW);
    digitalWrite(_GREEN_LED, HIGH);
  }

  //-- in case some other device changed Shuffle mode
  if (playbackState->shuffle)
        digitalWrite(_SHUFFLE_LED, HIGH);
  else  digitalWrite(_SHUFFLE_LED, LOW);

  yield();

} //  loop()
