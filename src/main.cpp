#include <Arduino.h>
#include <DAQLine.hpp>
#include <ECUSensors.hpp>
#include <SensorDefinitions.h>

MF::DAQLine<CAN1> ECULine;
MF::DAQLine<CAN2> DAQLine;
int prevTime1 = 0;
int prevTime2 = 0;

void setup() {
  //Serial.begin(9600); 
  Serial2.begin(57600);
  SD.begin(BUILTIN_SDCARD);

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);                             

  digitalWrite(2, 1);
  digitalWrite(3, 1);
  digitalWrite(4, 1);

  ECULine.addSensor(1280, BATTERY_STATUS, 0);
  ECULine.addSensor(280, ENGINE_STATUS, 0);
  ECULine.addSensor(281, GEAR_STATUS, 0);
  ECULine.addSensor(1284, PUMP_STATUS, 0);
  ECULine.addSensor(1600, THROTTLE_STATUS, 0);
  ECULine.addSensor(1604, ENGINE_RUNTIME, 0);
  ECULine.addSensor(1609, ENGINE_TEMPERATURE, 0);
  ECULine.addSensor(1621, BRAKE_STATUS, 0);

  DAQLine.addSensor(1, ACCELEROMETER, 0);
  DAQLine.addSensor(2, GYROSCOPE, 0);

  ECULine.SDLoggingMode();
  ECULine.enableLiveTelemetry(Serial2);

  DAQLine.SDLoggingMode();
  DAQLine.enableDynamicSensors();
  DAQLine.enableLiveTelemetry(Serial2);

  ECULine.begin();
  DAQLine.begin();
}

void loop() {
  ECULine.update();
  DAQLine.update();

  if (millis()-prevTime1 > 5000){
    MF::SensorFactory::sendReadOut(Serial2);
    ECULine.flushSD();
    DAQLine.flushSD();
    prevTime1 = millis();
  }
}