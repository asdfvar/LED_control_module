#include "menu-system.h"
#include "lighting-program.h"

void WSerialActive::paint()
{
   WLabel::paint(F("Serial Active..."), 20, 55, ILI9341_GREEN, ILI9341_BLACK);
}

void WSerialActive::touch(uint16_t x, uint16_t y)
{
}
