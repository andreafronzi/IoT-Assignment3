#ifndef __WEATHER_CHANNEL_TASK
#define __WEATHER_CHANNEL_TASK

#include "kernel/Task.h"
#include "devices/ServoMotor/ServoMotor.h"
#include "devices/Button/Button.h"
#include "devices/Potentiometer/Pot.h"
#include <WString.h>
#include "config.h"

class WaterChannelTask : public Task
{

public:
    WaterChannelTask();
    void tick();

private:
    ServoMotor *canalDoor;
    Button *button;
    Potentiometer *potentiometer;

    bool lastButtonState;
    void updateServoPosition(int percentage);
};

#endif