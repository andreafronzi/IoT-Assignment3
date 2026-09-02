#include "CommunicationTask.h"
#include <Arduino.h>

CommunicationTask::CommunicationTask()
{
    this->lastTxTime = 0;
}

void CommunicationTask::tick()
{
    // ==========================================
    // 1. RICEZIONE COMANDI DA CUS (Back-end)
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
                // Questo valore viene utilizzato sia in AUTOMATIC che in MANUAL
                targetValveOpening = static_cast<uint8_t>(val);
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
        }

        delete msg; // IMPORTANTISSIMO: deallocare la memoria del messaggio per evitare memory leak!
    }

    // ==========================================
    // 2. FASE DI TRASMISSIONE A CUS (Back-end)
    // ==========================================
    unsigned long currentMillis = millis();

    // Inviamo lo stato periodicamente per non inondare la Seriale (es. 2 volte al secondo)
    if (currentMillis - lastTxTime >= TX_PERIOD)
    {
        lastTxTime = currentMillis;

        String stateStr = "";
        switch (currentState)
        {
        case AUTOMATIC: stateStr = "AUTOMATIC"; break;
        case MANUAL: stateStr = "MANUAL"; break;
        case UNCONNECTED: stateStr = "UNCONNECTED"; break;
        }

        String msg = "MODE:" + stateStr + "|VALVE:" + String(currentValveOpening);

        // Invia il pacchetto completo al CUS
        MsgService.sendMsg(msg);
    }
}
