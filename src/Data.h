/*
  Stanford Student Space Initiative
  Balloons | VALBAL | February 2017
  Davy Ragland | dragland@stanford.edu

  File: data.h
  --------------------------
  Data frame structure.
*/

#ifndef data_H
#define data_H

#include "Config.h"

/****************************  RAW SENSORS  ***********************************/
struct RawFrame{
  double       PRESSURE_1                     =                               0;
  double       PRESSURE_2                     =                               0;
  double       PRESSURE_3                     =                               0;
  double       PRESSURE_4                     =                               0;

  //TODO: Add the rest of sensor values

};

/**********************************  DATA  ************************************/
struct DataFrame {
  uint32_t     TIME                           =                               0;
  double       MINUTES                        =                               0;
  uint16_t     LOOP_RATE                      =                               0;
  double       VOLTAGE                        =                               0;
  double       CURRENT                        =                               0;
  double       CURRENT_GPS                    =                               0;
  double       CURRENT_RB                     =                               0;
  double       CURRENT_MOTORS                 =                               0;
  double       CURRENT_PAYLOAD                =                               0;
  double       ALTITUDE_BMP                   =                               0;
  double       ASCENT_RATE                    =                               0;
  double       TEMP                           =                               0;
  float        LAT_GPS                        =                               0;
  float        LONG_GPS                       =                               0;
  double       SPEED_GPS                      =                               0;
  double       HEADING_GPS                    =                               0;
  double       ALTITUDE_GPS                   =                               0;
  double       PRESS_BMP                      =                               0;
  uint32_t     NUM_SATS_GPS                   =                               0;
  uint16_t     RB_SENT_COMMS                  =                               0;
  bool         CUTDOWN_STATE                  =                           false;

  bool         BAT_GOOD_STATE                 =                           false;
  bool         CURR_GOOD_STATE                =                           false;
  bool         PRES_GOOD_STATE                =                           false;
  bool         TEMP_GOOD_STATE                =                           false;
  bool         CAN_GOOD_STATE                 =                           false;
  bool         RB_GOOD_STATE                  =                           false;
  bool         GPS_GOOD_STATE                 =                           false;
  bool         LOOP_GOOD_STATE                =                           false;

  bool         VALVE_STATE                    =                           false;
  bool         BALLAST_STATE                  =                           false;
  float        VALVE_INCENTIVE                =                             0.0;
  float        BALLAST_INCENTIVE              =                             0.0;

  float        VALVE_SETPOINT                 =   VALVE_SETPOINT_DEFAULT;
  float        BALLAST_SETPOINT               =   BALLAST_SETPOINT_DEFAULT;
  float        INCENTIVE_THRESHOLD            =   INCENTIVE_THRESHOLD_DEFAULT;
  float        RE_ARM_CONSTANT                =   RE_ARM_DEFAULT ;
  float        VALVE_VELOCITY_CONSTANT        =   VALVE_VELOCITY_DEFAULT;
  float        VALVE_ALTITUDE_DIFF_CONSTANT   =   VALVE_ALTITUDE_DIFF_DEFAULT;
  float        VALVE_LAST_ACTION_CONSTANT     =   VALVE_LAST_ACTION_DEFAULT;
  float        BALLAST_VELOCITY_CONSTANT      =   BALLAST_VELOCITY_DEFAULT;
  float        BALLAST_ALTITUDE_DIFF_CONSTANT =   BALLAST_ALTITUDE_DIFF_DEFAULT;
  float        BALLAST_LAST_ACTION_CONSTANT   =   BALLAST_LAST_ACTION_DEFAULT;

  bool         SETUP_STATE                    =                            true;
  bool         DEBUG_STATE                    =                            true;
  bool         SHOULD_CUTDOWN                 =                           false;
  bool         FORCE_VALVE                    =                           false;
  bool         FORCE_BALLAST                  =                           false;
  double       ALTITUDE_LAST                  =                               0;
  double       VALVE_ALT_LAST                 =                               0;
  double       BALLAST_ALT_LAST               =                        -90000.0;
  uint64_t     COMMS_LAST                     =                               0;
  uint64_t     LOOP_START                     =                               0;
  uint16_t     COMMS_LENGTH                   =                               0;

  RawFrame     RAW;
};

#endif
