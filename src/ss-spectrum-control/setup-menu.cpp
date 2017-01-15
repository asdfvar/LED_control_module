
#include "lighting-program.h"
#include "menu-system.h"

static const WLabel program_list_button( 14, 100,  10);
static const WLabel edit_time_button(    14, 100,  70);
static const WLabel edit_cal_button(     14, 100, 130);

void WSetupMenu::paint()
{
    program_list_button.paint(F("EDIT PROGRAMS"), ILI9341_GREEN, DARK_COLOR);
    edit_time_button.paint(F("SET TIME"), ILI9341_GREEN, DARK_COLOR);
    edit_cal_button.paint(F("SET CALENDAR"), ILI9341_GREEN, DARK_COLOR);

    back_button.paint(F("BACK"), ILI9341_GREEN, DARK_COLOR);
}

void WSetupMenu::touch(uint16_t x, uint16_t y)
{
    if (program_list_button.hit(x, y))
       menu.setMenu(program_list_menu);
    if (edit_time_button.hit(x, y))
       menu.setMenu(edit_current_time_menu);
    if (edit_cal_button.hit(x, y))
       menu.setMenu(edit_calendar_menu);
    if (back_button.hit(x, y))
    {
       menu.setMenu(main_menu);
//       menu.prevMenu();
    }
}

