/*
 *****************************************************************************
 *  @file   Generic_CAN_Ids.h
 *  @author Horst Rupp
 *  @brief  Created on: 21.11.2020
 *  @brief  Last Change : 28.11.2020
 *  @brief  These are the valid IDs for any data transfer on the CAN Bus.
 *****************************************************************************
 */

#ifndef  __Generic_CAN_Ids_h
  #define  __Generic_CAN_Ids_h

  //
  //  **** Sensor-Bit-Masks ***************************************************
  //
  enum SENSOR_IDs
  {
      GNSS_AVAILABLE          = 1,
      D_GNSS_AVAILABLE        = 2,
      GNSS_VELOCITY_ACC_BAD   = 4,
      MAGNETIC_DISTURBANCE    = 8,

      MTI_SENSOR_AVAILABLE    = 0x10,
      FXOS_SENSOR_AVAILABLE   = 0x20,
      L3GD20_SENSOR_AVAILABLE = 0x40,
      MS5611_STATIC_AVAILABLE = 0x80,

      MS5611_PITOT_AVAILABLE  = 0x100,
      PITOT_SENSOR_AVAILABLE  = 0x200,
      AIR_SENSOR_AVAILABLE    = 0x400,

      USB_OUTPUT_ACTIVE       = 0x1000,
      BLUEZ_OUTPUT_ACTIVE     = 0x2000,
      CAN_OUTPUT_ACTIVE       = 0x4000,
      USART_2_OUTPUT_ACTIVE   = 0x8000
  };


  //  **** Signal IDs **********************************************************
  //
  enum CAN_SIGNAL_IDs
  {
    c_SigId_NoSignal,
    c_SigId_AutoChange,
    c_SigId_InvAutoChange,
    c_SigId_Alarm,
    c_SigId_Transfer,
    c_SigId_Click,
    c_SigId_Beep,
    c_SigId_maxSigId,
  };

  //
  //  ******** Package IDs *****************************************************
  //
  enum CAN_PACKAGE_IDs
  {
    //
    //  CAN packages with source SNS (Sensor)
    //
    c_CID_SNO_HeartBeat         = 0x100,    //!< uint32_t version as 0x0102002a "1.02 Build 42"
    c_CID_SNO_EulerAngles       = 0x101,    //!< int16_t (roll, pitch, heading) / 1/1000 rad -- earth frame
    c_CID_SNO_Airspeed          = 0x102,    //!< uint16_t TAS, IAS / km/h
    c_CID_SNO_Vario             = 0x103,    //!< int16_t , -integrator / mm/s
    c_CID_SNO_GPS_Date_Time     = 0x104,    //!< uint8_t year-2000, month, day, hour, mins, secs
    c_CID_SNO_GPS_LatLon        = 0x105,    //!< int32_t lat, lon / 10^-7 degrees
    c_CID_SNO_GPS_Alt           = 0x106,    //!< int64_t MSL altitude / mm
    c_CID_SNO_GPS_Trk_Spd       = 0x107,    //!< int16_t ground vector / 1/1000 rad, uint16_t groundspeed / km/h
    c_CID_SNO_Wind              = 0x108,    //!< int16_t 1/1000 rad, uint16_t km/h    Current Wind
                                            //!> int16_t 1/1000 rad, uint16_t km/h    Average Wind
    c_CID_SNO_Atmosphere        = 0x109,    //!< uint16_t pressure / Pa uint16_t density / g/m^3
    c_CID_SNO_GPS_Sats          = 0x10a,    //!< uint8_t No of Sats
                                            //!> uint8_t Fix-Type NO=0 2D=1 3D=2 RTK=3

    c_CID_SNO_Acceleration      = 0x10b,    //!< int16_t representing TOTAL G-force in mm/s^2
                                            //!< acceleration counted positive downward relative to plane
                                            //!< stored as float m/s^2 counted positive upward

                                            //!< int16_t representing NETTO G-force in mm/s^2
                                            //!< acceleration counted positive downward relative to earth
                                            //!< stored as float m/s^2 counted positive upward

                                            //!> int16_t representing GPS vertical speed as seen from earth
                                            //!> in mm/s, stored as float m/s                            !!! not used !!!!!!!!!!!!!!

                                            //!> uint8_t, representing enum
                                            //!>    ( c_SS_Cruising, c_SS_Transient, c_SS_Climbing )

    c_CID_SNO_TurnCoord         = 0x10c,    //!< int16_t 1/1000 rad slip      -- plane frame
                                            //!< deflection of the apparent vertical inward / outward in the turn
                                            //!< int16_t 1/1000 rad turnrate  -- plane frame
                                            //!< int16_t 1/1000 rad nick      -- plane frame
                                            //!< deflection of the apparent vertical forward / back by braking      !!! not used !!!!!!!!!!!!!!

    c_CID_SNO_SystemState       = 0x10d,    //!< uint32_t bit pattern
                                            //!< uint32_t version info
    c_CID_SNO_Vdd               = 0x112,    //!< unit16_t as float voltage * 10

    // ------------------------------------------------------------------------

    c_CID_SNN_Roll_Pitch        = 0x120,    //!< float roll-angle, float pitch-angle (FRONT-RIGHT-DOWN-system)
    c_CID_SNN_Heading           = 0x121,    //!< float true heading, turn-rate
    c_CID_SNN_Airspeed          = 0x122,    //!< float TAS, float IAS / m/s
    c_CID_SNN_Vario             = 0x123,    //!< float vario, float vario-average / m/s
    c_CID_SNN_Wind              = 0x124,    //!< float wind direction (where from), float wind speed
    c_CID_SNN_Wind_Average      = 0x125,    //!< float average wind direction, float average wind speed m/s
    c_CID_SNN_Atmosphere        = 0x126,    //!< float ambient pressure / Pa, float air density / kg/m^3
    c_CID_SNN_Acceleration      = 0x127,    //!< float body-frame G-load m/s^2, vertical acceleration world frame
    c_CID_SNN_SlipPitch         = 0x128,    //!< float slip angle from body-acc, float pitch angle from body-acc
    c_CID_SNN_Voltage_Circle    = 0x129,    //!< float supply voltage, uint8_t circle-mode
    c_CID_SNN_SystemState       = 0x12a,    //!< u32 system_state, u32 git_tag dec

    c_CID_SNN_GPS_Date_Time     = 0x140,    //!< uint8_t year-2000, month, day, hour, mins, secs
    c_CID_SNN_GPS_Lat           = 0x141,    //!< double latitude
    c_CID_SNN_GPS_Lon           = 0x142,    //!< double longitude
    c_CID_SNN_GPS_Alt           = 0x143,    //!< float MSL altitude, float geo separation
    c_CID_SNN_GPS_Trk_Spd       = 0x144,    //!< float ground track, float groundspeed / m/s
    c_CID_SNN_GPS_Sats          = 0x145,    //!< uin8_t No of Sats, (uint8_t)bool SAT FIX type

    //
    //  CAN packages with source AUD (Horst-Audio)
    //
    c_CID_UTL_HeartBeat         = 0x200,    //!< uint32_t  version as 0x0102002a "1.02 Build 42"
    c_CID_UTL_CMD_2_XCSOAR      = 0x201,    //!< uint8_t command for XCSoar
                                            //!> = 0 Unforce XCSoar CLIMB-GLIDE
                                            //!> = 1 Force XCSoar to CLIMB
                                            //!> = 2 Force XCSoar to GLIDE
                                            //!> = 3 Unforce XCSoar WINDUP
                                            //!> = 4 Force XCSoar to WINDUP
    c_CID_UTL_Noise             = 0x202,    //!< ??   TODO
    c_CID_UTL_Temperature       = 0x203,    //!< int32_t  as float temp * 1000
    c_CID_UTL_Humidity          = 0x204,    //!< unit32_t as float hum * 1000
    c_CID_UTL_Pressure          = 0x205,    //!< unit32_t as float press * 1000
    c_CID_UTL_Flaps_Data        = 0x206,    //!< uint16_t position [percent * 100]
                                            //   0x207 : 10.10.2026 free ( was the reboot button on the Audio )

    //
    //  CAN packages with source AD57
    //
    c_CID_AD57_HeartBeatMaster  = 0x300,    // uint32_t  version as 0x0102002a "1.02 Build 42"
    c_CID_AD57_CMD_2_XCSOAR     = 0x301,    //!< uint8_t command for XCSoar
                                            //!> = 0 Unforce XCSoar CLIMB-GLIDE
                                            //!> = 1 Force XCSoar to CLIMB
                                            //!> = 2 Force XCSoar to GLIDE
                                            //!> = 3 Unforce XCSoar WINDUP
                                            //!> = 4 Force XCSoar to WINDUP
    c_CID_AD57_Noise            = 0x302,    //!< ??   TODO
    c_CID_AD57_Temperature      = 0x303,    //!< int32_t  as float temp * 1000
    c_CID_AD57_Humidity         = 0x304,    //!< unit32_t as float hum * 1000
    c_CID_AD57_Pressure         = 0x305,    //!< unit32_t as float press * 1000
    c_CID_AD57_Vdd              = 0x306,    //!< unit16_t as float voltage * 10
    c_CID_AD57_TCs              = 0x307,    //!< int16_t as float sec * 10 tau for fast wind in glide +
                                            //!< int16_t as float sec * 10 tau for slow wind in glide +
                                            //!< int16_t as float sec * 10 tau for fast wind in climb +
                                            //!< int16_t as float sec * 10 tau for slow wind in climb
    c_CID_AD57_Sw_Hysteresis    = 0x308,    //!< int16_t as float sec * 10
    c_CID_AD57_Euler_SetUp      = 0x309,    //!< int16_t as float dec deg * 10 +  // Roll
                                            //!< int16_t as float dec deg * 10 +  // Pitch
                                            //!< int16_t as float dec deg * 10    // Yaw
    c_CID_AD57_DeCinGlination   = 0x30a,    //!< int16_t as float dec deg * 10 +  // Declination
                                            //!< int16_t as float dec deg * 10    // Inclination
    c_CID_AD57_IAS_Offset       = 0x30b,    //!< int16_t as float km/h * 10


    c_CID_AD57_Signal           = 0x310,    //!< uint8_t signal_id +
                                            //!< uint8_t signal_volume

    c_CID_AD57_Audio            = 0x311,    //!< int16_t  audio_frequency +
                                            //!< uint16_t interval +
                                            //!< uint16_t audio-volume +
                                            //!< uint8_t  duty cycle
                                            //!< uint8_t  climb-mode

    //  27.09.2026 : 0x312 c_CID_AD57_Flaps_Status ( FE -> Audio, LED bar
    //  for the flaps ) removed - the LED bar is dropped.
    //  Do not reuse 0x312 while old Audio SW is still flying.
    c_CID_AD57_HeartBeatSlave   = 0x313,    //!< uint32_t  version as 0x0102002a "1.02 Build 42"

    //
    //  Sync of the two AD57 in the two-seater ( 13.10.2026, new concept,
    //  Master_Slave_Lib.c ). Replaces Master/Slave, MakeMeMaster and
    //  Sync-Mode.
    //
    //  0x322, 0x323, 0x324, 0x326, 0x327 are FREE, but do NOT reuse them
    //  while AD57 with SW before 7.23 Build 10 are flying : their
    //  receivers still read these ids as GRAB/LOST_CONTROL,
    //  SYNC_PARAMETER, aircraft type and registration. That is why the new
    //  ids are at 0x328 .. 0x32D, which the old SW ignores.
    //
    c_CID_AD57_SWITCH_STATE     = 0x325,    //!< 26.09.2026 : micro switches gear/airbrakes and flap detent to the other AD57
                                            //!< uint8_t  c_Switch_State_Magic
                                            //!< uint8_t  Gear_Locked ( at the switch point )
                                            //!< uint8_t  Brakes_Locked ( retracted )
                                            //!< uint8_t  Gear_Switch_Position ( 0 / 1 )
                                            //!< uint8_t  valid : Bit 0 switches, Bit 1 flaps
                                            //!< uint8_t  flap detent
                                            //!< 13.10.2026 evening : DLC back to 6 ( byte 6 seat dropped ).
                                            //!< Bit 0 / Bit 1 say that switches / poti are connected at the sender.
    //
    //  13.10.2026 evening : simple sync ( c_Sync_Version 2,
    //  Master_Slave_Lib.c ). 0x32E ( SEAT_SET from midday ) is free, but
    //  do not reuse it while AD57 with the midday build are flying.
    //
    c_CID_AD57_SYNC_INFO        = 0x328,    //!< header of the sync, with every HeartBeat, DLC 8
                                            //!< uint8_t  c_Sync_Version
                                            //!< uint8_t  Bit 0 : owns the tone, Bit 1 : ready ( start-up done )
                                            //!< uint8_t  tone claim ( counter )
                                            //!< uint8_t  Flight_Phase
                                            //!< uint32_t run time since start in s
    c_CID_AD57_SYNC_ONE         = 0x329,    //!< one shared value, DLC 8
                                            //!< uint16_t key
                                            //!< uint8_t  Bit 0 : part of "all values once"

                                            //!< uint8_t  0
                                            //!< uint32_t value ( int32_t or float bits )
    c_CID_AD57_SYNC_ACK         = 0x32A,    //!< ACK for SYNC_ONE, DLC 2
                                            //!< uint16_t key
    c_CID_AD57_SYNC_HEAD        = 0x32B,    //!< header of the pilot set, DLC 8
                                            //!< uint8_t  4 ( c_Sync_Grp_P )
                                            //!< uint8_t  number of values
    c_CID_AD57_SYNC_VALUE       = 0x32C,    //!< one value of the pilot set, DLC 8
                                            //!< uint8_t  4 ( c_Sync_Grp_P )
                                            //!< uint8_t  running number 0 .. count-1
                                            //!< uint16_t key
                                            //!< uint32_t value ( int32_t or float bits )
    c_CID_AD57_PILOT_SET        = 0x32D,    //!< uint8_t 1 offer / 2 request / 3 rejected


    /*
    c_CID_AD57_MC_CREADY        = 0x320,
    //!< uint8_t McCready value / 10cm/s, uint8_t audio volume

    c_CID_AD57_HAVE_CONTROL     = 0x321,
    //!< empty package, just a trigger

    When a FrontEnd receives "have_control", it sets itself to
    Slave automatically and sends nothing more on the CAN.

    A FrontEnd that wants to become Master sends this package
    without content, DLC=0, and then starts to send by itself.

    I use c_CID_AD57_MC_CREADY to sync audio volume and
    MC value on both devices.
    */

    //
    //  Initiate Audio and reboot
    //
    c_CID_AD57_Requests_Audio_Reboot              = 0x330,    //!< empty package, just a trigger
    //
    //  Mass Data Transfer Protocol
    //
    c_CID_AD57_Requests_Audio_CAN_MDP_Initialize  = 0x340,    //!<

    c_CID_AD57_Requests_Audio_CAN_MDP_Continuance = 0x350,    //!<

    //
    //  Heart Beats  ?????????????????? TODO
    //
    c_CID_SNN_Heartbeat_Sens    = 0x520,
    c_CID_SNN_Heartbeat_GNSS    = 0x540,
    c_CID_SNN_Heartbeat_IMU     = 0x560,

    //
    //  Configuration
    //
    c_CID_SNN_Set_Config_Item   = 0x402,    //!< uint16_t CMD_ID
                                            //!< data_h[0] = CMD_...
                                            //!< dlc = 2

  };

  //
  //  **** Item IDs for c_CID_SNN_Set_Config_Item ( 0x402 ) *******************
  //
  //  Payload : data_h[0] = CMD, dlc = 2
  //
  //  The CMD_ codes are pure triggers.
  //
  #define CMD_MEASURE_LEFT      0x3000    //  Measurement 1 : left wing down
  #define CMD_MEASURE_RIGHT     0x3001    //  Measurement 2 : right wing down
  #define CMD_MEASURE_LEVEL     0x3002    //  Measurement 3 : wings level
  #define CMD_CALCULATE         0x3003    //  compute and store the mounting orientation
  #define CMD_TUNE              0x3004    //  fine calibration in flight
  #define CMD_RESET_SENSOR      0x3005    //  reset the sensor box

#endif  // __Generic_CAN_Ids_h

// ****************************************************************************
//  EOF
// ****************************************************************************

