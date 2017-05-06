#include "menu-system.h"
#include "lighting-program.h"

enum edit_list {
   NOT_EDITING,
   E_DAY,
   E_WEEK,
};

static enum edit_list am_editing;

static uint8_t day;
static uint8_t week;

static WLabel weeks_label(2,    58, 20);
static WLabel days_label(2,    201, 20);

static const WLabel clear_button(6, 10, 100);
static const WLabel set_button(11, 100, 100);

static void update_widget_for(enum edit_list n, uint16_t c)
{
   switch (n) {
      case NOT_EDITING:
         break;
      case E_DAY:
         days_label.paint_two_digits(day, c, ILI9341_WHITE);
         break;
      case E_WEEK:
         weeks_label.paint_two_digits(week, c, ILI9341_WHITE);
         break;
   }
}

static uint8_t *knob_for(enum edit_list n, uint8_t *max)
{
   switch (n) {
      case NOT_EDITING: return nullptr;
      case E_DAY: *max = 7; return &day;
      case E_WEEK: *max = 23; return &week;
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

void WEditCalendar::paint()
{
   lp.stop();

   lp.getCycleTime(week, day);

   week += 1;
   day += 1;

   update_widget_for(E_DAY, ILI9341_BLACK);
   update_widget_for(E_WEEK, ILI9341_BLACK);

   WLabel::paint(F("WEEK"),      5, 23, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F("DAY"),     160, 23, ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   clear_button.paint(F("RESET"), ILI9341_RED, DARK_COLOR);
   set_button.paint(F("SET ROUTINE"), ILI9341_GREEN, DARK_COLOR);

   menu.paintChangeControls();
   start_editing(NOT_EDITING);
}

static void edit_value(int chg)
{
   uint8_t max = 0;
   uint8_t *knob = knob_for(am_editing, &max);
   int result = (int)(*knob) + chg;

   if (result < 1)
      result = 1;
   if (result > max)
      result = max;

   *knob = result;
   update_widget_for(am_editing, ILI9341_CYAN);
}

void WEditCalendar::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y)) {
      lp.setCycleTime(week - 1, day - 1);
      lp.saveCalendar();
      lp.restart();
      menu.setMenu(setup_menu);
   } 
   else if (clear_button.hit(x, y)) {
      start_editing(NOT_EDITING);
      day = week = 1;
      update_widget_for(E_DAY, ILI9341_BLACK);
      update_widget_for(E_WEEK, ILI9341_BLACK);
   }
   else if (set_button.hit(x, y)) {
      lp.setCycleTime(week - 1, day - 1);
      menu.setMenu(set_routine_menu);
   }

   else if (up_slow.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(1);
   else if (up_fast.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(10);
   else if (down_fast.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(-10);
   else if (down_slow.hit(x, y) && (am_editing != NOT_EDITING))
      edit_value(-1);

   else if (days_label.hit(x, y)) start_editing(E_DAY);
   else if (weeks_label.hit(x, y)) start_editing(E_WEEK);
   else
      start_editing(NOT_EDITING);
}
