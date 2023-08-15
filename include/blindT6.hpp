#ifndef _BLIND_T6_HPP__
#define _BLIND_T6_HPP__
#include <rtl_433_ESP.h>

/**
 * Class to control blind T6 curtains
 * 
*/

class BlindT6Control {
public:
    /**
     * Constructor
     * @param rf Reference to the rtl_433_ESP object
     * @param radio Reference to the SX1278 object
     * @param id 24 bits device ID
     * @param unit Unit number
    */
    BlindT6Control(rtl_433_ESP& rf, SX1278& radio, uint32_t id, uint8_t unit);
    virtual ~BlindT6Control() = default;
    inline void setId(u_int32_t id) { id_ = id; }
    inline void setUnit(uint8_t unit) { unit_ = unit; }
    void open();
    void close();
    void toggleLight();
    void stop();
private:
    void pulse(bool value, uint32_t us);
    void sendCommand(uint8_t command);
    void sendBits(uint32_t bits, uint count);
    uint32_t id_;
    uint8_t unit_;
    rtl_433_ESP& rf_;
    SX1278& radio_;
    static const int ZERO_HIGH = 400;
    static const int ZERO_LOW = 800;
    static const int ONE_HIGH = 800;
    static const int ONE_LOW = 300;
    static const int START_HIGH = 4200;
    static const int START_LOW = 1500;
    static const int TRY_DELAY = 7800;
    static const int TRIES = 1;
    static const uint8_t CMD_OPEN = 0x33;
    static const uint8_t CMD_CLOSE = 0x11;
    static const uint8_t CMD_STOP = 0x55;
    static const uint8_t CMD_LIGHT = 0x0F;
};

#endif