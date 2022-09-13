

//------------------------------------------------------------
bool showNowPlaying(bool force)
{
  //if (force || ((millis() +playbackState->progress) > playbackState->checkProgress) )
  if (force || (millis() >= playbackState->checkProgress) )
  {
    if (parsePlaybackState(spotify.getPlaybackState(), false) )
    {
      DebugTf( "\r\n\n      Player: [%-22.22s] %-7.7s  Volume[%6d%%]\r\n"
               , playbackState->deviceName
               , playbackState->shuffle ? "shuffle":" "
               , playbackState->deviceVolume);
      Debugf("context Name: [%s]\r\n", playbackState->contextName);
      Debugf(" context URI: [%s]\r\n", playbackState->contextUri);
      Debugf("      Artist: [%s]\r\n", playbackState->artistName);
      Debugf("       Album: [%s]\r\n", playbackState->albumName);
      Debugf("     playing: [%-30.30s] [%4ds of %4ds]\r\n"
             , playbackState->itemName
             , (playbackState->progress/1000)
             , (playbackState->itemDuration/1000));
      Debugf("   track URI: [%s]\r\n", playbackState->itemUri);
      Debugln();

      updateGuiPlayingInfo();

    }
    else
    {
      DebugTf( "\r\n     Player: [%s] Volume[%d%%]\r\n", systemDevice.deviceName
               , systemDevice.deviceVolume);
      Debugf("NOT playing: [%s] ?!?!\r\n", playbackState->contextName);
      Debugf("           : [%s] ?!?!\r\n", playbackState->contextUri);
      return false;
    }
  } //  force or time ..

  return true;

} //  showNowPlaying()


//------------------------------------------------------------
void addUriToStore(const char *artist, const char *album, const char *uri)
{
  DebugTf("adding [%s] @%d\r\n", artist, inUriStore);
  snprintf(uriStore[inUriStore].nfcTag, sizeof(uriStore[inUriStore].nfcTag), "-");
  snprintf(uriStore[inUriStore].artist, sizeof(uriStore[inUriStore].artist), "%s", artist);
  snprintf(uriStore[inUriStore].album,  sizeof(uriStore[inUriStore].album),  "%s", album);
  snprintf(uriStore[inUriStore].playlistUri,    sizeof(uriStore[inUriStore].playlistUri),    "%s", uri);
  uriStore[inUriStore].tracks = 1;
  inUriStore++;
  //DebugTf("now in store [%d]\r\n", inUriStore);

} //  addUriToStore()


//------------------------------------------------------------
int populateUriStore()
{
  int saveInStore;

  DebugTln("LOAD=========================================");
  inUriStore  = loadUriStore(_URI_STORE_FILE);
  saveInStore = inUriStore;
  DebugTf("There are [%d] records in uriStore\r\n", inUriStore);

  qsort(uriStore, inUriStore, sizeof(uriStore[0]), sortByPlaylist);

  //listUriStore();

  //-- now get playlists from spotify that are not already in Store
  inUriStore = parseMyPlaylists(spotify.getPlaylists(""));

  DebugTf("Added [%d] records, total [%d] records in uriStore\r\n", (inUriStore-saveInStore)
          ,  inUriStore);

  //listUriStore();

  return inUriStore;

} //  polpulateUriStore()


//------------------------------------------------------------
int searchPlaylistByName(const char *pName)
{
  for (int i=0; i<inUriStore; i++)
  {
    if (strlen(uriStore[i].playlistName) == 0)
    {
      return -1;
    }
    //DebugTf("[%d] check [%s]\r\n", i, uriStore[i].playlistName);
    //DebugTf(" ==>  with [%s]", pName);
    if (strncasecmp(uriStore[i].playlistName, pName, strlen(uriStore[i].playlistName)) == 0)
    {
      //Debugln(" => yes!");
      return i;
    }
    //Debugln(" =! no");
  }

  return -1;

} //  searchPlaylistByName()


//------------------------------------------------------------
int searchPlaylistByUri(const char *pName)
{
  for (int i=0; i<inUriStore; i++)
  {
    if (strlen(uriStore[i].playlistUri) == 0)
    {
      return -1;
    }
    //DebugTf("[%d] check [%-30.30s] ", i, uriStore[i].playlistUri);
    //DebugTf(" ==>  with [%s]", pName);
    if (strncasecmp(uriStore[i].playlistUri, pName, strlen(uriStore[i].playlistUri)) == 0)
    {
      //Debugln(" => yes!");
      //DebugTf("\r\nLooking for [%-30.30s] -> found[%-30.30s]\r\n", pName, uriStore[i].playlistUri);
      return i;
    }
    //Debugln(" =! no");
  }

  return -1;

} //  searchPlaylistByUri()


//------------------------------------------------------------
int searchPlaylistByNfcTag(const char *pName)
{
  for (int i=0; i<inUriStore; i++)
  {
    if (strlen(uriStore[i].nfcTag) < 20)  return -1;

    //DebugTf("[%d]check [%s]==[%s]?",i, uriStore[i].nfcTag, pName);
    if (strncasecmp(uriStore[i].nfcTag, pName, strlen(uriStore[i].nfcTag)) == 0)
    {
      //Debugln(" => yes!");
      //DebugTf("Looking for [%s] -> found[%s]\r\n", pName, uriStore[i].nfcTag);
      return i;
    }
    //Debugln(" no");
  }

  return -1;

} //  searchPlaylistByNfcTag()


//------------------------------------------------------------
int searchPlayerByName(const char *pName)
{
  for (int i=0; i<numDevices; i++)
  {
    //DebugTf("check [%s]==[%s]?", aDevices[i].deviceName, pName);
    if (strcasecmp(aDevices[i].deviceName, pName) == 0)
    {
      //Debugf(" => id[%s] yes!\r\n", aDevices[i].deviceId);
      return i;
    }
    //Debugln(" no");
  }

  return -1;

} //  searchPlayerByName()


//--------------------------------------------------------------------
void listUriStore()
{
  for(int i=0; i<inUriStore; i++)
  {
    Debugf("[%3d] - %03d %-30.30s (%4d) [%s]\r\n", (i+1)
           , uriStore[i].recNr
           , uriStore[i].playlistName
           , uriStore[i].tracks
           , uriStore[i].nfcTag);
  }

} //  listUriStore()


//--------------------------------------------------------------------
bool eepromRead()
{
  memset(&eepromStore, 0, sizeof(eepromStore));
  DebugTf("eepromStore [%d]bytes\r\n", sizeof(eepromStore));
  EEPROM.get(0, eepromStore);
  if (eepromStore.goodState == _EEPROM_VERSION)
  {
    DebugTf("[r]   PlaylistName [%s]\r\n", eepromStore.playlistName);
    DebugTf("[r]   PlaylistUri  [%s]\r\n", eepromStore.playlistUri);
    return true;
  }

  return false;

} // eepromRead()


//--------------------------------------------------------------------
bool eepromWrite()
{
  eepromStore.goodState         = _EEPROM_VERSION;
  strlcpy(eepromStore.playlistName, playbackState->contextName, sizeof(eepromStore.playlistName));
  strlcpy(eepromStore.playlistUri,  playbackState->contextUri,  sizeof(eepromStore.playlistUri));
  EEPROM.put(0, eepromStore);
  EEPROM.commit();
  DebugTf("[w]  PlaylistName [%s]\r\n", eepromStore.playlistName);
  DebugTf("[w]  PlaylistUri  [%s]\r\n", eepromStore.playlistUri);

  return true;

} // eepromWrite()


//-------------------------------------------------------
//    str[] - The string to search in.
//    search[] - The substring to search for.
//
//    Returns -1 if substring not found.
int getIndex(const char *str, const char *search, int index, int i, int j)
{
  //-- Substring found.
  if( search[j] == '\0' )
    return index;
  //-- Search string ended but substring not found.
  if( str[i] == '\0' )
    return -1;

  if( str[i] == search[j] )
  {
    //-- Sets index equal to the index of first charector of search in str
    if( j == 0 ) index = i;
    j++;
  }
  else
  {
    // Sets index back to -1 due to missmatched charector.
    index = -1;
    j = 0;
  }
  //-- Continue search.
  return getIndex(str, search, index, ++i, j);

} //  getIndex()


//----------------------------------------------------------
String urlencode(String str)
{
  String encodedString="";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i =0; i < str.length(); i++)
  {
    c=str.charAt(i);
    if (c == ' ')
    {
      encodedString+= '+';
    }
    else if (isalnum(c))
    {
      encodedString+=c;
    }
    else
    {
      code1=(c & 0xf)+'0';
      if ((c & 0xf) >9)
      {
        code1=(c & 0xf) - 10 + 'A';
      }
      c=(c>>4)&0xf;
      code0=c+'0';
      if (c > 9)
      {
        code0=c - 10 + 'A';
      }
      code2='\0';
      encodedString+='%';
      encodedString+=code0;
      encodedString+=code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;

} //  urlencode()


//--------------------------------------------------------------------
void blinkAllLeds(int num)
{
  int led[] = {_SHUFFLE_LED, _GREEN_LED, _B0_LED, _WHITE_LED, _B1_LED, _ERROR_LED };
  int16_t waitTime = 500;
  int numberOfLeds = sizeof(led) / sizeof(led[0]);

  DebugTf("Start blinking (%d leds)\r\n", numberOfLeds);
  //-- switch off all the leds
  for(int l=0; l<numberOfLeds; l++)
  {
    digitalWrite(led[l],   LOW);
  }

  //-- loop number of blinks
  for (int b=0; b<num; b++)
  {
    if (waitTime < 50) waitTime = 50;
    Serial.printf("[%d] (%dms)\r\n", b, waitTime);
    for(int ledOn=0; ledOn<numberOfLeds; ledOn++)
    {
      for(int l=0; l<numberOfLeds; l++)
      {
        if (l==ledOn) digitalWrite(led[l], HIGH);
        else          digitalWrite(led[l], LOW);
      }
      delay(waitTime);
    }
    waitTime -= 100;
  }
  //-- switch off all the leds again
  for(int l=0; l<numberOfLeds; l++)  digitalWrite(led[l],   LOW);
  digitalWrite(_GREEN_LED, HIGH);
  Debugln();

} //  blinkAllLeds()


//--------------------------------------------------------------------
void pulseWhiteLeds()
{
  static int    brightness = 0;
  static int8_t changeStep = 4;

  if (DUE(whiteLedsTimer))
  {
    if (nfcTagPresent) return;
    brightness += changeStep;
    if (brightness > 90)  changeStep = -4;
    if (brightness < 10)  changeStep = +4;
    analogWrite(_WHITE_LED, brightness);
  }

} //  pulseWhiteLeds()
