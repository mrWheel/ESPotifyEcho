
//---------------------------------------------------------------------------
void loadSettings(const char *fileName)
{
  // Load configuration
  bool loaded = loadSettingsFile(fileName);

  if (!loaded) 
  {
    DebugTln("Using default settings");
    strcpy(settings->wifiSSID, "SSID");
    strcpy(settings->wifiPassword, "Password");
    strcpy(settings->hostName, _HOSTNAME);
    strcpy(settings->spotifyClientId, "get from Spotify");
    strcpy(settings->spotifyClientSecret, "get from Spotify");
    strcpy(settings->spotifyRefreshToken, "get from Spotify");
    saveSettingsFile(_SETTINGS_FILE);
  }

  // Dump settings file
  //printFile(fileName);

  if (!loaded)
        DebugTln("Restart to load the config");
  else  DebugTln("Done!");
    
} //  loadSettings()


//---------------------------------------------------------------------------
bool loadSettingsFile(const char *fileName) 
{
  // Open file for reading
  File file = LittleFS.open(fileName, "r");

  // This may fail if the file is missing
  if (!file) 
  {
    DebugTf("Failed to open %s file\r\n", fileName);
    return false;
  }

  // Allocate the JsonDocument
  DynamicJsonDocument doc(2000);

  // Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  // This may fail if the JSON is invalid
  if (err) 
  {
    DebugT("Failed to deserialize configuration: ");
    Debugln(err.f_str());
    return false;
  }

  // Extract config from the JSON document
  strlcpy(settings->wifiSSID, doc["wifiSSID"] | "not Set", sizeof(settings->wifiSSID));
  strlcpy(settings->wifiPassword, doc["wifiPassword"] | "not Set", sizeof(settings->wifiPassword));
  strlcpy(settings->hostName, doc["hostName"] | _HOSTNAME, sizeof(settings->hostName));
  strlcpy(settings->spotifyClientId, doc["spotifyClientId"] | "not Set", sizeof(settings->spotifyClientId));
  strlcpy(settings->spotifyClientSecret, doc["spotifyClientSecret"] | "not Set", sizeof(settings->spotifyClientSecret));
  strlcpy(settings->spotifyRefreshToken, doc["spotifyRefreshToken"] | "not Set", sizeof(settings->spotifyRefreshToken));

  return true;
  
} //  loadSettingsFile()


//---------------------------------------------------------------------------
void saveSettingsFile(const char *fileName) 
{
  DebugTln("save 4 generations of Settings ... ");
  rotateFile(fileName, 4);

  // Open file for writing
  File file = LittleFS.open(fileName, "w");
  if (!file) 
  {
    DebugTf("Failed to create %s file\r\n", fileName);
    return;
  }

  // Allocate the JsonDocument
  DynamicJsonDocument doc(3000);

  // Fill JSON document from settings
  doc["wifiSSID"] = settings->wifiSSID;
  doc["wifiPassword"] = settings->wifiPassword;
  doc["hostName"] = settings->hostName;
  doc["spotifyClientId"] = settings->spotifyClientId;
  doc["spotifyClientSecret"] = settings->spotifyClientSecret;
  doc["spotifyRefreshToken"] = settings->spotifyRefreshToken;

  DebugTln("---------------------------------------------------");
  serializeJsonPretty(doc, Serial);
  Debugln();
  DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;
  if (!success) 
  {
    DebugTln("Failed to serialize and write settings to file");
  }

} //  saveSettingsFile()


//---------------------------------------------------------------------------
bool loadDeviceFile(const char *fileName) 
{
  DebugTf("FileName [%s]\r\n", fileName);
  // Open file for reading
  File file = LittleFS.open(fileName, "r");

  // This may fail if the file is missing
  if (!file) 
  {
    DebugTf("Failed to open %s file\r\n", fileName);
    return false;
  }

  // Allocate the JsonDocument
  DynamicJsonDocument doc(2000);

  // Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  // This may fail if the JSON is invalid
  if (err) 
  {
    DebugT("Failed to deserialize configuration: ");
    Debugln(err.f_str());
    return false;
  }

  //-- Extract config from the JSON document
  strlcpy(systemDevice.deviceName, doc["deviceName"].as<const char*>(), sizeof(systemDevice.deviceName));
  strlcpy(systemDevice.deviceId,   doc["deviceId"].as<const char*>(),   sizeof(systemDevice.deviceId));
  systemDevice.deviceVolume  = doc["deviceVolume"].as<int>();
  systemDevice.deviceShuffle = doc["deviceShuffle"].as<bool>();
  
  return true;
  
} //  loadSettingsFile()


//---------------------------------------------------------------------------
void saveDeviceFile(const char *fileName) 
{
  DebugTln("save Device settings ... ");

  // Open file for writing
  File file = LittleFS.open(fileName, "w");
  if (!file) 
  {
    DebugTf("Failed to create %s file\r\n", fileName);
    return;
  }

  // Allocate the JsonDocument
  SpiRamJsonDocument doc(2000);

  // Fill JSON document from systemDevice
  doc["deviceName"]     = systemDevice.deviceName;
  doc["deviceId"]       = systemDevice.deviceId;
  doc["deviceVolume"]   = systemDevice.deviceVolume;
  doc["deviceShuffle"]  = systemDevice.deviceShuffle;

  DebugTln("---------------------------------------------------");
  serializeJsonPretty(doc, Serial);
  Debugln();
  DebugTln("---------------------------------------------------");
  // Serialize JSON to file
  bool success = serializeJsonPretty(doc, file) > 0;

  if (!success) 
  {
    DebugTf("Failed to serialize and write device [%s]\r\n", _DEVICE_FILE);
  }
  
} //  saveDeviceFile()


//-----------------------------------------------------------------
int loadUriStore(const char *fileName)
{
  memset(uriStore, 0, sizeof(uriStoreStruct));

  //-- Load uriStore
  int i = loadUriStoreFile(fileName);
  
  if (i < 0) 
  {
    DebugTln("Using default uriStore");
    i=0;
    strcpy(uriStore[i].playlistName, "playlistName");
    strcpy(uriStore[i].nfcTag, "00000000000000000000");
  }

  //-- Dump settings file
  //printFile(fileName);

  return i;  
    
} //  loadUriStore()


//-----------------------------------------------------------------
int loadUriStoreFile(const char *fileName) 
{
  // Open file for reading
  File file = LittleFS.open(fileName, "r");

  // This may fail if the file is missing
  if (!file) 
  {
    DebugTf("Failed to open %s file\r\n", fileName);
    return false;
  }

  //-- Allocate the JsonDocument
  DebugTf("Psram Free [%d]bytes\r\n", ESP.getFreePsram() );
  DebugTln("Claiming 100.000 bytes for playlistStore ..");
  SpiRamJsonDocument doc(100000);
  DebugTf("After claim  Psram Free [%d]bytes\r\n", ESP.getFreePsram() );

  // Parse the JSON object in the file
  DeserializationError err = deserializeJson(doc, file);

  if (checkDeserializationError(__FUNCTION__, doc, err)) return -1;

  //DebugTln("==================================");
  //serializeJsonPretty(doc, Serial);
  //Debugln();
  //DebugTln("==================================");

  JsonArray playlists = doc["uriStore"];
  
  DebugTf("Found [%d] playlists\r\n", playlists.size());
  DebugFlush();

  //-- Extract config from the JSON document
  //-- and place it in uriStore
  int i = -1;
  for (JsonObject playlist : playlists) 
  {
    // Increment inStore count
    i++;
    if (i>_MAX_URISTORE) break;

    Debug('.');
    // Load the playlist
    //int  recNr;
    //char nfcTag[_NFCTAG_LEN+1];  //-- make room for '0'
    //char playlistName[_PLAYLIST_NAME_LEN];
    //char playlistUri[_URI_LEN];
    //int  tracks;
    //char artist[_ARTIST_NAME_LEN];
    //char album[_ALBUM_NAME_LEN];

    snprintf(uriStore[i].nfcTag,      sizeof(uriStore[i].nfcTag),      "%s", playlist["nfcTag"].as<const char*>() );
    snprintf(uriStore[i].playlistName,sizeof(uriStore[i].playlistName),"%s", playlist["playlistName"].as<const char*>() );
    snprintf(uriStore[i].playlistUri, sizeof(uriStore[i].playlistUri), "%s", playlist["playlistUri"].as<const char*>() );
    //snprintf(uriStore[i].artist,      sizeof(uriStore[i].artist),      "%s", playlist["artist"].as<const char*>() );
    //snprintf(uriStore[i].album,       sizeof(uriStore[i].album),       "%s", playlist["album"].as<const char*>() );
    uriStore[i].recNr   = playlist["recNr"].as<int>();
    uriStore[i].tracks  = playlist["tracks"].as<int>();

  }
  Debugln();
  return (i+1);
  
} //  loadUriStoreFile()


//-------------------------------------------------------------------------
bool saveUriStoreFile(const char* fileName)
{
  char comma[3] = "";
  char indent[] = "     ";
  
  DebugTf("save 4 generations. In Store [%d]\r\n", inUriStore);
  rotateFile(fileName, 4);
  
  // Open file for write
  File file = LittleFS.open(fileName, "w");
  // This may fail if the file is missing
  if (!file) 
  {
    DebugTf("Failed to open %s file\r\n", fileName);
    return false;
  }

  file.print("{\"uriStore\":[");

  int recNr = 0;
  for(int r=0; r<inUriStore; r++)
  {
    if (   (strlen(uriStore[r].nfcTag) != 20) 
        || (strcmp(uriStore[r].nfcTag, "00000000000000000000") == 0) )
    {
      DebugTf("Skip record [%d]\r\n", r);
      continue;
    }
    //int  recNr;
    //char nfcTag[_NFCTAG_LEN+1];  //-- make room for '0'
    //char playlistName[_PLAYLIST_NAME_LEN];
    //char playlistUri[_URI_LEN];
    //int  tracks;
    //char artist[_ARTIST_NAME_LEN];
    //char album[_ALBUM_NAME_LEN];

    file.printf("%s\r\n   {\r\n"      , comma);
    file.printf("%s\"recNr\": %d,\r\n" , indent, recNr);
    file.printf("%s\"nfcTag\": \"%s\",\r\n"
                                      , indent, uriStore[r].nfcTag);
    file.printf("%s\"playlistName\": \"%s\",\r\n"
                                      , indent
                                      , uriStore[r].playlistName);
    file.printf("%s\"playlistUri\": \"%s\",\r\n"
                                      , indent
                                      , uriStore[r].playlistUri);
    file.printf("%s\"tracks\": %d\r\n   }"
                                      , indent
                                      , uriStore[r].tracks);
    strlcpy(comma, ",", sizeof(comma));
    recNr++;
    
  } // for r..

  file.println("\r\n]}");

  DebugTf("Saved [%d] playlist records\r\n", (recNr+1));

  return true;

} //  saveUriStoreFile()


//---------------------------------------------------------------------------
void splitsFileName(const char* fName, const char* extension, char* bareName)
{
  int i=0;
  
  int extPos = getIndex(fName, extension, -1, 0 , 0);
  for(i=0; i<extPos; i++) bareName[i] = fName[i];
  bareName[i] = 0;

} //  splitsFileName()


//-------------------------------------------------------------------------
bool rotateFile(const char* fileName, int maxGen)
{
  char bareName[32] = {};
  char tmpName[32]  = {};
  char newName[32]  = {};
  int i = 0;

  DebugTf("[%s] -> maxGen [%d] ", fileName, maxGen);
  splitsFileName(fileName, ".json", bareName);
  Debugf("bareName is [%s]\r\n", bareName);

  for (int x=9; x>=maxGen; x--)
  {
    snprintf(newName, sizeof(newName), "%s%d.json", bareName, x);
    DebugTf("remove [%s]\r\n", newName);
    if (LittleFS.exists(newName)) { LittleFS.remove(newName); }
  }
  for (int s=maxGen; s>1; s--)
  {
    snprintf(tmpName, sizeof(tmpName), "%s%d.json", bareName, (s-1));
    snprintf(newName, sizeof(newName), "%s%d.json", bareName, s);
    DebugTf("rename[%s] => [%s]\r\n", tmpName, newName);
    if (LittleFS.exists(tmpName)) { LittleFS.rename(tmpName, newName); }
    
  }
  snprintf(newName, sizeof(newName), "%s%d.json", bareName, 1);
  DebugTf("rename[%s] => [%s]\r\n", fileName, newName);
  if (LittleFS.exists(fileName)) { LittleFS.rename(fileName, newName); }

  return true;

} //  rotateFile()


//-------------------------------------------------------------------------
// Prints the content of a file to the Serial
void printFile(const char *fileName) 
{
  // Open file for reading
  File file = LittleFS.open(fileName, "r");
  if (!file) 
  {
    DebugTln("Failed to open config file");
    return;
  }

  // Extract each by one by one
  DebugTf("-- file [%s] -------------------------\r\n", fileName);
  while (file.available()) 
  {
    char x = (char)file.read();
    Debug(x);
  }

  Debugln("\r\n-- eof -----------------------------------------------------");
  
} //  printFile()
