
#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#include <stdint.h>
#include <stdbool.h>


#endif
#define DIM_DELAY_MS 2000u
#define OFF_DELAY_MS 3000u

typedef enum
{
    ON,
    DIM,
    OFF,

} bl_states_t;

typedef enum
{
    KEY,
    TIMEOUT,
} bl_events_t;

typedef enum
{
    NONE,
    START,
    RESTART,
    STOP,
} bl_actions_t;

typedef struct
{
   bl_events_t new_state;
   bl_actions_t action;
   uint16_t delay;
}bl_t;


bl_t backlight(bl_states_t current_state, bl_events_t event);