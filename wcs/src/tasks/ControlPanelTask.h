#ifndef __CONTROL_LCD_TASK__
#define __CONTROL_LCD_TASK__

#include "kernel/Task.h"
#include <LiquidCrystal_I2C.h>
#include "devices/led/Led.h"
#include "config.h"

class ControlPanelTask : public Task
{

public:
    ControlPanelTask(hangar_state *hangarState, alarm_state *alarmState, bool *blinking);
    void tick();

private:
    typedef enum 
    {
        UNCONNECTED,
        CONNECTED,
    } ;

    void updateState();
    void setState(lcdState state);
    long elapsedTimeInState();
    void log(const String &msg);
    bool checkAndSetJustEntered();

    long stateTimestamp;
    bool justEntered;

    LiquidCrystal_I2C *lcd;
};

#endif