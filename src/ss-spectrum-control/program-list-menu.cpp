#include "menu-system.h"
#include "lighting-program.h"

static const WLabel prog_index(    1,  100, 0);
static const WLabel active_button( 9,  205, 0);
static const WLabel prog_name(     9,   65, 55);
static const WLabel rename_button( 6,  238, 55);
static const WLabel edit_button(  10,   20, 110);
static const WLabel uvb_button(    5,  165, 110);
static const WLabel clear_button(  5,  248, 110);

static void paint_active_button()
{
   if (lp.getActiveProgram() == lp.getLoadedProgram()) {
      active_button.paint(F("ACTIVE"), ILI9341_GREEN, DARK_COLOR);
   } else {
      active_button.paint(F("INACTIVE"), ILI9341_RED, DARK_COLOR);
   }
}

static void paint_new_program()
{
   WLabel::paint(F("Program#"),   5,  0, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   prog_index.paint( (uint8_t)(lp.getLoadedProgram() + 1), ILI9341_GREEN, ILI9341_BLACK);
   paint_active_button();
   WLabel::paint(F("Name:"),      5, 55, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   prog_name.paint(lp.getLoadedProgramName(), ILI9341_GREEN, ILI9341_BLACK);
   rename_button.paint(F("RENAME"), ILI9341_GREEN, DARK_COLOR);
   edit_button.paint(F("EDIT STEPS"), ILI9341_GREEN, DARK_COLOR);
   uvb_button.paint(F("UVB"), ILI9341_GREEN, DARK_COLOR);
   clear_button.paint(F("CLEAR"), ILI9341_GREEN, DARK_COLOR);

}

static void edit_value(int v)
{
   int result = v + lp.getLoadedProgram();

   if (result < 0)
      result = 0;
   if (result >= NPROGRAMS)
      result = (NPROGRAMS - 1);

   if (result != lp.getLoadedProgram()) {
      lp.loadProgram(result);
      paint_new_program();
   }
}

void WProgramList::paint()
{
   lp.stop();
   menu.paintChangeControls(true);
   paint_new_program();
}

void WProgramList::touch(uint16_t x, uint16_t y)
{
   if (back_button.hit(x, y)) {
      lp.restart();
      menu.setMenu(setup_menu);
   }

   else if (active_button.hit(x, y)) {
      lp.setActiveProgram(lp.getLoadedProgram());
      paint_new_program();
   }

   else if (clear_button.hit(x, y)) {
      menu.setMenu(are_you_sure_menu);
   }

   else if (edit_button.hit(x, y))
      menu.setMenu(basic_edit_program_menu);

   else if (uvb_button.hit(x, y))
      menu.setMenu(uvb_menu);

   else if (rename_button.hit(x, y))
      menu.setMenu(edit_program_name);

   else if (up_slow.hit(x, y))
      edit_value(1);

   else if (up_fast.hit(x, y))
      edit_value(10);

   else if (down_slow.hit(x, y))
      edit_value(-1);

   else if (down_fast.hit(x, y))
      edit_value(-10);
}
