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

void multiFrameTransfer(const uint32_t id, uint64_t dataTypeSig, const uint8_t *payload, const uint8_t payloadLength){
  // Create Frame
  uint8_t crcCalc[payloadLength + 8];
  for(uint8_t x = 0; x < 8; x++){
    crcCalc[x] = dataTypeSig >> 
  }
  // 0x40, 0x1D, 0xA1, 0x20, 0x26, 0x81, 0x2A, 0x0b

  // CRC Calc
  // uint8_t payload[] = {0x40, 0x1D, 0xA1, 0x20, 0x26, 0x81, 0x2A, 0x0b, 0x00, 0x16, 0x00, 0x0F, 0x00, 0x0F, 0x51, 0x32, 0x34, 0x33, 0x38, 0x33, 0x37};
  uint16_t crc = crc16_ccitt_false(payload, 21);
  uint8_t crcH = crc >> 8;
  uint8_t crcL = crc & 0x00FF; 

  // TODO: Tail Bit Calc


  // Transmit
  CAN_message_t msgSend;
  msgSend.id = id;
  msgSend.flags.extended = true;
  msgSend.len = 8;
  msgSend.buf[0] = crcL;
  msgSend.buf[1] = crcH;
  msgSend.buf[2] = payload[0];
  msgSend.buf[3] = payload[1];
  msgSend.buf[4] = payload[2];
  msgSend.buf[5] = payload[3];
  msgSend.buf[6] = payload[4];
  msgSend.buf[7] = 0x81;
  GPSCan.write(msgSend);

  msgSend.buf[0] = payload[5];
  msgSend.buf[1] = payload[6];
  msgSend.buf[2] = payload[7];
  msgSend.buf[3] = payload[8];
  msgSend.buf[4] = payload[9];
  msgSend.buf[5] = payload[10];
  msgSend.buf[6] = payload[11];
  msgSend.buf[7] = 0x21;
  GPSCan.write(msgSend);

  msgSend.buf[0] = payload[12];
  msgSend.buf[1] = 0x41;
  msgSend.buf[2] = 0;
  msgSend.buf[3] = 0;
  msgSend.buf[4] = 0;
  msgSend.buf[5] = 0;
  msgSend.buf[6] = 0;
  msgSend.buf[7] = 0;
  msgSend.len = 2;
  
  GPSCan.write(msgSend);
}


// void idBreakDown(uint8_t canID){
//   uint8_t sourceID = canID & 0x7F;


// }

void print_can_sniff(const CAN_message_t &msg){
  if((msg.id & 0x7F) == 0){
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

    // Node ID Allocation
    if((msg.id & 0x7F) == 0 && firstNode){
      firstNode = false;

      CAN_message_t msgSend;
      msgSend.id = 0x1E000101;

      for ( uint8_t i = 1; i < msg.len-1; i++ ) {
        msgSend.buf[i] = msg.buf[i];
      } 
      msgSend.buf[0] = 0;
      msgSend.buf[msg.len-1] = 0xC0;
      msgSend.len = 8;

      msgSend.flags.extended = true;
      GPSCan.write(msgSend);
      Serial.println("Send message 1");
    }

    if(((msg.id & 0xFFFC00) >> 10) == 0xED4){
      CAN_message_t msgSend;
      msgSend.id = 0x1E000101;
      msgSend.buf[0] = 0x05;
      msgSend.buf[1] = 0xB0;
      msgSend.buf[2] = 0x00;
      msgSend.buf[3] = 0x44;
      msgSend.buf[4] = 0xC0;
      msgSend.buf[5] = 0x8B;
      msgSend.buf[6] = 0x63;
      msgSend.buf[7] = 0x81;
      msgSend.len = 8;
      
      msgSend.flags.extended = true;
      GPSCan.write(msgSend);

      msgSend.id = 0x1E000101;
      msgSend.buf[0] = 0x5E;
      msgSend.buf[1] = 0x05;
      msgSend.buf[2] = msg.buf[1];
      msgSend.buf[3] = msg.buf[2];
      msgSend.buf[4] = msg.buf[3];
      msgSend.buf[5] = msg.buf[4];
      msgSend.buf[6] = msg.buf[5];
      msgSend.buf[7] = 0x21;
      msgSend.len = 8;
      
      msgSend.flags.extended = true;
      GPSCan.write(msgSend);

      msgSend.id = 0x1E000101;
      msgSend.buf[0] = msg.buf[6];
      msgSend.buf[1] = 0x41;
      msgSend.buf[2] = 0;
      msgSend.buf[3] = 0;
      msgSend.buf[4] = 0;
      msgSend.buf[5] = 0;
      msgSend.buf[6] = 0;
      msgSend.buf[7] = 0;
      msgSend.len = 2;
      
      msgSend.flags.extended = true;
      GPSCan.write(msgSend);
      Serial.println("Send message 2");
    }
  } else {
    Serial.print("MB "); Serial.print(msg.mb);
    Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
    Serial.print("  LEN: "); Serial.print(msg.len);
    Serial.print(" EXT: "); Serial.print(msg.flags.extended);
    Serial.print(" TS: "); Serial.print(msg.timestamp);
    Serial.print(" ID: "); Serial.print(msg.id, HEX);
    Serial.print(" SNID: "); Serial.print(msg.id & 0x7F, DEC);
    Serial.print(" Buffer: ");
    for ( uint8_t i = 0; i < msg.len; i++ ) {
        Serial.print(msg.buf[i], DEC); Serial.print(" ");
    } 
    Serial.println();
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

// void canTransmit(){

//   CAN_message_t msg;
//   msg.id = 0x010;
//   msg.buf[0] = up;
//   msg.buf[1] = down;
//   msg.timestamp = frame.timestamp_usec;
//   msg.flags.extended = true;
//   Can0.write(msg);

//   msg.id = frame.extended_can_id;
  
  
// 	std::copy(reinterpret_cast<uint8_t const *>(frame.payload), reinterpret_cast<uint8_t const *>(frame.payload) + static_cast<uint8_t const>(frame.payload_size), msg.buf);
//   msg.len = static_cast<uint8_t const>(frame.payload_size);

//   GPSCan.write(msg);
// }



void loop() {
  // if (Serial.available() > 0) { // Check if data is available to read
  //   String input = Serial.readStringUntil('\n'); // Read data until newline character is received
  //   Serial.print("You typed: ");
  //   Serial.println(input); // Print the received data back to the serial monitor
  // }
  GPSCan.events();
}


// static int const NODE_ID  = 123;                    // 1 to 127
// static int const LED_BLINK_PIN = 2;                 // UAVCAN Detector
// static int const SILENT_CAN_PIN  = 21;              // S-pin TJA1051T/3,118
// static CanardPortID const BIT_PORT_ID  = 1620U;     //
// static byte const DEBUG = 0;                        // Debug for serial monitor

// /**************************************************************************************
//  * FUNCTION DECLARATION
//  **************************************************************************************/

// //CAN
// void    can_normal       ();
// void    can_silent       ();

// //UAVCAN
// bool    transmitCanFrame(CanardFrame const &);

// /**************************************************************************************
//  * GLOBAL VARIABLES
//  **************************************************************************************/

// 

// ArduinoUAVCAN uc(NODE_ID, transmitCanFrame);
// Heartbeat_1_0<> hb;

// volatile bool blinkFlag = true;

// /**************************************************************************************
//  * SETUP/LOOP
//  **************************************************************************************/

// void setup()
// {
//   Serial.begin(9600);
//   Serial.println("Hello World...");

//   

//   /* Configure initial heartbeat */
//   hb.data.uptime = 0;
//   hb = Heartbeat_1_0<>::Health::ADVISORY;
//   hb = Heartbeat_1_0<>::Mode::INITIALIZATION;
//   hb.data.vendor_specific_status_code = 7;
// }

// void loop()
// {
//   /* Update the heartbeat object */
//   hb.data.uptime = millis() / 1000;
//   hb = Heartbeat_1_0<>::Mode::MAINTENANCE;

//   /* Publish the heartbeat once/second */
//   static unsigned long prev = 0;
//   unsigned long const now = millis();
//   if(now - prev > 1000) {
//     uc.publish(hb);
//     prev = now;
//     blinkFlag = !blinkFlag;
//   }

//   /* Transmit all enqeued CAN frames */
//   while(uc.transmitCanFrame()) { }
// }

// /**************************************************************************************
//  * FUNCTION DEFINITION
//  **************************************************************************************/

// void can_normal()
// {
//   pinMode (SILENT_CAN_PIN, OUTPUT);
//   digitalWrite(SILENT_CAN_PIN, LOW);
// }

// void can_silent()
// {
//   pinMode (SILENT_CAN_PIN, OUTPUT);
//   digitalWrite(SILENT_CAN_PIN, HIGH);
// }

// void led_blink()
// {
//   if (blinkFlag) {
//   pinMode (LED_BLINK_PIN, OUTPUT);
//   analogWrite(LED_BLINK_PIN, 1);
// }
//   else {
//     pinMode (LED_BLINK_PIN, OUTPUT);
//     digitalWrite(LED_BLINK_PIN, LOW);
//   }
// }

// bool transmitCanFrame(CanardFrame const & frame)
// {
//   CAN_message_t msg;

//   msg.id = frame.extended_can_id;
//   msg.timestamp = frame.timestamp_usec;
//   msg.flags.extended = true;
// 	std::copy(reinterpret_cast<uint8_t const *>(frame.payload), reinterpret_cast<uint8_t const *>(frame.payload) + static_cast<uint8_t const>(frame.payload_size), msg.buf);
//   msg.len = static_cast<uint8_t const>(frame.payload_size);

//   Can1.write(msg);
//   led_blink();

//   if (Serial && DEBUG) {
//       //Frame decode
//   Serial.print("  LEN: "); Serial.print(frame.payload_size);
//   Serial.print(" TS: "); Serial.print(frame.timestamp_usec);
//   Serial.print(" CAN_ID: "); Serial.print(frame.extended_can_id, HEX);
//   Serial.print(" Buffer: ");
//   for ( uint8_t i = 0; i < static_cast<uint8_t const>(frame.payload_size); i++ ) {
//     Serial.print(reinterpret_cast<uint8_t const *>(frame.payload)[i], HEX); Serial.print(" ");
//   } Serial.println();
// }

//   return true;
// }
