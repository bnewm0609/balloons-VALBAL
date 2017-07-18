/*
  Stanford Student Space Initiative
  Balloons | VALBAL | June 2017
  Davy Ragland | dragland@stanford.edu
  Claire Huang | chuang20@stanford.edu
  Matthew Tan  | mratan@stanford.edu

  File: Hardware.h
  --------------------------
  Interface to PCB hardware.
*/

#ifndef HARDWARE_H
#define HARDWARE_H

#include "Config.h"
#include <EEPROM.h>

class Hardware {
public:
/**********************************  SETUP  ***********************************/
  void            init();
  void            initResolutions();
/********************************  FUNCTIONS  *********************************/
  void            runLED(bool on);

  static void     EEPROMWritelong(uint8_t address, int32_t value);
  static int32_t  EEPROMReadlong(uint8_t address);

private:
/*********************************  HELPERS  **********************************/

/*********************************  OBJECTS  **********************************/
};

#endif
