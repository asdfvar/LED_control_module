#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

enum edit_mode {
   EDIT_NEITHER,
   EDIT_ON_HOUR,
   EDIT_ON_MINUTE,
   EDIT_OFF_HOUR,
   EDIT_OFF_MINUTE
};

static edit_mode mode = EDIT_NEITHER;

static struct program_step on_step;
static struct program_step off_step;

static const WLabel on_hour_label   (2, 80,  60);
static const WLabel on_minute_label (2, 140, 60);

   // TODO: off time save and load and modify

static const WLabel off_hour_label   (2, 80,  120);
static const WLabel off_minute_label (2, 140, 120);

static bool initial = false;

void WuvbMenu::paint()
{
   // load on/off step at start of invocation of this menu
   if (!initial) {
      on_step = lp.loadUVBonTime();
      initial = true;
   }

   // on selection
   WLabel::paint (F("ON AT"), 17, 60, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(F(":"),       130, 60, ILI9341_GREEN, ILI9341_BLACK, 2, 0); 
   if (mode == EDIT_ON_HOUR) {
      on_hour_label.paint_two_digits (on_step.hour, ILI9341_BLACK, ILI9341_CYAN);
   } else {
      on_hour_label.paint_two_digits (on_step.hour, ILI9341_BLACK, ILI9341_WHITE);
   }
   if (mode == EDIT_ON_MINUTE) {
      on_minute_label.paint_two_digits (on_step.minute, ILI9341_BLACK, ILI9341_CYAN);
   } else {
      on_minute_label.paint_two_digits (on_step.minute, ILI9341_BLACK, ILI9341_WHITE);
   }

   // off selection
   WLabel::paint(F("OFF AT"),   5, 120, ILI9341_GREEN, ILI9341_BLACK, 2, 0); 
   WLabel::paint(F(":"),      130, 120, ILI9341_GREEN, ILI9341_BLACK, 2, 0); 
   if (mode == EDIT_OFF_HOUR) {
      off_hour_label.paint_two_digits (off_step.hour, ILI9341_BLACK, ILI9341_CYAN);
   } else {
      off_hour_label.paint_two_digits (off_step.hour, ILI9341_BLACK, ILI9341_WHITE);
   }
   if (mode == EDIT_OFF_MINUTE) {
      off_minute_label.paint_two_digits (off_step.minute, ILI9341_BLACK, ILI9341_CYAN);
   } else {
      off_minute_label.paint_two_digits (off_step.minute, ILI9341_BLACK, ILI9341_WHITE);
   }

   // change controls
   menu.paintChangeControls();
}

void WuvbMenu::touch(uint16_t x, uint16_t y)
{
   // edit the mode
   if (on_hour_label.hit (x, y)) {
      mode = (mode != EDIT_ON_HOUR) ? EDIT_ON_HOUR : EDIT_NEITHER;
   }
   else if (on_minute_label.hit (x, y)) {
      mode = (mode != EDIT_ON_MINUTE) ? EDIT_ON_MINUTE : EDIT_NEITHER;
   }
   else if (off_hour_label.hit (x, y)) {
      mode = (mode != EDIT_OFF_HOUR) ? EDIT_OFF_HOUR : EDIT_NEITHER;
   }
   else if (off_minute_label.hit (x, y)) {
      mode = (mode != EDIT_OFF_MINUTE) ? EDIT_OFF_MINUTE : EDIT_NEITHER;
   }

   // edit the time
   else if (up_slow.hit (x,y)) {
      if (mode == EDIT_ON_HOUR) {
         on_step.hour += 1;
         if (on_step.hour >= 24) on_step.hour = 23;
         lp.saveUVBonTime( on_step );
      }
      else if (mode == EDIT_ON_MINUTE) {
         on_step.minute += 1;
         if (on_step.minute >= 60) on_step.minute = 59;
      }
   }

   else if (up_fast.hit (x,y)) {
      if (mode == EDIT_ON_HOUR) {
         on_step.hour += 10;
         if (on_step.hour >= 24) on_step.hour = 24;
         lp.saveUVBonTime( on_step );
      }
      else if (mode == EDIT_ON_MINUTE) {
         on_step.minute += 10;
         if (on_step.minute >= 60) on_step.minute = 59;
      }
   }

   // TODO: down_slow and down_fast

   // redisplay after update
   this->paint();

   if (save_button.hit (x, y)) {
      mode = EDIT_NEITHER;
      lp.saveUVBonTime( on_step );
      initial = false;
      menu.setMenu (program_list_menu);
   }
}
