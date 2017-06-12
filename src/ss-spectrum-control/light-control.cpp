#include "menu-system.h"
#include "lighting-program.h"

static uint16_t z_desired_intensity;

static       WLabel CR_intensity       ( 0,    251, 10  );
static       WLabel NL_intensity_label ( 2,    251, 60  );
static       WLabel AL_intensity_label ( 2,    251, 110 );
static const WLabel clear_button       ( 6,     10, 120 );
static       WLabel on_off_button      ( 4,      4,   4 );

static void common_proc( void );

void WLightControl::paint()
{
   WLabel::paint(      F("SET POINT" ),  100, 10,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(      F("MEASURED"),  100, 60,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(      F("AL INTENSITY"      ),  100, 110, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   clear_button.paint( F("RESET"),      ILI9341_RED,   DARK_COLOR);

   if ( lp.get_enable_light_control() )
   {
      on_off_button.paint( F("ON"),        ILI9341_GREEN, DARK_COLOR);
   }
   else
   {
      on_off_button.paint( F("OFF"),        ILI9341_RED, DARK_COLOR);
   }

   z_desired_intensity = lp.getDesiredIntensity();
   CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE);

   common_proc();

   menu.paintChangeControls();
}

void WLightControl::touch(uint16_t x, uint16_t y)
{
   if ( save_button.hit(x, y) )
   {
      lp.saveLightControl( z_desired_intensity );
      lp.saveEnableLightControl( lp.get_enable_light_control() );
      lp.restart();
      menu.setMenu( setup_menu );
   } 
   else if ( clear_button.hit(x, y) )
   {
      // reset the intensity level to zero
      z_desired_intensity = 0;
      CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( up_slow.hit(x, y) )
   {
      // increment by 1
      if ( z_desired_intensity < 99 ) z_desired_intensity++;
      CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( up_fast.hit(x, y) )
   {
      // increment by 10. Max of 99
      if ( z_desired_intensity < 89 ) z_desired_intensity += 10;
      else                  z_desired_intensity  = 99;
      CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( down_fast.hit(x, y) )
   {
      // decrement by 10. Min of 0
      if ( z_desired_intensity > 9 ) z_desired_intensity -= 10;
      else                 z_desired_intensity  = 0;
      CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( down_slow.hit(x, y) )
   {
      // decrement by 1. Min of 0
      if ( z_desired_intensity > 0 ) z_desired_intensity--;
      CR_intensity.paint_two_digits( z_desired_intensity, ILI9341_BLACK, ILI9341_WHITE );
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( on_off_button.hit(x, y) )
   {
      bool current_setting = lp.get_enable_light_control();
      lp.set_enable_light_control( !current_setting );

      if ( !current_setting == true )
      {
         on_off_button.paint( F("ON"),        ILI9341_GREEN, DARK_COLOR);
      }
      else
      {
         on_off_button.paint( F("OFF"),        ILI9341_RED, DARK_COLOR);
      }
   }
}

void WLightControl::tick( void )
{
   common_proc();
}

static void common_proc( void )
{
   uint16_t NL_intensity = lp.get_NL_intensity();
   NL_intensity_label.paint_two_digits( NL_intensity,
                                        ILI9341_BLACK,
                                        ILI9341_WHITE );

   uint16_t AL_intensity = lp.get_AL_intensity();
   AL_intensity_label.paint_two_digits( AL_intensity,
                                        ILI9341_BLACK,
                                        ILI9341_WHITE );
}
