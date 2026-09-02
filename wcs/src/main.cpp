#include <Arduino.h>
#include "config.h"
#include "kernel/Scheduler.h"
#include "kernel/MsgService.h"
#include "tasks/WaterChannelTask.h"
#include "tasks/ControlLCDTask.h"
#include "tasks/CommunicationTask.h"

Scheduler sched;

/* Global variables declared extern in config.h */

// --- STATO REALE ---
wcs_state_t currentState;
uint8_t currentValveOpening;
int targetValveOpening;

// --- INTENTI PENDENTI ---
bool intentToggleMode;
bool intentSetValve;
uint8_t requestedValveOpening;

void setup()
{
    MsgService.init();
    MsgService.sendMsg("Arduino Hangar System Booting...");

    sched.init(100);
    MsgService.sendMsg("Scheduler initialized.");

    MsgService.sendMsg("Initializing tasks...");
    Task *waterChannelTask = new WaterChannelTask();
    waterChannelTask->init(200);
    Task *controlLCDTask = new ControlLCDTask();
    controlLCDTask->init(300);
    Task *communicationTask = new CommunicationTask();
    communicationTask->init(200);

    sched.addTask(waterChannelTask);
    sched.addTask(controlLCDTask);
    sched.addTask(communicationTask);
}

void loop()
{
    sched.schedule();
}