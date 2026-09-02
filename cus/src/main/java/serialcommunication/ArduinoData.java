package serialcommunication;

import backend.CusState;

public class ArduinoData {

    // ==========================================================
    // 1. DATI DI TELEMETRIA PASSIVA (Heartbeat da Arduino)
    // Rispecchiano lo stato FISICO del sistema.
    // 'volatile' garantisce la visibilità immediata tra thread.
    // ==========================================================
    private volatile CusState telemetryMode = CusState.AUTOMATIC;
    private volatile double telemetryValve = 0.0;

    // ==========================================================
    // 2. INTENTI (Richieste hardware o web)
    // Usiamo metodi 'synchronized' per evitare Race Conditions.
    // ==========================================================
    private boolean intentToggleMode = false;

    private boolean intentExplicitModeFlag = false;
    private CusState intentExplicitModeValue = CusState.AUTOMATIC;

    private boolean intentValveFlag = false;
    private double intentValveValue = 0.0;

    // --------------------------------------------------------
    // METODI PER LA TELEMETRIA
    // --------------------------------------------------------

    public CusState getTelemetryMode() {
        return telemetryMode;
    }

    public void setTelemetryMode(final CusState telemetryMode) {
        this.telemetryMode = telemetryMode;
    }

    public double getTelemetryValve() {
        return telemetryValve;
    }

    public void setTelemetryValve(final double telemetryValve) {
        this.telemetryValve = telemetryValve;
    }

    // --------------------------------------------------------
    // METODI PER GLI INTENTI: CAMBIO MODALITÀ (Bottone Fisico)
    // --------------------------------------------------------

    public synchronized void setIntentToggleMode(final boolean val) {
        this.intentToggleMode = val;
    }

    public synchronized boolean hasIntentToggleMode() {
        return this.intentToggleMode;
    }

    public synchronized void clearIntentToggleMode() {
        this.intentToggleMode = false;
    }

    // --------------------------------------------------------
    // METODI PER GLI INTENTI: MODALITÀ ESPLICITA (Dashboard Web)
    // --------------------------------------------------------

    public synchronized void setIntentExplicitMode(final CusState mode) {
        this.intentExplicitModeValue = mode;
        this.intentExplicitModeFlag = true;
    }

    public synchronized boolean hasIntentExplicitMode() {
        return this.intentExplicitModeFlag;
    }

    public synchronized CusState getIntentExplicitMode() {
        return this.intentExplicitModeValue;
    }

    public synchronized void clearIntentExplicitMode() {
        this.intentExplicitModeFlag = false;
    }

    // --------------------------------------------------------
    // METODI PER GLI INTENTI: MOVIMENTO VALVOLA (Potenziometro/Web)
    // --------------------------------------------------------

    public synchronized void setIntentValve(double val) {
        this.intentValveValue = val;
        this.intentValveFlag = true;
    }

    public synchronized boolean hasIntentValve() {
        return this.intentValveFlag;
    }

    public synchronized double getIntentValve() {
        return this.intentValveValue;
    }

    public synchronized void clearIntentValve() {
        this.intentValveFlag = false;
    }
}