#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "kernel/MsgService.h"
#include "config.h"
#include "devices/ProximitySensor/ProximitySensor.h"

class CommunicationTask : public Task {

public:
    CommunicationTask(hangar_state *h_state, alarm_state *a_state, bool *takeoff_flag, bool *land_flag, ProximitySensor *sonar);
    void tick();

private:
    hangar_state *h_state;
    alarm_state *a_state;
    bool *takeoff_flag;
    bool *land_flag;
    ProximitySensor *sonar;
    
    // Variabile per gestire l'invio periodico della telemetria 
    unsigned long lastTelemetryTime;
    
    // Invia i dati ogni 500ms (regolabile) per non intasare la Seriale
    const unsigned long TELEMETRY_PERIOD = 500; 
};

#endif
