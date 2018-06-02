#include "menu-system.h"
#include "lighting-program.h"

static uint16_t z_desired_intensity;

static bool z_AL_setting_edit = false;
static bool z_CR_setting_edit = false;

static       WLabel CR_intensity       ( 0,    251, 10  );
static       WLabel NL_intensity_label ( 2,    251, 60  );
static       WLabel AL_intensity_label ( 2,    251, 110 );
static const WLabel clear_button       ( 6,     10, 120 );
static       WLabel on_off_button      ( 4,      4,   4 );

static void common_proc( void );

void WLightControl::paint()
{
   // desired intensity level
   WLabel::paint(      F("SET POINT"    ), 100, 10,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);

   // measured intensity level
   WLabel::paint(      F("MEASURED"     ), 100, 60,  ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   WLabel::paint(      F("MAX AL LEVEL" ), 100, 110, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
   clear_button.paint( F("RESET"),      ILI9341_RED,   DARK_COLOR);

   if ( lp.get_enable_light_control() )
   {
      on_off_button.paint( F("ON"),   ILI9341_GREEN, DARK_COLOR);
   }
   else
   {
      on_off_button.paint( F("OFF"),  ILI9341_RED, DARK_COLOR);
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
      menu.setMenu( setup_menu );
      z_AL_setting_edit = false;
      z_CR_setting_edit = false;
   } 
   else if ( clear_button.hit(x, y) )
   {
      // reset the intensity level to zero
      z_desired_intensity = 0;
      lp.saveLightControl( z_desired_intensity );
   }
   else if ( up_slow.hit(x, y) )
   {
      // increment by 1
      if ( z_CR_setting_edit )
      {
         z_desired_intensity = min (z_desired_intensity + 1, 99);
         lp.saveLightControl( z_desired_intensity );
      }
      else if (z_AL_setting_edit)
      {
         uint16_t max_light_level = lp.get_max_AL_intensity();
         max_light_level          = min (max_light_level + 1, 99);
         lp.set_max_AL_intensity( max_light_level );
         lp.saveMaxLightControl( max_light_level );
      }
   }
   else if ( up_fast.hit(x, y) )
   {
      // increment by 10. Max of 99
      if ( z_CR_setting_edit )
      {
         z_desired_intensity = min (z_desired_intensity + 10, 99);
         lp.saveLightControl( z_desired_intensity );
      }
      else if (z_AL_setting_edit)
      {
         uint16_t max_light_level = lp.get_max_AL_intensity();
         max_light_level          = min (max_light_level + 10, 99);
         lp.set_max_AL_intensity( max_light_level );
         lp.saveMaxLightControl( max_light_level );
      }
   }
   else if ( down_fast.hit(x, y) )
   {
      // decrement by 10. Min of 0
      if ( z_CR_setting_edit )
      {
         z_desired_intensity = max (z_desired_intensity - 10, 0);
         lp.saveLightControl( z_desired_intensity );
      }
      else if (z_AL_setting_edit)
      {
         uint16_t max_light_level = lp.get_max_AL_intensity();
         max_light_level          = max (max_light_level - 10, 0);
         lp.set_max_AL_intensity( max_light_level );
         lp.saveMaxLightControl( max_light_level );
      }
   }
   else if ( down_slow.hit(x, y) )
   {
      // decrement by 1. Min of 0
      if ( z_CR_setting_edit )
      {
         z_desired_intensity = max (z_desired_intensity - 1, 0);
         lp.saveLightControl( z_desired_intensity );
      }
      else if (z_AL_setting_edit)
      {
         uint16_t max_light_level = lp.get_max_AL_intensity();
         max_light_level          = max (max_light_level - 1, 0);
         lp.set_max_AL_intensity( max_light_level );
         lp.saveMaxLightControl( max_light_level );
      }
   }
   else if ( on_off_button.hit(x, y) )
   {
      bool current_setting = lp.get_enable_light_control();
      lp.set_enable_light_control( !current_setting );

      if ( !current_setting == true )
      {
         on_off_button.paint( F("ON"),  ILI9341_GREEN, DARK_COLOR);
      }
      else
      {
         on_off_button.paint( F("OFF"), ILI9341_RED, DARK_COLOR);
      }
   }
   else if ( AL_intensity_label.hit(x, y) )
   {
      z_CR_setting_edit = false;
      z_AL_setting_edit = !z_AL_setting_edit;
   }
   else if ( CR_intensity.hit(x, y) )
   {
      z_AL_setting_edit = false;
      z_CR_setting_edit = !z_CR_setting_edit;
   }

   if ( !save_button.hit(x, y) )
   {
      // update the screen
      common_proc();
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

   uint16_t max_AL_intensity = lp.get_max_AL_intensity();

   // paint the artificial light max setting
   if (z_AL_setting_edit == false)
   {
      AL_intensity_label.paint_two_digits(
            max_AL_intensity,
            ILI9341_BLACK,
            ILI9341_WHITE );
   } else {
      AL_intensity_label.paint_two_digits(
            max_AL_intensity,
            ILI9341_CYAN,
            ILI9341_WHITE );
   }

   // paint the CR intensity
   if (z_CR_setting_edit == false)
   {
      CR_intensity.paint_two_digits(
            z_desired_intensity,
            ILI9341_BLACK,
            ILI9341_WHITE);
   } else {
      CR_intensity.paint_two_digits(
            z_desired_intensity,
            ILI9341_CYAN,
            ILI9341_WHITE);
   }

}
