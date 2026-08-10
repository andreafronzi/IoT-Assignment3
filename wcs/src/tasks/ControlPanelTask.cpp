#include "ControlPanelTask.h"
#include <Arduino.h>

ControlPanelTask::ControlPanelTask(hangar_state *hangarState, alarm_state *alarmState, bool *blinking)
{
    setState(INSIDE);
    this->stateTimestamp = 0;

    this->lcd = new LiquidCrystal_I2C(0x27, 16, 2);
    this->lcd->init();
    this->lcd->backlight();
    this->blinking = blinking;
    this->hangarState = hangarState;
    this->lastHangarState = *hangarState;
    this->alarmState = alarmState;
}

void ControlPanelTask::tick()
{
    updateState();
    if (checkAndSetJustEntered())
    {
        switch (state)
        {
        case INSIDE:
            *blinking = false;
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("INSIDE");
            break;
        case TAKEOFF:
            *blinking = true;
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("TAKEOFF");
            break;
        case OUTSIDE:
            *blinking = false;
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("OUTSIDE");
            break;
        case LANDING:
            *blinking = false;
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("LANDING");
            break;
        case ALARM:
            *blinking = false;
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("ALARM");
            break;
        default:
            break;
        }
    }
}

void ControlPanelTask::updateState()
{
    if (this->state != ALARM && *this->alarmState == alarm_state::ALARM)
    {
        setState(ALARM);
    }
    else if(this->lastHangarState != *this->hangarState)
    {
        this->lastHangarState = *this->hangarState;
        switch (*this->hangarState)
        {
        case INSIDE:
            setState(INSIDE);
            break;
        case TAKEOFF:
            setState(TAKEOFF);
            break;
        case OUTSIDE:
            setState(OUTSIDE);
            break;
        case LANDING:
            setState(LANDING);
            break;
        default:
            break;
        }
    }
}

void ControlPanelTask::setState(lcdState state)
{
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long ControlPanelTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool ControlPanelTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}