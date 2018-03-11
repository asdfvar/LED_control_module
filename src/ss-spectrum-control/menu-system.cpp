
#include "menu-system.h"

WMainMenu              main_menu;
WManualMode            manual_mode_menu;
WSetupMenu             setup_menu;
WEditCurrentTime       edit_current_time_menu;
WEditCountdownDuration edit_countdown_duration_menu;
WProgramList           program_list_menu;
WEditProgramName       edit_program_name;
WBasicEditProgram      basic_edit_program_menu;
WuvbMenu               uvb_menu;
WAdvancedEditProgram   advanced_edit_program_menu;
WAreYouSure            are_you_sure_menu;
WEditCalendar          edit_calendar_menu;
WLightControl          light_control_menu;
WSetRoutine            set_routine_menu;
WSelectProgram         select_program_menu;
WSerialActive          serial_active_menu;

/* some common widgets that get used */
WLabel home_button(4, 260, 180, 2, 55);
WLabel save_button(4, 10,  180, 2, 55);
WLabel back_button(4, 10,  180, 2, 55);

WLabel up_slow(1,  80, 180, 2, 55);
WLabel up_fast(1, 140, 180, 4, 55);

WLabel down_fast(1, 200, 180, 4, 55);
WLabel down_slow(1, 260, 180, 2, 55);


void MenuSystem::paintChangeControls(bool back) const
{
   //    home_button.paint(F("HOME"), ILI9341_GREEN, DARK_COLOR);
   up_slow.paint    (F("+"), ILI9341_GREEN, DARK_COLOR);
   up_fast.paint    (F("+"), ILI9341_GREEN, DARK_COLOR);
   down_slow.paint  (F("-"), ILI9341_GREEN, DARK_COLOR);
   down_fast.paint  (F("-"), ILI9341_GREEN, DARK_COLOR);

   if (back) 
      back_button.paint(F("BACK"), ILI9341_GREEN, DARK_COLOR);
   else
      save_button.paint(F("SAVE"), ILI9341_GREEN, DARK_COLOR);
}

void MenuSystem::setMenu(WMenuBase& menu)
{
   previous = current;
   current  = &menu;
   tft.setTextSize(2);
   tft.fillScreen(ILI9341_BLACK);
   current->paint();
}

void MenuSystem::prevMenu(void)
{
   WMenuBase *menu;
   menu     = previous;
   previous = current;
   current  = menu;
   tft.setTextSize(2);
   tft.fillScreen(ILI9341_BLACK);
   current->paint();
}

bool MenuSystem::isMainMenu() const
{
   return (current == &main_menu);
}
