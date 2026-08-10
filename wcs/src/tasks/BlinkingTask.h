#ifndef __BLINKING_TASK__
#define __BLINKING_TASK__

#include "kernel/Task.h"
#include "devices/led/Led.h"
#include <Arduino.h>

class BlinkingTask : public Task
{

public:
  BlinkingTask(bool *blinking);
  void tick();

private:
  typedef enum
  {
    OFF,
    ON
  } BlinkingState;

  void setState(BlinkingState state);
  long elapsedTimeInState();
  void log(const String &msg);
  bool checkAndSetJustEntered();

  long stateTimestamp;
  bool justEntered;

  BlinkingState state;
  bool *blk;
  Led *pLed;
};

#endif