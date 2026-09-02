#include "WaterChannelTask.h"
#include "devices/ServoMotor/ServoMotorImpl.h"
#include "devices/Button/ButtonImpl.h"
#include <Arduino.h>

// aggiungere nel costruttore la varibile dell'allarme per poterla considerare prima di atterrare e prima di decollare

WaterChannelTask::WaterChannelTask()
{
    this->canalDoor = new ServoMotorImpl(MOTOR_PIN);
    this->canalDoor->on();

    this->potentiometer = new Potentiometer(POT_PIN);
    this->potentiometer->sync();

    this->button = new ButtonImpl(BUTTON_PIN);

    this->lastButtonState = false;
    this->lastPotValue = -1;
    this->currentAngle = 0;
}

void WaterChannelTask::tick()
{
    // 1. Gestione della pressione del pulsante per il cambio di modalita (supporta anche l'override da UNCONNECTED)
    bool isButtonPressed = this->button->isPressed();
    if (isButtonPressed && !lastButtonState)
    {
        if (currentState == AUTOMATIC || currentState == UNCONNECTED)
        {
            currentState = MANUAL;
        }
        else if (currentState == MANUAL)
        {
            currentState = AUTOMATIC;
        }
    }
    this->lastButtonState = isButtonPressed;

    switch (currentState)
    {
    case MANUAL:
    {
        this->potentiometer->sync();
        int potVal = (int)(this->potentiometer->getValue() * 100.0);
        potVal = constrain(potVal, 0, 100);

        // Se il potenziometro viene mosso in modo significativo (>= 2%), sovrascrive il target
        if (this->lastPotValue == -1 || abs(this->lastPotValue - potVal) >= 2)
        {
            this->lastPotValue = potVal; 
            targetValveOpening = potVal;
        }

        currentValveOpening = targetValveOpening;
        this->updateServoPosition(currentValveOpening);
        
        break;
    }

    case AUTOMATIC:
        this->lastPotValue = -1; // Reset per reinizializzare alla rientrata in MANUAL
        currentValveOpening = targetValveOpening;
        this->updateServoPosition(currentValveOpening);
        break;

    case UNCONNECTED:
        this->lastPotValue = -1;
        currentValveOpening = 0;
        this->updateServoPosition(0);
        break;

    default:
        break;
    }
}

void WaterChannelTask::updateServoPosition(int percentage)
{
    // Mapping della percentuale (0 .. 100) in un angolo (0 .. 90)
    int targetAngle = map(percentage, 0, 100, 0, 90);

    // Movimento graduale del servomotore per evitare scatti ed eccessivo assorbimento di corrente (step max 5 gradi per tick)
    if (this->currentAngle < targetAngle)
    {
        this->currentAngle += min(5, targetAngle - this->currentAngle);
    }
    else if (this->currentAngle > targetAngle)
    {
        this->currentAngle -= min(5, this->currentAngle - targetAngle);
    }

    this->canalDoor->setPosition(this->currentAngle);
}
