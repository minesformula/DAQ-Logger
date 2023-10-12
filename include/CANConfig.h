#ifndef CANCONFIG_H
#define CANCONFIG_H

#include <FlexCAN_T4.h>

enum CANLINES {
    ECU,
    DAQ
};

enum SENSORS {
    UNKNOWN,
    ECU_RPM_WATERTEMP,
    ECU_OILTEMP,
    ECU_PUMPS_FAN,
    ECU_VOLTAGE,
    ECU_GEAR
};

struct MSG {
    SENSORS sensor;
    uint8_t* buf;
    float timestamp;
};

#endif