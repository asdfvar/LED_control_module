#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

static uint8_t step_index;
static struct program_step *step;
static uint8_t minH, minM;
static bool was_active;

enum edit_list {
   NOT_EDITING,
   E_HOURS,
   E_MINUTES,
   E_RED,
   E_WHITE,
   E_BLUE
};

static enum edit_list am_editing;

static WLabel prog_label(5,   0, 0);
static WLabel prog_index(2,  80, 0);

static WLabel active_button(9, 205, 0);
static bool last_active;

static WLabel hours_label(2,    20, 60);
static WLabel minutes_label(2, 140, 60);

static WLabel red_label(2,    20, 120);
static WLabel white_label(2,  80, 120);
static WLabel blue_label(2,  140, 120);

/* forward decl. */
static void update_widget_for(enum edit_list n, uint16_t c);

static void finish_edit()
{
   if (step) {
      lp.endEditing(step_index);
      step = nullptr;
   }
}

static void update_active_button()
{
   if (last_active != step->active) {
      if (step->active) {
         active_button.paint(F("ACTIVE"), ILI9341_GREEN, DARK_COLOR);
      } else {
         active_button.paint(F("INACTIVE"), ILI9341_RED, DARK_COLOR);
      }
      last_active = step->active;
   }
}

// only allow toggling active if
// the step is valid, and isn't step zero
static void maybe_toggle_active()
{
   if ((step_index == 0) || (step_index == (PROGRAM_STEPS - 1)))
      step->active = true;
   else if (!lp.step_time_overflows(step))
      step->active = !step->active;
   else
      step->active = false;

   was_active = step->active;
   update_active_button();
}

// after updating a rule's time,
// figure out if we need to disable it.
static void maybe_disable()
{
   if (step_index) {
      if (lp.step_time_overflows(step))
         step->active = false;
      else
         step->active = was_active;
      update_active_button();
   }
}

// clamp time 
static void check_updated_time(void)
{
   if (step->hour < minH) {
      step->hour = minH;
      step->minute = minM;
   } else if (step->hour == minH) {
      if (step->minute < minM)
         step->minute = minM;
   }
   update_widget_for(E_HOURS, am_editing == E_HOURS ? ILI9341_CYAN : ILI9341_BLACK );
   update_widget_for(E_MINUTES, am_editing == E_MINUTES ? ILI9341_CYAN : ILI9341_BLACK );
}

static void paint_new_step()
{
   finish_edit();
   step = lp.startEditing(step_index, &minH, &minM);
   was_active = step->active;

   prog_label.paint(F("Step:"), ILI9341_GREEN, ILI9341_BLACK);
   uint8_t arg_in = step_index + 1;
   prog_index.paint( arg_in, ILI9341_GREEN, ILI9341_BLACK);

   update_widget_for(E_HOURS, ILI9341_BLACK);
   WLabel::paint(F(":"),      80, 60, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
   update_widget_for(E_MINUTES, ILI9341_BLACK);

   update_widget_for(E_RED, ILI9341_BLACK);
   update_widget_for(E_WHITE, ILI9341_BLACK);
   update_widget_for(E_BLUE, ILI9341_BLACK);

   last_active = !step->active;
   maybe_disable();
   update_active_button();

   menu.paintChangeControls();
}

static void update_widget_for(enum edit_list n, uint16_t c)
{
   switch (n) {
      case NOT_EDITING:
         paint_new_step();
         break;
      case E_HOURS:
         hours_label.paint_two_digits(step->hour, c, ILI9341_WHITE);
         break;
      case E_MINUTES:
         minutes_label.paint_two_digits(step->minute, c, ILI9341_WHITE);
         break;
      case E_RED:
         red_label.paint(step->r, c, ILI9341_RED);
         break;
      case E_WHITE:
         white_label.paint(step->wh, c, ILI9341_WHITE);
         break;
      case E_BLUE:
         blue_label.paint(step->b, c, ILI9341_BLUE);
         break;
   }
}

static uint8_t value_for(enum edit_list n, uint8_t *max)
{
   switch (n) {
      case NOT_EDITING: *max = (PROGRAM_STEPS - 1); return step_index;
      case E_HOURS:   *max = 23; return step->hour;
      case E_MINUTES: *max = 59; return step->minute;
      case E_RED:     *max = 99; return step->r;
      case E_WHITE:   *max = 99; return step->wh;
      case E_BLUE:    *max = 99; return step->b;
   }
   return 0;
}

static void set_value_for(enum edit_list n, uint8_t value)
{
   switch (n) {
      case NOT_EDITING: step_index   = value; break;
      case E_HOURS:     step->hour   = value; break;
      case E_MINUTES:   step->minute = value; break;
      case E_RED:       step->r      = value; break;
      case E_WHITE:     step->wh     = value; break;
      case E_BLUE:      step->b      = value; break;
   }
   if ((n == E_BLUE) || (n == E_WHITE) || (n == E_RED)) {
      channels[CH_RED]   = step->r;
      channels[CH_WHITE] = step->wh;
      channels[CH_BLUE]  = step->b;
      sendManualUpdate();
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
   uint8_t max    = 0;
   uint8_t knob   = value_for(am_editing, &max);
   int     result = (int)(knob) + chg;

   if (result < 0)
      result = 0;
   if (result > max)
      result = max;

   // last step is always 0/0/0, you can't change it.
   if ((step_index == (PROGRAM_STEPS - 1)) && 
         ((am_editing == E_RED) || (am_editing == E_BLUE) || (am_editing == E_WHITE))) {
      result = 0;
   }

   set_value_for(am_editing, result);

   if ((am_editing == E_MINUTES) || (am_editing == E_HOURS)) {
      check_updated_time();
      maybe_disable();
   } else {
      update_widget_for(am_editing, ILI9341_CYAN);
   }
}

void WAdvancedEditProgram::paint()
{
   step = nullptr;
   step_index = 0;
   paint_new_step();
   start_editing(NOT_EDITING);
}

void WAdvancedEditProgram::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y)) {
      finish_edit();
      lp.saveProgram();
      menu.setMenu(program_list_menu);
   } 

   else if (up_slow.hit(x, y))
      edit_value(1);

   else if (up_fast.hit(x, y))
      edit_value(10);

   else if (down_slow.hit(x, y))
      edit_value(-1);

   else if (down_fast.hit(x, y))
      edit_value(-10);

   else if (hours_label.hit(x, y)) start_editing(E_HOURS);
   else if (minutes_label.hit(x, y)) start_editing(E_MINUTES);
   else if (red_label.hit(x, y)) start_editing(E_RED);
   else if (white_label.hit(x, y)) start_editing(E_WHITE);
   else if (blue_label.hit(x, y)) start_editing(E_BLUE);

   else if (active_button.hit(x, y)) {
      start_editing(NOT_EDITING);
      maybe_toggle_active();
   }

   else
      start_editing(NOT_EDITING);
}
