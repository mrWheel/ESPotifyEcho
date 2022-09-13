

#define _PROX_BUTTONS               2
#define _PROX_SHORT_RELEASE       500
#define _PROX_VERY_LONG_RELEASE 15000  // restart
#define _PROX_DEBOUNCE             50

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t        lasttouched = 0;
uint16_t        currtouched = 0;
static uint32_t proxPressStart[12];
static uint8_t  proxState[12];


//-----------------------------------------------------------------
bool mpr121Init()
{
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A))
  {
    //Debugln("MPR121 not found, check wiring?");
    return false;
  }
  //Debugln("MPR121 found!");
  //cap.setThresholds(7, 2);  // till 14-08 touch, release
  cap.setThresholds(8, 2);  // touch, release

  for (uint8_t i=0; i<11; i++)
  {
    proxState[i] = PS_INIT;
  }

  return true;

} //  mpr121Init()


//-----------------------------------------------------------------
void mpr121UpdateStates()
{
  // Get the currently touched pads
  currtouched = cap.touched();
  uint32_t  wait4Debounce;

  for (uint8_t i=_CAP_SW_UP; i<(_CAP_SW_UP+_PROX_BUTTONS); i++)
  {
    //----- is it realy a state change?? --------------------
    //-- it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) )
    {
      proxPressStart[i] = millis();
      //Serial.printf("[%d]  touched ", i);
      proxState[i] = PS_PRESSED;
      if (i==_CAP_SW_UP)    digitalWrite(_B0_LED, HIGH);
      if (i==_CAP_SW_DOWN)  digitalWrite(_B1_LED, HIGH);
    } // state changed
  }

  for (uint8_t i=_CAP_SW_UP; i<(_CAP_SW_UP+_PROX_BUTTONS); i++)
  {
    if ((proxState[i] == PS_PRESSED) && (millis() -proxPressStart[i]) > _PROX_SHORT_RELEASE)
    {
      if ((millis() -proxPressStart[i]) > _PROX_DEBOUNCE) proxState[i] = PS_LONG_PRESS;
    }

    //----- state changed from "on" to "off" ---------------
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) )
    {
      if (i==_CAP_SW_UP)    digitalWrite(_B0_LED, LOW);
      if (i==_CAP_SW_DOWN)  digitalWrite(_B1_LED, LOW);
      DebugTf("[%d] Released after [%d]ms\r\n", i, (millis()-proxPressStart[i]));
      if ( (proxState[i] == PS_PRESSED) && ((millis() -proxPressStart[i]) > _PROX_DEBOUNCE) )
            proxState[i] = PS_SHORT_RELEASE;
      else  proxState[i] = PS_INIT;
    }
  } // for ..
  // reset our state
  lasttouched = currtouched;

} //  mpr121UpdateStates()


//----------------------------------------------------------------
uint8_t mpr121GetState(int button)
{
  uint8_t xs = proxState[button];

  if (proxState[button] == PS_SHORT_RELEASE) proxState[button] = PS_INIT;

  return xs;

} //  mpr121Pressed()


//-----------------------------------------------------------------
void mpr121Handle()
{
  if ( (proxState[_CAP_SW_DOWN] == PS_LONG_PRESS) && (proxState[_CAP_SW_UP] == PS_LONG_PRESS) )
  {
    do
    {
      mpr121UpdateStates();  // check if one is released
    } while( (proxState[_CAP_SW_DOWN] == PS_LONG_PRESS) || (proxState[_CAP_SW_UP] == PS_LONG_PRESS) );
    do
    {
      mpr121UpdateStates();  // now wait till both are released
    } while( (proxState[_CAP_SW_DOWN] == PS_LONG_PRESS) && (proxState[_CAP_SW_UP] == PS_LONG_PRESS) );
    //if ( (proxState[0] == PS_VERY_LONG_PRESS) && (proxState[1] == PS_VERY_LONG_PRESS) )
    if (   ((millis() -proxPressStart[_CAP_SW_UP])    >= _PROX_VERY_LONG_RELEASE )
           && ((millis() -proxPressStart[_CAP_SW_DOWN])  >= _PROX_VERY_LONG_RELEASE )
       )
    {
      DebugTln("Restart ESPotifyEcho ...");
      ESP.restart();
      delay(2000);
    }
    proxState[_CAP_SW_UP]   = PS_INIT;
    proxState[_CAP_SW_DOWN] = PS_INIT;
    DebugTln("Toggle Shuffel mode!");
    setShuffleMode(!playbackState->shuffle);
  }
  else
  {
    if (proxState[_CAP_SW_DOWN] == PS_SHORT_RELEASE)
    {
      proxState[_CAP_SW_DOWN] = PS_INIT;
      //DebugTln("next Track!");
      nextTrack();
    }
    else if (proxState[_CAP_SW_DOWN] == PS_LONG_PRESS)
    {
      setVolume(playbackState->deviceVolume +5);
      Debugf("Volume UP [%d%%]\r\n", playbackState->deviceVolume);
    }
    if (proxState[_CAP_SW_UP] == PS_SHORT_RELEASE)
    {
      proxState[_CAP_SW_UP] = PS_INIT;
      //DebugTln("previous Track!");
      previousTrack();
    }
    else if (proxState[_CAP_SW_UP] == PS_LONG_PRESS)
    {
      setVolume(playbackState->deviceVolume -5);
      Debugf("Volume DOWN [%d%%]\r\n", playbackState->deviceVolume);
    }
  }

} //  mpr121Handle()

/*eof*/
