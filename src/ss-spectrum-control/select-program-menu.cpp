#include "menu-system.h"
#include "lighting-program.h"

// what happens if you select an inactive program for a phase?
// what happens if you disable a program, but it is in a phase?
// maximum durations? how many weeks can you set things for?

static const WLabel prog_index(    1, 100, 0);
static const WLabel active_button( 9, 205, 0);

static const WLabel prog_name(     9,  65, 55);

static const WLabel select_button(  10,   20, 110);

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

   select_button.paint(F("SELECT"), ILI9341_GREEN, DARK_COLOR);
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

void WSelectProgram::paint()
{
   up_slow.paint(F("+"), ILI9341_GREEN, DARK_COLOR);
   up_fast.paint(F("+"), ILI9341_GREEN, DARK_COLOR);
   down_slow.paint(F("-"), ILI9341_GREEN, DARK_COLOR);
   down_fast.paint(F("-"), ILI9341_GREEN, DARK_COLOR);
   paint_new_program();
}

void WSelectProgram::touch(uint16_t x, uint16_t y)
{
   if (select_button.hit(x, y)) {
      lp.setPhaseProgram(phase, lp.getLoadedProgram());
      menu.setMenu(set_routine_menu);
   }

   else if (up_slow.hit(x, y))
      edit_value(1);

   else if (up_fast.hit(x, y))
      edit_value(10);

   else if (down_slow.hit(x, y))
      edit_value(-1);

   else if (down_fast.hit(x, y))
      edit_value(-10);
}
