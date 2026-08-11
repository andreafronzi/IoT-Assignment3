#ifndef __CONFIG__
#define __CONFIG__

#define MOTOR_PIN 9
#define POT_PIN A0

typedef enum
{
    UNCONNECTED,
    AUTOMATIC,
    MANUAL
} wcs_state;

extern wcs_state state;
extern uint8_t sm_degree;

#endif