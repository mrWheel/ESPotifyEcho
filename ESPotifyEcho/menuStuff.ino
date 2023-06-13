
int activeMenu    = 0;
int activeStream  = 0;  // 1==Serial, 2=Telnet

//----------------------------------------------------------------------
void menuLoop()
{
  if (Serial.available())
    activeStream = 1;
  else if (TelnetStream.available())
    activeStream = 2;
  else
  {
    activeStream = 0;
    return;
  }

  do
  {
    switch(activeMenu)
    {
      case 0:
        activeMenu = topMenu();
        break;
      case 1:
        selectMusic();
        break;
      case 2:
        selectDevice();
        break;
      case 3:
        playerControl();
        break;
      case 4:
        selectSystem();
        break;
      default:
        activeMenu = topMenu();
        break;
    } //  activeMenu

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while(activeMenu >= 0);
  activeMenu = 0;
  if (spotifyAccessOK) showNowPlaying(true);

} //  menuLoop()


//----------------------------------------------------------------------
int topMenu()
{
  int keuze;

  //-- empty buffer's
  while(Serial.available())
  {
    Serial.read();
  }
  while(TelnetStream.available())
  {
    TelnetStream.read();
  }

  do
  {
    keuze = -1;
    Debugln("\r\n[Top Menu]--------------------");
    Debugln("  1 - select Music");
    Debugln("  2 - select Device");
    Debugln("  3 - player Control");
    Debugln("  4 - System");
    Debugln("  q - Quit menu");

    keuze = waitForInput(4);
    if (keuze == 0)
    {
      return -1;
    }

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) && (keuze > 4) );

  Debugf("\r\n\n[topMenu] You made your choice [%d]\r\n\n", keuze);
  return keuze;

} //  topMenu()


//----------------------------------------------------------------------
void selectArtist()
{
  int   keuze;
  char  deleteId[50] = {};
  char  cArtist[100] = {};
  char  urlArtist[100] = {};
  int totalTracks, offset = 0;

  if (!spotifyAccessOK)
  {
    activeMenu = -1;
    return;
  }

  DebugTln("Delete playlist [ESPlaylist]");
  if (parsePlaylists(spotify.getPlaylists(), "ESPlaylist", deleteId, sizeof(deleteId)) )
  {
    DebugTf("Going to delete tracks from [%s]\r\n", deleteId);
    do
    {
      totalTracks = parsePlaylistItems(spotify.getTracks("playlists", deleteId, 10, offset), deleteId );
      offset += 10;
      offset += 50;
    } while (offset < totalTracks);

    //if (spotify.deletePlaylistItems(deleteId)==200)
    //      DebugTf("Playlist [%s] is deleted\r\n", deleteId);
    //else  DebugTf("Error deleting playlist [%s]!\r\n", deleteId);
  }

  activeMenu = -1;

} //  selectArtist()


//----------------------------------------------------------------------
void rotateThisFile()
{
  rotateFile(_URI_STORE_FILE, 5);
  activeMenu = 0;

} //  rotateThisFile()


//----------------------------------------------------------------------
void saveStore()
{
  DebugTln("save Store to LittelFS ..");
  saveUriStoreFile(_URI_STORE_FILE);
  DebugTln("After saving Store ..");
  DebugTf("There are [%d] records in uriStore\r\n", inUriStore);

  activeMenu = 0;

} //  saveStore()


//----------------------------------------------------------------------
void selectMusic()
{
  int keuze;

  //-- empty buffer's
  while(Serial.available())
  {
    Serial.read();
  }
  while(TelnetStream.available())
  {
    TelnetStream.read();
  }

  do
  {
    keuze = -1;
    Debugln("\r\n[select Music] -------------------------------------------");
    if (spotifyAccessOK) listUriStore();
    Debugln("  q - Top menu");

    keuze = waitForInput(inUriStore);
    if (keuze == 0)
    {
      activeMenu = 0;
      return;
    }

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) && (keuze > inUriStore) );

  Debugf("\r\n\n[selectMusic] You made your choice [%d]\r\n\n", keuze);
  if (!spotifyAccessOK)
  {
    actMusic = -1;
    activeMenu = 0;
    return;
  }
  actMusic = keuze -1;

  strlcpy(playbackState->nfcTag,      uriStore[actMusic].nfcTag,       sizeof(playbackState->nfcTag) );
  strlcpy(actNfcTag,                  uriStore[actMusic].nfcTag,       sizeof(actNfcTag) );
  strlcpy(playbackState->contextName, uriStore[actMusic].playlistName, sizeof(playbackState->contextName) );
  strlcpy(playbackState->contextUri,  uriStore[actMusic].playlistUri,  sizeof(playbackState->contextUri) );
  spotify.currentUri = String(playbackState->contextUri);
  DebugTf("            nfcTag[%s]\r\n", playbackState->nfcTag);
  DebugTf("       contextName[%s]\r\n", playbackState->contextName);
  DebugTf("        contextUri[%s]\r\n", playbackState->contextUri);
  DebugTf("spotify.currentUri[%s]\r\n", spotify.currentUri.c_str());
  DebugTf("             album[%s]\r\n", uriStore[actMusic].album);
  DebugTln("Before: playSpotifyUri() ..");
  playSpotifyUri();
  spotify.repeat();
  if (strstr(playbackState->contextUri, ":playlist:") > 0)
        systemDevice.deviceShuffle = true;
  else  systemDevice.deviceShuffle = false;
  if (systemDevice.deviceShuffle)
  {
    spotify.shuffle(true);
    digitalWrite(_SHUFFLE_LED, HIGH);
  }
  else
  {
    spotify.shuffle(false);
    digitalWrite(_SHUFFLE_LED, LOW);
  }

  int nowVolume = (int)((int)(playbackState->deviceVolume / 5) * 5);
  playbackState->deviceVolume = nowVolume;
  setVolume(playbackState->deviceVolume);
  //eepromWrite();
  parsePlaybackState(spotify.getPlaybackState(), false);
  DebugTln("after: paresPlaybackState():");
  DebugTf("            nfcTag[%s]\r\n", playbackState->nfcTag);
  DebugTf("       contextName[%s]\r\n", playbackState->contextName);
  DebugTf("        contextUri[%s]\r\n", playbackState->contextUri);
  DebugTf("spotify.currentUri[%s]\r\n", spotify.currentUri.c_str());
  DebugTf("             album[%s]\r\n", uriStore[actMusic].album);

  showNowPlaying(true);

} //  selectMusic()


//----------------------------------------------------------------------
void selectDevice()
{
  int keuze;

  if (!spotifyAccessOK)
  {
    activeMenu = 0;
    return;
  }

  if (!parseDevices(spotify.getDevices()) )
  {
    DebugTln("No devices found!");
    //--- set error led
  }

  //-- empty buffer's
  while(Serial.available())
  {
    Serial.read();
  }
  while(TelnetStream.available())
  {
    TelnetStream.read();
  }

  do
  {
    keuze = -1;
    Debugln("\r\n[selectDevice] Menu--------------------");
    for(int i=0; i<numDevices; i++)
    {
      Debugf("%3d - %-30.30s\r\n", (i+1), aDevices[i].deviceName);
    }
    Debugln("  q - Top menu");

    keuze = waitForInput(numDevices);
    if (keuze == 0)
    {
      activeMenu = 0;
      return;
    }

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) && (keuze > numDevices) );

  Debugf("\r\n\n[selectDevice] You made your choice [%d]\r\n\n", keuze);
  activeMenu = 0;

  actDeviceNum = keuze -1;

  //-----------------------------------------------------------------------------
  //-- putting the next 12 lines of code in a wrapper function crashes the ESP32
  //-----------------------------------------------------------------------------

  strlcpy(systemDevice.deviceName, aDevices[actDeviceNum].deviceName, sizeof(systemDevice.deviceName));
  actDeviceNum  = searchPlayerByName(systemDevice.deviceName);
  strlcpy(systemDevice.deviceId, aDevices[actDeviceNum].deviceId, sizeof(systemDevice.deviceId) );
  DebugTf("new systemDevice[%s] Id[%s]\r\n", systemDevice.deviceName, systemDevice.deviceId);
  systemDevice.deviceState = true;
  saveDeviceFile(_DEVICE_FILE);
  //printFile(_DEVICE_FILE);

  DebugTf("Change player from [%s] ", spotify.deviceName);
  strlcpy(spotify.deviceName, systemDevice.deviceName, sizeof(spotify.deviceName));
  Debugf("to [%s]\r\n", spotify.deviceName);
  setSystemDevice();
  parsePlaybackState(spotify.getPlaybackState(), true);
  updateGuiDevices();

  DebugTln("Done!");
  DebugFlush();

} //  selectDevice()


//----------------------------------------------------------------------
void playerControl()
{
  int keuze;
  int volume;

  //-- empty buffer's
  while(Serial.available())
  {
    Serial.read();
  }
  while(TelnetStream.available())
  {
    TelnetStream.read();
  }

  do
  {
    Debugln("\r\n[Player Control]--------------------");
    Debugln("  1 - next Track");
    Debugln("  2 - previous Track");
    Debugln("  3 - Pause");
    Debugln("  4 - Play");
    Debugln("  5 - Shuffel - on");
    Debugln("  6 - Shuffel - off");
    Debugln("  7 - volume Down");
    Debugln("  8 - volume Up");
    Debugln("  q - Quit menu");
    keuze = waitForInput(8);
    if (keuze == 0)
    {
      activeMenu = 0;
      parsePlaybackState(spotify.getPlaybackState(), false);
      return;
    }

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) && (keuze > 8) );

  if (!spotifyAccessOK)
  {
    activeMenu = 0;
    return;
  }

  switch(keuze)
  {
    case 1:
      Debugln("Next Track");
      nextTrack();
      break;
    case 2:
      Debugln("Previous Track");
      previousTrack();
      break;
    case 3:
      Debugln("Pause Player");
      spotify.pause();
      parsePlaybackState(spotify.getPlaybackState(), true);
      showNowPlaying(true);
      break;
    case 4:
      Debugln("Play Player");
      //spotify.setDeviceId(aDevices[actDeviceNum].deviceId);
      spotify.currentUri = String(uriStore[actMusic].playlistUri);
      spotify.play(playbackState->contextUri, playbackState->itemUri, playbackState->progress);
      parsePlaybackState(spotify.getPlaybackState(), true);
      break;
    case 5:
      Debugln("Shuffle on");
      //spotify.setDeviceId(aDevices[actDeviceNum].deviceId);
      setShuffleMode(true);
      //aDevices[actDeviceNum].deviceShuffle = true;
      //digitalWrite(_SHUFFLE_LED, HIGH);
      //showNowPlaying(true);
      parsePlaybackState(spotify.getPlaybackState(), true);
      break;
    case 6:
      Debugln("Shuffle off");
      //spotify.setDeviceId(aDevices[actDeviceNum].deviceId);
      setShuffleMode(false);
      //aDevices[actDeviceNum].deviceShuffle = false;
      //digitalWrite(_SHUFFLE_LED, LOW);
      //showNowPlaying(true);
      parsePlaybackState(spotify.getPlaybackState(), true);
      break;
    case 7:
    {
      setVolume((playbackState->deviceVolume -5));
      Debugf("Volume Down [%d%%]\r\n", playbackState->deviceVolume);
    }
    break;
    case 8:
    {
      setVolume(playbackState->deviceVolume +5);
      Debugf("Volume UP [%d%%]\r\n", playbackState->deviceVolume);
    }
    break;
  } // keuze

} //  playerControl()


//----------------------------------------------------------------------
void selectSystem()
{
  int keuze;
  int volume;

  //-- empty buffer's
  while(Serial.available())
  {
    Serial.read();
  }
  while(TelnetStream.available())
  {
    TelnetStream.read();
  }

  do
  {
    Debugln("\r\n[System] -----------------------------------");
    Debugln("  1 - reboot System");
    Debugf( "  2 - rotate [%s]\r\n", _URI_STORE_FILE);
    Debugln("  3 - save Store");
    Debugln("  4 - Test");
    Debugln("  q - Quit menu");
    keuze = waitForInput(4);
    if (keuze == 0)
    {
      activeMenu = 0;
      return;
    }

    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) && (keuze > 4) );

  switch(keuze)
  {
    case 1:
      Debugln("Reboot ESPotify in 3 seconds");
      delay(3000);
      ESP.restart();
      delay(5000);
      break;
    case 2:
      rotateThisFile();
      break;
    case 3:
      saveStore();
      break;
    case 4:
      Serial.println(spotify.getPlaybackState());
      break;
  } // keuze

} //  selectSystem()


//------------------------------------------------------------
int waitForInput(int maxIn)
{
  char  cKeuze[5] = {};
  int   keuze;
  int   len;
  do
  {
    keuze = -1;
    memset(cKeuze, 0, sizeof(cKeuze));

    Debug("\r\nEnter choice : ");

    switch(activeStream)
    {
      case 1:
        Serial.setTimeout(20000);
        len = Serial.readBytesUntil('\n', cKeuze, sizeof(cKeuze));
        break;
      case 2:
        TelnetStream.setTimeout(20000);
        len = TelnetStream.readBytesUntil('\r', cKeuze, sizeof(cKeuze));
        break;
    }

    //-- empty buffer's
    while(Serial.available())
    {
      Serial.read();
    }
    while(TelnetStream.available())
    {
      TelnetStream.read();
    }

    if (len <= 0)
    {
      Debugln("\r\nTimeout! Exit Menu\r\n");
      return 0;
    }
    if (activeStream == 1) Serial.println(cKeuze);
    else                   TelnetStream.println();
    if (strcasecmp(cKeuze, "q") == 0)
    {
      Debugln("\r\nExit Menu\r\n");
      return 0;
    }
    keuze = atoi(cKeuze);
    
    httpServer.handleClient();
    webSocket.loop();
    pulseWhiteLeds();

  } while( (keuze < 0) || (keuze > maxIn) );

  return keuze;

} //  waitForInput()

/*eof*/
