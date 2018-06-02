
#include "lighting-program.h"
#include "menu-system.h"

#define LIGHT_CONTROL_BUTTON

static const WLabel program_list_button(  14, 100,  10);
static const WLabel edit_time_button(     14, 100,  70);
static const WLabel edit_cal_button(      14, 100, 130);
#ifdef LIGHT_CONTROL_BUTTON
static const WLabel light_control_button( 14, 100, 190);
#endif

void WSetupMenu::paint()
{
    program_list_button.paint(F("EDIT PROGRAMS"), ILI9341_GREEN, DARK_COLOR);
    edit_time_button.paint(F("SET TIME"), ILI9341_GREEN, DARK_COLOR);
    edit_cal_button.paint(F("SET CALENDAR"), ILI9341_GREEN, DARK_COLOR);
#ifdef LIGHT_CONTROL_BUTTON
    light_control_button.paint(F("LIGHT CONTROL"), ILI9341_GREEN, DARK_COLOR);
#endif

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
#ifdef LIGHT_CONTROL_BUTTON
    if (light_control_button.hit(x, y))
       menu.setMenu(light_control_menu);
#endif
    if (back_button.hit(x, y))
    {
       menu.setMenu(main_menu);
//       menu.prevMenu();
    }
}

