#ifndef SDLOGGING_H
#define SDLOGGING_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <stdio.h>

void createLog();
void writeToLog(char input [50]);


#endif