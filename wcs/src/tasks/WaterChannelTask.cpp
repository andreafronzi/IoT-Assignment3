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
}

void WaterChannelTask::tick()
{
    // 1. Gestione della pressione del pulsante per il cambio di modalita
    bool isButtonPressed = this->button->isPressed();
    // Quando il pulsante viene premuto, si verifica se lo era gia prima per evitare che venga cambiato piu volte ad ogni stato.
    // Ad esempio se ci si trovasse in in uno stato di MANUAL, si premesse il pulsante, si cambiasse stato, lo scheduler cambiasse
    // task in esecuzione e ritornasse a questo task, il pulsante sarebbe ancora premuto e quindi si cambierebbe di nuovo stato.
    if (isButtonPressed && !lastButtonState)
    {
        if (currentState == AUTOMATIC)
        {
            currentState = MANUAL;
        }
        else if (currentState == MANUAL)
        {
            currentState = AUTOMATIC;
        }
    }

    switch (currentState)
    {
    case MANUAL:
        this->potentiometer->sync();
        // Mappiamo la lettura del potenziometro (0.0 .. 1.0) in percentuale (0 .. 100)
        currentValveOpening = (int)(this->potentiometer->getValue() * 100.0);
        if (currentValveOpening < 0)
            currentValveOpening = 0;
        if (currentValveOpening > 100)
            currentValveOpening = 100;
        this->updateServoPosition(currentValveOpening);
        break;

    case AUTOMATIC:
        currentValveOpening = targetValveOpening;
        this->updateServoPosition(currentValveOpening);
        break;
    case UNCONNECTED:
        this->updateServoPosition(0);
        break;
    default:
        break;
    }
}

void WaterChannelTask::updateServoPosition(int percentage)
{
    // Mapping della percentuale (0 .. 100) in un angolo (0 .. 90)
    int angle = map(percentage, 0, 100, 0, 90);
    this->canalDoor->setPosition(angle);
}
