
#include "menu.h"
#include "serial-comm.h"
#include "color-line.h"
#include "lighting-program.h"

// how many minutes do we stay in manual mode by default
#define DEFAULT_TIMEOUT_MINUTES		2	

static WColorLine red_line(0, ILI9341_RED, 10);
static WColorLine white_line(1, ILI9341_WHITE, 70);
static WColorLine blue_line(2, ILI9341_BLUE, 130);

static WLabel countdown_time(8, 120, 180, 2, 55);
static WLabel view_button(4, 256, 180, 2, 55);
static uint8_t saved_settings[3];
static uint16_t countdown_seconds;

static void update_countdown_timer(void)
{
   if (countdown_seconds == 0) {
      countdown_time.paint(F("OVERRIDE"), ILI9341_RED, DARK_COLOR);
   } else {
      char buf[8];
      int m = countdown_seconds / 60;
      int s = countdown_seconds % 60;

      snprintf(buf, sizeof(buf), "%d:%2.2d", m, s);
      countdown_time.paint(buf, ILI9341_GREEN, DARK_COLOR);
   }
}

void WManualMode::tick()
{
   if (countdown_seconds) {
      --countdown_seconds;
      if (countdown_seconds == 0) {
         lp.restart();
         menu.setMenu(main_menu);
      } else {
         update_countdown_timer();
      }
   }
}

void WManualMode::paint()
{
   if (override) {
      countdown_seconds = 0;
   } else {
      if (saved_countdown_minutes == 0)
         saved_countdown_minutes = DEFAULT_TIMEOUT_MINUTES;
      countdown_seconds = 60 * saved_countdown_minutes;
   }
   update_countdown_timer();
   saved_settings[0] = 0xff;
   lp.stop();
   red_line.paint();
   white_line.paint();
   blue_line.paint();
   back_button.paint(F("BACK"), ILI9341_GREEN, DARK_COLOR);
   view_button.paint(F("VIEW"), ILI9341_GREEN, DARK_COLOR);
}

void WManualMode::touch(uint16_t x, uint16_t y)
{
   if (back_button.hit(x, y))
   {
      lp.restart();
      override = false;
      //        menu.prevMenu();
      menu.setMenu( main_menu);
   } else if (view_button.hit(x, y)) {
      if (saved_settings[0] == 0xff) {
         memcpy(saved_settings, output_channels, 3);
         output_channels[CH_RED] = 0;
         output_channels[CH_WHITE] = 99;
         output_channels[CH_BLUE] = 0;
      } else {
         memcpy(output_channels, saved_settings, 3);
         saved_settings[0] = 0xff;
      }
      red_line.update();
      white_line.update();
      blue_line.update();
      sendManualUpdate();
   } else if (countdown_time.hit(x, y)) {
      override = false;
      menu.setMenu(edit_countdown_duration_menu);
   } else {
      red_line.touch(x, y);
      white_line.touch(x, y);
      blue_line.touch(x, y);
      sendManualUpdate();
   }
}
