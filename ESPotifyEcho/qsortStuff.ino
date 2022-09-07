

//---------------------------------------------------------
// qsort requires you to create a sort function
int sortByPlaylist(const void *cmp1, const void *cmp2)
{
  struct uriStoreStruct *ia = (struct uriStoreStruct *)cmp1;
  struct uriStoreStruct *ib = (struct uriStoreStruct *)cmp2;
  //Serial.printf("a[%-15.15s] b[%-15.15s]\r\n", ia->artist, ib->artist);
  return strcmp(ia->playlistName, ib->playlistName);

} //  sortByPlaylist()


//---------------------------------------------------------
// qsort requires you to create a sort function
int sortByDeviceName(const void *cmp1, const void *cmp2)
{
  struct deviceStruct *ia = (struct deviceStruct *)cmp1;
  struct deviceStruct *ib = (struct deviceStruct *)cmp2;
  //Serial.printf("a[%-15.15s] b[%-15.15s]\r\n", ia->deviceName, ib->deviceName);
  return strcmp(ia->deviceName, ib->deviceName);

} //  sortDeviceName()
