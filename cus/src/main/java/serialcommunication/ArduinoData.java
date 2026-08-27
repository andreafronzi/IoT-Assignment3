package serialcommunication;

import backend.CusState;

public class ArduinoData {
    
    private double valve;
    private CusState state;

    public double getValve() {
        return valve;
    }

    public void setValve(final double valve) {
        this.valve = valve;
    }

    public CusState getState() {
        return state;
    }

    public void setState(final CusState state) {
        this.state = state;
    }

}
