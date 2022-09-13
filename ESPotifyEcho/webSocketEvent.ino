
//------------------------------------------------------------------------------------
//-- This function gets a call when a WebSocket event occurs
void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{
  //String  wsPayloadS = String((char *) &payload[0]);
  char   *wsPayload = (char *) &payload[0];
  char    thisSource[20] = {};
  char    thisCommand[20] = {};

  switch (type)
  {
    case WStype_DISCONNECTED: // enum that read status this is used for debugging.
      //DebugTf("WS Type [%d] : DISCONNECTED\r\n", type);
      break;

    case WStype_CONNECTED:  // Check if a WebSocket client is connected or not
      //DebugTf("WS Type [%d] : CONNECTED\r\n", type);
      break;

    case WStype_TEXT: // check responce from client
      //Serial.printf("payload(char*) : [%s]\r\n", wsPayload);
      // Deserialize the document
      SpiRamJsonDocument source(500);
      SpiRamJsonDocument data(20000);
      DeserializationError err;

      int skipPos = getIndex(wsPayload, "}###{", -1, 0, 0);
      err = deserializeJson(source, wsPayload);
      if (checkDeserializationError(__FUNCTION__, source, err)) return;
      snprintf(thisSource, sizeof(thisSource),   "%s", source["source"].as<const char *>() );
      snprintf(thisCommand, sizeof(thisCommand), "%s", source["command"].as<const char *>() );

      //-- replace source[] part of message with spaces --
      for(int i=0; i<(skipPos+4); i++)  wsPayload[i] = ' ';
      err = deserializeJson(data, wsPayload);
      if (checkDeserializationError(__FUNCTION__, data, err)) return;

      if (strcasecmp(thisSource, "indexGUI") == 0)
      {
        handleGUImessage(thisCommand, data);
      }
      else if (strcasecmp(thisSource, "settingsGUI") == 0)
      {
        handleSettingsGUImessage(thisCommand, data);
      }
      else if (strcasecmp(thisSource, "playlistsGUI") == 0)
      {
        handlePlaylistsGUImessage(thisCommand, data);
      }
      break;

  } //  switch(type)

} //  webSocketEvent()


//------------------------------------------------------------------------------
void handleGUImessage(const char *command, const JsonDocument &data)
{
  //DebugT(":");
  //serializeJsonPretty(data, Serial);
  //Debugln();
  //DebugFlush();

  if (strncasecmp(command, "reload", sizeof(command)) == 0)
  {
    updateGuiPlayingInfo();
    updateGuiDevices();
  }
  else if (strncasecmp(command, "playingInfo", sizeof(command)) == 0)
  {
    //DebugTln("Got a \"playingInfo\" message! ");
    updateGuiPlayingInfo();
  }
  else if (strncasecmp(command, "devices", sizeof(command)) == 0)
  {
    DebugTln("Got a \"devices\" message! ");
    updateGuiDevices();
  }
  else if (strncasecmp(command, "volume", sizeof(command)) == 0)
  {
    DebugTln("Got a \"volume\" message! ");
    int newVolume = data["volume"].as<int>();
    setVolume(newVolume);
    Debugf("Volume set to [%d%%]\r\n", newVolume);
  }
  else if (strncasecmp(command, "saveDevice", sizeof(command)) == 0)
  {
    DebugTln("Got a \"saveDevices\" message! ");
    //-----------------------------------------------------------------------------
    //-- putting the next 12 lines of code in a wrapper function craches the ESP32
    //-----------------------------------------------------------------------------

    strlcpy(systemDevice.deviceName, data["new_device"].as<const char *>(), sizeof(systemDevice.deviceName));
    actDeviceNum  = searchPlayerByName(systemDevice.deviceName);
    strlcpy(systemDevice.deviceId, aDevices[actDeviceNum].deviceId, sizeof(systemDevice.deviceId) );
    DebugTf("new systemDevice[%s] Id[%s]\r\n", systemDevice.deviceName, systemDevice.deviceId);

    saveDeviceFile(_DEVICE_FILE);
    //printFile(_DEVICE_FILE);

    DebugTf("Change player from [%s] ", spotify.deviceName);
    strlcpy(spotify.deviceName, systemDevice.deviceName, sizeof(spotify.deviceName));
    Debugf("to [%s]\r\n", spotify.deviceName);
    setSystemDevice();
    parsePlaybackState(spotify.getPlaybackState(), true);
    updateGuiDevices();
  }
  else if (strncasecmp(command, "play", sizeof(command)) == 0)
  {
    DebugTln("Got a \"play\" message! ");
    spotify.play(playbackState->contextUri, playbackState->itemUri, playbackState->progress);
  }
  else if (strncasecmp(command, "pause", sizeof(command)) == 0)
  {
    DebugTln("Got a \"pause\" message! ");
    spotify.pause();
    parsePlaybackState(spotify.getPlaybackState(), true);
  }
  else if (strncasecmp(command, "previous", sizeof(command)) == 0)
  {
    DebugTln("Got a \"previous\" message! ");
    previousTrack();
  }
  else if (strncasecmp(command, "toggle", sizeof(command)) == 0)
  {
    DebugT("Got a \"toggle\" message! ");
    setShuffleMode(!playbackState->shuffle);
  }
  else if (strncasecmp(command, "next", sizeof(command)) == 0)
  {
    DebugTln("Got a \"next\" message! ");
    nextTrack();
  }

} //  handleGUImessage()


//------------------------------------------------------------------------------
void updateGuiPlayingInfo()
{
  SpiRamJsonDocument playingDoc(5000);

  // create object's
  JsonObject playing = playingDoc.to<JsonObject>();

  playing["type"]     = "playingInfo";
  playing["shuffle"]  = playbackState->shuffle ? "ON":"OFF";
  playing["playlist"] = playbackState->contextName;
  playing["album"]    = playbackState->albumName;
  playing["artist"]   = playbackState->artistName;
  playing["track"]    = playbackState->itemName;
  //playing["volume"]   = (int)((int)(playbackState->deviceVolume/5)*5);
  playing["volume"]   = playbackState->deviceVolume;
  playing["duration"] = (int)playbackState->itemDuration / 1000;  // seconds
  playing["progress"] = (int)(playbackState->progress / 5000) *5; // seconds
  playing["ipAddress"] = WiFi.localIP().toString();

  //DebugTf("progress[%d], duration[%d] ==> [%d%%]\r\n", (playbackState->progress/1000)
  //                                          , playbackState->itemDuration/1000
  //                                          , (((playbackState->progress/1000) *100) / (playbackState->itemDuration/1000)));

  //-- serialize the object and save teh result to the string variable.
  serializeJson(playing, jsonString);
  //-- print the string for debugging.
  //Serial.println( jsonString );
  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

} //  updateGuiPlayingInfo()


//------------------------------------------------------------------------------
void updateGuiDevices()
{
  SpiRamJsonDocument devicesDoc(5000);

  // create object's
  JsonObject devices = devicesDoc.to<JsonObject>();
  /*
   aDevices[x].deviceName
   aDevices[x].deviceShuffle ? "shuffle":" "
   aDevices[x].deviceVolume
   aDevices[x].deviceShuffle
   */

  devices["type"]   = "devices";

  if (!parseDevices(spotify.getDevices()) )
  {
    devices["devices"][0]["name"] = "No Devices";
    devices["devices"][0]["active"] = false;
  }
  else
  {
    for(int i=0; i<numDevices; i++)
    {
      devices["devices"][i]["name"] = aDevices[i].deviceName;
      if (strcmp(aDevices[i].deviceName, systemDevice.deviceName) == 0)
      {
        devices["devices"][i]["active"] = true;
      }
      else
      {
        devices["devices"][i]["active"] = false;
      }
    }

  }

  //-- serialize the object and save teh result to teh string variable.
  serializeJson(devices, jsonString);
  //-- print the string for debugging.
  DebugTln( jsonString.c_str() );
  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

} //  updateGuiDevices()



//------------------------------------------------------------------------------
void handleSettingsGUImessage(const char *command, const JsonDocument &data)
{
  bool hasChanged = false;
  //serializeJsonPretty(data, Serial);
  //Serial.println();

  if (strcasecmp(command, "reload") == 0)
  {
    //DebugT("Got a \"reload\" message! ");
  }
  else if (strcasecmp(command, "reStart") == 0)
  {
    DebugTln("Got a \"reStart\" message! .. ");
    uint32_t waitABit = millis();
    while ((millis() - waitABit) < 5000)
    {
      httpServer.handleClient();
      webSocket.loop();
    }
    DebugTln("ESP.restart() now!");
    ESP.restart();
    delay(3000);
  }
  else if (strcasecmp(command, "save") == 0)
  {
    DebugTln("Got a \"save\" message! ");
    if (strcmp(data["new_wifiSSID"].as<const char *>(), settings->wifiSSID) != 0)
    {
      snprintf(settings->wifiSSID, sizeof(settings->wifiSSID), "%s"
               , data["new_wifiSSID"].as<const char *>() );
      hasChanged = true;
    }

    if (strcmp(data["new_wifiPassword"].as<const char *>(), settings->wifiPassword) != 0)
    {
      snprintf(settings->wifiPassword, sizeof(settings->wifiPassword), "%s"
               , data["new_wifiPassword"].as<const char *>() );
      hasChanged = true;
    }

    if (strcmp(data["new_hostName"].as<const char *>(), settings->hostName) != 0)
    {
      snprintf(settings->hostName, sizeof(settings->hostName), "%s"
               , data["new_hostName"].as<const char *>() );
      hasChanged = true;
    }

    if (strcmp(data["new_spotifyClientId"].as<const char *>(), settings->spotifyClientId) != 0)
    {
      snprintf(settings->spotifyClientId, sizeof(settings->spotifyClientId), "%s"
               , data["new_spotifyClientId"].as<const char *>() );
      hasChanged = true;
    }

    if (strcmp(data["new_spotifyClientSecret"].as<const char *>(), settings->spotifyClientSecret) != 0)
    {
      snprintf(settings->spotifyClientSecret, sizeof(settings->spotifyClientSecret), "%s"
               , data["new_spotifyClientSecret"].as<const char *>() );
      hasChanged = true;
    }

    if (strcmp(data["new_spotifyRefreshToken"].as<const char *>(), settings->spotifyRefreshToken) != 0)
    {
      snprintf(settings->spotifyRefreshToken, sizeof(settings->spotifyRefreshToken), "%s"
               , data["new_spotifyRefreshToken"].as<const char *>() );
      hasChanged = true;
    }

    if (hasChanged)
    {
      DebugTln("Something has changed! Save it!");
      saveSettingsFile(_SETTINGS_FILE);
    }
    else
    {
      DebugTln("Nothing has changed!");
    }

  } // --save--

  DebugTln("Now update settingsGUI!");
  DebugFlush();
  updateSettingsGUI();

} //  handleSettingsGUImessage()


//------------------------------------------------------------------------------
void updateSettingsGUI()
{
  SpiRamJsonDocument doc(5000);

  // create an object
  JsonObject object = doc.to<JsonObject>();

  object["wifiSSID"] = settings->wifiSSID;
  object["wifiPassword"] = settings->wifiPassword;
  object["hostName"] = settings->hostName;
  object["spotifyClientId"] = settings->spotifyClientId;
  object["spotifyClientSecret"] = settings->spotifyClientSecret;
  object["spotifyRefreshToken"] = settings->spotifyRefreshToken;
  object["ipAddress"] = WiFi.localIP().toString();

  if (!parseDevices(spotify.getDevices()) )
  {
    object["devices"][0]["name"] = "No Devices";
    object["devices"][0]["active"] = false;
  }
  else
  {
    for(int i=0; i<numDevices; i++)
    {
      object["devices"][i]["name"] = aDevices[i].deviceName;
      if (strcmp(aDevices[i].deviceName, systemDevice.deviceName) == 0)
            object["devices"][i]["active"] = true;
      else  object["devices"][i]["active"] = false;
    }

  }

  //-- serialize the object and save the result to the string variable.
  serializeJson(doc, jsonString);

  //DebugTf("broadcastTXT(%s)\r\n", jsonString.c_str());
  //DebugFlush();

  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

} //  updateSettingsGUI()


//------------------------------------------------------------------------------
void handlePlaylistsGUImessage(const char *command, const JsonDocument &data)
{
  bool hasChanged = false;
  DebugTf("Command [%s] => Data:\r\n", command);
  serializeJsonPretty(data, Serial);
  Serial.println();

  if (strcasecmp(command, "reload") == 0)
  {
    inUriStore = populateUriStore();
  }
  else if (strcasecmp(command, "save") == 0)
  {
    DebugTln("Got a \"save\" message! ");
    saveUriStoreFile(_URI_STORE_FILE);
  } // --save--
  else if (strcasecmp(command, "readNfcTag") == 0)
  {
    DebugTln("Got a \"readNfcTag\" message! ");
    DebugTf("NFC tag on cube [%s]\r\n", actNfcTag);
    int nfcTagNr = searchPlaylistByNfcTag(actNfcTag);

    if (nfcTagNr >= 0)
    {
      //uriStore[nfcTagNr].playlistUri
      sendNewRFID(uriStore[nfcTagNr].playlistName);
    }
    else  sendNewRFID(actNfcTag);

    return;

  } // --readRFID--
  else if (strcasecmp(command, "newLink") == 0)
  {
    char tmpNfcTag[_NFCTAG_LEN+1] = {};
    DebugTln("Got a \"newLink\" message! ");
    strlcpy(tmpNfcTag, data["nfcTag"], sizeof(tmpNfcTag));
    DebugTf("RFID [%s]\r\n", tmpNfcTag);
    int plstNr   = searchPlaylistByName(data["playlistName"]);
    int nfcTagNr = searchPlaylistByNfcTag(tmpNfcTag);
    if ((nfcTagNr < 0) && (plstNr >= 0))
    {
      strlcpy(uriStore[plstNr].nfcTag, tmpNfcTag, sizeof(uriStore[plstNr].nfcTag));
      DebugTf("Linked RFID[%s] to [%s]\r\n", uriStore[plstNr].nfcTag
              , uriStore[plstNr].playlistName);
    }
    else
    {
      DebugTf("Error! RFID[%s] already linked to [%s]\r\n", tmpNfcTag
              , uriStore[nfcTagNr].playlistName);;
    }

  } // --newLink--
  else if (strcasecmp(command, "unlink") == 0)
  {
    DebugTln("Got a \"unlink\" message! ");
    int unlinkNr = searchPlaylistByName(data["playlistName"]);
    if (unlinkNr >= 0)
    {
      snprintf(uriStore[unlinkNr].nfcTag,  sizeof(uriStore[unlinkNr].nfcTag), "-");
      DebugTf("playlist [%s] now has nfcTag[%s]\r\n", uriStore[unlinkNr].playlistName
              , uriStore[unlinkNr].nfcTag);
    }
  } // --unlink--

  DebugTln("Now update playlistsGUI!");
  DebugFlush();
  updatePlaylistsGUI();

} //  handlePlaylistsGUImessage()


//------------------------------------------------------------------------------
void sendNewRFID(const char *newRFID)
{
  SpiRamJsonDocument doc(5000);

  // create an object
  JsonObject object = doc.to<JsonObject>();

  doc["type"]  = "newRFID";
  doc["newRFID"] = newRFID;

  //-- serialize the object and save teh result to teh string variable.
  serializeJson(doc, jsonString);

  DebugTf("broadcastTXT(%s)\r\n", jsonString.c_str());
  DebugFlush();

  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

} //  sendNewRFID()


//------------------------------------------------------------------------------
void updatePlaylistsGUI()
{
  SpiRamJsonDocument linkedDoc(50000);
  SpiRamJsonDocument notLinkedDoc(50000);

  // create an object's
  JsonObject linkedObj    = linkedDoc.to<JsonObject>();
  JsonObject notLinkedObj = notLinkedDoc.to<JsonObject>();

  linkedObj["type"]         = "Linked";
  linkedObj["ipAddress"]    = WiFi.localIP().toString();
  notLinkedObj["type"]      = "notLinked";
  notLinkedObj["ipAddress"] = WiFi.localIP().toString();

  //inUriStore = populateUriStore();

  int linkedNr = 0;
  int notLinkedNr = 0;
  for(int i=0; i<inUriStore; i++)
  {
    //playlistName
    //nfcTag
    if (strlen(uriStore[i].nfcTag) == 20)
    {
      linkedObj["linked"][linkedNr]["playlistName"] = uriStore[i].playlistName;
      linkedObj["linked"][linkedNr]["nfcTag"] = uriStore[i].nfcTag;
      linkedNr++;
    }
    else
    {
      notLinkedObj["notLinked"][notLinkedNr]["playlistName"] = uriStore[i].playlistName;
      notLinkedObj["notLinked"][notLinkedNr]["nfcTag"] = "-";
      notLinkedNr++;
    }

  }

  //-- serialize the object and save teh result to teh string variable.
  serializeJson(linkedDoc, jsonString);

  DebugTf("broadcastTXT(%s)\r\n", jsonString.c_str());
  DebugFlush();

  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

  serializeJson(notLinkedDoc, jsonString);

  DebugTf("broadcastTXT(%s)\r\n", jsonString.c_str());
  DebugFlush();

  //-- send the JSON object through the websocket
  webSocket.broadcastTXT(jsonString);
  jsonString = ""; // clear the String.

} //  updatePlaylistsGUI()




/*eof*/
