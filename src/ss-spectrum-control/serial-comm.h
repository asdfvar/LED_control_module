#ifndef SERIAL_CONTROL_H
#define SERIAL_CONTROL_H

#include <stdint.h>

enum channels {
   CH_RED,
   CH_WHITE,
   CH_BLUE
};

// current values
extern uint8_t channels[3];

// output values
extern uint8_t output_channels[3];

// init serial comms
void initSerialComms();

// send current value to hardware
void sendManualUpdate();

// send current value to hardware, but repeat any 0/0/0 commands every 15 minutes
void sendProgrammedUpdate();

// h/w work-around: repeat the last command after a few hundred ms
void repeatedUpdatePoll();

// tick, every second, to poll for repeated 0/0/0 commands
void serialCommTick();

// entry point for command interface.
// teensy has two seperate serial ports, so maybe this shouldn't be here.. exactly.
void serial_poll();

#endif /* SERIAL_CONTROL_H */
