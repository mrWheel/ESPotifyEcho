/*
***************************************************************************  
**  Program  : index.js, part of FSmanager template
**  Version  : v2.0.0   (17-02-2021)
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
  const APIGW='http://'+window.location.host+'/api/';

"use strict";

  let webSocketConn;
  let needReload  = true;
  let buttonsActive = true;
  let cPensil     = "&#9998;";
  let cDiskette   = "&#128190;";
  let cCatalog    = "&#128452;";    // &#x1F5C4;
  let cReturn     = "&#9166;";      // &#x23CE;
  let cFRev       = "&#9194;";
  let cFFrwd      = "&#9193;";
  let cNextFree   = "&#9197;";
  let cRecord     = "&#9210;";
  let cPlay       = "&#128316;";
  let nameEdit    = false;
  let descEdit    = false;
  let lockEdit    = false;
  let gID         = 0;
  let gLock       = false;
  let actDESC     = false;
  let actPTP      = false;
  let actASM      = false;
  let readCatalog = false;
    
  
  window.onload=bootsTrapMain;
  window.onfocus = function() {
    if (needReload) {
      window.location.reload(true);
    }
  };

      
  //============================================================================  
  function bootsTrapMain() 
  {
    console.log("bootsTrapMain()");
    needReload = true;

    let count = 0;
    while (document.getElementById('devVersion').value == "[version]") {
      count++;
      console.log("count ["+count+"] devVersion is ["+document.getElementById('devVersion').value+"]");
      if (count > 10) {
        alert("Count="+count+" => reload from server!");
        window.location.reload(true);
      }
      setTimeout("", 500);
    }

    document.getElementById('M_FSmanager').addEventListener('click',function() 
                                                { console.log("newTab: goFSmanager");
                                                  location.href = "/FSmanager?sort=sort";
                                                });

    needReload = false;
    /*
    document.getElementById("displayMainPage").style.display       = "block";
    */
  
  } // bootsTrapMain()

 /**
  //============================================================================  
  function handleRadioChoice(radioChoice) 
  {
    console.log("handleRadioChoice("+radioChoice+")");

    if (!buttonsActive) return;
    
    console.log("send["+radioChoice+"]");
    webSocketConn.send(radioChoice);
    var show = document.getElementsByName('show'); 
              
    if (show[0].checked) actDESC = true
    else                 actDESC = false;
    if (show[1].checked) actPTP  = true
    else                 actPTP  = false;
    if (show[2].checked) actASM  = true
    else                 actASM  = false;
    
  } // handleRadioChoice()
  **/
  
  //============================================================================  
  String.prototype.replaceAll = function(str1, str2, ignore) 
  {
    return this.replace(new RegExp(str1.replace(/([\/\,\!\\\^\$\{\}\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignore?"gi":"g")),(typeof(str2)=="string")?str2.replace(/\$/g,"$$$$"):str2);
  //return this.replace(new RegExp(str1.replace(/([\/\,\!\\\^\$\[\]\(\)\.\*\+\?\|\<\>\-\&])/g,"\\$&"),(ignore?"gi":"g")),(typeof(str2)=="string")?str2.replace(/\$/g,"$$$$"):str2);
  }
   
/*
***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
***************************************************************************
*/
