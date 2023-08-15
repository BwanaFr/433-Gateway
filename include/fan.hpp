#ifndef _FAN_HPP__
#define _FAN_HPP__
#include <rtl_433_ESP.h>

/**
 * Class to control a 433Mhz fan sold in France by Leroy Merlin with name
 * Colonne d'air à poser, EQUATION, 3trix blanc 24 W, D27 cm
 * Protocol uses a two byte command coded in pulse-distance coding, withg a pulse of ~372ns
 * and a dead time of ~498us for a 0, ~1493 for a 1.
 * Command (int16) to be send (MSB first) is :
 * [0..3] : Fan speed (0 to 12)
 * [4..7] : Timer value (hours)
 * [8..9] : Rotation 0 -> 0 deg, 1-> 60 deg, 2 -> 120 deg, 3 -> 180 deg
 * [10..11] : Mode 0-> Normal, 1-> night, 2-> Boost, 3 -> Natural flow (open window)
 * [12] : Set when ON
 * [13] : Set to switch ON
*/
class FANControl {
public:
    FANControl(rtl_433_ESP& rf, SX1278& radio);
    virtual ~FANControl() = default;
    enum Rotation {DEG_ZERO = 0, DEG_60, DEG_120, DEG_180};
    enum Mode {NORMAL = 0, NIGHT, BOOST, NATURAL_FLOW};
    
    /**
     * Switch on the fan
     * @param speed Fan speed (default: 5)
     * @param time Sleep timer (default : 0)
     * @param rotation Fan rotation (default : None)
     * @param mode Fan mode (default : Normal)
    */
    void switchOn(uint8_t speed=5, uint8_t timer=0, Rotation rotation=DEG_ZERO, Mode mode=NORMAL);

    /**
     * Switch the fan Off
    */
    void switchOff();

    /**
     * Sets the fan speed
     * @param speed Fan speed
    */
    inline void setSpeed(uint8_t speed) { speed_ = speed; }

    /**
     * Sets the fan rotation
     * @param rotation Fan rotation
    */
    inline void setRotation(Rotation rotation) { rotation_ = rotation; }

    /**
     * Sets the fan mode
     * @param mode Fan mode
    */
    inline void setMode(Mode mode) { mode_ = mode; }

    /**
     * Sets timer
     * @param time Sleep timer
    */
    inline void setTimer(uint8_t time) { timer_ = time; }

    /**
     * Sends the command
    */
    inline void sendCommand() { sendCommand(buildCommand()); }

private:
    void pulse(bool value, uint32_t us);
    void sendBit(bool value);
    uint16_t buildCommand(bool startOn = false);
    void sendCommand(uint16_t command);

    bool isOn_;
    uint8_t speed_;
    uint8_t timer_;
    Rotation rotation_;
    Mode mode_;
    rtl_433_ESP& rf_;
    SX1278& radio_;
};

#endif