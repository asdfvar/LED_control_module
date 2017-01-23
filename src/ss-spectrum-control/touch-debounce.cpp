
#include "touch-debounce.h"

void TouchDebounce::begin(void)
{
    nohit();
}

void TouchDebounce::hit(uint16_t x, uint16_t y)
{
    last_x   = x;
    last_y   = y;
    last_hit = true;

    if (index < MAX_INDEX)
    {
       accum_x[index] = (int16_t)x;
       accum_y[index] = (int16_t)y;
       index++;
    }
}

bool TouchDebounce::long_hit( void )
{
    if (index < MAX_INDEX) return false;

    // Reset the index
    index = 0;

    int16_t min_x = accum_x[0];
    int16_t min_y = accum_y[0];
    int16_t max_x = accum_x[0];
    int16_t max_y = accum_y[0];

    // Find the bounds where the screen has been touched
    for (int k = 1; k < MAX_INDEX; k++)
    {
       if (min_x > accum_x[k]) min_x = accum_x[k];
       if (max_x < accum_x[k]) max_x = accum_x[k];
       if (min_y > accum_y[k]) min_y = accum_y[k];
       if (max_y < accum_y[k]) max_y = accum_y[k];
    }

    // Determine if the bounds fall within tolerance
    if ((max_x - min_x < 4) && (max_y - min_y < 4))
    {
       return touched();
    }

    return false;
//return touched();
}

void TouchDebounce::nohit(void)
{
    did_touch = false;
    last_hit  = false;
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
