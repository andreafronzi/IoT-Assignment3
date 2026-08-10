#include "CommunicationTask.h"
#include <Arduino.h>

CommunicationTask::CommunicationTask(hangar_state *h_state, alarm_state *a_state, bool *takeoff_flag, bool *land_flag, ProximitySensor *sonar) 
{
    this->h_state = h_state;
    this->a_state = a_state;
    this->takeoff_flag = takeoff_flag;
    this->land_flag = land_flag;
    this->sonar = sonar;
    this->lastTelemetryTime = 0;
}

void CommunicationTask::tick() 
{
    // ==========================================
    // 1. FASE DI RICEZIONE (RX) - Da DRU ad Arduino
    // ==========================================
    if (MsgService.isMsgAvailable()) {
        Msg* msg = MsgService.receiveMsg();
        String content = msg->getContent();
        
        // Decodifica del comando ricevuto
        if (content == "TAKEOFF") {
            *takeoff_flag = true;
        } else if (content == "LAND") {
            *land_flag = true;
        }
        
        delete msg; // IMPORTANTISSIMO: deallocare la memoria del messaggio per evitare memory leak!
    }

    // ==========================================
    // 2. FASE DI TRASMISSIONE (TX) - Da Arduino a DRU
    // ==========================================
    unsigned long currentMillis = millis();
    
    // Inviamo lo stato periodicamente per non inondare la Seriale (es. 2 volte al secondo)
    if (currentMillis - lastTelemetryTime >= TELEMETRY_PERIOD) {
        lastTelemetryTime = currentMillis;

        // Costruiamo una stringa di telemetria (formato custom, facile da parsare in Java)
        // Esempio output: "STATE:TAKEOFF|ALARM:NO_ALARM|DIST:25.50"
        
        String telemetry = "";
        switch (*h_state) {
            case INSIDE:  telemetry += "INSIDE";  break;
            case TAKEOFF: telemetry += "TAKEOFF"; break;
            case OUTSIDE: telemetry += "OUTSIDE"; break;
            case LANDING: telemetry += "LANDING"; break;
        }

        telemetry += "|";
        switch (*a_state) {
            case NO_ALARM:  telemetry += "NO_ALARM";  break;
            case PRE_ALARM: telemetry += "PRE_ALARM"; break;
            case ALARM:     telemetry += "ALARM";     break;
        }

        // La consegna chiede di visualizzare la distanza specialmente in fase di atterraggio
        // Leggiamo il sensore solo se il drone è in movimento per evitare misurazioni inutili
        if (*h_state == LANDING || *h_state == TAKEOFF) {
            float dist = sonar->getDistance();
            telemetry += "|" + String(dist);
        } else {
            // -1 o 0 per indicare che il dato non è attualmente rilevante
            telemetry += "|-1"; 
        }

        // Invia il pacchetto completo al PC
        MsgService.sendMsg(telemetry);
    }
}
