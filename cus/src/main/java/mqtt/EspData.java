package mqtt;

public class EspData {

    private double waterLevel;

    public EspData(final double waterLevel) {
        this.waterLevel = waterLevel;
    }

    public double getWaterLevel() {
        return waterLevel;
    }

    public void setWaterLevel(final double waterLevel) {
        this.waterLevel = waterLevel;
    }

}
