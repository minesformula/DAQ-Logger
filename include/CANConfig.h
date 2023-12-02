#ifndef CANCONFIG_H
#define CANCONFIG_H

#include <Arduino.h>

enum class Sensors:uint8_t {
        UNKNOWN,
        ECU_RPM_WATERTEMP,
        ECU_OILTEMP,
        ECU_PUMPS_FAN,
        ECU_VOLTAGE,
        ECU_GEAR
};

struct LogMsg {
    Sensors type = Sensors::UNKNOWN;
    uint8_t sensorNum;
    uint8_t buf[8];
    uint32_t timestamp;
    uint16_t CANID;
};

#endif