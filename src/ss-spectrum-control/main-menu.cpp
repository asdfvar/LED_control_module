#include <utility>

#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

static uint8_t last_week, last_day, last_enabled;
static WLabel manual_button(6,  10, 180, 2, 55);
static WLabel cycle_week(2,    130, 192, 2, 0);
static WLabel cycle_day(2,     185, 192, 2, 0);
static WLabel setup_button(5,  240, 180, 2, 55);

static uint8_t last_hour, last_minute, last_second, last_rwhb[3];
static WLabel now_hours(2,      128,    4,      4, 0);
static WLabel now_minutes(2,    194,    4,      4, 0);
static WLabel now_seconds(2,    260,    4,      4, 0);

static WLabel now_r(2,           45,    95,     4, 0);
static WLabel now_wh(2,         149,    95,     4, 0);
static WLabel now_b(2,          264,    95,     4, 0);

static uint8_t last_program;

static void add_fade_time(const struct program_step *from, struct program_step *to)
{
   uint16_t fade_time = lp.getFadeDuration();
   uint16_t m = LightingProgram::to_minutes(from);
   m += fade_time;
   LightingProgram::from_minutes(to, m);
}

void WMainMenu::tick()
{
   uint8_t weeks, days;
   bool cal_enabled = lp.getCycleTime(weeks, days);

   weeks += 1;
   days  += 1;

   if (last_enabled != cal_enabled)
   {
      last_enabled = cal_enabled;
      if ( cal_enabled )
      {
         last_week = last_day = 0xff;
      } else {
         cycle_week.paint(F("--"), ILI9341_GREEN, ILI9341_BLACK);
         cycle_day.paint(F("--"), ILI9341_GREEN, ILI9341_BLACK);
      }
   }

   if (cal_enabled) {
      if (weeks != last_week) {
         last_week = weeks;
         cycle_week.paint_two_digits(last_week, ILI9341_GREEN, ILI9341_BLACK);
      }
      if (days != last_day) {
         last_day = days;
         cycle_day.paint_two_digits(last_day, ILI9341_GREEN, ILI9341_BLACK);
      }
   }

   if (lp.now_hour() != last_hour) {
      last_hour = lp.now_hour();
      now_hours.paint_two_digits(last_hour, ILI9341_GREEN, ILI9341_BLACK);
   }
   if (lp.now_minute() != last_minute) {
      last_minute = lp.now_minute();
      now_minutes.paint_two_digits(last_minute, ILI9341_GREEN, ILI9341_BLACK);
   }
   if (lp.now_second() != last_second) {
      last_second = lp.now_second();
      now_seconds.paint_two_digits(last_second, ILI9341_GREEN, ILI9341_BLACK);
   }
   if (last_rwhb[CH_RED] != output_channels[CH_RED]) {
      last_rwhb[CH_RED] = output_channels[CH_RED];
      if ( output_channels[CH_RED] != channels[CH_RED] && lp.get_enable_light_control() )
         now_r.paint_two_digits(last_rwhb[CH_RED], ILI9341_RED, ILI9341_BLACK);
      else
         now_r.paint_two_digits(channels[CH_RED], ILI9341_GREEN, ILI9341_BLACK);
   }
   if (last_rwhb[CH_WHITE] != output_channels[CH_WHITE]) {
      last_rwhb[CH_WHITE] = output_channels[CH_WHITE];
      if ( output_channels[CH_WHITE] != channels[CH_WHITE] && lp.get_enable_light_control() )
         now_wh.paint_two_digits(last_rwhb[CH_WHITE], ILI9341_RED, ILI9341_BLACK);
      else
         now_wh.paint_two_digits(channels[CH_WHITE], ILI9341_GREEN, ILI9341_BLACK);
   }
   if (last_rwhb[CH_BLUE] != output_channels[CH_BLUE]) {
      last_rwhb[CH_BLUE] = output_channels[CH_BLUE];
      if ( output_channels[CH_BLUE] != channels[CH_BLUE] && lp.get_enable_light_control() )
         now_b.paint_two_digits(last_rwhb[CH_BLUE], ILI9341_RED, ILI9341_BLACK);
      else
         now_b.paint_two_digits(channels[CH_BLUE], ILI9341_GREEN, ILI9341_BLACK);
   }

   uint8_t program = lp.getLoadedProgram();
   if (program != last_program) {
      last_program = program;

      char buf[32];
      const char *name = lp.getLoadedProgramName();
      const struct program_step *first = lp.getStepAt(0);
      const struct program_step *last = lp.getStepAt(PROGRAM_STEPS - 1);
      struct program_step last_adjusted;

      if (LightingProgram::is_off(first))
         std::swap(first, last);

      WLabel::paint(name,       120, 46, ILI9341_GREEN, ILI9341_BLACK, 4, 0);

      add_fade_time(last, &last_adjusted);
      snprintf(buf, sizeof(buf), "On:%2.2d:%2.2d  Off:%2.2d:%2.2d",
            first->hour, first->minute, last_adjusted.hour, last_adjusted.minute);
      WLabel::paint(buf,    30, 130, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   }
}

void WMainMenu::paint()
{
#ifdef FAST_CLOCK
   WLabel::paint(F("FAST:"),   0, 0, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
#else
   WLabel::paint(F("TIME:"),   0, 0, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
#endif

   WLabel::paint(F(":"),     174, 0, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
   WLabel::paint(F(":"),     240, 0, ILI9341_GREEN, ILI9341_BLACK, 4, 0);

   WLabel::paint(F("PROG:"),   0, 46, ILI9341_GREEN, ILI9341_BLACK, 4, 0);

   WLabel::paint(F("R"),      20, 91, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
   WLabel::paint(F("W"),     125, 91, ILI9341_GREEN, ILI9341_BLACK, 4, 0);
   WLabel::paint(F("B"),     240, 91, ILI9341_GREEN, ILI9341_BLACK, 4, 0);

   WLabel::paint(F("W"),     115, 180, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F("D"),     170, 180, ILI9341_GREEN, ILI9341_BLACK, 2, 0);


   manual_button.paint(F("MANUAL"), ILI9341_GREEN, DARK_COLOR);
   setup_button.paint(F("SETUP"), ILI9341_GREEN, DARK_COLOR);
   last_program = last_enabled = last_week = last_day = last_hour = last_minute = last_second = 0xff;
   memset(last_rwhb, 0xff, 3);
   tick();
}

void WMainMenu::touch(uint16_t x, uint16_t y)
{
   if (manual_button.hit(x, y))
      menu.setMenu(manual_mode_menu);
   if (setup_button.hit(x, y))
      menu.setMenu(setup_menu);
}
