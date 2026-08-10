#include "BlinkingTask.h"
#include "config.h"
#include <Arduino.h>

BlinkingTask::BlinkingTask(bool *blinking)
{
    this->pLed = new Led(L2);
    this->blk = blinking;
    this->justEntered = true;
    setState(OFF);
}

void BlinkingTask::tick()
{
    checkAndSetJustEntered();
    if (*blk == true)
    {
        switch (state)
        {
        case OFF:
            pLed->switchOff();
            if (elapsedTimeInState() > BT)
            {
                setState(ON);
            }
            break;
        case ON:
            pLed->switchOn();
            if (elapsedTimeInState() > BT)
            {
                setState(OFF);
            }
            break;
        default:
            break;
        }
    }
}

void BlinkingTask::setState(BlinkingState state)
{
    this->state = state;
    stateTimestamp = millis();
    justEntered = true;
}

long BlinkingTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool BlinkingTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}

    void log(const String &msg);