#ifndef MENU_SYSTEM_H
#define MENU_SYSTEM_H

#include "RTClib.h"
#include "menu.h"

class MenuSystem
{
   public:
      void setMenu(WMenuBase& menu);
      void prevMenu(void);
      void paintChangeControls(bool back=false) const;
      bool isMainMenu() const;

      inline void begin(void)
      {
         setMenu( main_menu );
      }

      inline void tick()
      {
         current->tick();
      }

      inline void touch(uint16_t x, uint16_t y)
      {
         current->touch(x, y);
      }

   private:
      WMenuBase *current;
      WMenuBase *previous;
};

extern MenuSystem menu;
extern WLabel     back_button;
extern WLabel     save_button;
extern WLabel     home_button;
extern WLabel     up_fast;
extern WLabel     up_slow;
extern WLabel     down_slow;
extern WLabel     down_fast;

#endif /* MENU_SYSTEM_H */
