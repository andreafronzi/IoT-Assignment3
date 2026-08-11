#ifndef __COMMUNICATION_TASK__
#define __COMMUNICATION_TASK__

#include "kernel/Task.h"
#include "kernel/MsgService.h"
#include "config.h"

class CommunicationTask : public Task {

public:
    CommunicationTask();
    void tick();

private:
   unsigned long lastTxTime;
   const unsigned long TX_PERIOD = 300;
};

#endif
