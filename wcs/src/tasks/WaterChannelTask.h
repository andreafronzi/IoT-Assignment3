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
    WaterChannelTask(wcs_state *statem, uint8_t *sm_degree);
    void tick();

private:
    void setState(wcs_state newState);
    long elapsedTimeInState();
    bool checkAndSetJustEntered();

    long stateTimestamp;
    bool justEntered;
    
    bool conditionStarted;
    unsigned long conditionStartTime;

    ServoMotor *canalDoor;
    Button *button;
    Potentiometer *potentiometer;

    uint8_t *sm_degree;
    wcs_state *currentState;
};

#endif