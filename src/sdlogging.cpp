#include "sdlogging.h"

// Arduino.h has it's own built in string library just with an uppercase S (E.x: String instead of string)

char logFile[50];

// Creating a new log.txt file. Should not overwrite other log files.
void createLog()
{

    int i = 0;
    sprintf(logFile, "logFile0.txt");

    while (SD.exists(logFile))
    {
        i++;

        sprintf(logFile, "logFile%d.txt", i);
    }
    sprintf(logFile, "logFile%d.txt", i); // Remove when possible
}

// Should write an input to a file. Possibly an issue with file not being opened due to code structure.
void writeToLog(char input[100])
{
    File writeFile = SD.open(logFile, FILE_WRITE);
    Serial.println(logFile);

    double time = millis() / 1000; // millis() returns time since program start in milliseconds

    writeFile.print(time);
    writeFile.print(", ");
    writeFile.print(input);
    writeFile.println();
    writeFile.close();
}

void writeMsg(const CAN_message_t &msg){
    File writeFile = SD.open(logFile, FILE_WRITE);
    writeFile.print("ID: ");
    writeFile.print(msg.id);

    writeFile.print("BUF: ");
    for(int i = 0; i < 8; i++){
        writeFile.print(msg.buf[i]);
        writeFile.print(" ");
    }

    writeFile.println();
    writeFile.close();
}