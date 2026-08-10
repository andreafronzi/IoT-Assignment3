#include "AlarmTask.h"
#include "devices/Button/ButtonImpl.h"
#include <Arduino.h>

AlarmTask::AlarmTask(alarm_state *alarmState)
{
    setState(NOT_ALARMED);
    this->stateTimestamp = 0;

    this->alarmStateExternal = alarmState;
    *alarmStateExternal = NO_ALARM;
    this->resetButton = new ButtonImpl(BT_RESET);
    this->l3 = new Led(11);
}

void AlarmTask::tick()
{
    checkAndSetJustEntered();
    switch (state)
    {
    case NOT_ALARMED:
        getTemperature();
        if (temp > TEMP1)
        {
            if (elapsedTimeInState() > T3)
            {
                setState(PRE_ALARMED);
                *alarmStateExternal = PRE_ALARM;
            }
        }
        break;
    case PRE_ALARMED:
        getTemperature();
        if (temp > TEMP2)
        {
            if (elapsedTimeInState() > T4)
            {
                setState(ALARMED);
                *alarmStateExternal = ALARM;
            }
        }
        else if (temp < TEMP1)
        {
            setState(NOT_ALARMED);
            *alarmStateExternal = NO_ALARM;
        }
        break;
    case ALARMED:
        if (resetButton->isPressed())
        {
            setState(NOT_ALARMED);
            *alarmStateExternal = NO_ALARM;
        }
        break;
    default:
        break;
    }
}

void AlarmTask::getTemperature()
{
    temp = tempSensor->getTemperature();
}

void AlarmTask::setState(alarmState state)
{
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long AlarmTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool AlarmTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}