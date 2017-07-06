// ugh, crc32? or crc8? bleah. 
#include "menu-system.h"
#include "lighting-program.h"

#define FRAME_START	0xde
#define FRAME_END	0xad

#define START_OFFSET	0
#define LEN_OFFSET	1	
#define CMD_OFFSET	2
#define DATA_OFFSET	3

#define COMMAND_MAX	256

#define CMD_START		0x1
#define CMD_VERSION		0x2
#define CMD_DUMP_PROGRAM	0x3
#define CMD_DUMP_CALENDAR	0x4
#define CMD_ERASE_CALENDAR	0x5
#define CMD_ERASE_PROGRAM	0x6
#define CMD_LOAD_CALENDAR	0x7
#define CMD_LOAD_PROGRAM	0x8

#define CMD_STOP		0xfe
#define CMD_NAK			0xff


static uint8_t buf[COMMAND_MAX];
static uint8_t offset;
static uint8_t pos;
static bool session;

static void start_reply(uint8_t cmd)
{
   buf[0] = FRAME_START;
   buf[2] = cmd;
   pos = DATA_OFFSET;
}

static void push_u8(uint8_t val)
{
   buf[pos++] = val;
}

static void send_reply()
{
   buf[pos++] = FRAME_END;
   buf[1] = pos;
   Serial.write(buf, pos);
   offset = pos = 0;
}


static void send_nak()
{
   start_reply(CMD_NAK);
   send_reply();
}

static void start_session_cmd()
{
   start_reply(CMD_VERSION);
   push_u8(1);
   send_reply();
}

static void stop_session_cmd()
{
   session = 0;
   start_reply(CMD_STOP);
   send_reply();
}

static void dump_program_cmd()
{
   start_reply(CMD_DUMP_PROGRAM);

   send_reply();
}

static void dump_calendar_cmd()
{
}

static void erase_calendar_cmd()
{
}

static void erase_program_cmd()
{
}

static void load_calendar_cmd()
{
}

static void load_program_cmd()
{
}


static void process_command()
{
   switch (buf[CMD_OFFSET]) {
      case CMD_START: start_session_cmd(); return;
      case CMD_DUMP_PROGRAM: dump_program_cmd(); return;
      case CMD_DUMP_CALENDAR: dump_calendar_cmd(); return;
      case CMD_ERASE_CALENDAR: erase_calendar_cmd(); return;
      case CMD_ERASE_PROGRAM: erase_program_cmd(); return;
      case CMD_LOAD_CALENDAR: load_calendar_cmd(); return;
      case CMD_LOAD_PROGRAM: load_program_cmd(); return;
      case CMD_STOP: stop_session_cmd(); return;
      default:
                     send_nak();
                     break;
   }
}

static bool valid_command()
{
   if (buf[0] != FRAME_START)
      return false;
   if (buf[1] > pos)
      return false;

   uint8_t len = buf[1];
   if (buf[len - 1] != FRAME_END)
      return false;

   offset = 0;
   return true;
}

static bool __serial_poll()
{
   int r;

   while (-1 != (r = Serial.read())) {
      if ((pos == 0) && (r != FRAME_START))
         continue;
      buf[pos++] = r;
      if (valid_command())
         return true;
      if (pos == COMMAND_MAX)
         pos = 0;
   }
   return false;
}

#define TMO	(5 * 1000)

static void serial_loop()
{
   long start = millis();
   long n;

   session = 1;
   menu.setMenu(serial_active_menu);
   lp.stop();

   do {
      if (__serial_poll()) {
         process_command();
         start = millis(); 
      }
      n = millis();
   } while (session && ((start + TMO) > n));

   lp.restart();
   menu.setMenu(main_menu);
}

void serial_poll()
{
   if (menu.isMainMenu()) {
      if (__serial_poll()) {
         process_command();
         serial_loop();
      }
   } else {
      pos = 0;
      /* flush serial rx buffer:
       * if you tried to connect via the app, we don't want to see stale messages later..
       * TODO: maybe instead we should NAK so the app can tell the user we're busy?
       */
      while (-1 != Serial.read())
         ;
   }
}

