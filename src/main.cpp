/*
 Basic rtl_433_ESP example for OOK/ASK Devices

*/

#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <rtl_433_ESP.h>

#include "fan.hpp"
#include "blindT6.hpp"

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf; // use -1 to disable transmitter

extern SX1278 radio;
// SX1278 radio = RADIO_LIB_MODULE;

int count = 0;
FANControl fanCtrl(rf, radio);
BlindT6Control curtainControl(rf, radio, 0x9A276B0, 0x5);

void logJson(JsonObject& jsondata) {
#if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
  char JSONmessageBuffer[jsondata.measureLength() + 1];
#else
  char JSONmessageBuffer[JSON_MSG_BUFFER];
#endif
  jsondata.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
}

void rtl_433_Callback(char* message) {
  DynamicJsonBuffer jsonBuffer2(JSON_MSG_BUFFER);
  JsonObject& RFrtl_433_ESPdata = jsonBuffer2.parseObject(message);
  logJson(RFrtl_433_ESPdata);
  count++;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
#ifndef LOG_LEVEL
  LOG_LEVEL_SILENT
#endif
  Log.begin(LOG_LEVEL, &Serial);
  Log.notice(F(" " CR));
  Log.notice(F("****** setup ******" CR));
  rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
  rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);

  // radio.setBitRate(32.768002); //TODO: Test if we need it for TX
  rf.enableReceiver();
  Log.notice(F("****** setup complete ******" CR));
  rf.getModuleStatus();
}

unsigned long sendTime = millis() + 10000;

void loop() {
  static unsigned int currentSpeed = 0;
  static const int nbSpeed = 12;

  rf.loop();
  //Fan control testing
  if(millis() >= sendTime){
    Serial.println("Sending data");
    curtainControl.stop();
    /*if(currentSpeed == 0){
      currentSpeed = 1;
      fanCtrl.switchOn(currentSpeed);
    }else{
      fanCtrl.setSpeed(currentSpeed);
      fanCtrl.sendCommand();
    }
    if(++currentSpeed > nbSpeed){
      fanCtrl.switchOff();
      currentSpeed = 0;
    }*/

    sendTime = millis() + 10000;
  }

  delay(5);

}