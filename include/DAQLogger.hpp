#ifndef DAQLOGGER_H
#define DAQLOGGER_H

#include <FlexCAN_T4.h>
#include <Arduino.h>

#include <SDCard.hpp>
#include <Sensor.hpp>

template <CAN_DEV_TABLE T>
class DAQLogger {
    public:
        DAQLogger();
        void update();

        bool addSensor(Sensor::SensorTypes type, uint8_t number, int CANID);

    private:

        SDCard* card;
        FlexCAN_T4<T, RX_SIZE_256, TX_SIZE_16> can;

        uint8_t mailBox;
};

template <CAN_DEV_TABLE T>
DAQLogger<T>::DAQLogger(){
    this->card = new SDCard("logFile");

    this->can.begin();
    this->can.setBaudRate(1000000);
    this->can.setMaxMB(64);
    this->can.enableFIFO();
    this->can.enableFIFOInterrupt();
    this->can.setMBFilter(REJECT_ALL);
    this->can.mailboxStatus();

    Serial2.begin(57600); //Serial 2 is the telemetry radio. Baud Rate is 57600
}

template <CAN_DEV_TABLE T>
void DAQLogger<T>::update(){
    can.events();
}

template <CAN_DEV_TABLE T>
bool DAQLogger<T>::addSensor(Sensor::SensorTypes type, uint8_t number, int CANID){
    if(mailBox == 64){
        Serial.println("Error can't add sensor; Too few mailboxes");
        return false;
    }

    Sensor* temp = new Sensor(type, number, card);

    this->can.setMBFilter(mailBox, CANID);
    this->can.onReceive(mailBox, temp->writeMessage);
    mailBox++;

    return true;
}

#endif