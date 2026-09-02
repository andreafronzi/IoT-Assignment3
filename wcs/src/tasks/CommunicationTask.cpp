#include "CommunicationTask.h"
#include <Arduino.h>

CommunicationTask::CommunicationTask()
{
    this->lastTxTime = 0;
}

void CommunicationTask::tick()
{
    // ==========================================
    // 1. INVIO INTENTI PENDENTI A CUS
    // ==========================================
    if (intentToggleMode)
    {
        String msg = "INTENT:TOGGLE_MODE";
        MsgService.sendMsg(msg);
        intentToggleMode = false; // Reset dell'intento dopo l'invio
    }
    if (intentSetValve)
    {
        String msg = "INTENT:SET_VALVE:" + String(requestedValveOpening);
        MsgService.sendMsg(msg);
        intentSetValve = false; // Reset dell'intento dopo l'invio
    }

    // ==========================================
    // 2. RICEZIONE COMANDI DA CUS (Back-end)
    // ==========================================
    if (MsgService.isMsgAvailable())
    {
        Msg *msg = MsgService.receiveMsg();
        String content = msg->getContent();

        if (content.startsWith("SET_VALVE:"))
        {
            int val = content.substring(10).toInt();
            if (val >= 0 && val <= 100)
            {
                targetValveOpening = val;
            }
        }
        else if (content.startsWith("SET_MODE:"))
        {
            String modeStr = content.substring(9);
            if (modeStr == "AUTOMATIC")
            {
                currentState = AUTOMATIC;
            }
            else if (modeStr == "MANUAL")
            {
                currentState = MANUAL;
            }
            else if (modeStr == "UNCONNECTED")
            {
                currentState = UNCONNECTED;
            }
        }

        delete msg; // IMPORTANTISSIMO: deallocare la memoria del messaggio per evitare memory leak!
    }


    // ==========================================
    // 3. FASE DI TRASMISSIONE A CUS (Back-end) PERIODICO
    // ==========================================
    unsigned long currentMillis = millis();

    // Inviamo lo stato periodicamente per non inondare la Seriale (es. 2 volte al secondo)
    if (currentMillis - lastTxTime >= TX_PERIOD)
    {
        lastTxTime = currentMillis;

        String stateStr = "";
        switch (currentState)
        {
        case AUTOMATIC:
            stateStr = "AUTOMATIC";
            break;
        case MANUAL:
            stateStr = "MANUAL";
            break;
        case UNCONNECTED:
            stateStr = "UNCONNECTED";
            break;
        }

        // Questo garantisce che Java e Dashboard sappiano esattamente 
        // a che percentuale fisica si trova la valvola ogni mezzo secondo
        String msg = "HEARTBEAT:MODE:" + stateStr + "|VALVE:" + String(currentValveOpening);
        MsgService.sendMsg(msg);
    }
}
