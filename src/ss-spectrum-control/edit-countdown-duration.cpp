#include "menu-system.h"
#include "lighting-program.h"

static WLabel duration_label( 3,  130,  5);
static WLabel override_button(8,  111, 95, 2, 55);
static int countdown_duration_minutes;

static void update()
{
   duration_label.paint( (uint8_t)countdown_duration_minutes, ILI9341_BLACK, ILI9341_WHITE);
}

void WEditCountdownDuration::paint()
{
   countdown_duration_minutes = manual_mode_menu.getCountdownMinutes();

   WLabel::paint(F("(minutes)"), 106, 55, ILI9341_GREEN, ILI9341_BLACK);

   update();
   menu.paintChangeControls();
   override_button.paint(F("OVERRIDE"), ILI9341_RED, DARK_COLOR);
}

static void edit_value(int chg)
{
   int result = countdown_duration_minutes + chg;

   if (result >= 120)
      result = 120;
   if (result < 1)
      result = 1;

   if (countdown_duration_minutes != result) {
      countdown_duration_minutes = result;
      update();
   }
}

void WEditCountdownDuration::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y)) {
      manual_mode_menu.setCountdownMinutes(countdown_duration_minutes);
      menu.setMenu(manual_mode_menu);
   } else if (override_button.hit(x, y)) {
      manual_mode_menu.setOverride();
      menu.setMenu(manual_mode_menu);
   }

   else if (up_slow.hit(x, y))
      edit_value(1);
   else if (up_fast.hit(x, y))
      edit_value(10);
   else if (down_fast.hit(x, y))
      edit_value(-10);
   else if (down_slow.hit(x, y))
      edit_value(-1);
}
