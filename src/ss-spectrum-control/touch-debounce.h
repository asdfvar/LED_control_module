#ifndef TOUCH_DEBOUNCE_H
#define TOUCH_DEBOUNCE_H

#include <stdint.h>

/* TODO:
 * for now this is doing simple edge detection
 * but it should avg several samples to get an accurate hit
 * we could also handle long/short presses here too, and/or repeats
 * -RGF
 */

class TouchDebounce {
public:
    void begin(void);
    void hit(uint16_t x, uint16_t y);
    void nohit(void);

    bool touched();
    int x();
    int y();

private:
    bool last_hit;
    bool did_touch;
    int last_x;
    int last_y;
};

#endif /* TOUCH_DEBOUNCE_H */
