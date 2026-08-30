package mqtt;

public class EspData {

    private double waterLevel;
    private long time;

    public EspData(final double waterLevel) {
        this.waterLevel = waterLevel;
    }

    public double getWaterLevel() {
        return waterLevel;
    }

    public void setWaterLevel(final double waterLevel) {
        this.waterLevel = waterLevel;
    }

    public long getTime() {
        return time;
    }

    public void setTime(final long time) {
        this.time = time;
    }

}
