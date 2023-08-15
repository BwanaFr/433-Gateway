#include "blindT6.hpp"


BlindT6Control::BlindT6Control(rtl_433_ESP& rf, SX1278& radio, uint32_t id, uint8_t unit) : 
    id_(id), unit_(unit), rf_(rf), radio_(radio)
{
}

void BlindT6Control::open()
{
    sendCommand(CMD_OPEN);
}

void BlindT6Control::close()
{
    sendCommand(CMD_CLOSE);
}

void BlindT6Control::toggleLight()
{
    sendCommand(CMD_LIGHT);
}

void BlindT6Control::stop()
{
    sendCommand(CMD_STOP);
}

void BlindT6Control::pulse(bool value, uint32_t us)
{
    digitalWrite(RF_MODULE_DIO2, value ? 1 : 0);
    delayMicroseconds(us);
}

void BlindT6Control::sendBits(uint32_t bits, uint count)
{
    for(uint i=0;i<count;++i){
        uint8_t bit = (bits >> ((count-1)-i)) & 0x1;
        pulse(1, (bit == 1 ? ONE_HIGH : ZERO_HIGH));
        pulse(0, (bit == 1 ? ONE_LOW : ZERO_LOW));
    }
}

void BlindT6Control::sendCommand(uint8_t command)
{
    //Disable receiver
    rf_.disableReceiver();
    radio_.setDataShapingOOK(0);
#ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, HIGH);
#endif

    if(radio_.transmitDirect() != RADIOLIB_ERR_NONE){
        Serial.println("Radiolib error!");
    }

    pinMode(RF_MODULE_DIO2, OUTPUT);    //Data pin
    for(int j=0;j<TRIES;++j){
        if(j > 0){
            //Wait 7.8ms before next
            delayMicroseconds(TRY_DELAY);
        }
        //Pulse high for ~4.8ms
        pulse(1, START_HIGH);
        //Pulse low for ~1.3ms
        pulse(0, START_LOW);
        //Send ID
        sendBits(id_, 28);
        //Send unit
        sendBits(unit_, 4);
        //Send command
        sendBits(command, 8);
    }
    //Put receiver back to normal mode
    pinMode(RF_MODULE_DIO2, INPUT);
    radio_.setDataShapingOOK(2);
    radio_.receiveDirect();
    rf_.enableReceiver();
#ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
#endif
}