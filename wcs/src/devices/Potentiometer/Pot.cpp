#include "Pot.h"
#include "Arduino.h"

Potentiometer::Potentiometer(int pin){
  this->pin = pin;
  this->value = 0.0;
} 
  
void Potentiometer::sync(){
  int current = analogRead(pin);
  if (value == 0 && current != 0) { // rough check for first time
      value = current;
  } else {
      value = 0.2 * current + 0.8 * value; // EMA smoothing
  }
  updateSyncTime(millis());
}

float Potentiometer::getValue(){
  return value / 1023.0;
}

void Potentiometer::updateSyncTime(long time){
	lastTimeSync = time;
}

long Potentiometer::getLastSyncTime(){
	return lastTimeSync;
}
