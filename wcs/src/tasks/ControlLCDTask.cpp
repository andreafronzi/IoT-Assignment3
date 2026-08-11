#include "ControlLCDTask.h"
#include <Arduino.h>

ControlLCDTask::ControlLCDTask()
{
    this->lcd = new LiquidCrystal_I2C(0x27, 16, 2);
    this->lcd->init();
    this->lcd->backlight();
}

void ControlLCDTask::tick()
{
    //visto che il processo di visualizzazione su LCD risulta lento, si e scelto di aggiornare lo schermo solo quando lo stato o l'apertura della valvola cambiano
    if (this->last_state != currentState || this->lastValveOpening != currentValveOpening)
    {
        this->last_state = currentState;
        this->lastValveOpening = currentValveOpening;

        this->lcd->clear();
        this->lcd->setCursor(0, 0);
        this->lcd->print("Mode:");

        switch (currentState)
        {
        case UNCONNECTED: lcd->print("UNCONNECTED"); break;
        case MANUAL: this->lcd->print("MANUAL"); break;
        case AUTOMATIC: this->lcd->print("AUTOMATIC"); break;
        default:
            break;
        }

        this->lcd->setCursor(0, 1);
        this->lcd->print("Valve:");
        this->lcd->print(currentValveOpening);
        this->lcd->print("%");
    }
}