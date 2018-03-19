
#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

enum edit_list {
   NOT_EDITING,
   E_ON_HOURS,
   E_ON_MINUTES,
   E_OFF_HOURS,
   E_OFF_MINUTES,
   E_FADE,
   E_RED,
   E_WHITE,
   E_BLUE
};

static enum edit_list am_editing;

static const WLabel red_label(      2,  20,   5);
static const WLabel white_label(    2,  80,   5);
static const WLabel blue_label(     2, 140,   5);
static const WLabel advanced_button(8, 205,   5);

static const WLabel on_hours_label(       2,  80,  60);
static const WLabel on_minutes_label(     2, 140,  60);

static const WLabel off_hours_label(      2,  80, 120);
static const WLabel off_minutes_label(    2, 140, 120);

static const WLabel fade_label(           2, 270, 120);

static const WLabel total_label(          5, 198, 145, 2, 0);

static struct program_step on_step;
static struct program_step off_step;

static struct program_step *first_step;
static struct program_step *second_step;

static uint8_t fade_time;
static uint8_t total_h, total_m;

static void add_fade_time()
{
   uint16_t m = LightingProgram::to_minutes(&off_step);
   m += fade_time;
   LightingProgram::from_minutes(&off_step, m);
}

static void subtract_fade_time()
{
   uint16_t m = LightingProgram::to_minutes(&off_step);
   if (m < fade_time)
      m = 0;
   else
      m -= fade_time;
   LightingProgram::from_minutes(&off_step, m);
}

static void total_on_time(uint8_t *hours, uint8_t *minutes)
{
   uint16_t start = LightingProgram::to_minutes(&on_step);
   uint16_t end = LightingProgram::to_minutes(&off_step);

   if (end < start) end += 24 * 60;

   LightingProgram::from_minutes(hours, minutes, (end - start));
}

static void update_widget_for(enum edit_list n, uint16_t c)
{
   uint8_t th, tm;

   switch (n) {
      case NOT_EDITING: break;
      case E_ON_HOURS:
                        on_hours_label.paint_two_digits(on_step.hour, c, ILI9341_WHITE);
                        break;
      case E_ON_MINUTES:
                        on_minutes_label.paint_two_digits(on_step.minute, c, ILI9341_WHITE);
                        break;

      case E_OFF_HOURS:
                        off_hours_label.paint_two_digits(off_step.hour, c, ILI9341_WHITE);
                        break;
      case E_OFF_MINUTES:
                        off_minutes_label.paint_two_digits(off_step.minute, c, ILI9341_WHITE);
                        break;
      case E_FADE:
                        fade_label.paint_two_digits(fade_time, c, ILI9341_WHITE);
                        break;
      case E_RED:
                        red_label.paint(on_step.r, c, ILI9341_RED);
                        break;
      case E_WHITE:
                        white_label.paint(on_step.wh, c, ILI9341_WHITE);
                        break;
      case E_BLUE:
                        blue_label.paint(on_step.b, c, ILI9341_BLUE);
                        break;
   }

   // fade time is zero here because off step includes
   // fade time, until you click the save button. confusing, i know.
   total_on_time(&th, &tm);
   if ((th != total_h) || (tm != total_m)) {
      char buf[32];
      total_h = th;
      total_m = tm;
      snprintf(buf, sizeof(buf), "%d:%2.2d", total_h, total_m);
      total_label.paint(buf, ILI9341_WHITE, ILI9341_BLACK);
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

static uint8_t value_for(enum edit_list n, uint8_t *min, uint8_t *max)
{
   *min = 0;
   switch (n) {
      case NOT_EDITING: *max = 0; return 0;
      case E_ON_HOURS: *max = 23; return on_step.hour;
      case E_ON_MINUTES: *max = 59; return on_step.minute;
      case E_OFF_HOURS: *max = 23; return off_step.hour;
      case E_OFF_MINUTES: *max = 59; return off_step.minute;
      case E_FADE: *max = 99; return fade_time;
      case E_RED: *min = 1; *max = 99; return on_step.r;
      case E_WHITE: *min = 1; *max = 99; return on_step.wh;
      case E_BLUE: *min = 1; *max = 99; return on_step.b;
   }
   return 0;
}

static void set_value_for(enum edit_list n, uint8_t value)
{
   switch (n) {
      case NOT_EDITING: break;
      case E_ON_HOURS: on_step.hour = value; break;
      case E_ON_MINUTES: on_step.minute = value; break;
      case E_OFF_HOURS: off_step.hour = value; break;
      case E_OFF_MINUTES: off_step.minute = value; break;
      case E_FADE: fade_time = value; break;
      case E_RED:   on_step.r = value; break;
      case E_WHITE: on_step.wh = value; break;
      case E_BLUE:  on_step.b = value; break;
   }
   if ((n == E_BLUE) || (n == E_WHITE) || (n == E_RED)) {
      channels[CH_RED]   = on_step.r;
      channels[CH_WHITE] = on_step.wh;
      channels[CH_BLUE]  = on_step.b;
      sendManualUpdate();
   }
}

static void edit_value(int chg)
{
   uint8_t max = 0, min = 0;
   int knob = value_for(am_editing, &min, &max);
   int result = knob + chg;

   if (result < min)
      result = min;
   if (result > max)
      result = max;

   set_value_for(am_editing, result);
   update_widget_for(am_editing, ILI9341_CYAN);
}

//
// program steps must be in time-order.
// to support over-night lighting modes, we make sure that first/last steps are set properly
//
static void load_steps()
{
   uint8_t mH, mM;

   first_step = lp.startEditing(0, &mH, &mM);
   second_step = lp.startEditing(PROGRAM_STEPS - 1, &mH, &mM);

   if (LightingProgram::is_off(first_step) && !LightingProgram::is_off(second_step)) {
      off_step = *first_step;
      on_step = *second_step;
   } else {
      on_step = *first_step;
      off_step = *second_step;
   }

   if (!on_step.r) on_step.r = 1;
   if (!on_step.b) on_step.b = 1;
   if (!on_step.wh) on_step.wh = 1;

   off_step.r = off_step.b = off_step.wh = 0;
}

static void save_steps()
{
   uint16_t off_m = LightingProgram::to_minutes(&off_step);
   uint16_t on_m = LightingProgram::to_minutes(&on_step);

   if (off_m > on_m) {
      *first_step = on_step;
      *second_step = off_step;
   } else {
      *first_step = off_step;
      *second_step = on_step;
   }
}

void WBasicEditProgram::paint()
{
   fade_time = lp.getFadeDuration();
   menu.paintChangeControls();

   load_steps();
   add_fade_time();

   total_h = total_m = 0xff;

   update_widget_for(E_ON_HOURS,   ILI9341_BLACK);
   update_widget_for(E_ON_MINUTES, ILI9341_BLACK);
   WLabel::paint(F("ON AT"),    17, 60, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F(":"),       130, 60, ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   update_widget_for(E_OFF_HOURS,   ILI9341_BLACK);
   update_widget_for(E_OFF_MINUTES, ILI9341_BLACK);
   WLabel::paint(F("OFF AT"),   5, 120, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F(":"),      130, 120, ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   update_widget_for(E_RED, ILI9341_BLACK);
   update_widget_for(E_WHITE, ILI9341_BLACK);
   update_widget_for(E_BLUE, ILI9341_BLACK);

   update_widget_for(E_FADE,   ILI9341_BLACK);
   WLabel::paint(F("Sunset/"),   220, 43, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F("Sunrise"),   220, 73, ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   if (lp.isAdvancedProgram()) {
      advanced_button.paint(F("ADVANCED"), ILI9341_WHITE, ILI9341_RED);
   } else {
      advanced_button.paint(F("ADVANCED"), ILI9341_GREEN, DARK_COLOR);
   }

   WLabel::paint(F("Total On"), 200, 135, ILI9341_WHITE, ILI9341_BLACK, 1, 0, 10);
}

void WBasicEditProgram::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y)) {
      subtract_fade_time();
      lp.setFadeDuration(fade_time);
      save_steps();
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

   else if (on_hours_label.hit(x, y)) start_editing(E_ON_HOURS);
   else if (on_minutes_label.hit(x, y)) start_editing(E_ON_MINUTES);
   else if (off_hours_label.hit(x, y)) start_editing(E_OFF_HOURS);
   else if (off_minutes_label.hit(x, y)) start_editing(E_OFF_MINUTES);
   else if (fade_label.hit(x, y)) start_editing(E_FADE);
   else if (red_label.hit(x, y)) start_editing(E_RED);
   else if (white_label.hit(x, y)) start_editing(E_WHITE);
   else if (blue_label.hit(x, y)) start_editing(E_BLUE);

   else if (advanced_button.hit(x, y)) {
      subtract_fade_time();
      menu.setMenu(advanced_edit_program_menu);
   }
   else
      start_editing(NOT_EDITING);
}
