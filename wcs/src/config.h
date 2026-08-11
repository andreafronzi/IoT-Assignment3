#ifndef __CONFIG__
#define __CONFIG__

#define BUTTON_PIN 2
#define MOTOR_PIN 9
#define POT_PIN A0

// Stati del Subsystem WCS
    typedef enum {
        AUTOMATIC,
        MANUAL,
        UNCONNECTED
    } wcs_state_t;
    
    // Variabili globali condivise tra i Task. Definita nel main e accessibile a tutti
    extern wcs_state_t currentState;
    //utilizzo di uint8_t per rappresentare la percentuale di apertura della valvola (0-100): permette di rappresentare sufficiente range di valori e risparmia memoria
    extern uint8_t currentValveOpening; // Percentuale apertura valvola (0 - 100%)
    extern uint8_t targetValveOpening;  // Apertura desiderata in AUTOMATIC (impostata da CUS)

#endif