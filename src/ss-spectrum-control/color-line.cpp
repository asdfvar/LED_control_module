
#include "color-line.h"


static WLabel       _on(2,  46*0,  0);
static WLabel _slow_inc(1,  46*1,  0, 3);
static WLabel _fast_inc(1,  46*2,  0, 4);
static WLabel    _value(2,  46*3,  0);
static WLabel _fast_dec(1,  46*4,  0, 4);
static WLabel _slow_dec(1,  46*5,  0, 3);
static WLabel      _off(3,  46*6,  0);

static void set_y(uint16_t y)
{
   _on.y = y;
   _slow_inc.y = y;
   _fast_inc.y = y;
   _value.y = y;
   _fast_dec.y = y;
   _slow_dec.y = y;
   _off.y = y;
}

WColorLine::WColorLine(int idx, uint16_t color, int y)
   : idx(idx), color(color), y(y)
{
}

void WColorLine::paint()
{
   set_y(y);
   _on.paint(       F("ON"), ILI9341_GREEN,  ILI9341_BLACK);
   _slow_inc.paint( F("+"), ILI9341_GREEN,   ILI9341_BLACK);
   _fast_inc.paint( F("+"), ILI9341_GREEN,   ILI9341_BLACK);
   _value.paint(    output_channels[idx],    ILI9341_BLACK, color);
   _fast_dec.paint( F("-"), ILI9341_GREEN,   ILI9341_BLACK);
   _slow_dec.paint( F("-"), ILI9341_GREEN,   ILI9341_BLACK);
   _off.paint(      F("OFF"), ILI9341_GREEN, ILI9341_BLACK);
}

void WColorLine::update()
{
   set_y(y);
   _value.paint(output_channels[idx], ILI9341_BLACK, color);
}

void WColorLine::touch(uint16_t x, uint16_t y)
{
   int updated = output_channels[idx];

   set_y(this->y);

   if (_on.hit(x, y))  {
      updated = 99;
   } else if (_off.hit(x, y)) {
      updated = 0;
   } else if (_slow_inc.hit(x, y)) {
      updated += 1;
   } else if (_fast_inc.hit(x, y)) {
      updated += 10;
   } else if (_fast_dec.hit(x, y)) {
      updated -= 10;
   } else if (_slow_dec.hit(x, y)) {
      updated -= 1;
   }

   if (updated < 0)
      updated = 0;
   if (updated > 99)
      updated = 99;

   if (updated != output_channels[idx]) {
      output_channels[idx] = updated;
      update();
   }
}

