package backend;

import mqtt.EspData;
import serialcommunication.ArduinoData;
import serialcommunication.SerialCommChannel;

public class Cus {

    private CusState currentState = CusState.AUTOMATIC;
    
    private final SerialCommChannel serialChannel;
    private final EspData espData;
    private final ArduinoData arduinoData;

    private final double L1 = 60.0;
    private final double L2 = 75.0;
    private final double T1 = 5000; // 5 secondi
    private final long T2 = 10000;  // 10 secondi

    private long lastTimeT1LevelExceeded = 0;
    private boolean isAboveL1 = false;
    private double rainLevel;

    // FILTRI DELTA: Impediscono l'inondazione della seriale
    private String lastSentValveLevel = "";
    private String lastSentMode = "";

    private static final String VALVE_CLOSED = "0";
    private static final String VALVE_HALF_OPEN = "50";
    private static final String VALVE_FULL_OPEN = "100";

    public Cus(final SerialCommChannel serialChannel, final EspData espData, final ArduinoData arduinoData) {
        this.serialChannel = serialChannel;
        this.espData = espData;
        this.arduinoData = arduinoData;
        this.currentState = CusState.AUTOMATIC;
    }

    public void run() {
        // Invia lo stato iniziale per sincronizzare Arduino (Cold Start)
        sendWcsMode(this.currentState.toString());

        while (true) {
            manageRainLevel();
            checkConnection();
            processIntents();

            switch (this.currentState) {
                case AUTOMATIC:
                    wcsUpdateAutomatic();
                    break;
                case MANUAL:
                    // In manuale la valvola viene mossa gestendo gli intenti (in processIntents).
                    // Non serve inviare comandi ripetitivi qui.
                    break;
                case UNCONNECTED:
                    wcsUpdateUnconnected();
                    break;
            }

            try {
                Thread.sleep(100); 
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void manageRainLevel() {
        this.rainLevel = this.espData.getWaterLevel();
    }

    private void checkConnection() {
        long timeSinceLastUpdate = System.currentTimeMillis() - this.espData.getTime();
        
        if (timeSinceLastUpdate > T2) {
            if (this.currentState != CusState.UNCONNECTED) {
                this.currentState = CusState.UNCONNECTED;
                sendWcsMode(this.currentState.toString());
                System.out.println("[CUS] Connessione persa (>" + T2 + "ms). Stato: UNCONNECTED");
            }
        } else if (this.currentState == CusState.UNCONNECTED) {
            // Se i messaggi riprendono ad arrivare, usciamo da UNCONNECTED
            this.currentState = CusState.AUTOMATIC;
            sendWcsMode(this.currentState.toString());
            System.out.println("[CUS] Connessione ripristinata. Stato: AUTOMATIC");
        }
    }

    private void processIntents() {
        // 1. Gestione richiesta Bottone Fisico (Toggle)
        if (this.arduinoData.hasIntentToggleMode()) {
            if (this.currentState == CusState.AUTOMATIC || this.currentState == CusState.UNCONNECTED) {
                this.currentState = CusState.MANUAL;
            } else {
                this.currentState = CusState.AUTOMATIC;
            }
            sendWcsMode(this.currentState.toString());
            this.arduinoData.clearIntentToggleMode(); 
        }

        // 2. Gestione richiesta Web Dashboard (Esplicita)
        if (this.arduinoData.hasIntentExplicitMode()) {
            this.currentState = this.arduinoData.getIntentExplicitMode();
            sendWcsMode(this.currentState.toString());
            this.arduinoData.clearIntentExplicitMode();
        }

        // 3. Gestione richiesta Valvola (Sia da Web che da Potenziometro)
        if (this.arduinoData.hasIntentValve()) {
            if (this.currentState == CusState.MANUAL) {
                double requestedValve = this.arduinoData.getIntentValve();
                sendWcsValveLevel(String.valueOf((int) requestedValve));
            }
            this.arduinoData.clearIntentValve(); 
        }
    }

    private void sendWcsValveLevel(final String valveLevel) {
        // FILTRO EDGE-TRIGGER: Scrive sulla Seriale SOLO se il comando è variato
        if (!valveLevel.equals(this.lastSentValveLevel)) {
            this.serialChannel.sendMsg("SET_VALVE:" + valveLevel);
            this.lastSentValveLevel = valveLevel;
            System.out.println("[CUS -> WCS] Valvola: " + valveLevel);
        }
    }

    private void sendWcsMode(final String mode) {
        // FILTRO EDGE-TRIGGER: Scrive sulla Seriale SOLO se lo stato è variato
        if (!mode.equals(this.lastSentMode)) {
            this.serialChannel.sendMsg("SET_MODE:" + mode);
            this.lastSentMode = mode;
            System.out.println("[CUS -> WCS] Modalità: " + mode);
        }
    }

    private void wcsUpdateAutomatic() {
        if (rainLevel >= L2) {
            sendWcsValveLevel(VALVE_FULL_OPEN);
            this.isAboveL1 = false; 
            this.lastTimeT1LevelExceeded = 0;
        } 
        else if (rainLevel >= L1) {
            if (!this.isAboveL1) {
                this.isAboveL1 = true;
                this.lastTimeT1LevelExceeded = System.currentTimeMillis();
            } else {
                if (System.currentTimeMillis() - this.lastTimeT1LevelExceeded >= T1) {
                    sendWcsValveLevel(VALVE_HALF_OPEN);
                }
            }
        } 
        else {
            this.isAboveL1 = false;
            this.lastTimeT1LevelExceeded = 0;
            sendWcsValveLevel(VALVE_CLOSED);
        }
    }

    private void wcsUpdateUnconnected() {
        sendWcsValveLevel(VALVE_CLOSED);
        this.isAboveL1 = false;
        this.lastTimeT1LevelExceeded = 0;
    }
}