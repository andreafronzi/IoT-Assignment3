package esiot.backend;

import serialcommunication.CommChannel;

public class CusLogic {
    
    private final CommChannel serialChannel;
    private final DataService dataService;
    
    // Configurable thresholds
    private static final double L1 = 15.0; // Level 1 threshold
    private static final double L2 = 30.0; // Level 2 threshold
    private static final long T1 = 5000;   // 5 seconds above L1
    private static final long T2 = 10000;  // 10 seconds without TMS data -> UNCONNECTED
    
    private String currentMode = "AUTOMATIC";
    private int valveOpening = 0;
    
    private long lastTmsTime = 0;
    private long l1ExceedStartTime = 0;
    private boolean isAboveL1 = false;
    
    public CusLogic(CommChannel serialChannel, DataService dataService) {
        this.serialChannel = serialChannel;
        this.dataService = dataService;
        this.lastTmsTime = System.currentTimeMillis();
        
        // Start watchdog thread for T1 and T2
        new Thread(this::watchdogLoop).start();
    }
    
    public synchronized void onRainwaterLevel(double level) {
        this.lastTmsTime = System.currentTimeMillis();
        dataService.addDataPoint(level);
        
        if (currentMode.equals("UNCONNECTED")) {
            // Network restored
            currentMode = "AUTOMATIC";
            sendToWcs("SET_MODE:AUTOMATIC");
        }
        
        if (currentMode.equals("AUTOMATIC")) {
            evaluateLogic(level);
        }
    }
    
    private void evaluateLogic(double level) {
        if (level >= L2) {
            isAboveL1 = false; // Reset L1 timer
            setValve(100);
        } else if (level >= L1) {
            if (!isAboveL1) {
                isAboveL1 = true;
                l1ExceedStartTime = System.currentTimeMillis();
            } else {
                if (System.currentTimeMillis() - l1ExceedStartTime >= T1) {
                    setValve(50);
                }
            }
        } else {
            isAboveL1 = false;
            setValve(0);
        }
    }
    
    public synchronized void onWcsMessage(String msg) {
        System.out.println("[CUS] WCS MSG: " + msg);
        // Parse "MODE:MANUAL|VALVE:30"
        try {
            if (msg.startsWith("MODE:")) {
                String[] parts = msg.split("\\|");
                String modeStr = parts[0].substring(5);
                int valve = Integer.parseInt(parts[1].substring(6));
                
                if (!modeStr.equals("UNCONNECTED")) {
                    this.currentMode = modeStr;
                }
                this.valveOpening = valve;
            }
        } catch (Exception e) {
            // Ignore parse errors
        }
    }
    
    public synchronized void setModeFromDashboard(String newMode) {
        if (currentMode.equals("UNCONNECTED")) return;
        this.currentMode = newMode;
        sendToWcs("SET_MODE:" + newMode);
        
        if (newMode.equals("AUTOMATIC")) {
            // Let watchdog/next reading evaluate
            isAboveL1 = false;
        }
    }
    
    public synchronized void setValveFromDashboard(int valve) {
        if (currentMode.equals("MANUAL")) {
            setValve(valve);
        }
    }
    
    private void setValve(int percentage) {
        this.valveOpening = percentage;
        sendToWcs("SET_VALVE:" + percentage);
    }
    
    private void sendToWcs(String msg) {
        if (serialChannel != null) {
            serialChannel.sendMsg(msg);
        }
    }
    
    private void watchdogLoop() {
        while (true) {
            try {
                Thread.sleep(1000);
                long now = System.currentTimeMillis();
                synchronized (this) {
                    // Check T2 (TMS Unconnected)
                    if (now - lastTmsTime > T2 && !currentMode.equals("UNCONNECTED")) {
                        System.out.println("[CUS] TMS Network timeout! Entering UNCONNECTED mode.");
                        currentMode = "UNCONNECTED";
                        setValve(0);
                        sendToWcs("SET_MODE:UNCONNECTED");
                    }
                    
                    // If in AUTOMATIC, check T1 timer
                    if (currentMode.equals("AUTOMATIC") && isAboveL1 && (now - l1ExceedStartTime >= T1)) {
                        setValve(50);
                    }
                }
            } catch (InterruptedException e) {
                break;
            }
        }
    }
    
    public synchronized String getCurrentMode() {
        return currentMode;
    }
    
    public synchronized int getValveOpening() {
        return valveOpening;
    }
}
