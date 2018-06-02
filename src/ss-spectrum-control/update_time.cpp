#include "update_time.h"
#include "lighting-program.h"

void LightingProgram::update_time (bool invokeRTC)
{
#ifdef FAST_CLOCK
   static bool initted;
   static unsigned long last = 0;
   static DateTime rtc_now;
   static TimeSpan ex(1);

   unsigned long m_now = millis();

   if (initted == false)
   {
      last = m_now;
      rtc_now = rtc.now();
      initted = true;
   }
   if (m_now != last)
   {
      rtc_now = (rtc_now + ex);
      last = m_now;
   }
   now = rtc_now;
#else
   static unsigned long lastMillis = millis();
   static DateTime lastTime = now;

   // occasionally, calls to the RTC will hang. to mitigate
   // this, calls to the RTC are made only once per hour (or
   // whichever time-length interval is defined by
   // TIME_SYNC_LENGTH) to sync with the current time.
   // otherwise, time is updated from the less accurate
   // built-in clock

   // invoke the RTC once per time interval as defined
   // by TIME_SYNC_LENGTH and at start
   if (invokeRTC)
   {
      // set the current time
      now = rtc.now();

      // set the last millisecond count
      lastMillis = millis();

      // set the last time update since the RTC update
      lastTime = now;
   }
   else
   {
      unsigned long nowMillis   = millis();
      unsigned long deltaMillis = nowMillis - lastMillis;

      // update the time based on the default, internal clock. in
      // the case of overflow, which happens every ~49.7 days,
      // update the time based on the RTC and start again
      if (deltaMillis <= TIME_OVERFLOW_CHECK) {
         now = lastTime + TimeSpan ((uint32_t)(deltaMillis / 1000));
      } else {

         // call this routine again with the RTC flag set
         update_time (true);
      }

      // re-sync the clock with the RTC chip after the time
      // has surpassed the interval defined by TIME_SYNC_LENGTH
      if (deltaMillis > TIME_SYNC_LENGTH) update_time (true);
   }
#endif
}

DateTime LightingProgram::now_DateTime (void) {
   return now;
}

uint16_t LightingProgram::now_year (void) const {
   return now.year();
}

uint8_t LightingProgram::now_month (void) const {
   return now.month();
}

uint8_t LightingProgram::now_day (void) const {
   return now.day();
}

uint8_t LightingProgram::now_hour (void)const  {
   return now.hour();
}

uint8_t LightingProgram::now_minute (void)const  {
   return now.minute();
}

uint8_t LightingProgram::now_second (void)const  {
   return now.second();
}
