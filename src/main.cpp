/*
 Basic rtl_433_ESP example for OOK/ASK Devices

*/

#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <rtl_433_ESP.h>

#include "fan.hpp"

#include "pulse_data.h"
#include "signalDecoder.h"

#define JSON_MSG_BUFFER 512

char messageBuffer[JSON_MSG_BUFFER];

rtl_433_ESP rf; // use -1 to disable transmitter

extern SX1278 radio;
// SX1278 radio = RADIO_LIB_MODULE;

int count = 0;


void logJson(JsonObject& jsondata) {
#if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
  char JSONmessageBuffer[jsondata.measureLength() + 1];
#else
  char JSONmessageBuffer[JSON_MSG_BUFFER];
#endif
  jsondata.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
  Log.setShowLevel(false);
  Log.notice(F("."));
  Log.setShowLevel(true);
#else
  Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
#endif
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
  // radio.setBitRate(32.768002);
  // radio.setBitRate(4.8);
  rf.enableReceiver();
  Log.notice(F("****** setup complete ******" CR));
  rf.getModuleStatus();
}

unsigned long uptime() {
  static unsigned long lastUptime = 0;
  static unsigned long uptimeAdd = 0;
  unsigned long uptime = millis() / 1000 + uptimeAdd;
  if (uptime < lastUptime) {
    uptime += 4294967;
    uptimeAdd += 4294967;
  }
  lastUptime = uptime;
  return uptime;
}

int next = uptime() + 30;

#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)

#  ifdef setBitrate
#    define TEST    "setBitrate" // 17.24 was suggested
#    define STEP    2
#    define stepMin 1
#    define stepMax 300
// #    define STEP    1
// #    define stepMin 133
// #    define stepMax 138
#  elif defined(setFreqDev) // 40 kHz was suggested
#    define TEST    "setFrequencyDeviation"
#    define STEP    1
#    define stepMin 5
#    define stepMax 200
#  elif defined(setRxBW)
#    define TEST "setRxBandwidth"

#    ifdef defined(RF_SX1276) || defined(RF_SX1278)
#      define STEP    5
#      define stepMin 5
#      define stepMax 250
#    else
#      define STEP    5
#      define stepMin 58
#      define stepMax 812
// #      define STEP    0.01
// #      define stepMin 202.00
// #      define stepMax 205.00
#    endif
#  endif
float step = stepMin;
#endif

unsigned long sendTime = millis() + 10000;

void loop() {
  static unsigned int currentCmd = 0;
  static const uint16_t cmd[] = {0x4FFE, 0x6FFE, 0x6FFD, 0x6FFC, 0x6FFB, 0x7FFF};
  static const int cmdCount = sizeof(cmd)/sizeof(uint16_t);

  rf.loop();
  // if(millis() >= sendTime){
  //   Serial.printf("Sending command 0x%02X\n", cmd[currentCmd]);
  //   sendCommand(rf, radio, cmd[currentCmd]);
  //   ++currentCmd;
  //   if(currentCmd >= cmdCount){
  //       currentCmd = 0;
  //   }
  //   sendTime = millis() + 10000;
  // }

  // if(millis() >= sendTime){
  //   Serial.printf("Sending pulses\n");
  //   pulse_data_t* rtl_pulses = (pulse_data_t*)heap_caps_calloc(1, sizeof(pulse_data_t), MALLOC_CAP_INTERNAL);
    
  //   const uint8_t data[] = {
  //     1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1
  //   };
  //   rtl_pulses->signalDuration = (sizeof(data)/sizeof(uint8_t)) * 2000;
  //   int pulseNr = 0;
  //   uint8_t prevVal = 0;
  //   for(int i=0;i<sizeof(data)/sizeof(uint8_t);++i){
  //     if(data[i] == 1){
  //       rtl_pulses->pulse[pulseNr] = 960;
  //       rtl_pulses->gap[pulseNr] = 960;
  //       ++pulseNr;
  //     }else{
  //       if(prevVal){
  //         rtl_pulses->gap[pulseNr] = 1952;
  //         prevVal = 0;
  //       }else{
  //         rtl_pulses->pulse[pulseNr] = 1952;
  //         prevVal = 1;
  //         ++pulseNr;
  //       }
  //     }
  //   }
  //   rtl_pulses->num_pulses = pulseNr + 1;
  //   processSignal(rtl_pulses);

  //   sendTime = millis() + 10000;
  // }



  delay(5);
#if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
  char stepPrint[8];
  if (uptime() > next) {
    next = uptime() + 120; // 60 seconds
    dtostrf(step, 7, 2, stepPrint);
    Log.notice(F(CR "Finished %s: %s, count: %d" CR), TEST, stepPrint, count);
    step += STEP;
    if (step > stepMax) {
      step = stepMin;
    }
    dtostrf(step, 7, 2, stepPrint);
    Log.notice(F("Starting %s with %s" CR), TEST, stepPrint);
    count = 0;

    int16_t state = 0;
#  ifdef setBitrate
    state = rf.setBitRate(step);
    RADIOLIB_STATE(state, TEST);
#  elif defined(setFreqDev)
    state = rf.setFrequencyDeviation(step);
    RADIOLIB_STATE(state, TEST);
#  elif defined(setRxBW)
    state = rf.setRxBandwidth(step);
    if ((state) != RADIOLIB_ERR_NONE) {
      Log.notice(F(CR "Setting  %s: to %s, failed" CR), TEST, stepPrint);
      next = uptime() - 1;
    }
#  endif

    rf.receiveDirect();
    // rf.getModuleStatus();
  }
#endif
}