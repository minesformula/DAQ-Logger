#ifndef SDCARD_H
#define SDCARD_H

#include <Arduino.h>
#include <FlexCan_T4.h>
#include <SD.h>
#include <SPI.h>
#include <stdio.h>

class SDCard {
    public:
        SDCard(String filename);

        void writeToLog(String input);
        void writeBytes(char* buf, unsigned int length);


    private:
        String filename;

        void createLog(String input);
};

#endif