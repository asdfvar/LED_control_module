#include <SPI.h>
#include "Adafruit_ILI9341.h"
#include "Adafruit_FT6236.h"
#include "RTClib.h"
#include "lighting-program.h"
#include "serial-comm.h"
#include "menu-system.h"
#include "touch-debounce.h"

//#define ROTATE_DISPLAY
#define TIME_SYNC_LENGTH    3600000  // milliseconds / hour
#define TIME_OVERFLOW_CHECK 86400000 // milliseconds / day

static WLabel debug_display1( 2, 201, 60  );
static WLabel debug_display2( 2, 20,  60  );
static WLabel debug_display3( 2, 201, 100 );
static WLabel debug_display4( 2, 20,  100 );

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

// The FT6236 uses hardware I2C (SCL/SDA)
Adafruit_FT6236 ctp = Adafruit_FT6236();

// Debouncing for the touch screen controller
TouchDebounce debouncer;

// I2C RTC
RTC_DS3231 rtc;

// Our lighting programs
LightingProgram lp;

// The menu controller
MenuSystem menu;

// Last second
static uint8_t last_second;

// Current time
DateTime now;

/* needed only on the teensy
 * the rtc is not reset properly, and if we reset during a read
 * the i2c state machine in the rtc hangs.. waiting for more clocks
 * so just feed it some clocks
 */
static void whack_i2c_with_clocks()
{
   pinMode(19, OUTPUT);

   for (int i = 0; i < 10; i++)
   {
      digitalWrite(19, LOW);
      delayMicroseconds(3);
      digitalWrite(19, HIGH);
      delayMicroseconds(3);
   }
   pinMode(19, INPUT);
}

static void update_now()
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
   static bool invokeRTC = true;

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

      // set flag to call internal clock for the next hour
      invokeRTC = false;

      // set the last millisecond count
      lastMillis = millis();

      // set the last time update since the RTC update
      lastTime = now;
   }
   else
   {
      unsigned long nowMillis   = millis();
      unsigned long deltaMillis = nowMillis - lastMillis;

      // check for overflow by comparing the number of milliseconds in a day
      // to the difference between the current millisecond count from the previous
      // (comparing the number of milliseconds in a day).
      // overflow happens every ~49.7 days
      if (deltaMillis <= TIME_OVERFLOW_CHECK) {
         now = lastTime + TimeSpan ((uint32_t)(deltaMillis / 1000));
      } else {

         // set flag to invoke the RTC
         invokeRTC = true;

         // call this routine again with the RTC flag set
         update_now();
      }

      // re-sync the clock with the RTC chip after the time
      // has surpassed the interval defined by TIME_SYNC_LENGTH
      if (deltaMillis > TIME_SYNC_LENGTH) invokeRTC = true;
   }
#endif
}

void setup(void)
{
   whack_i2c_with_clocks();
   Serial.begin(115200);
   initSerialComms();
   tft.begin();
   rtc.begin();
   debouncer.begin();
   update_now();

   lp.begin();

   if (! ctp.begin(40) )
   {
      Serial.println(F("ERR: unable to start touchscreen."));
   } 

   // origin = left,top landscape (USB left upper)
   tft.setRotation(3); 
   menu.begin();
}

void loop()
{
   update_now();
   if ( now.second() != last_second )
   {
      last_second = now.second();
      lp.tick();
      menu.tick();
      serialCommTick();
   }

   if (ctp.touched())
   {
      TS_Point p1 = ctp.getPoint();
      delay( 10 );
      TS_Point p2 = ctp.getPoint();

      int16_t x1 = p1.x;
      int16_t y1 = p1.y;
      int16_t x2 = p2.x;
      int16_t y2 = p2.y;
      int16_t temp;

      // map to the display configuration
      temp = map(y1, 0, 320, 320, 0);
      y1   = map(x1, 0, 240, 0, 240);
      x1   = temp;

      temp = map(y2, 0, 320, 320, 0);
      y2   = map(x2, 0, 240, 0, 240);
      x2   = temp;

#ifdef ROTATE_DISPLAY
      // map back to the Adafruit FT6206 display configuration
      x1 = map(x1, 0, 320, 320, 0);
      y1 = map(y1, 0, 240, 240, 0);

      x2 = map(x2, 0, 320, 320, 0);
      y2 = map(y2, 0, 240, 240, 0);
#endif

      debouncer.hit(x1, y1, x2, y2);
   } else {
      debouncer.nohit();
   }

   if (debouncer.touched())
      menu.touch(debouncer.x(), debouncer.y());

   repeatedUpdatePoll();

   serial_poll();
}
