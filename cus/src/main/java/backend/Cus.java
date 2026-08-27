package backend;

import mqtt.EspData;
import serialcommunication.SerialCommChannel;

public class Cus {

    private CusState currentState = CusState.AUTOMATIC;

    private SerialCommChannel serialChannel;

    private final EspData espData;

    private final double L1 = 5.0;
    private final double L2 = 10.0;

    private final double T1 = 5000; // 5 seconds above L1
    private final double T2 = 10000; // 10 seconds without TMS data -> UNCONNECTED

    private long lastTimeTmsMessage;
    private long lastTimeT1LevelExceeded;

    private boolean isAboveL1 = false;

    private double rainLevel;

    private static final String VALVE_CLOSED = "0";
    private static final String VALVE_HALF_OPEN = "50";
    private static final String VALVE_FULL_OPEN = "100";

    public Cus(final String port, final EspData espData) throws Exception { // mettere channel nel costruttore
        this.serialChannel = new SerialCommChannel(port, 9600);
        this.espData = espData;
    }

    public void run() {

        // Controllare connessione TMS NON FATTO
        // Ricezione messaggio TMS NON FATTO
        // Controllare se il livello di pioggia è sopra L1 o L2 NON FATTO
        // Inviare messaggio WCS FATTO
        // Ricevere messaggio WCS NON FATTO
        // Aggiornare GUI NON FATTO

        while (true) {

            // Controllare connessione TMS
            if (System.currentTimeMillis() - this.lastTimeTmsMessage > T2) {
                this.currentState = CusState.UNCONNECTED;
                sendWcsMode(this.currentState.toString());
            }

            // Ricezione messaggio TMS
            String rainLevelRaw = receiveTmsMessage(); // Se il messaggio è arrivato e stato è UNCONNECTED, cambiare
                                                       // stato a AUTOMATIC
            // Sistemare messaggio per prendere livello acqua
            this.lastTimeTmsMessage = System.currentTimeMillis();

            // Operazioni sul wcs
            switch (currentState) {
                case AUTOMATIC:
                    wcsUpdateAutomatic();
                    break;
                case MANUAL:
                    wcsUpdateManual();
                    break;
                case UNCONNECTED:
                    wcsUpdateUnconnected();
                    break;
            }

            // Ricevere messaggio WCS
            String wcsResponse = receiveWcsMessage();

            // Aggiornare GUI
            updateGui(rainLevel, wcsResponse);
        }
    }

    private void sendWcsWaterLevel(final String valveLevel) {
        this.serialChannel.sendMsg("SET_VALVE:" + valveLevel);
    }

    private void sendWcsMode(final String mode) {
        this.serialChannel.sendMsg("SET_MODE:" + mode);
    }

    private void wcsUpdateAutomatic() {
        // Controllare se il livello di pioggia è sopra L1 o L2
        if (rainLevel > L2) {
            // Inviare messaggio WCS per aprire valvola al 100%
            sendWcsWaterLevel(VALVE_FULL_OPEN);
        } else if (rainLevel > L1) {
            // Inviare messaggio WCS per aprire valvola al 50%
            if (!this.isAboveL1) {
                this.isAboveL1 = true;
                this.lastTimeT1LevelExceeded = System.currentTimeMillis();
            } else {
                if (System.currentTimeMillis() - this.lastTimeT1LevelExceeded >= T1) {
                    sendWcsWaterLevel(VALVE_HALF_OPEN);
                }
            }
        } else {
            // Inviare messaggio WCS per chiudere valvola
            this.isAboveL1 = false;
            this.lastTimeT1LevelExceeded = 0;
            sendWcsWaterLevel(VALVE_CLOSED);
        }
    }

    private void wcsUpdateManual() {
        rainLevelFromView();
        sendWcsWaterLevel(Double.toString(rainLevel));
    }

    private void wcsUpdateUnconnected() {

    }

    private void rainLevelFromView() {
        this.rainLevel = // prendere dall html;
    }

}
