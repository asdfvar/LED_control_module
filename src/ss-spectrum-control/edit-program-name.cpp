#include "menu-system.h"
#include "lighting-program.h"

#define SZ	52

#define NKEYS	((2 * 6) + 4)

WLabel keypad[NKEYS] = {
   { 3,  SZ*0,  60 },  // F("ABC")   0
   { 3,  SZ*1,  60 },  // F("DEF")   1
   { 3,  SZ*2,  60 },  // F("GHI")   2
   { 3,  SZ*3,  60 },  // F("JKL")   3
   { 3,  SZ*4,  60 },  // F("MNO")   4
   { 3,  SZ*5,  60 },  // F("PQR")   5

   { 3,  SZ*0, 120 },  // F("STU")   6
   { 3,  SZ*1, 120 },  // F("VWX")   7
   { 3,  SZ*2, 120 },  // F("YZ"),   8
   { 3,  SZ*3, 120 },  // F("0"),    9
   { 3,  SZ*4, 120 },  // F("123"), 10
   { 3,  SZ*5, 120 },  // F("456"), 11

   { 3,  SZ*2, 180 },  // F("789"), 12 
   { 3,  SZ*3, 180 },  // F("<<"),  13
   { 3,  SZ*4, 180 },  // F("DEL")  14
   { 3,  SZ*5, 180 },  // F(">>")   15
};

static char name[PROGRAM_NAME_LEN + 1];
static uint8_t cursor_position;
static uint8_t press_timeout;
static uint8_t last_key;

static uint8_t cycle_start;
static uint8_t cycle_length;
static uint8_t cycle_pos;

static uint8_t ch_to_x(int i)
{
   return 20 + (i * 30);
}

static void paint_cursor(uint16_t color)
{
   tft.fillRect( ch_to_x(cursor_position),   // x
         42,
         25,	                  // w
         4,
         color);
}

static void move_cursor(int direction)
{
   int next = direction + cursor_position;

   press_timeout = 0;
   paint_cursor(DARK_COLOR);
   if (next < 0)
      next = 0;
   if (next >= PROGRAM_NAME_LEN)
      next = (PROGRAM_NAME_LEN - 1);
   cursor_position = next;
   paint_cursor(ILI9341_RED);
}

static void paint_at(uint8_t i)
{
   WLabel::paint(name[i],   ch_to_x(i), 0, ILI9341_GREEN, DARK_COLOR, 3, 25);
}

static void paint_initial()
{
   const char *n = lp.getLoadedProgramName();
   uint8_t l = strlen(n);

   for (int i = 0; i < PROGRAM_NAME_LEN; i++) {
      name[i] = (i < l) ? n[i] : ' ';
      paint_at(i);
   }
   name[PROGRAM_NAME_LEN] = '\0';
   cursor_position = 0;
   paint_cursor(ILI9341_RED);
}

static void del_at()
{
   name[cursor_position] = ' ';
   paint_at(cursor_position);
   paint_cursor(ILI9341_RED);
}

static void info(uint8_t key)
{
   switch (key) {
      case  0: cycle_start = 'A'; cycle_length = cycle_pos = 3; break;
      case  1: cycle_start = 'D'; cycle_length = cycle_pos = 3; break;
      case  2: cycle_start = 'G'; cycle_length = cycle_pos = 3; break;
      case  3: cycle_start = 'J'; cycle_length = cycle_pos = 3; break;
      case  4: cycle_start = 'M'; cycle_length = cycle_pos = 3; break;
      case  5: cycle_start = 'P'; cycle_length = cycle_pos = 3; break;
      case  6: cycle_start = 'S'; cycle_length = cycle_pos = 3; break;
      case  7: cycle_start = 'V'; cycle_length = cycle_pos = 3; break;
      case  8: cycle_start = 'Y'; cycle_length = cycle_pos = 2; break;
      case  9: cycle_start = '0'; cycle_length = cycle_pos = 1; break;
      case 10: cycle_start = '1'; cycle_length = cycle_pos = 3; break;
      case 11: cycle_start = '4'; cycle_length = cycle_pos = 3; break;
      case 12: cycle_start = '7'; cycle_length = cycle_pos = 3; break;
   }
}

static void cycle_button()
{
   cycle_pos += 1;
   if (cycle_pos >= cycle_length)
      cycle_pos = 0;
   name[cursor_position] = cycle_start + cycle_pos;
   paint_at(cursor_position);
   paint_cursor(ILI9341_YELLOW);
}

static void start_cycle(uint8_t key)
{
   info(key);
   cycle_button();
}

static void keyboard_hit(uint8_t key)
{
   if (press_timeout) {
      if (key == last_key) {
         cycle_button();
      } else {
         move_cursor(1);
         start_cycle(key);
      }
   } else {
      start_cycle(key);
   }
   last_key = key;
   press_timeout = 2;
}

void WEditProgramName::tick()
{
   if (press_timeout) {
      --press_timeout;
      if (press_timeout == 0) {
         move_cursor(1);
      }
   }
}

void WEditProgramName::paint()
{
   const __FlashStringHelper *list[NKEYS] = {
      F("ABC"),
      F("DEF"),
      F("GHI"),
      F("JKL"),
      F("MNO"),
      F("PQR"),

      F("STU"),
      F("VWX"),
      F("YZ"),
      F("0"),  
      F("123"),
      F("456"),

      F("789"),
      F("<<"),
      F("DEL"),
      F(">>")  
   };


   paint_initial();
   for (int i = 0; i < NKEYS; i++)
      keypad[i].paint(list[i], ILI9341_GREEN, DARK_COLOR);
   save_button.paint(F("SAVE"), ILI9341_GREEN, DARK_COLOR);
}

void WEditProgramName::touch(uint16_t x, uint16_t y)
{
   if (save_button.hit(x, y))
   {
      lp.setLoadedProgramName(name);
      lp.saveProgram();
      menu.setMenu(program_list_menu);
   }
   else if (keypad[13].hit(x, y))
      move_cursor(-1);
   else if (keypad[14].hit(x, y))
      del_at();
   else if (keypad[15].hit(x, y))
      move_cursor(1);
   else
   {
      for (int i = 0; i < 13; i++)
      {
         if (keypad[i].hit(x, y))
         {
            keyboard_hit(i);
            break;
         }
      }
   }
}
