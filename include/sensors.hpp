//Defining sensor functions

#ifndef SENSORS_HPP
#define SENSORS_HPP

#include <arduino.h>
#include <functional>
#include <FlexCAN_T4.h>
#include <vector>
#include "sensorList.h"

//General Sensor Structure
struct Sensor{
    function<void(Sensor*, FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16>)> query;
    vector<String> units;
    vector<int> canID;
};

#endif
