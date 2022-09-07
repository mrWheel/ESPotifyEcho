/*
 * DECLARE_TIMER(timername, interval)
 *  Declares two unsigned longs: 
 *    <timername>_last for last execution
 *    <timername>_interval for interval in milliseconds
 *  with random noise to avoid aligned timers
 *  same macro with _ postfix doesnt add noise
 *    
 * DECLARE_TIMERms is same as DECLARE_TIMER **but** uses milliseconds!
 *
 * ELAPSED(msStart, msEnd) returns ms between times, even after rollover at MAXMS (2^32-1) milliseconds
 * 
 * SINCE(timername) is elapsed ms since last time due-time
 *    
 * DUE(timername) 
 *  returns false (0) if interval hasn't elapsed since last DUE-time
 *          true (current millis) if it has
 *  new interval starts NOW
 *  !! updates <timername>_last !!
 *  
 * DUE_(timername)
 *   as DUE but tries to align new interval at interval beat
 *   in other words, new interval doesn't start now, but at previous timer expiration
 * 
 * RESET_TIMER(timername)
 *   updates <timername>_interval = millis() + <timername>_interval
 *   
 * to expedite testing, declare timers as __TIMER(....) 
 * without SPEEDUP defined BEFORE including timers.h __TIMER is in minutes!
 * with    SPEEDUP defined BEFORE including timers.h __TIMER is in seconds!
 * 
 *  Usage example:
 *  
 *  DECLARE_TIMER(screenUpdate, 20) // update screen every 20s
 *  ...
 *  loop()
 *  {
 *  ..
 *    if ( DUE(screenUpdate) ) {
 *      // update screen
 *    }
 *  }
 */
#ifndef myTIMERS_H
#define myTIMERS_H

// timers with random noise for second, minute, hour and millisecond intervals

#define DECLARE_TIMER(timerName, timerTime)     static unsigned long timerName##_interval = timerTime * 1000,        timerName##_last = millis()+random(timerName##_interval);
#define DECLARE_TIMERm(timerName, timerTime)    static unsigned long timerName##_interval = timerTime * 60 * 1000,   timerName##_last = millis()+random(timerName##_interval);
#define DECLARE_TIMERh(timerName, timerTime)    static unsigned long timerName##_interval = timerTime * 3600 * 1000, timerName##_last = millis()+random(timerName##_interval);
#define DECLARE_TIMERms(timerName, timerTime)   static unsigned long timerName##_interval = timerTime,               timerName##_last = millis()+random(timerName##_interval);

// same timers without random noise

#define DECLARE_TIMER_(timerName, timerTime)    static unsigned long timerName##_interval = timerTime * 1000,        timerName##_last = millis();
#define DECLARE_TIMERm_(timerName, timerTime)   static unsigned long timerName##_interval = timerTime * 60 * 1000,   timerName##_last = millis();
#define DECLARE_TIMERh_(timerName, timerTime)   static unsigned long timerName##_interval = timerTime * 3600 * 1000, timerName##_last = millis();
#define DECLARE_TIMERms_(timerName, timerTime)  static unsigned long timerName##_interval = timerTime,               timerName##_last = millis();

#define DECLARE_TIMERs DECLARE_TIMER

#define MAXms 4294967295L // 2^32 -1 is the max number of ms before rollover to 0

// ELAPSED is a function to avoid repeatetively invoking any function (like millis()) passed in as argument in macros expansion
// take rollover at MAXms into account!

static unsigned long ELAPSED(unsigned long __s, unsigned long __e) { return __e >= __s ? __e - __s : (MAXms - __s) + __e ; } 

#define SINCE(timerName)  ELAPSED(timerName##_last,millis())
#define DUE(timerName)    (( SINCE(timerName) < timerName##_interval) ? 0 : (timerName##_last=millis()))
#define DUE_(timerName)   (( SINCE(timerName) < timerName##_interval) ? 0 : (timerName##_last=(timerName##_interval*(millis()/timerName##_interval))))

#define RESET_TIMER(timerName) ( timerName##_last = millis() );

#ifdef SPEEDUP
  #define __TIMER DECLARE_TIMERs
#else
  #define __TIMER DECLARE_TIMERm
#endif

#endif // TIMERS_H
