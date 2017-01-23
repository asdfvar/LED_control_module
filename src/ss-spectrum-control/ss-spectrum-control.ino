#include <SPI.h>
#include "Adafruit_ILI9341.h"
#include "Adafruit_FT6236.h"
#include "RTClib.h"
#include "lighting-program.h"
#include "serial-comm.h"
#include "menu-system.h"
#include "touch-debounce.h"

#define ROTATE_DISPLAY

static WLabel debug_display( 2, 201, 60 );

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

    for (int i = 0; i < 10; i++) {
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

    if (initted == false) {
	last = m_now;
	rtc_now = rtc.now();
	initted = true;
    }
    if (m_now != last) {
	rtc_now = (rtc_now + ex);
	last = m_now;
    }
    now = rtc_now;
#else
    now = rtc.now();
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

    if (! ctp.begin(40)) { 
	Serial.println(F("ERR: unable to start touchscreen."));
    } 

    // origin = left,top landscape (USB left upper)
    tft.setRotation(3); 
    menu.begin();
}

void loop()
{
    update_now();
    if (now.second() != last_second)
    {
	last_second = now.second();
	lp.tick();
	menu.tick();
	serialCommTick();
    }

    if (ctp.touched())
    {
       TS_Point p = ctp.getPoint();

       int x, y;

       // map to the 180 degrees rotated display
       x = map(p.y, 0, 320, 320, 0);
       y = map(p.x, 0, 240, 0, 240);

       // map back to the Adafruit display configuration
       x = map(x, 0, 320, 320, 0);
       y = map(y, 0, 240, 240, 0);

#ifdef ROTATE_DISPLAY
       // Rotate back
       x = map(x, 0, 320, 320, 0);
       y = map(y, 0, 240, 240, 0);
#endif

#if 0
WLabel::paint(F("x"),
              x,
              y,
              ILI9341_RED,
              ILI9341_BLUE,
              10,
              10);
debug_display.paint_four_digits( (uint16_t)ctp.readRegister8(0x03), ILI9341_BLACK, ILI9341_WHITE);
#endif

       debouncer.hit(x, y);
    } else {
       debouncer.nohit();
    }

    if (debouncer.touched())
       menu.touch(debouncer.x(), debouncer.y());

    repeatedUpdatePoll();

    serial_poll();
}
