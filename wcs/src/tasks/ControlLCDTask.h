#ifndef __CONTROL_LCD_TASK__
#define __CONTROL_LCD_TASK__

#include "kernel/Task.h"
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include <devices/ServoMotor/ServoMotor.h>

class ControlLCDTask : public Task
{

public:
    ControlLCDTask(wcs_state *state, uint8_t *sm_degree);
    void tick();

private:
    void setState(wcs_state newState);
    long elapsedTimeInState();
    bool checkAndSetJustEntered();

    long stateTimestamp;
    bool justEntered;

    uint8_t *sm_degree;
    wcs_state *currentState;

    LiquidCrystal_I2C *lcd;
};

#endif