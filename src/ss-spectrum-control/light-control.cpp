#include "menu-system.h"
#include "lighting-program.h"

static uint16_t intensity;

static       WLabel intensity_label( 2,    201,  20 );
static const WLabel clear_button(    6,     10, 100 );

void WLightControl::paint()
{
    lp.stop();

    WLabel::paint(      F("INTENSITY"),  5, 26, ILI9341_GREEN, ILI9341_BLACK, 2, 0);
    clear_button.paint( F("RESET"),      ILI9341_RED, DARK_COLOR);

    intensity = lp.loadLightControlNew();
    intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);

    menu.paintChangeControls();
}

void WLightControl::touch(uint16_t x, uint16_t y)
{
    if ( save_button.hit(x, y) )
    {
        lp.saveLightControlNew( intensity );
        lp.restart();
        menu.setMenu( setup_menu );
    } 
    else if ( clear_button.hit(x, y) )
    {
       // reset the intensity level to zero
       intensity = 0;
       intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);
    }
    else if ( up_slow.hit(x, y) )
    {
       // increment by 1
       if ( intensity < 2999 ) intensity++;
       intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);
    }
    else if ( up_fast.hit(x, y) )
    {
       // increment by 10
       if ( intensity < 2990 ) intensity += 10;
       else                    intensity  = 99;
       intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);
    }
    else if ( down_fast.hit(x, y) )
    {
       // decrement by 10
       if ( intensity > 9 ) intensity -= 10;
       else                 intensity  = 0;
       intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);
    }
    else if ( down_slow.hit(x, y) )
    {
       // decrement by 1
       if ( intensity > 0 ) intensity--;
       intensity_label.paint_four_digits( intensity, ILI9341_BLACK, ILI9341_WHITE);
    }
}
