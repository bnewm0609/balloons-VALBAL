/*
  Stanford Student Space Initiative
  Balloons | VALBAL | March 2017
  Davy Ragland | dragland@stanford.edu
  Aria Tedjarati | satedjarati@stanford.edu

  File: RockBlock.cpp
  --------------------------
  Implimentation of RockBlock.h
*/

#include "RockBLOCK.h"

/**********************************  SETUP  ***********************************/
/*
 * Function: init
 * -------------------
 * This function initializes the RockBlock module.
 */
bool RockBLOCK::init(bool shouldStartup) {
  bool success = false;
  pinMode(RB_GATE, OUTPUT);
  digitalWrite(RB_GATE, LOW);
  delay(2000);
  isbd.attachConsole(Serial);
  isbd.attachDiags(Serial);
  isbd.setPowerProfile(1);
  Serial3.begin(RB_BAUD);
  if (shouldStartup) {
    restart();
    success = true;
  }
  return success;
}

/********************************  FUNCTIONS  *********************************/
/*
 * Function: restart
 * -------------------
 * This function restarts the RockBLOCK.
 */
void RockBLOCK::restart() {
  EEPROM.write(EEPROMAddress, false);
  digitalWrite(RB_GATE, HIGH);
  delay(1000);
  isbd.begin();
  EEPROM.write(EEPROMAddress, true);
}

/*
 * Function: shutdown
 * -------------------
 * This function shutdown the RockBLOCK.
 */
void RockBLOCK::shutdown() {
  digitalWrite(RB_GATE, LOW);
  EEPROM.write(EEPROMAddress, false);
  isbd.begin();
}

/*
 * Function: writeRead
 * -------------------
 * This function writes a bitstream across the communication interface.
 * It returns the length of a read message.
 */
int16_t RockBLOCK::writeRead(char* buff, uint16_t len) {
  if(len > BUFFER_SIZE) return -1;
  if(len < 0) return -1;
  size_t  bufferSize = sizeof(rxBuffer);
  write(buff, len);
  delay(200);
  Serial.println("Sending RB message");
  if(isbd.sendReceiveSBDBinary(rxBuffer, len, rxBuffer, bufferSize) != ISBD_SUCCESS) return -1 ;
  read(buff, bufferSize);
  return bufferSize;
}

/*********************************  HELPERS  **********************************/
/*
 * Function: write
 * -------------------
 * This function writes a bitstream to the rockblock buffer.
 */
void RockBLOCK::write(char* buff, uint16_t len) {
  for(size_t i = 0; i < len; i++) {
    rxBuffer[i] = buff[i];
  }
}

/*
 * Function: read
 * -------------------
 * This function reads a bitstream from the rockblock buffer.
 */
void RockBLOCK::read(char* buff, uint16_t len) {
  for(size_t i = 0; i < len; i++) {
    buff[i] = rxBuffer[i];
  }
}
