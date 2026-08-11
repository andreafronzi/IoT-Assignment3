#include "ControlLCDTask.h"
#include <Arduino.h>

ControlLCDTask::ControlLCDTask(wcs_state *state, uint8_t *sm_degree)
{
    this->stateTimestamp = 0;
    this->sm_degree = sm_degree;
    this->currentState = state;

    this->lcd = new LiquidCrystal_I2C(0x27, 16, 2);
    this->lcd->init();
    this->lcd->backlight();
}

void ControlLCDTask::tick()
{
    switch (*this->currentState)
    {
    case wcs_state::UNCONNECTED:
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("UNCONNECTED");
        break;
    case wcs_state::MANUAL:
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("MANUAL:" + String(*this->sm_degree));
        break;
    case wcs_state::AUTOMATIC:
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print("AUTOMATIC:" + String(*this->sm_degree));
        break;
    default:
        break;
    }
}

void ControlLCDTask::setState(wcs_state state)
{
    *this->currentState = state;
    stateTimestamp = millis();
    justEntered = true;
}

long ControlLCDTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool ControlLCDTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}