#ifndef SENSOR_H
#define SENSOR_H

#include <FlexCAN_T4.h>

class Sensor {
    public:

        enum class SensorTypes:uint8_t {
            UNKNOWN,
            ECU_RPM_WATERTEMP,
            ECU_OILTEMP,
            ECU_PUMPS_FAN,
            ECU_VOLTAGE,
            ECU_GEAR
        };

        struct LogMsg {
            SensorTypes type;
            uint8_t sensorNum;
            uint8_t buf[8];
            uint32_t timestamp;
        };

        Sensor(SensorTypes type, uint8_t number, SDCard* card);

        LogMsg getCurrentMsg();

        void writeMessage(CAN_message_t &msg);

    private:

        SensorTypes type;
        uint8_t number;

        SDCard* card;

        LogMsg recentMsg;
};

Sensor::Sensor(SensorTypes type, uint8_t number, SDCard *card){
    this->type = type;
    this->number = number;

    this->recentMsg.type = type;
    this->recentMsg.sensorNum = number;

    this->card = card;
    
}

Sensor::LogMsg Sensor::getCurrentMsg(){
    return recentMsg;
}

void Sensor::writeMessage(CAN_message_t &msg){

    for (int i = 0; i < 8; i++){
        recentMsg.buf[i] = msg.buf[i];
    }

    recentMsg.timestamp = static_cast<uint8_t>(static_cast<float>(millis())/1000);

    char* temp = static_cast<char*>(static_cast<void*>(&recentMsg));

    card->writeBytes(temp, sizeof(recentMsg));
    Serial2.write(temp, sizeof(recentMsg));
    Serial2.write('\n');
}

#endif