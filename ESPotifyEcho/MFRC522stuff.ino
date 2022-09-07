

//------------------------------------------------------------
//-- no card present => -1 -> MFRC522_NO_TAG
//-- same card       =>  0 -> MFRC522_SAME_TAG
//-- new card        =>  1 -> MFRC522_NEW_TAG
int readNfcTag()
{
  static char prevNfcTag[21] = {0};
  static char tmpNfcTag[21]  = {0};
  static int  noTagFoundCounter = 0;

  if (strncmp(tmpNfcTag, _NO_NFC_TAG, 20) == 0)
  {
    mfrc522.PCD_Reset();
    //-- Init MFRC522
    mfrc522.PCD_Init();   
    delay(100);  
  }
  if ( !mfrc522.PICC_IsNewCardPresent()) 
  {
    //-- sort of "debounce"
    if (noTagFoundCounter < 2)
    {
      //if (noTagFoundCounter >= 1) DebugTf("debounce [%d]\r\n", noTagFoundCounter); 
      noTagFoundCounter++;
      delay(100);
      return MFRC522_NO_ACTION;
    }
    noTagFoundCounter = 0;

    for(int u=0; u<10; u++)
    {
      mfrc522.uid.uidByte[u] = 0;
    }
    snprintf(tmpNfcTag,  sizeof(tmpNfcTag),  _NO_NFC_TAG); 
    snprintf(prevNfcTag, sizeof(prevNfcTag), _NO_NFC_TAG); 
    snprintf(actNfcTag,  sizeof(actNfcTag),  _NO_NFC_TAG); 
    nfcTagPresent = false;
    return MFRC522_NO_TAG;
  }
  
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    Serial.println("Error reading Card!");
    //return;
  }
  noTagFoundCounter = 0;
  snprintf(actNfcTag, sizeof(actNfcTag) 
                                        , "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x"
                                          , mfrc522.uid.uidByte[0]
                                          , mfrc522.uid.uidByte[1]
                                          , mfrc522.uid.uidByte[2]
                                          , mfrc522.uid.uidByte[3]
                                          , mfrc522.uid.uidByte[4]
                                          , mfrc522.uid.uidByte[5]
                                          , mfrc522.uid.uidByte[6]
                                          , mfrc522.uid.uidByte[7]
                                          , mfrc522.uid.uidByte[8]
                                          , mfrc522.uid.uidByte[9]);

  //-- is it the same TAG??
  if (strncasecmp(prevNfcTag, actNfcTag, sizeof(actNfcTag)) == 0)
  {
    //Serial.printf("SAME NFC Tag[%s]\r\n", actNfcTag);
    nfcTagPresent = true;
    return MFRC522_SAME_TAG;
  }
  //-- it's a new TAG!!
  strlcpy(prevNfcTag, actNfcTag, sizeof(prevNfcTag));
  nfcTagPresent = true;
  return MFRC522_NEW_TAG;
  
} // readNfcTag()


//------------------------------------------------------------
void handleNoTag()
{
  spotify.pause();

} //  handleNoTag()


//------------------------------------------------------------
void handleSameTag()
{
  analogWrite(_WHITE_LED, 40);
  //DebugTln("Same tag ..");

} //  handleSameTag()


//------------------------------------------------------------
void handleNewTag()
{
  //spotify.pause();
  actMusic=searchPlaylistByNfcTag(actNfcTag);
  DebugTf("searchePlaylist by [%s] found [%d]\r\n", actNfcTag, actMusic);
  spotify.currentUri = String(uriStore[actMusic].playlistUri);
  Debugf("context[ %s], contextUri[%s]\r\n", uriStore[actMusic].playlistName, spotify.currentUri.c_str());
  playSpotifyUri();

  parsePlaybackState(spotify.getPlaybackState(), true);

  DebugTf("NEW -> contextName [%s]\r\n", playbackState->contextName);
  DebugTf("    ---> albumName [%s]\r\n", playbackState->albumName);

  if (strstr(playbackState->contextUri, ":playlist:") > 0)
        setShuffleMode(true);
  else  setShuffleMode(false);
  
  strlcpy(playbackState->contextUri, uriStore[actMusic].playlistUri, sizeof(playbackState->contextUri) );
  setSystemDevice();
  setVolume(systemDevice.deviceVolume);
  parsePlaybackState(spotify.getPlaybackState(), false);
  eepromWrite();
  analogWrite(_WHITE_LED, 50);

} //  handleNewTag()


//------------------------------------------------------------
void hexDump(byte *dataBuffer)
{
  DebugTln("hexDump: ");
  for (byte index = 0; index < BUFFERSiZE; index++)
  {
    if (index % 4 == 0)
    {
      Debugln();
    }
    Debug(dataBuffer[index] < 0x10 ? F(" 0") : F(" "));
    Debug(dataBuffer[index], HEX);
  }
  
} //  hexDump()

/*eof*/
