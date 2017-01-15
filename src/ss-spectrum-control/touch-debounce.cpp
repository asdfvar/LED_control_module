
#include "touch-debounce.h"

void TouchDebounce::begin(void)
{
    nohit();
}

void TouchDebounce::hit(uint16_t x, uint16_t y)
{
    last_x = x;
    last_y = y;
    last_hit = true;
}

void TouchDebounce::nohit(void)
{
    did_touch = false;
    last_hit = false;
}

bool TouchDebounce::touched()
{
    if ((did_touch == false) && (last_hit == true))
    {
       did_touch = true;
       return true;
    }
    return false;
}

int TouchDebounce::x()
{
    return last_x;
}

int TouchDebounce::y()
{
    return last_y;
}
