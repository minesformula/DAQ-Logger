#include "sdlogging.h"
#include <Arduino.h>
#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void transmit(String source, String type, String units, String value){
  String temp = source + " " + type + " " + units + " " + value;
  Serial2.println(temp);

}

void setRPM(uint16_t value){
  transmit("ECU", "Engine_Revs", "[RPM]", String(value, DEC));
}

void setOilTemp(uint16_t value){
  transmit("ECU", "OilTemp", "ºC", String(value, DEC));
}

void setFanValue(uint16_t value){
  transmit("ECU", "Fan", "IDK", String(value, DEC));
}

void setWaterPumpValue(uint16_t value){
  transmit("ECU", "WaterPump", "IDK", String(value, DEC));
}

void setFuelPumpValue(uint16_t value){
  transmit("ECU", "FuelPump", "IDK", String(value, DEC));
}

void setVoltage(uint16_t value){
  transmit("ECU", "Voltage", "V", String(value, DEC));
}

void setWaterTemp(uint16_t value){
  transmit("ECU", "WaterTemp", "ºC", String(value, DEC));
}

void setNeutral(uint16_t value){
  transmit("ECU", "Neutral", "value", String(value, DEC));
}

void receive_can_updates(const CAN_message_t &msg){
    switch (msg.id){
        case 280:
            setRPM(msg.buf[0] * 100);
            setWaterTemp(msg.buf[3]);
            break;

        case 281:
            setNeutral((msg.buf[4] & 16 ) << 0x10);
            break;

        case 1280:
            if(msg.buf[0] == 48){
                setVoltage(msg.buf[2] * 0.1216);
            }
            break;

        case 1284:
            if(msg.buf[0] == 0 || msg.buf[0] == 1){
                if(msg.buf[1] == 0 || msg.buf[1] == 1){
                    setWaterPumpValue(msg.buf[1]);
                } 
                else
                    Serial.println("Water Pump Error");

                if(msg.buf[0] == 0 || msg.buf[0] == 1){
                    setFuelPumpValue(msg.buf[0]);
                }   
                else
                    Serial.println("Fuel Pump Error");

                if(msg.buf[3] == 0 || msg.buf[3] == 1){
                    setFanValue(msg.buf[3]);
                }
                else
                    Serial.println("Fan Error");
            }
        break;
        
        default:
          writeMsg(msg);
        break;
    }
}

void setup()
{
  Serial.begin(9600);
  Serial2.begin(57600);

  while(!Serial2);

  Can1.begin();
  Can1.setBaudRate(1000000);
  Can1.setMaxMB(16);
  Can1.enableFIFO();
  Can1.enableFIFOInterrupt();
  Can1.onReceive(receive_can_updates);
  Can1.mailboxStatus();

  createLog();
}

void loop()
{
  Can1.events();
  delay(5000);
}