#ifndef _FAN_HPP__
#define _FAN_HPP__
#include <rtl_433_ESP.h>

/**
 *  Fan command is :
 *  0xABCD
 *  A -> To fully understand (3 -> Power on, 1 -> Already on, 0 -> Off)
 *  B -> Bits 0..1 Rotation 0 -> 0 deg, 1-> 60 deg, 2 -> 120 deg, 3 -> 180 deg
 *       Bits 2..3 Mode 0-> Normal, 1-> night, 2-> Boost, 3 -> Open window
 *  C -> Timer value (in hours)
 *  D -> Speed 1 -> 12
 **/

void sendCommand(rtl_433_ESP& rf, SX1278& radio, uint16_t command);

#endif