#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

// what data are we supposed to have here?
//  10 phases (NPHASES)
//  active/inactive button i guess?
//  weeks + days
//  step number
//  program assignment


static uint8_t phase_index;
static struct phase *phase;
static uint8_t weeks, days;

enum edit_list {
   NOT_EDITING,
   E_WEEKS,
   E_DAYS
};

static enum edit_list am_editing;

static WLabel phase_label(2,  80, 0);

static WLabel active_button(9, 205, 0);
static bool last_active;

static WLabel weeks_label(2,    10, 60);
static WLabel days_label(2,    160, 60);

static WLabel program_button(9,    101, 120);

/* forward decl. */
static void update_widget_for(enum edit_list n, uint16_t c);

static void days2wd(uint8_t total_days, uint8_t& weeks, uint8_t& days)
{
   weeks = total_days / 7;
   days = total_days % 7;
}

static uint8_t wd2days(uint8_t weeks, uint8_t days)
{
   return (weeks * 7) + days;
}

static void update_active_button()
{
   if (last_active != phase->active) {
      if (phase->active) {
         active_button.paint(F("ACTIVE"), ILI9341_GREEN, DARK_COLOR);
      } else {
         active_button.paint(F("INACTIVE"), ILI9341_RED, DARK_COLOR);
      }
      last_active = phase->active;
   }
}

// we currently always allow toggling phase.. but.. who knows.
static void maybe_toggle_active()
{
   phase->active = !phase->active;
   update_active_button();
}

static void finish_edit()
{
   if (phase) {
      phase->days = wd2days(weeks, days);
      phase = nullptr;
   }
}

static void paint_new_phase()
{
   finish_edit();

   phase = lp.getPhase(phase_index);

   days2wd(phase->days, weeks, days);

   WLabel::paint(F("Phase:"),      5, 0, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   phase_label.paint( (uint8_t)(phase_index + 1), ILI9341_GREEN, ILI9341_BLACK);

   update_widget_for(E_WEEKS, ILI9341_BLACK);
   update_widget_for(E_DAYS, ILI9341_BLACK);
   WLabel::paint(F("WEEKS"),    65, 63, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F("DAYS"),    215, 63, ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   WLabel::paint(F("Program:"),   5, 120, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   program_button.paint(lp.getProgramName(phase->program), ILI9341_GREEN, DARK_COLOR);

   last_active = !phase->active;
   update_active_button();

   menu.paintChangeControls(1);
}


static void update_widget_for(enum edit_list n, uint16_t c)
{
   switch (n) {
      case NOT_EDITING:
         paint_new_phase();
         break;
      case E_WEEKS:
         weeks_label.paint_two_digits(weeks, c, ILI9341_WHITE);
         break;
      case E_DAYS:
         days_label.paint_two_digits(days, c, ILI9341_WHITE);
         break;
   }
}

static uint8_t value_for(enum edit_list n, uint8_t *max)
{
   switch (n) {
      case NOT_EDITING: *max = (NPHASES - 1); return phase_index;
      case E_WEEKS: *max = 23; return weeks;
      case E_DAYS: *max = 6; return days;
   }
   return 0;
}

static void set_value_for(enum edit_list n, uint8_t value)
{
   switch (n) {
      case NOT_EDITING: phase_index = value; break;
      case E_WEEKS: weeks = value; break;
      case E_DAYS: days = value; break;
   }
}

static void start_editing(enum edit_list n)
{
   if (am_editing != NOT_EDITING)
      update_widget_for(am_editing, ILI9341_BLACK);
   am_editing = n;
   if (am_editing != NOT_EDITING)
      update_widget_for(am_editing, ILI9341_CYAN);
}

static void edit_value(int chg)
{
   uint8_t max = 0;
   uint8_t knob = value_for(am_editing, &max);
   int result = (int)(knob) + chg;

   if (result < 0)
      result = 0;
   if (result > max)
      result = max;

   set_value_for(am_editing, result);
   update_widget_for(am_editing, ILI9341_CYAN);
}

void WSetRoutine::paint()
{
   phase = nullptr;
   // returning from previous menu should
   // start at the same phase.
   phase_index = select_program_menu.phase;
   select_program_menu.phase = 0;
   paint_new_phase();
   start_editing(NOT_EDITING);
}

void WSetRoutine::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y)) {
      finish_edit();
      menu.setMenu(edit_calendar_menu);
   } 

   else if (up_slow.hit(x, y)) edit_value(1);
   else if (up_fast.hit(x, y)) edit_value(10);
   else if (down_slow.hit(x, y)) edit_value(-1);
   else if (down_fast.hit(x, y)) edit_value(-10);

   else if (weeks_label.hit(x, y)) start_editing(E_WEEKS);
   else if (days_label.hit(x, y)) start_editing(E_DAYS);
   else if (program_button.hit(x, y)) {
      select_program_menu.phase = phase_index;
      menu.setMenu(select_program_menu);
   }
   else if (active_button.hit(x, y)) {
      start_editing(NOT_EDITING);
      maybe_toggle_active();
   }

   else
      start_editing(NOT_EDITING);
}
