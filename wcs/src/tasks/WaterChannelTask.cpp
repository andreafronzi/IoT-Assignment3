#include "WaterChannelTask.h"
#include "devices/ServoMotor/ServoMotorImpl.h"
#include <Arduino.h>

// aggiungere nel costruttore la varibile dell'allarme per poterla considerare prima di atterrare e prima di decollare

WaterChannelTask::WaterChannelTask(wcs_state *state, uint8_t *sm_degree)
{
    this->stateTimestamp = 0;
    this->sm_degree = sm_degree;
    this->currentState = state;

    this->canalDoor = new ServoMotorImpl(MOTOR_PIN);
    this->potentiometer = new Potentiometer(POT_PIN);
    this->potentiometer->sync();
}

void WaterChannelTask::tick()
{
    switch (*this->currentState)
    {
    case wcs_state::UNCONNECTED:
        if (checkAndSetJustEntered())
        {
            this->canalDoor->setPosition(90);
        }
        break;
    case wcs_state::AUTOMATIC:
        if (this->button->isPressed())
        {
            setState(wcs_state::MANUAL);
        }
        break;
    case wcs_state::MANUAL:
        if (this->button->isPressed())
        {
            setState(wcs_state::AUTOMATIC);
        }
        this->potentiometer->sync();
        *this->sm_degree = (uint8_t)this->potentiometer->getValue();
        break;

    default:
        break;
    }
}

void WaterChannelTask::setState(wcs_state state)
{
    *this->currentState = state;
    stateTimestamp = millis();
    justEntered = true;
}

long WaterChannelTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool WaterChannelTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
