#include "backlight.h"

bl_t backlight(bl_states_t current_state, bl_events_t event){
    bl_t backlight;

    if(current_state != ON && current_state != DIM && current_state != OFF){
        backlight.new_state = OFF;
        backlight.action = STOP;
        backlight.delay = 0;
        return backlight;
    }
    if(event != KEY && event != TIMEOUT){
        backlight.new_state = current_state;
        backlight.action = NONE;
        backlight.delay = 0;
        return backlight;
    }
    if(current_state == OFF && event == KEY){
        backlight.new_state = ON;
        backlight.action = START;
        backlight.delay = DIM_DELAY_MS;
    }
    if(current_state == ON && event == KEY){
        backlight.new_state = ON;
        backlight.action = RESTART;
        backlight.delay = DIM_DELAY_MS;
    }
    if(current_state == DIM && event == KEY){
        backlight.new_state = ON;
        backlight.action = RESTART;
        backlight.delay = DIM_DELAY_MS;
    }
    if(current_state == ON && event == TIMEOUT){
        backlight.new_state = DIM;
        backlight.action = START;
        backlight.delay = OFF_DELAY_MS;
    }
    if(current_state == DIM && event == TIMEOUT){
        backlight.new_state = OFF;
        backlight.action = STOP;
        backlight.delay = 0;
    }
    if(current_state == OFF && event == TIMEOUT){
        backlight.new_state = OFF;
        backlight.action = NONE;
        backlight.delay = 0;
    }

    return backlight;
}
