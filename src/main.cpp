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

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);                             

  digitalWrite(2, 1);
  digitalWrite(3, 1);
  digitalWrite(4, 1);

  DAQLine.begin();
  DAQLine.SDLoggingMode();
  DAQLine.enableDynamicSensors();
  DAQLine.enableLiveTelemetry(Serial2);

  DAQLine.addSensor(1280, BATTERY_STATUS, 0);
  DAQLine.addSensor(280, ENGINE_STATUS, 0);
  DAQLine.addSensor(281, GEAR_STATUS, 0);
  DAQLine.addSensor(1284, PUMP_STATUS, 0);
  DAQLine.addSensor(1600, THROTTLE_STATUS, 0);
  DAQLine.addSensor(1604, ENGINE_RUNTIME, 0);
  DAQLine.addSensor(1609, ENGINE_TEMPERATURE, 0);
  DAQLine.addSensor(1621, BRAKE_STATUS, 0);
}

void loop() {
  DAQLine.update();

  if (millis()-prevTime1 > 5000){
    MF::SensorFactory::sendReadOut(Serial2);
    DAQLine.flushSD();
    prevTime1 = millis();
  }
}