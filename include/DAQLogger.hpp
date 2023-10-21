#ifndef DAQLOGGER_H
#define DAQLOGGER_H

#include <FlexCAN_T4.h>
#include <Arduino.h>

#include <SDCard.hpp>

template <CAN_DEV_TABLE T>
class DAQLogger {
    public:
        DAQLogger();
        void update();

        bool addSensor(Sensors type, uint8_t number, int CANID);


    private:
        static void onReceive(const CAN_message_t &msg);

        static LogMsg sensors[2048];
        static SDCard* card;

        FlexCAN_T4<T, RX_SIZE_256, TX_SIZE_16> can;
};

template <CAN_DEV_TABLE T>
SDCard* DAQLogger<T>::card;

template <CAN_DEV_TABLE T>
LogMsg DAQLogger<T>::sensors[2048];

template <CAN_DEV_TABLE T>
DAQLogger<T>::DAQLogger(){
    card = new SDCard();

    this->can.begin();
    this->can.setBaudRate(1000000);
    this->can.enableFIFO();
    this->can.enableFIFOInterrupt();
    this->can.onReceive(onReceive);
    this->can.mailboxStatus();
}

template <CAN_DEV_TABLE T>
void DAQLogger<T>::update(){
    if(!(card->initialized)){
        card->initialize();
    }

    can.events();
}

template <CAN_DEV_TABLE T>
bool DAQLogger<T>::addSensor(Sensors type, uint8_t number, int CANID){
    if (CANID > 2048 || CANID < 0){
        return false;
    } 

    sensors[CANID].type = type;
    sensors[CANID].sensorNum = number;

    return true;
}


template <CAN_DEV_TABLE T>
void DAQLogger<T>::onReceive(const CAN_message_t &msg){
    if (msg.id > 2048){
        return;
    }
    
    for (int i = 0; i < 8; i++){
        sensors[msg.id].buf[i] = msg.buf[i];
    }

    uint8_t* temp = (uint8_t) &sensors[msg.id];

    if(sensors[msg.id].type != Sensors::UNKNOWN){
        for (int i = 0; i < 10; i++){
            Serial2.write(temp[i]);
        }
        Serial2.write('\n');
    }

    sensors[msg.id].timestamp = millis();
    sensors[msg.id].CANID = msg.id;

    card->writeMsg(sensors[msg.id]);
}

#endif