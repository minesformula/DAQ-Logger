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

SDCard::SDCard(String filename){
    createLog(filename);
}

// Creating a new log.txt file. Should not overwrite other log files.
void SDCard::createLog(String input)
{
    int i = 0;
    while (SD.exists(input.c_str()+String(i)))
    {
        i++;
    }
    this->filename = input.c_str()+String(i);
}

// Should write an input to a file. Possibly an issue with file not being opened due to code structure.
void SDCard::writeToLog(String input)
{
    File writeFile = SD.open(filename.c_str(), FILE_WRITE);
    
    writeFile.print(input);

    writeFile.close();
}

void SDCard::writeBytes(char* buf, unsigned int length){
    File writeFile = SD.open(filename.c_str(), FILE_WRITE);
    writeFile.write(buf, length);
    writeFile.close();
}

#endif