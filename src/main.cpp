#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <CANConfig.h>

#include <DAQLogger.hpp>
#include <SD.h>
#include <SPI.h>

DAQLogger<CAN1>* ECULine;

void setup()
{
  Serial.begin(9600);
  Serial2.begin(57600); //Serial 2 is the telemetry radio. Baud Rate is 57600

  ECULine = new DAQLogger<CAN1>();
  ECULine->addSensor(Sensors::ECU_VOLTAGE, 1, 1280);
  ECULine->addSensor(Sensors::ECU_GEAR, 1, 281);
  ECULine->addSensor(Sensors::ECU_RPM_WATERTEMP, 1, 280);
  ECULine->addSensor(Sensors::ECU_RPM_WATERTEMP, 1, 280);
  ECULine->addSensor(Sensors::ECU_PUMPS_FAN, 1, 1284);
}

void loop()
{
  ECULine->update();
}