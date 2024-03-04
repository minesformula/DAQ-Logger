#include <Arduino.h>
#include "../lib/UAVCAN/src/ArduinoUAVCAN.h"
#include "../lib/FlexCAN_T4/FlexCAN_T4.h"

// Namespace
using namespace uavcan::node;

bool firstNode = true;
// Global Variables
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> GPSCan;

// CRC-16-CCITT-FALSE parameters
#define CRC_POLYNOMIAL 0x1021
#define CRC_INITIAL_VALUE 0xFFFF

uint8_t handshake2[13];

void printCanMessage(const CAN_message_t &msg){
  if((msg.id & 0x7F) == 0){ // Anonymous Message
    Serial.print("Anonymous: ");
    Serial.print(" LEN: "); Serial.print(msg.len);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" Discriminator: "); Serial.print(((msg.id & 0xFFFC00) >> 10), HEX);
    Serial.print(" MesType: "); Serial.print(((msg.id & 0x300) >> 8), HEX);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < msg.len; i++ ) {
        Serial.print(msg.buf[i], DEC); Serial.print(" ");
    } 
    Serial.println();

  } else { // Standard Message
    Serial.print("MB "); Serial.print(msg.mb);
    Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" EXT: "); Serial.print(msg.flags.extended);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" ID: "); Serial.print(msg.id, HEX);
    Serial.print(" Mess Type: "); Serial.print(msg.id & 0x7F, DEC);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < msg.len; i++ ) {
        Serial.print(msg.buf[i], DEC); Serial.print(" ");
    } 
    Serial.println();
  }
}

void printCanTransmit(const CAN_message_t &msg){
  Serial.print("Transmit: ");
  Serial.print(" OVERRUN: "); Serial.print(msg.flags.overrun);
  Serial.print(" LEN: "); Serial.print(msg.len);
  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
  Serial.print(" TS: "); Serial.print(msg.timestamp);
  Serial.print(" ID: "); Serial.print(msg.id, HEX);
  Serial.print(" Mess Type: "); Serial.print(msg.id & 0x7F, DEC);
  Serial.print(" Buffer: ");
  for ( uint8_t i = 0; i < msg.len; i++ ) {
      Serial.print(msg.buf[i], DEC); Serial.print(" ");
  } 
  Serial.println();
}

uint16_t crc16_ccitt_false(const uint8_t *data, uint32_t length) {
    uint16_t crc = CRC_INITIAL_VALUE;

    for (uint32_t i = 0; i < length; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

void nodeIDHandshake(const uint32_t id, uint64_t dataTypeSig, const uint8_t *payload, const uint8_t payloadLength){
  // Create Frame
  uint8_t crcCalc[payloadLength + 8];
  for(int i = 0; i < 8; i++) {
    crcCalc[i] = (dataTypeSig >> (i * 8)) & 0xFF;
  }
  
  for(int i = 0; i < payloadLength; i++){
    crcCalc[i+8] = payload[i];
  }

  // CRC Calc
  uint16_t crc = crc16_ccitt_false(crcCalc, payloadLength+8);
  uint8_t crcH = crc >> 8;
  uint8_t crcL = crc & 0x00FF; 

  // TODO: Tail Bit Calc

  // Transmit TODO: change to support any length multi-transfer
  CAN_message_t msgSend1;
  msgSend1.id = id;
  msgSend1.flags.extended = true;
  msgSend1.len = 8;
  msgSend1.buf[0] = crcL;
  msgSend1.buf[1] = crcH;
  msgSend1.buf[2] = payload[0];
  msgSend1.buf[3] = payload[1];
  msgSend1.buf[4] = payload[2];
  msgSend1.buf[5] = payload[3];
  msgSend1.buf[6] = payload[4];
  msgSend1.buf[7] = 0x81;

  CAN_message_t msgSend2;
  msgSend2.id = id;
  msgSend2.flags.extended = true;
  msgSend2.len = 8;
  msgSend2.buf[0] = payload[5];
  msgSend2.buf[1] = payload[6];
  msgSend2.buf[2] = payload[7];
  msgSend2.buf[3] = payload[8];
  msgSend2.buf[4] = payload[9];
  msgSend2.buf[5] = payload[10];
  msgSend2.buf[6] = payload[11];
  msgSend2.buf[7] = 0x21;

  CAN_message_t msgSend3;
  msgSend3.id = id;
  msgSend3.flags.extended = true;
  msgSend3.len = 2;
  msgSend3.buf[0] = payload[12];
  msgSend3.buf[1] = 0x41;
  msgSend3.buf[2] = 0;
  msgSend3.buf[3] = 0;
  msgSend3.buf[4] = 0;
  msgSend3.buf[5] = 0;
  msgSend3.buf[6] = 0;
  msgSend3.buf[7] = 0;
  
  GPSCan.write(msgSend1);
  GPSCan.write(msgSend2);
  GPSCan.write(msgSend3);

  printCanTransmit(msgSend1);
  printCanTransmit(msgSend2);
  printCanTransmit(msgSend3);
}

// void nodeIDAllocation(){
//   // Data Sig:  // 0x40, 0x1D, 0xA1, 0x20, 0x26, 0x81, 0x2A, 0x0b
// }



void print_can_sniff(const CAN_message_t &msg){
  printCanMessage(msg);

  if((msg.id & 0x7F) == 0){
    // Node ID Allocation
    if((msg.id & 0x7F) == 0 && firstNode){
      firstNode = false;

      CAN_message_t msgSend;
      msgSend.id = 0x1E000101;

      for ( uint8_t i = 1; i < msg.len-1; i++ ) {
        msgSend.buf[i] = msg.buf[i];
        handshake2[i] = msg.buf[i];
      } 
      handshake2[0] = 0;

      msgSend.buf[0] = 0;
      msgSend.buf[msg.len-1] = 0xC0;
      msgSend.len = 8;

      msgSend.flags.extended = true;
      GPSCan.write(msgSend);
      printCanTransmit(msgSend);
    }

    if(((msg.id & 0xFFFC00) >> 10) == 0xED4){

      for(uint8_t i = 1; i < msg.len-1; i++){
        handshake2[i+7] = msg.buf[i];
      }

      nodeIDHandshake(0x1E000101, 0x0b2a812620a11d40, handshake2, 12);
      Serial.println("Send message 2");
    }
  } 
}

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to initialize

  Serial.println("Serial communication initialized. Type something and press Enter.");

  /* Setup CAN access */
  GPSCan.begin();
  GPSCan.setBaudRate(1000000);
  GPSCan.setMaxMB(16);
  GPSCan.enableFIFO();
  GPSCan.enableFIFOInterrupt();
  GPSCan.onReceive(print_can_sniff);
  GPSCan.mailboxStatus();
}


void loop() {
  // if (Serial.available() > 0) { // Check if data is available to read
  //   String input = Serial.readStringUntil('\n'); // Read data until newline character is received
  //   Serial.print("You typed: ");
  //   Serial.println(input); // Print the received data back to the serial monitor
  // }
  GPSCan.events();
}