
#include <Arduino.h>

#include "serial-comm.h"
#include "lighting-program.h"

// how long do we wait between repeating programmed 0/0/0 commands?
#define ALL_OFF_REPEAT_DURATION_SECONDS		(15 * 60)

// Serial port used for lighting comms
#define Lights	Serial1

// current values
uint8_t channels[3];

// last sent values
static uint8_t last_channels[3];

// is last_channels valid yet?
static bool last_valid;

// current value, string version!
static char output[32];

// how long to delay between the repeated start, ms
static const int repeat_delay = 300;

// when we should send the next one
static unsigned long after_time; 

// for programmed 0/0/0, repeat every 15 minutes
static uint16_t repeat_programmed_off_seconds;

static bool channels_are_all_off(void)
{
    if ((channels[0] == 0) && (channels[1] == 0) && (channels[2] == 0))
	return true;
    return false;
}

static void raw_send_update()
{
    Lights.print (output);
    after_time = millis() + repeat_delay;
}

static void sendUpdate(uint16_t repeat_programmed_off)
{
    if (!last_valid || memcmp(last_channels, channels, 3)) {
	snprintf(output, sizeof(output), "ABC..NextLevel:%d:%d:%d:00\r\n",
		channels[CH_RED], channels[CH_WHITE], channels[CH_BLUE]);

	memcpy(last_channels, channels, 3);
	raw_send_update();
	last_valid = true;
	repeat_programmed_off_seconds = 0;
	if (channels_are_all_off())
	    repeat_programmed_off_seconds = repeat_programmed_off;
    }
}

void sendManualUpdate()
{
    sendUpdate(0);
}

void sendProgrammedUpdate()
{
    sendUpdate(ALL_OFF_REPEAT_DURATION_SECONDS);
}

void repeatedUpdatePoll()
{
    if (after_time)
    {
	if (millis() > after_time)
	{
	    Lights.print (output);
	    after_time = 0;
	}
    }
}

void serialCommTick()
{
    if (repeat_programmed_off_seconds)
    {
	--repeat_programmed_off_seconds;
	if (repeat_programmed_off_seconds == 0)
	{
	    raw_send_update();
	    repeat_programmed_off_seconds = ALL_OFF_REPEAT_DURATION_SECONDS;
	}
    }
}

void initSerialComms()
{
    Lights.begin(1200);
}
