#ifndef LIGHTING_PROGRAM_H
#define LIGHTING_PROGRAM_H

#include "RTClib.h"

#define PROGRAM_STEPS		20
#define PROGRAM_NAME_LEN	 8
#define NPROGRAMS		10 
#define NPHASES			10 /* how many phases can be in the calendar */

struct program_step {
   uint8_t active:1;
   uint8_t r:7;
   uint8_t wh;
   uint8_t b;
   uint8_t hour;
   uint8_t minute;
};

struct program
{
   char name[PROGRAM_NAME_LEN + 1];
   struct program_step steps[PROGRAM_STEPS];
};

struct program_settings
{
   uint8_t fade_duration_minutes;
   uint8_t active_program;
};

struct phase
{
   uint8_t active:1;
   uint8_t program:7;
   uint8_t days;
};

struct calendar
{
   uint32_t zeroDays;
   struct   phase phases[NPHASES];
};

class LightingProgram
{
   public:

      // load program from eep; start running!
      void begin();

      // stop running program for now
      void stop();

      // load active program from eeprom, restart execution
      void restart();

      // once every second, tick tock, tick tock.
      void tick();

      // load program from EEPROM: should only be done when not running
      void loadProgram(uint8_t index);

      // loaded program to defaults
      void resetProgram(bool initial=false);

      // flush loaded program out to EEPROM
      void saveProgram();

      // save calendar to the EEPROM
      void saveCalendar();

      // save lighting control parameters to the EEPROM
      void saveLightControl( uint16_t light_level );

      // edit active program index
      inline uint8_t getActiveProgram() const { return settings.active_program; }
      void setActiveProgram(uint8_t index);

      // edit fade duration
      inline uint8_t getFadeDuration() const { return settings.fade_duration_minutes; }
      void setFadeDuration(uint8_t minutes);

      // start editing a step
      struct program_step *startEditing(uint8_t step, uint8_t *minH, uint8_t *minM);

      // get a phase, yo
      struct phase *getPhase(uint8_t phase) {
         return cal.phases + phase;
      }

      // finish editing this step.
      // this updates all steps to conform to fade duration limitations
      inline void endEditing(uint8_t step) {
         recalculate(step);
      }

      // can this step be activated?
      bool step_time_overflows(const struct program_step *step) const;

      // currently loaded program
      inline uint8_t getLoadedProgram() const {
         return loaded_program;
      }

      inline const char *getLoadedProgramName() const {
         return program.name;
      }

      inline void setLoadedProgramName(const char *name) {
         strncpy(program.name, name, PROGRAM_NAME_LEN + 1);
      }

      inline const char *getProgramName(int pindex) {
         loadProgram(pindex);
         return getLoadedProgramName();
      }

      inline const struct program_step *getStepAt(uint8_t step) const {
         return program.steps + step;
      }

      // if there are any intermediate steps enabled, this is an advanced program.
      inline bool isAdvancedProgram(void) const {
         for (int i = 1; i < (PROGRAM_STEPS - 1); i++)
            if (program.steps[i].active)
               return true;
         return false;
      }

      /* assign a given program to a phase */
      inline void setPhaseProgram(uint8_t phase, uint8_t program) {
         cal.phases[phase].program = program;
      }

      /* set our position in the cycle */
      void setCycleTime(uint8_t weeks, uint8_t days);

      /* where are we in the current cycle? */
      bool getCycleTime(uint32_t& days) const;
      bool getCycleTime(uint8_t& weeks, uint8_t& days) const;

      // some time manipulation bits
      static uint16_t to_minutes(const struct program_step *s);
      static void from_minutes(struct program_step *s, uint16_t m);
      static void from_minutes(uint8_t *hours, uint8_t *minutes, uint16_t m);

      // is this step off?
      static bool is_off(const struct program_step *s) {
         return !(s->wh || s->r || s->b);
      }

      // Artificial and Natural light intensity readings and manipulation
      uint16_t read_NL_intensity( void );
      uint16_t get_AL_intensity( void );
      uint16_t getDesiredIntensity( void );

   private:
      bool calendar_enabled;
      uint8_t current_step;
      uint8_t current_phase;
      struct calendar cal;              // growing phases calendar
      struct program_settings settings; // settings stored in eeprom
      uint8_t loaded_program;           // index of program loaded into 'program'

      float color_delta[3]; // delta to add to current
      float color_value[3]; // current, unrounded value for smoothing
      uint16_t time_delta; // how much time between each command (seconds)
      uint8_t fade_steps_left; // 0-10, where are we in the fade

      long lasttime; // timestamp, last poll. for fade.
      bool enabled; // should we be running the program?

      struct program program;
      void loadSettings();
      void saveSettings();
      void loadCalendar();
      uint16_t loadLightControl( void );
      void sort(void);
      uint8_t find(const DateTime& now);
      void forceStep();
      long delta_t();
      void run_step();

      void initialVeg();
      void initialBloom();

      long last_AL_update_time;
      uint16_t AL_intensity; // artificial light-level intensity
      uint16_t NL_intensity; // natural light-level intensity
      uint16_t desired_intensity; // desired light-level intensity

      // eeprom address for a given program
      uint16_t offsetOfProgram(uint8_t index) const;

      // helper for recalculate(): handle a single step
      void recalculate_step(uint8_t step);

      // recalculate timing information if fade duration has changed
      void recalculate(uint8_t step);

      // walk backwards from (s - 1) returning the first valid step
      uint8_t valid_step_before(uint8_t s);

      // figure out what active phase we are in (if any)
      bool findActivePhase(uint8_t &phase);

};

extern LightingProgram lp;
extern DateTime now;
extern RTC_DS3231 rtc;

#endif /* LIGHTING_PROGRAM_H */
