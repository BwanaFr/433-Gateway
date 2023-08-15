#include "fan.hpp"

#define BIT_START_PULSE 372
#define BIT_1_PULSE 1493
#define BIT_0_PULSE 498

#define START_PREAMBLE_1 9201
#define START_PREAMBLE_0 2614
#define END_PULSE_0 4850

#define SEND_TRIES 20

FANControl::FANControl(rtl_433_ESP& rf, SX1278& radio) : 
    isOn_(false), speed_(5), timer_(0), rotation_(DEG_ZERO), mode_(NORMAL),
    rf_(rf), radio_(radio)
{
}

void FANControl::switchOn(uint8_t speed, uint8_t timer, Rotation rotation, Mode mode)
{
    setSpeed(speed);
    setTimer(timer);
    setRotation(rotation);
    setMode(mode);
    isOn_ = true;
    sendCommand(buildCommand(true));
}

void FANControl::switchOff()
{
    isOn_ = false;
    sendCommand(0x0);
}

uint16_t FANControl::buildCommand(bool startOn)
{
    uint16_t ret = 0;
    if(startOn){
        ret |= (1 << 13);
    }
    if(isOn_){
        ret |= (1 << 12);
    }
    ret |= ((mode_ & 0x3) << 10);
    ret |= ((rotation_ & 0x3) << 8);
    ret |= ((timer_ & 0xF) << 4);
    ret |= (speed_ & 0xF);
    return ret;
}

void FANControl::pulse(bool value, uint32_t us)
{
    digitalWrite(RF_MODULE_DIO2, value ? 1 : 0);
    delayMicroseconds(us);
}

void FANControl::sendBit(bool value)
{
    //Send pulse
    pulse(1, BIT_START_PULSE);
    pulse(0, value ? BIT_1_PULSE : BIT_0_PULSE);
}

void FANControl::sendCommand(uint16_t command)
{
    //Disable receiver
    rf_.disableReceiver();
    
    if(radio_.transmitDirect() != RADIOLIB_ERR_NONE){
        Serial.println("Radiolib error!");
    }

    pinMode(RF_MODULE_DIO2, OUTPUT);    //Data pin
    for(int j=0;j<SEND_TRIES;++j){
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
    pinMode(RF_MODULE_DIO2, INPUT);
    radio_.setDataShapingOOK(2);
    radio_.receiveDirect();
    rf_.enableReceiver();
}