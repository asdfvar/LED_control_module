#include <stdlib.h>

#include "lighting-program.h"
#include "serial-comm.h"
#include "EEPROM.h"
#include "crc32.h"

#define HWSERIAL Serial1

/* eeprom offsets */
#define EEPROM_SIZE      0x800
#define CRC_LENGTH       4

#define SETTINGS_OFFSET      (EEPROM_SIZE     - CRC_LENGTH - sizeof( struct program_settings ))
#define CALENDAR_OFFSET      (SETTINGS_OFFSET - CRC_LENGTH - sizeof( struct calendar         ))
#define LIGHT_CONTROL_OFFSET (CALENDAR_OFFSET - CRC_LENGTH - sizeof( uint16_t                ))
#define MAX_LIGHT_CONTROL_OFFSET (LIGHT_CONTROL_OFFSET - CRC_LENGTH - sizeof( uint16_t       ))
#define ENABLE_LIGHT_CONTROL_OFFSET (MAX_LIGHT_CONTROL_OFFSET - CRC_LENGTH - sizeof( uint16_t    ))
#define UVB_ON_TIME_OFFSET (ENABLE_LIGHT_CONTROL_OFFSET - CRC_LENGTH - sizeof( struct program_step    ))
#define UVB_OFF_TIME_OFFSET (UVB_ON_TIME_OFFSET - CRC_LENGTH - sizeof( struct program_step   ))

uint16_t LightingProgram::to_minutes(const struct program_step *s)
{
   return (s->hour * 60) + s->minute;
}

void LightingProgram::from_minutes(uint8_t *hours, uint8_t *minutes, uint16_t m)
{
   if (m >= 1440)
   {
      m = 1439;
   }

   *hours = 0;

   while (m >= 60)
   {
      ++(*hours);
      m -= 60;
   }
   *minutes = m;
}

void LightingProgram::from_minutes(struct program_step *s, uint16_t m)
{
   LightingProgram::from_minutes(&s->hour, &s->minute, m);
}

static bool time_before(const struct program_step *s, uint8_t h, uint8_t m)
{
   if (h == s->hour)
   {
      return (m < s->minute);
   }
   return (h < s->hour);
}

uint8_t LightingProgram::valid_step_before(uint8_t s)
{
   for (int i = s - 1; i >= 0; i--)
   {
      if (program.steps[i].active)
      {
         return i;
      }
   }
   return 0; /* step 0 is always active! */
}

uint8_t LightingProgram::find(const DateTime& now)
{
   uint8_t h = now.hour();
   uint8_t m = now.minute();

   // check for fade == 0, off == on, basic program, always use on time/color.
   if ((getFadeDuration() == 0) && (!isAdvancedProgram()))
   {
      if ((program.steps[0].hour == program.steps[PROGRAM_STEPS - 1].hour) &&
            (program.steps[0].minute == program.steps[PROGRAM_STEPS - 1].minute))
      {
         return 0;
      }
   }

   for (int i = 0; i < PROGRAM_STEPS; i++)
   {
      const struct program_step *s = program.steps + i;

      if (!s->active)
      {
         continue;
      }
      if (time_before(s, h, m))
      {
         if (i == 0) return valid_step_before(PROGRAM_STEPS);
         return valid_step_before(i);
      }
   }
   return valid_step_before(PROGRAM_STEPS);
}

void LightingProgram::initialVeg()
{
   snprintf(program.name, sizeof(program.name), "Veg 1");

   program.steps[0].active = 1;
   program.steps[0].hour   = 5;
   program.steps[0].minute = 0;
   program.steps[0].r      = 50;
   program.steps[0].wh     = 50;
   program.steps[0].b      = 99;

   program.steps[PROGRAM_STEPS - 1].active = 1;
   program.steps[PROGRAM_STEPS - 1].hour   = 23;
   program.steps[PROGRAM_STEPS - 1].minute = 0;
   program.steps[PROGRAM_STEPS - 1].r      = 0;
   program.steps[PROGRAM_STEPS - 1].wh     = 0;
   program.steps[PROGRAM_STEPS - 1].b      = 0;
}

void LightingProgram::initialBloom()
{
   snprintf(program.name, sizeof(program.name), "Bloom 1");

   program.steps[0].active = 1;
   program.steps[0].hour   = 8;
   program.steps[0].minute = 0;
   program.steps[0].r      = 99;
   program.steps[0].wh     = 99;
   program.steps[0].b      = 99;

   program.steps[PROGRAM_STEPS - 1].active = 1;
   program.steps[PROGRAM_STEPS - 1].hour   = 20;
   program.steps[PROGRAM_STEPS - 1].minute = 0;
   program.steps[PROGRAM_STEPS - 1].r      = 0;
   program.steps[PROGRAM_STEPS - 1].wh     = 0;
   program.steps[PROGRAM_STEPS - 1].b      = 0;
}

void LightingProgram::resetProgram(bool initial)
{
   memset(program.steps, 0, sizeof(program.steps));

   snprintf(program.name, sizeof(program.name), "PROG %c  ", 'A' + loaded_program);
   program.steps[0].active = 1;
   program.steps[PROGRAM_STEPS - 1].active = 1;

   if (initial)
   {
      switch (loaded_program)
      {
         case 1:
            initialVeg();
            break;
         case 2:
            initialBloom();
            break;
      }
   }
}

/* ll read loop */
static void readEEPBytes(uint16_t offset, void *raw, size_t len)
{
   uint8_t *ptr = (uint8_t *)raw;
   for (size_t i = 0; i < len; i++)
   {
      ptr[i] = EEPROM.read(offset + i);
   }
}

/* load.. last four bytes are crc32. return if valid / not */
static bool loadEEPBytes(uint16_t offset, void *ptr, size_t len)
{
   uint32_t crc;

   readEEPBytes(offset, ptr, len);
   readEEPBytes(offset + len, &crc, 4);

   uint32_t calc = crc32((const uint8_t *)ptr, len);

   if (calc == crc) 
   {
      return true;
   }

   return false;
}

/* ll save, don't calc crc, just write data */
static void updateEEPBytes(uint16_t offset, const void *raw, size_t len)
{
   uint8_t *ptr = (uint8_t *)raw;
   for (size_t i = 0; i < len; i++)
   {
      EEPROM.update(offset + i, ptr[i]);
   }
}

/* save.. last four bytes are crc32, calculated here */
static void saveEEPBytes(uint16_t offset, const void *ptr, size_t len)
{
   uint32_t crc = crc32((const uint8_t *)ptr, len);

   updateEEPBytes(offset, ptr, len);
   updateEEPBytes(offset + len, &crc, 4);
}

uint16_t LightingProgram::offsetOfProgram(uint8_t index) const
{
   return ((sizeof(struct program) + CRC_LENGTH) * index);
}

void LightingProgram::loadCalendar()
{
   if (!loadEEPBytes(CALENDAR_OFFSET, &cal, sizeof(cal)))
   {
      memset(&cal, 0, sizeof(cal));
   }
}

void LightingProgram::saveCalendar()
{
   saveEEPBytes(CALENDAR_OFFSET, &cal, sizeof(cal));
}

void LightingProgram::saveLightControl( uint16_t light_level )
{
   saveEEPBytes(LIGHT_CONTROL_OFFSET, &light_level, sizeof(light_level));
   desired_intensity = light_level;
}

void LightingProgram::saveMaxLightControl( uint16_t light_level )
{
   saveEEPBytes(MAX_LIGHT_CONTROL_OFFSET, &light_level, sizeof(light_level));
   desired_intensity = light_level;
}

void LightingProgram::saveUVBonTime (struct program_step step)
{
   saveEEPBytes(UVB_ON_TIME_OFFSET, &step, sizeof(step));
}

struct program_step LightingProgram::loadUVBonTime (void)
{
   struct program_step step;
   loadEEPBytes (UVB_ON_TIME_OFFSET, &step, sizeof(step));

   return step;
}

void LightingProgram::saveUVBoffTime (struct program_step step)
{
   saveEEPBytes(UVB_OFF_TIME_OFFSET, &step, sizeof(step));
}

struct program_step LightingProgram::loadUVBoffTime (void)
{
   struct program_step step;
   loadEEPBytes (UVB_OFF_TIME_OFFSET, &step, sizeof(step));

   return step;
}

void LightingProgram::saveEnableLightControl( bool enabled )
{
   uint16_t enabled_setting = 0;
   if ( enabled )
   {
      enabled_setting = 1;
   }
   else
   {
      enabled_setting = 0;
   }

   saveEEPBytes(ENABLE_LIGHT_CONTROL_OFFSET, &enabled_setting, sizeof(enabled_setting));

}

uint16_t LightingProgram::getDesiredIntensity( void )
{
   return desired_intensity;
}

// Called upon initiation
uint16_t LightingProgram::loadLightControlSettings( void )
{
   // TODO: can this modify the private variable directly?
   // TODO: move this to its own private function
   uint16_t light_level = 0;
   if (!loadEEPBytes(LIGHT_CONTROL_OFFSET, &light_level, sizeof(light_level)))
   {
      light_level = 0;
   }

   uint16_t max_light_level = 0;
   if (!loadEEPBytes(MAX_LIGHT_CONTROL_OFFSET, &max_light_level, sizeof(max_light_level)))
   {
      max_light_level = 0;
   }

   desired_intensity = light_level;
   max_AL_intensity  = max_light_level;

   read_NL_intensity();

   if ( desired_intensity > NL_intensity && NL_intensity > 0)
   {
      AL_intensity = desired_intensity - NL_intensity;
   } else {
      AL_intensity = 0;
   }

   uint16_t enabled_setting = 0;
   if (!loadEEPBytes(ENABLE_LIGHT_CONTROL_OFFSET, &enabled_setting, sizeof(enabled_setting)))
   {
      enabled_setting = 0;
   }

   if ( enabled_setting == 1 )
   {
      enable_light_control = true;
   }
   else
   {
      enable_light_control = false;
   }

   return light_level;
}

void LightingProgram::loadSettings()
{
   if (!loadEEPBytes(SETTINGS_OFFSET, &settings, sizeof(settings)))
   {
      settings.fade_duration_minutes = 0;
      settings.active_program = 0;
   }
}

void LightingProgram::saveSettings()
{
   saveEEPBytes(SETTINGS_OFFSET, &settings, sizeof(settings));
}

void LightingProgram::setActiveProgram(uint8_t index)
{
   settings.active_program = index;
   saveSettings();
}

void LightingProgram::setFadeDuration(uint8_t minutes)
{
   if (settings.fade_duration_minutes != minutes)
   {
      bool need_recalculate = (minutes > settings.fade_duration_minutes);
      settings.fade_duration_minutes = minutes;
      if (need_recalculate)
      {
         recalculate(0);
      }
      saveSettings();
   }
}

// change .. to make time run faster. fer testin', y'know?
#define DIVIDER            SECONDS_PER_DAY

void LightingProgram::setCycleTime(uint8_t weeks, uint8_t days)
{
   long nd = (now.secondstime() / DIVIDER);
   long td = ((weeks * 7) + days);
   cal.zeroDays = nd - td;
}

bool LightingProgram::getCycleTime(uint32_t& days) const
{
   long nd = (now.secondstime() / DIVIDER);
   days = nd - cal.zeroDays;
   return calendar_enabled;
}

bool LightingProgram::getCycleTime(uint8_t& weeks, uint8_t& days) const
{
   uint32_t nd;
   getCycleTime(nd);

   weeks = nd / 7;
   days = nd % 7;
   return calendar_enabled;
}

void LightingProgram::loadProgram(uint8_t index)
{
   uint16_t offset = offsetOfProgram(index);

   loaded_program = index;
   if (!loadEEPBytes(offset, &program, sizeof(struct program)))
   {
      resetProgram(true);
   }
   recalculate(0);
}

void LightingProgram::saveProgram(void)
{
   uint16_t offset = offsetOfProgram(loaded_program);
   saveEEPBytes(offset, &program, sizeof(struct program));
}

bool LightingProgram::findActivePhase(uint8_t& phase)
{
   uint32_t next_start = 0;
   uint32_t cycle_day;

   getCycleTime(cycle_day);

   for (int i = 0; i < NPHASES; i++)
   {
      const struct phase *p = getPhase(i);

      if (p->active)
      {
         next_start += p->days;
         if (cycle_day < next_start)
         {
            phase = i;
            return true;
         }
      }
   }
   return false;
}

/* add some minutes to starth/startm. true if we overflow */
static bool add_time(uint8_t *h, uint8_t *m, uint8_t minutes, uint8_t sh, uint8_t sm)
{
   sm += minutes;
   if (sm >= 60)
   {
      sm -= 60;
      sh += 1;
   }
   if (((sh == 23) && (sm >= 59)) || (sh >= 24))
   {
      *h = 23;
      *m = 59;
      return true;
   }
   *h = sh;
   *m = sm;
   return false;
}

bool LightingProgram::step_time_overflows(const struct program_step *step) const
{
   uint8_t end_hours, end_minutes;
   return add_time( &end_hours,
                    &end_minutes,
                     settings.fade_duration_minutes,
                     step->hour,
                     step->minute );
}


void LightingProgram::recalculate_step(uint8_t step)
{
   if (step == 0)
   {
      ; /* do nothing, step zero is always considered valid */
   } else {
      struct program_step *prev = program.steps + step - 1;
      struct program_step *s    = program.steps + step;

      uint8_t end_hours, end_minutes;

      if (add_time( &end_hours,
                    &end_minutes,
                     settings.fade_duration_minutes,
                     prev->hour,
                     prev->minute))
      {
         prev->active = false;
         s->active    = false;
      }
      if (time_before(s, end_hours, end_minutes)) return;
      s->hour   = end_hours;
      s->minute = end_minutes;
   }
}

// starting at step, recalculate all starting times 
// depending on fade duration and such
void LightingProgram::recalculate(uint8_t step)
{
   bool advanced = false;

   // this is wrong
   // but for now advanced and basic rules clash

   if (!program.steps[PROGRAM_STEPS - 1].active)
   {
      advanced = true;
   } else {
      for (int i = 1; i < PROGRAM_STEPS - 1; i++)
      {
         if (program.steps[i].active)
         {
            advanced = true;
            break;
         }
      } 
   }

   if (!advanced) return;

   for ( ; step < PROGRAM_STEPS; step++) recalculate_step(step);
}

struct program_step *LightingProgram::startEditing(uint8_t step, uint8_t *minH, uint8_t *minM)
{
   struct program_step *s = program.steps + step;

   if (step == 0)
   {
      *minH = *minM = 0;
   } else {
      uint8_t pi = valid_step_before(step);
      struct program_step *p = program.steps + pi;
      add_time(minH, minM, settings.fade_duration_minutes, p->hour, p->minute);
   }
   return s;
}

void LightingProgram::forceStep()
{
   const struct program_step *s = program.steps + current_step;
   channels[CH_RED]   = s->r;
   channels[CH_WHITE] = s->wh;
   channels[CH_BLUE]  = s->b;
   sendProgrammedUpdate();
}

void LightingProgram::begin()
{

   // start the real-time clock
   rtc.begin();

   // update the current time based on the real-time clock
   update_time (true);

   if (sizeof(program) > EEPROM.length())
   {
      Serial.println("woops");
      Serial.println(sizeof(program));
      Serial.println(EEPROM.length());
   }

   // initiate the last update time
   last_AL_update_time = now.secondstime();

   loadCalendar();
   loadLightControlSettings();
   loadSettings();

   restart();

   output_channels[CH_RED]   = channels[CH_RED];
   output_channels[CH_WHITE] = channels[CH_WHITE];
   output_channels[CH_BLUE]  = channels[CH_BLUE];

   sendProgrammedUpdate();
}

void LightingProgram::restart()
{
   if (findActivePhase(current_phase))
   {
      calendar_enabled = true;
      loadProgram(getPhase(current_phase)->program);
   } else {
      calendar_enabled = false;
      loadProgram(settings.active_program);
   }

   current_step = find(now);
   fade_steps_left = 0;
   enabled = true;
   forceStep();

   // update the time based on the real-time clock
   update_time (true);

   last_AL_update_time = now.secondstime();
}

void LightingProgram::stop(void)
{
   enabled = false;
}

long LightingProgram::delta_t()
{
   long nn = now.secondstime();
   long delta = nn - lasttime;
   return delta;
}

static float color_step(uint8_t from, uint8_t to)
{
   float f = from;
   float t = to;
   return ((t - f) / 10.0);
}

void LightingProgram::run_step()
{
   --fade_steps_left;
   lasttime = now.secondstime();

   color_value[CH_RED]   += color_delta[CH_RED];
   color_value[CH_WHITE] += color_delta[CH_WHITE];
   color_value[CH_BLUE]  += color_delta[CH_BLUE];

   channels[CH_RED]   = roundf(color_value[CH_RED]);
   channels[CH_WHITE] = roundf(color_value[CH_WHITE]);
   channels[CH_BLUE]  = roundf(color_value[CH_BLUE]); 

   sendProgrammedUpdate();
}

// read and return natural light level reading
int LightingProgram::read_NL_intensity( void )
{

  // code for reading in natural light level here between 0->99
  String content = "";
 
  // Format: "LightLevel:XY;"
  while(HWSERIAL.available())
  {
     char character = HWSERIAL.read();
     content.concat(character);
  }

  if (content != "")
  {
     // make attempts to find the desired string pattern
     for (int ind = 0; ind < 40; ind++)
     {
        String head    = content.substring( ind, ind + 11 );
        char term_dig1 = content.charAt( ind + 12 );
        char term_dig2 = content.charAt( ind + 13 );

        if ( head.equals("LightLevel:") && term_dig1 == ';' )
        {
           String str_number = content.substring( ind + 11 );
           NL_intensity = str_number.toInt();
           break;
        }
        else if ( head.equals("LightLevel:") && term_dig2 == ';' )
        {
           String str_number = content.substring( ind + 11, ind + 13 );
           NL_intensity = str_number.toInt();
           break;
        }
     }
  }

  return NL_intensity;
}

uint16_t LightingProgram::get_NL_intensity( void )
{
   return NL_intensity;
}

uint16_t LightingProgram::get_AL_intensity( void )
{
   return AL_intensity;
}

uint16_t LightingProgram::get_max_AL_intensity( void )
{
   return max_AL_intensity;
}

void LightingProgram::set_max_AL_intensity( uint16_t value )
{
   max_AL_intensity = value;
}

bool LightingProgram::get_enable_light_control( void )
{
   return enable_light_control;
}

bool LightingProgram::set_enable_light_control( bool setting )
{
   enable_light_control = setting;
   return setting;
}

void LightingProgram::tick()
{

   // update the time based on the internal clock
   update_time (false);

   if (enabled == false)
      return;

   if (calendar_enabled)
   {
      uint8_t next_phase;
      if (findActivePhase(next_phase))
      {
         if (next_phase != current_phase)
         {
            current_phase = next_phase;
            loadProgram(getPhase(current_phase)->program);
            current_step = 0xff; // force us to fade to new step.
         }
      } else {
         calendar_enabled = false;
      }
   }

   uint8_t next = find(now);

   if (next != current_step)
   {
      current_step = next;
      if (settings.fade_duration_minutes == 0)
      {
         forceStep();
      } else {
         const struct program_step *step = program.steps + current_step;

         fade_steps_left = 10;
         time_delta = (settings.fade_duration_minutes * 60) / 10;
         color_delta[CH_RED]   = color_step(channels[CH_RED], step->r);
         color_delta[CH_WHITE] = color_step(channels[CH_WHITE], step->wh);
         color_delta[CH_BLUE]  = color_step(channels[CH_BLUE], step->b);

         color_value[CH_RED]   = channels[CH_RED];
         color_value[CH_WHITE] = channels[CH_WHITE];
         color_value[CH_BLUE]  = channels[CH_BLUE];
         run_step();
      }
   }
   else if (fade_steps_left)
   {
      long dt = delta_t();

      if (dt >= time_delta)
      {
         if (fade_steps_left == 1)
         {
            forceStep();
            fade_steps_left = 0;
         } else {
            run_step();
         }
      }
   }

   // updates to perform at specified time increments
   const int update_delay = 5;
   if ( now.secondstime() >= last_AL_update_time + update_delay)
   {
      last_AL_update_time = now.secondstime();

      // read the natural light-level intensity
      read_NL_intensity(); 

      if ( desired_intensity > NL_intensity && NL_intensity > 0)
      {
         AL_intensity = desired_intensity - NL_intensity;
      } else {
         AL_intensity = 0;
      }

      // modify the output color channels if the NL intensity control is being used
      if ((channels[CH_RED] > 0 || channels[CH_WHITE] > 0 || channels[CH_BLUE] > 0) &&
           NL_intensity > 0                                                         &&
           enable_light_control )
      {

         // unit vector specifying the relation of RWB intensities
         const float unit_AL_mapping[3]  = { 0.81681f, 0.40793f, 0.40793f };

         // max possible values for the output channels which are bound above
         // by the current channel settings
         const float max_output_channels[3] = { (float)channels[CH_RED],
                                                (float)channels[CH_WHITE],
                                                (float)channels[CH_BLUE] };

         float k = ((float)(max_AL_intensity)) / (max_output_channels[0] * unit_AL_mapping[0] +
                                                  max_output_channels[1] * unit_AL_mapping[1] +
                                                  max_output_channels[2] * unit_AL_mapping[2]);

         // artificial-light mapping. Scaled for [0, 99] -> [0, max AL intensity]
         float AL_mapping[3] = { unit_AL_mapping[0] * k,
                                 unit_AL_mapping[1] * k,
                                 unit_AL_mapping[2] * k };

         int sum_sqr_channels = (channels[CH_RED]   * channels[CH_RED]   +
                                 channels[CH_WHITE] * channels[CH_WHITE] +
                                 channels[CH_BLUE]  * channels[CH_BLUE]);

         float norm_fact = 1.0f / sqrtf ((float)sum_sqr_channels);

         // channel vector of unit length
         float unit_channels[3] = { ((float)channels[CH_RED])   * norm_fact,
                                    ((float)channels[CH_WHITE]) * norm_fact,
                                    ((float)channels[CH_BLUE])  * norm_fact };

         // solving for output_channels such that:
         //                  AL_mapping \dot output_channels = AL_intensity
         // where
         //                      output_channels = c * unit_channels
         if (AL_intensity < max_AL_intensity)
         {
            float c = AL_intensity /
               ( AL_mapping[0] * unit_channels[0] +
                 AL_mapping[1] * unit_channels[1] +
                 AL_mapping[2] * unit_channels[2] );

            output_channels[CH_RED]   = (int)(c * unit_channels[0]);
            output_channels[CH_WHITE] = (int)(c * unit_channels[1]);
            output_channels[CH_BLUE]  = (int)(c * unit_channels[2]);
         } else {
            output_channels[CH_RED]   = channels[CH_RED];
            output_channels[CH_WHITE] = channels[CH_WHITE];
            output_channels[CH_BLUE]  = channels[CH_BLUE];
         }

      }
      else
      {
         output_channels[CH_RED]   = channels[CH_RED];
         output_channels[CH_WHITE] = channels[CH_WHITE];
         output_channels[CH_BLUE]  = channels[CH_BLUE];
      }

      // send updated output channel colors to the hardware
      sendProgrammedUpdate();
   }

}
