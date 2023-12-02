#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <FlexCan_T4.h>
#include <CANConfig.h>

#include <SD.h>
#include <SPI.h>
#include <stdio.h>

class SDCard {
    public:
        SDCard();

        void initialize();
        void writeToLog(String input);
        void writeMsg(LogMsg msg);

        static bool initialized;

    private:
        char filename[100];
};

bool SDCard::initialized = false;



SDCard::SDCard(){
    initialize();
}

void SDCard::initialize(){
    if(!SD.begin(BUILTIN_SDCARD)){
        Serial.println("SD Card Initialization Failed");
        return;
    }

    int i = 0;
    sprintf(filename, "logFile0.data");

    while (SD.exists(filename)){
        i++;

        sprintf(filename, "logFile%d.data", i);
    }
    sprintf(filename, "logFile%d.data", i);

    Serial.println("SD Card Initialized");
    SDCard::initialized = true;
}

void SDCard::writeToLog(String input)
{

    Serial.println(input);
    File writeFile = SD.open(filename, FILE_WRITE);
    
    writeFile.print(input);

    writeFile.close();
}

void SDCard::writeMsg(LogMsg msg){
    File writeFile = SD.open(filename, FILE_WRITE);

    uint8_t* temp = (uint8_t*)&msg;

    for (unsigned int i = 0; i < sizeof(LogMsg); i++){
        writeFile.write(temp[i]);
    }

    writeFile.close();
}

#endif