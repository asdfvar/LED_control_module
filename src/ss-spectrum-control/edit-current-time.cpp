#include "menu-system.h"
#include "lighting-program.h"

enum edit_list {
   NOT_EDITING,
   E_HOURS,
   E_MINUTES,
};

static enum edit_list am_editing;

static uint8_t hour;
static uint8_t minute;

static WLabel hours_label(  2, 90,  80);
static WLabel minutes_label(2, 160, 80);

static void update_widget_for(enum edit_list n, uint16_t c)
{
   switch (n) {
      case NOT_EDITING:
         break;
      case E_HOURS:
         hours_label.paint_two_digits(hour, c, ILI9341_WHITE);
         break;
      case E_MINUTES:
         minutes_label.paint_two_digits(minute, c, ILI9341_WHITE);
         break;
   }
}
static uint8_t *knob_for(enum edit_list n, uint8_t *max)
{
   switch (n) {
      case NOT_EDITING: return nullptr;
      case E_HOURS:   *max = 23; return &hour;
      case E_MINUTES: *max = 59; return &minute;
   }
   return nullptr;
}

static void start_editing(enum edit_list n)
{
   if (am_editing != NOT_EDITING)
      update_widget_for(am_editing, ILI9341_BLACK);

   am_editing = n;

   if (am_editing != NOT_EDITING)
      update_widget_for(am_editing, ILI9341_CYAN);
}

void WEditCurrentTime::paint()
{
   lp.stop();

   hour   = lp.now_hour();
   minute = lp.now_minute();

   update_widget_for(E_HOURS, ILI9341_BLACK);
   WLabel::paint(F(":"), 140, 80, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
   update_widget_for(E_MINUTES, ILI9341_BLACK);

   menu.paintChangeControls();
   start_editing(NOT_EDITING);
}

static void edit_value(int chg)
{
   uint8_t max   = 0;
   uint8_t *knob = knob_for(am_editing, &max);
   int result    = (int)(*knob) + chg;

   if (result < 0)
      result += (max + 1);
   if (result > max)
      result -= (max + 1);

   *knob = result;
   update_widget_for(am_editing, ILI9341_CYAN);
}

void WEditCurrentTime::touch(uint16_t x, uint16_t y)
{
   DateTime modified_time;
   if (save_button.hit(x, y)) {
      modified_time = DateTime (lp.now_year(), lp.now_month(), lp.now_day(), hour, minute);
      rtc.adjust (modified_time);
      lp.restart();
      menu.setMenu (setup_menu);
   }
#if 0
   if (home_button.hit(x, y))
   {
      menu.setMenu(main_menu);
   }
#endif

   else if (up_slow.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(1);
   else if (up_fast.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(10);
   else if (down_fast.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(-10);
   else if (down_slow.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(-1);

   else if (hours_label.hit(x, y)) start_editing(E_HOURS);
   else if (minutes_label.hit(x, y)) start_editing(E_MINUTES);
   else start_editing(NOT_EDITING);
}
