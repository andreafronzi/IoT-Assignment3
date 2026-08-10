#include "Arduino.h"
#include "MsgService.h"

String content;

MsgServiceClass MsgService;

bool MsgServiceClass::isMsgAvailable(){
  while (Serial.available() && !msgAvailable) {
    char ch = (char) Serial.read();
    if (ch == '\n'){
      currentMsg = new Msg(content);
      msgAvailable = true;      
      content = "";
    } else {
      content += ch;      
    }
  }
  return msgAvailable;
}

Msg* MsgServiceClass::receiveMsg(){
  if (msgAvailable){
    Msg* msg = currentMsg;
    msgAvailable = false;
    currentMsg = NULL;
    return msg;  
  } else {
    return NULL; 
  }
}

void MsgServiceClass::init(){
  Serial.begin(115200);
  content.reserve(256);
  content = "";
  currentMsg = NULL;
  msgAvailable = false;  
}

void MsgServiceClass::sendMsg(const String& msg){
  Serial.println(msg);  
}

bool MsgServiceClass::isMsgAvailable(Pattern& pattern){
  return (isMsgAvailable() && pattern.match(*currentMsg));
}

Msg* MsgServiceClass::receiveMsg(Pattern& pattern){
  if (msgAvailable && pattern.match(*currentMsg)){
    Msg* msg = currentMsg;
    msgAvailable = false;
    currentMsg = NULL;
    return msg;  
  } else {
    return NULL; 
  } 
}












