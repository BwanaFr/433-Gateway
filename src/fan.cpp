#include "fan.hpp"

#define BIT_START_PULSE 372
#define BIT_1_PULSE 1493
#define BIT_0_PULSE 498

#define START_PREAMBLE_1 9201
#define START_PREAMBLE_0 2614
#define END_PULSE_0 4850

inline void pulse(bool value, uint32_t us)
{
    // digitalWrite(RF_MODULE_DIO2, value ? 1 : 0);
    delayMicroseconds(us);
}

inline void sendBit(bool value)
{
    //Send pulse
    pulse(1, BIT_START_PULSE);
    pulse(0, value ? BIT_1_PULSE : BIT_0_PULSE);
}

void sendCommand(rtl_433_ESP& rf, SX1278& radio, uint16_t command)
{
    // rf.getStatus();

    rf.disableReceiver();
    
    if(radio.transmitDirect() != RADIOLIB_ERR_NONE){
        Serial.println("Radiolib error!");
    }
    // pinMode(RF_MODULE_DIO2, OUTPUT);    //Data pin
    for(int j=0;j<50;++j){
        //Pulse high for ~8ms
        pulse(1, START_PREAMBLE_1);
        //Pulse low for ~2550us
        pulse(0, START_PREAMBLE_0);
        //Send start bit
        sendBit(1);
        //Send the 15 bit command
        for(int i=14;i >= 0;--i){
            uint16_t cmd = (command >> i) & 0x1;
            sendBit(cmd);
        }
        //Send stop bit
        pulse(1, BIT_START_PULSE);
        pulse(0, END_PULSE_0);
    }
    //Put receiver back to normal mode
    // pinMode(RF_MODULE_DIO2, INPUT);
    // radio.setDataShapingOOK(2);
    radio.receiveDirect();
    rf.enableReceiver();
}