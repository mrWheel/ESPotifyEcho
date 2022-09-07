/*
***************************************************************************
**  Program  : Debug.h, part of ESPotifyController
**  Version  : v2.0.1
**
**  Copyright (c) 2020 .. 2022 Willem Aandewiel
**  Met dank aan Erik
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/
#ifndef _DEBUG_H
#define _DEBUG_H

/*---- start macro's ------------------------------------------------------------------*/
#ifdef _DEBUG
  #define Debug(...)      ({ Serial.print(__VA_ARGS__);         \
                             TelnetStream.print(__VA_ARGS__);   \
                          })
  #define Debugln(...)    ({ Serial.println(__VA_ARGS__);       \
                             TelnetStream.println(__VA_ARGS__);   \
                          })
  #define Debugf(...)     ({ Serial.printf(__VA_ARGS__);        \
                             TelnetStream.printf(__VA_ARGS__);   \
                          })
#else
  #define Debug(...)      ({;})
  #define Debugln(...)    ({;})
  #define Debugf(...)     ({;})
#endif

#define DebugFlush()      ({ Serial.flush(); })


#ifdef _DEBUG
  #define DebugT(...)     ({ _debugBOL(__FUNCTION__, __LINE__);  \
                             Debug(__VA_ARGS__);                 \
                          })
  #define DebugTln(...)   ({ _debugBOL(__FUNCTION__, __LINE__);  \
                             Debugln(__VA_ARGS__);               \
                          })
  #define DebugTf(...)    ({ _debugBOL(__FUNCTION__, __LINE__);  \
                             Debugf(__VA_ARGS__);                \
                        })
#else
  #define DebugT(...)      ({;})
  #define DebugTln(...)    ({;})
  #define DebugTf(...)     ({;})
#endif
/*---- einde macro's ------------------------------------------------------------------*/

char _bol[128];
void _debugBOL(const char *fn, int line)
{
/*
  snprintf(_bol, sizeof(_bol), "[%02d:%02d:%02d][%7u|%6u] %-12.12s(%4d): ", \
           hour(), minute(), second(), \
           ESP.getFreeHeap(), ESP.getMaxFreeBlockSize(), \
           fn, line);
*/
  snprintf(_bol, sizeof(_bol), "[%6d] %-12.12s(%4d): ", \
                            ESP.getFreeHeap(), fn, line);

  Serial.print(_bol);
}

#endif
