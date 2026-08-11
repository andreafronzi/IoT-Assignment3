#ifndef __CONTROL_LCD_TASK__
#define __CONTROL_LCD_TASK__

#include "kernel/Task.h"
#include <LiquidCrystal_I2C.h>
#include "config.h"
#include <devices/ServoMotor/ServoMotor.h>

class ControlLCDTask : public Task
{

public:
    ControlLCDTask();
    void tick();

private:
    //poichi l'LCD risulta lento cancellare e riscriverre lo stesso stato risulterebbe pesatente. Cosicche, per essere per migliorare la velocita, si e scelto di aggiungere tali var.
    wcs_state_t last_state;
    int lastValveOpening;

    LiquidCrystal_I2C *lcd;
};

#endif