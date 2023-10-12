#ifndef DAQLOGGER_H
#define DAQLOGGER_H

#include <map>

#include <SDCard.h>
#include <FlexCAN_T4.h>
#include <CANConfig.h>

template <CAN_DEV_TABLE T>
class DAQLogger {
    public:
        DAQLogger(CANLINES);
        void update();

    private:
        void processUpdate(CAN_message_t &msg);

        SDCard* SD;
        FlexCAN_T4<T, RX_SIZE_256, TX_SIZE_16> can;

        CANLINES CANLine;

        SENSORS sensorList[1285] = {UNKNOWN};
};

template <CAN_DEV_TABLE T>
DAQLogger<T>::DAQLogger(CANLINES CANLine){
    this->SD = new SDCard("logFile");
    this->CANLine = CANLine;

    if(CANLine == ECU){
        sensorList[280] = SENSORS::ECU_RPM_WATERTEMP;
        sensorList[281] = SENSORS::ECU_GEAR;
        sensorList[1280] = SENSORS::ECU_VOLTAGE;
        sensorList[1284] = SENSORS::ECU_PUMPS_FAN;
    }

    this->can.begin();
    this->can.setBaudRate(1000000);
    this->can.enableFIFO();
}

template <CAN_DEV_TABLE T>
void DAQLogger<T>::processUpdate(CAN_message_t &msg){
    MSG liveMSG;

    if (sensorList[msg.id] == UNKNOWN) return;

    liveMSG.sensor = sensorList[msg.id];
    liveMSG.buf = msg.buf;
    liveMSG.timestamp = millis()/1000;

    Serial2.write(static_cast<char*>(static_cast<void*>(&liveMSG)), sizeof(liveMSG));

    this->SD->writeBytes(static_cast<char*>(static_cast<void*>(&liveMSG)), sizeof(liveMSG));
    Serial2.flush();
}

template <CAN_DEV_TABLE T>
void DAQLogger<T>::update(){
    CAN_message_t msg;

    while (this->can.readFIFO(msg)){
        processUpdate(msg);
    }
}

#endif