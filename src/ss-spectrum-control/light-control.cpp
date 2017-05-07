#include "menu-system.h"
#include "lighting-program.h"

static uint16_t desired_intensity;

static       WLabel CR_intensity       ( 0,    251, 10  );
static       WLabel NL_intensity_label ( 2,    251, 60  );
static       WLabel AL_intensity_label ( 2,    251, 110 );
static const WLabel clear_button       ( 6,     10, 120 );

static void common_proc( void );

void WLightControl::paint()
{
   WLabel::paint(      F("DESIRED INTENSITY" ),  10, 10,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(      F("MEASURED INTENSITY"),  10, 60,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(      F("AL INTENSITY"      ),  100, 110, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   clear_button.paint( F("RESET"),      ILI9341_RED, DARK_COLOR);

   desired_intensity = lp.loadLightControlNew();
   CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE);

   common_proc();

   menu.paintChangeControls();
}

void WLightControl::touch(uint16_t x, uint16_t y)
{
   if ( save_button.hit(x, y) )
   {
      lp.saveLightControlNew( desired_intensity );
      lp.restart();
      menu.setMenu( setup_menu );
   } 
   else if ( clear_button.hit(x, y) )
   {
      // reset the intensity level to zero
      desired_intensity = 0;
      CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
   }
   else if ( up_slow.hit(x, y) )
   {
      // increment by 1
      if ( desired_intensity < 99 ) desired_intensity++;
      CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
   }
   else if ( up_fast.hit(x, y) )
   {
      // increment by 10. Max of 99
      if ( desired_intensity < 89 ) desired_intensity += 10;
      else                  desired_intensity  = 99;
      CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
   }
   else if ( down_fast.hit(x, y) )
   {
      // decrement by 10. Min of 0
      if ( desired_intensity > 9 ) desired_intensity -= 10;
      else                 desired_intensity  = 0;
      CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
   }
   else if ( down_slow.hit(x, y) )
   {
      // decrement by 1. Min of 0
      if ( desired_intensity > 0 ) desired_intensity--;
      CR_intensity.paint_four_digits( desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
   }
}

void WLightControl::tick( void )
{
   common_proc();
}

static void common_proc( void )
{
   uint16_t NL_intensity = lp.read_NL_intensity();
   NL_intensity_label.paint_four_digits( NL_intensity,
                                         ILI9341_BLACK,
                                         ILI9341_WHITE );

   uint16_t AL_intensity = lp.get_AL_intensity();
   AL_intensity_label.paint_four_digits( AL_intensity,
                                         ILI9341_BLACK,
                                         ILI9341_WHITE );
}
