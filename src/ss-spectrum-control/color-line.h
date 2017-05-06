#ifndef COLOR_LINE_H
#define COLOR_LINE_H

#include "menu-system.h"
#include "serial-comm.h"

class WColorLine : public WMenuBase {

   public:
      WColorLine(int idx, uint16_t color, int y);
      virtual void paint();
      virtual void touch(uint16_t x, uint16_t y);
      void update();

   private:
      uint8_t idx;
      uint16_t color;
      uint16_t y;
};

#endif /* COLOR_LINE_H */
