package serialcommunication;

import backend.CusState;

public class ArduinoData {

    private double valve;
    private CusState state;

    private CusState currentState;
    private double currentValve;

    public ArduinoData() {
        this.valve = 0.0;
        this.state = CusState.UNCONNECTED;
        this.currentState = CusState.UNCONNECTED;
        this.currentValve = 0.0;
    }

    public double getValve() {
        return valve;
    }

    public void setValve(final double valve) {
        this.valve = valve;
    }

    public CusState getCurrentState() {
        return this.currentState;
    }

    public double getCurrentValve() {
        return this.currentValve;
    }

    public CusState getState() {
        return state;
    }

    public void setState(final CusState state) {
        this.state = state;
    }

    public void setCurrent() {
        this.currentState = CusState.valueOf(this.state.toString());
    }

    public void setCurrentValve() {
        this.currentValve = this.valve;
    }

}
