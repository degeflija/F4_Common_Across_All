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
                                            //!< Ausschlag Scheinlot nach innen außen in der Kurve
                                            //!< int16_t 1/1000 rad turnrate  -- plane frame
                                            //!< int16_t 1/1000 rad nick      -- plane frame
                                            //!< Ausschlag Scheinlot nach vorn hinten durch Bremsen      !!! not used !!!!!!!!!!!!!!

    c_CID_SNO_SystemState       = 0x10d,    //!< uint32_t Bitmuster
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
    c_CID_AD57_DecInclination   = 0x30a,    //!< int16_t as float dec deg * 10 +  // Declination
                                            //!< int16_t as float dec deg * 10    // Inclination
    c_CID_AD57_IAS_Offset       = 0x30b,    //!< int16_t as float km/h * 10


    c_CID_AD57_Signal           = 0x310,    //!< uint8_t signal_id +
                                            //!< uint8_t signal_volume

    c_CID_AD57_Audio            = 0x311,    //!< int16_t  audio_frequency +
                                            //!< uint16_t interval +
                                            //!< uint16_t audio-volume +
                                            //!< uint8_t  duty cycle
                                            //!< uint8_t  climb-mode

    c_CID_AD57_Flaps_Status     = 0x312,    //!< uint8_t  0/1 on/off-switch
                                            //!< uint8_t  CurrentFlapsSetting
                                            //!< uint8_t  OptimalFlapsSetting
                                            //!< uint8_t  FlapsFlashControl
                                            //!< uint8_t  LEDDutyCycle in %
    c_CID_AD57_HeartBeatSlave   = 0x313,    //!< uint32_t  version as 0x0102002a "1.02 Build 42"

    //
    //  Master/Slave synchronisation
    //
    c_CID_AD57_GRAB_CONTROL     = 0x322,    //!< !!!!!!!!!!!!!!!!!!!
    c_CID_AD57_LOST_CONTROL     = 0x322,    //!< !!!!!!!!!!!!!!!!!!!
    c_CID_AD57_SYNC_PARAMETER   = 0x323,    //!< sync parameters on slave


    /*
    c_CID_AD57_MC_CREADY        = 0x320,
    //!< uint8_t McCready value / 10cm/s, uint8_t audio volume

    c_CID_AD57_HAVE_CONTROL     = 0x321,
    //!< empty package, just a trigger

    Wenn ein Frontend "have_control" empfängt, stellt es sich
    automatisch als Slave ein und sendet nichts mehr auf dem CAN.

    Ein Front-End, das Master werden will, sendet dieses Paket
    ohne Inhalt DLC=0 und beginnt danach selber zu senden.

    c_CID_AD57_MC_CREADY verwende ich, um Audio-Lautstärke
    und MC-Wert auf beiden Geräten zu synchronisieren.
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


  };

#endif  // __Generic_CAN_Ids_h
// ****************************************************************************
//  EOF
// ****************************************************************************

