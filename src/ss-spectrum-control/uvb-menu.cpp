#include "menu-system.h"
#include "lighting-program.h"
#include "serial-comm.h"

// static const WLabel 

void WuvbMenu::paint()
{
   back_button.paint(F("BACK"), ILI9341_GREEN, DARK_COLOR);
}

void WuvbMenu::touch(uint16_t x, uint16_t y)
{
   if (back_button.hit (x, y)) {
      menu.setMenu (program_list_menu);
   }
}
