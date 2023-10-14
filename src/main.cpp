#include <Arduino.h>
#include <FlexCAN_T4.h>

#include <DAQLogger.hpp>

DAQLogger<CAN1>* ECULine;

void setup()
{
  ECULine = new DAQLogger<CAN1>();

}

void loop()
{
  ECULine->update();
}