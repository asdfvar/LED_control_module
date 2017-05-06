#include "menu-system.h"
#include "lighting-program.h"

static const WLabel clear_button(6, 10, 180, 2, 55);
static const WLabel cancel_button(6, 220, 180, 2, 55);


void WAreYouSure::paint()
{
   WLabel::paint(F("Do you want to erase"), 20, 55, ILI9341_GREEN, ILI9341_BLACK);
   WLabel::paint(F("this program?"), 20, 85, ILI9341_GREEN, ILI9341_BLACK);

   cancel_button.paint(F("CANCEL"), ILI9341_GREEN, DARK_COLOR);
   clear_button.paint(F("CLEAR"), ILI9341_RED, DARK_COLOR);
}

void WAreYouSure::touch(uint16_t x, uint16_t y)
{
   if (clear_button.hit(x, y)) {
      lp.resetProgram();
      lp.saveProgram();
      menu.setMenu(program_list_menu);
   } else if (cancel_button.hit(x, y)) {
      menu.setMenu(program_list_menu);
   }
}
