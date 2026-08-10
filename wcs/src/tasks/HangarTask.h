#ifndef __HANGAR_TASK__
#define __HANGAR_TASK__

#include "kernel/Task.h"
#include "devices/ServoMotor/ServoMotor.h"
#include "devices/pir/PresenceSensor.h"
#include "devices/ProximitySensor/ProximitySensor.h"
#include <WString.h>
#include "config.h"
#include <devices/led/Led.h>

class HangarTask : public Task
{

public:
    HangarTask( 
        hangar_state *hangarState, 
        alarm_state *alarmState);
    void tick();

private:
    typedef enum
    {
        INSIDE,
        TAKEOFF,
        OUTSIDE,
        LANDING
    } StateOfHangar;

    void setState(StateOfHangar state);
    long elapsedTimeInState();
    void log(const String &msg);

    bool checkAndSetJustEntered();

    StateOfHangar state;
    long stateTimestamp;
    bool justEntered;

    ServoMotor *hangarDoor;
    ProximitySensor *droneDistanceDetector;
    PresenceSensor *dronePresenceDetector;
    Led *led;
    hangar_state *hangarState;
    alarm_state *alarmState;

    bool *drone_wants_to_takeoff;
    bool *drone_wants_to_land;
    
    bool conditionStarted;
    unsigned long conditionStartTime;
};

#endif