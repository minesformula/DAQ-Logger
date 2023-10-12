#include <SDCard.h>
#include <CANConfig.h>

// Arduino.h has it's own built in string library just with an uppercase S (E.x: String instead of string)

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