#ifndef __ALARM_TASK__
#define __ALARM_TASK__

#include "kernel/Task.h"
#include "devices/Button/Button.h"
#include "devices/led/Led.h"
#include "config.h"
#include "devices/temperature/TempSensor.h"

class AlarmTask : public Task
{
public:
    AlarmTask(alarm_state *alarmState);
    void tick();

private:
    typedef enum
    {
        NOT_ALARMED,
        PRE_ALARMED,
        ALARMED
    } alarmState;

    void setState(alarmState state);
    long elapsedTimeInState();
    bool checkAndSetJustEntered();
    void getTemperature();

    long stateTimestamp;
    bool justEntered;
    float temp;

    alarmState state;
    TempSensor *tempSensor;
    alarm_state *alarmStateExternal;
    Button *resetButton;
    Led *l3;
};
#endif