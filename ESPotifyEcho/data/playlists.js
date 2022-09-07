  /**************************** 
  **    playlists.js 
  *****************************/
  
  var gateway = "ws://"+location.host+":81/playlists";
  var websocket;
  window.addEventListener('load', onLoad);
  
  var aLinkedPlaylist = [];
  var aLinkedNfcTag   = [];
  var aLinkedIndx     = 0;
  var aNewPlaylist    = [];
  var aNewNfcTag      = [];
  var aNewIndx        = 0;
  var newRFID         = "";

       
  //----------------------------------------------------------------------------
  function initWebSocket() 
  {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  
  } //  initWebSocket()
  
  //----------------------------------------------------------------------------
  function onOpen(event) 
  {
    console.log('Connection opened');
    reload();
    //reloadTimer = setInterval(reload, 5000); // Time in milliseconds
  
  } //  onOpen()
  
  //----------------------------------------------------------------------------
  function onClose(event) 
  {
    console.log('Connection closed');
    clearInterval(reloadTimer);
    setTimeout(initWebSocket, 2000);
  
  } //  onClose()
  
  //----------------------------------------------------------------------------
  function onMessage(event) 
  {
    console.log("onMessage: --> event.data ["+JSON.stringify(event.data)+"]");
    const obj = JSON.parse(event.data);
    if (obj.type == "Linked")
    {
      console.log("ipAddress["+obj["ipAddress"]+"]");
      document.getElementById('ipAddress').innerHTML = "["+obj["ipAddress"]+"]";
      const linked = obj.linked;
      populateSelector("linkedList", linked);
      document.getElementById("nfcTagClock").style.display = "none";
      document.getElementById("nfcTag").style.display      = "block";
      for(var i = 0; i < linked.length; i++) 
      {
        aLinkedPlaylist[i]  = linked[i].playlistName;
        aLinkedNfcTag[i]    = linked[i].nfcTag;
      }
      linkedChanged(0);

    }
    if (obj.type == "notLinked")
    {
      console.log("ipAddress["+obj["ipAddress"]+"]");
      document.getElementById('ipAddress').innerHTML = "["+obj["ipAddress"]+"]";
      const notLinked = obj.notLinked;
      populateSelector("notLinkedList", notLinked);
      document.getElementById("newNfcTagClock").style.display = "none";
      document.getElementById("newNfcTag").style.display      = "block";

      for(var i = 0; i < notLinked.length; i++) 
      {
        aNewPlaylist[i]  = notLinked[i].playlistName;
        aNewNfcTag[i]    = notLinked[i].nfcTag;
      }
      notLinkedChanged(0);
    }
    if (obj.type == "newRFID")
    {
      newRFID = obj["newRFID"];
      document.getElementById('newNfcTag').innerHTML = newRFID;
    }

  } //  onMessage()
  
  //----------------------------------------------------------------------------
  function onLoad(event) 
  {
    document.getElementById("nfcTagClock").src    = "/timerOn.png";
    document.getElementById("newNfcTagClock").src = "/timerOn.png";

    initWebSocket();
    initButton();

  } //  onLoad()
  
  //----------------------------------------------------------------------------
  function initButton() 
  {
    document.getElementById('reload').addEventListener('click', reload);
    document.getElementById('readNfcTag').addEventListener('click', readNfcTag);
    document.getElementById('newLink').addEventListener('click', newLink);
    document.getElementById('unLink').addEventListener('click', unLink);
    document.getElementById('save').addEventListener('click', save);
    document.getElementById('M_goHome').addEventListener('click', goHome);
    
  } //  initButton()
    
  //----------------------------------------------------------------------------
  function populateSelector(elementId, jsonArray)
  {
    var select = document.getElementById(elementId);

    //-- clear dropdownlist --
    //document.getElementById(elementId).options.length = 0;
    select.options.length = 0;
    
    for(var i = 0; i < jsonArray.length; i++) 
    {
      var element = jsonArray[i];
      //console.log("["+i+"] name["+device.name+"] select["+device.active+"]");
      var el = document.createElement("option");
      el.text = element.playlistName;
      el.value = i;
      if (element.active)
      {
        el.setAttribute("selected",true);
      }
      select.add(el);
    }
    
  } //  populateSelector()

    
  //----------------------------------------------------------------------------
  function linkedChanged(newIndx) 
  {
    aLinkedIndx = newIndx;
    console.log("linkedChanged to ["+aLinkedIndx+"] -> ["+ aLinkedNfcTag[aLinkedIndx]+"]");
    document.getElementById('nfcTag').innerHTML = aLinkedNfcTag[aLinkedIndx];
    //newDevice = newValue;
    
  } //  linkedChanged()
    
  //----------------------------------------------------------------------------
  function notLinkedChanged(newIndx) 
  {
    aNewIndx = newIndx;
    console.log("notLinkedChanged to ["+aNewIndx+"] -> ["+ aNewNfcTag[aNewIndx]+"]");
    document.getElementById('newNfcTag').innerHTML = aNewNfcTag[aNewIndx];
    //newDevice = newValue;
    
  } //  notLinkedChanged()

  //----------------------------------------------------------------------------
  function unLink()
  {
    console.log("Unlink ["+aLinkedIndx+"] => ["+aLinkedNfcTag[aLinkedIndx]+"]");
    console.log("Unlink ["+aLinkedPlaylist[aLinkedIndx]+"]");
    var jsonSource = {};
    var jsonData = {};
    jsonSource["source"]      = "playlistsGUI";
    jsonSource["command"]     = "unlink";
    jsonData["nfcTag"]        = aLinkedNfcTag[aLinkedIndx];
    jsonData["playlistName"]  = aLinkedPlaylist[aLinkedIndx];
    //console.log("send("+JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData)+")");
    websocket.send(JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData));
    
  } //  unLink()

  //----------------------------------------------------------------------------
  function readNfcTag()
  {
    console.log("Read RFID");
    var jsonSource = {};
    var jsonData = {};
    jsonSource["source"]      = "playlistsGUI";
    jsonSource["command"]     = "readNfcTag";
    //console.log("send("+JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData)+")");
    websocket.send(JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData));
    
  } //  readNfcTag()

  //----------------------------------------------------------------------------
  function newLink()
  {
    console.log("new link ["+aNewIndx+"] => ["+newRFID+"]");
    console.log("new link ["+aNewPlaylist[aNewIndx]+"]");
    var jsonSource = {};
    var jsonData = {};
    jsonSource["source"]      = "playlistsGUI";
    jsonSource["command"]     = "newLink";
    jsonData["nfcTag"]        = newRFID;
    jsonData["playlistName"]  = aNewPlaylist[aNewIndx];
    console.log("send(###"+JSON.stringify(jsonData)+")");
    websocket.send(JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData));
    
  } //  newLink()
  
  //----------------------------------------------------------------------------
  function reload()
  {
    var jsonSource = {};
    var jsonData = {};
    jsonSource["source"]  = "playlistsGUI";
    jsonSource["command"] = "reload";
    //console.log("send("+JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData)+")");
    websocket.send(JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData));
    
  } //  reload()
  
  //----------------------------------------------------------------------------
  function save()
  {
    var jsonSource = {};
    var jsonData = {};
    jsonSource["source"]  = "playlistsGUI";
    jsonSource["command"] = "save";
    //console.log("send("+JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData)+")");
    websocket.send(JSON.stringify(jsonSource)+"###"+JSON.stringify(jsonData));
    
  } //  save()
    
  //----------------------------------------------------------------------------
  function goHome()
  {
    console.log("start Back");
    window.location.replace("/");
    
  } //  startSettings()
  
  //----------------------------------------------------------------------------
  function showClock(elemId) 
  {
    var theimage = document.createElement("img");
    theimage.setAttribute('src', "/turningClock.gif");
    theimage.setAttribute('alt', 'image');
    theimage.height = 50;
    theimage.width  = 50;
    document.getElementById(elemId).innerHTML = theimage;  
    
  } //  showClock()
  
