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
    
    // --- STATO REALE ---

    // Variabili globali condivise tra i Task. Definita nel main e accessibile a tutti
    extern wcs_state_t currentState;

    //utilizzo di uint8_t per rappresentare la percentuale di apertura della valvola (0-100): permette di rappresentare sufficiente range di valori e risparmia memoria
    extern uint8_t currentValveOpening; // Percentuale apertura valvola (0 - 100%)
    extern int targetValveOpening;  // Apertura desiderata in AUTOMATIC (impostata da CUS)


    // --- INTENTI PENDENTI ---

    // Descrive l'intento di cambiare modalità (MANUAL <-> AUTOMATIC). Viene notificato al CUS e deve ricevere la risposta prima di settera lo stato corrente.
    extern bool intentToggleMode;
    
    // Descrive l'intento di cambiare l'apertura della valvola. Viene notificato al CUS e deve ricevere la risposta prima di settera lo stato corrente.
    extern bool intentSetValve;
    // Descrive l'apertura della valvola richiesta dal CUS e derivante dal frontend.
    extern uint8_t requestedValveOpening;

#endif