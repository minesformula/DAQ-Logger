#include <Arduino.h>
#include <DAQLine.hpp>
#include <ECUSensors.hpp>
#include <SensorDefinitions.h>

MF::DAQLine<CAN1> DAQLine;
int prevTime1 = 0;
int prevTime2 = 0;

void setup() {
  Serial.begin(9600); 
  Serial2.begin(57600);

  DAQLine.begin();
  DAQLine.SDLoggingMode();
  DAQLine.enableDynamicSensors();
  //DAQLine.enableLiveTelemetry(Serial2);

  DAQLine.addSensor(1280, BATTERY_STATUS, 0);
  DAQLine.addSensor(280, ENGINE_STATUS, 0);
  DAQLine.addSensor(281, GEAR_STATUS, 0);
  DAQLine.addSensor(1088, BRAKE_STATUS, 0);
  DAQLine.addSensor(1284, PUMP_STATUS, 0);
  DAQLine.addSensor(1600, THROTTLE_STATUS, 0);
  DAQLine.addSensor(1604, ENGINE_RUNTIME, 0);
}

void loop() {
  DAQLine.update();

  if (millis()-prevTime1 > 5000){
    MF::SensorFactory::sendReadOut(Serial2);
    prevTime1 = millis();
  }

  if (millis()-prevTime2 > 250){
    DAQLine.sendReadOut(Serial2);
    prevTime2 = millis();
  }
}