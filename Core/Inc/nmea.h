#ifndef NMEA_H
#define NMEA_H

#include "main.h"

typedef struct
{
  uint16_t integer, decimal;
  uint8_t precision;
} decimal_t;

typedef struct
{
  // GNRMC
  _Bool hasRMC;
  uint8_t status;
  _Bool hasSpeed, hasCourse;
  decimal_t speed_knot, speed_kmh, course;

  // GNGGA
  _Bool hasGGA;
  _Bool hasTime;
  uint8_t hourChr[2], minuteChr[2], secondChr[2], hour, minute, second;
  uint8_t quality;
  uint8_t satelliteCountChr[2], satelliteCount;
  _Bool hasAltitude;
  decimal_t altitude;

  // GNGSA
  _Bool hasGSA;
  decimal_t hdop, vdop;
} NMEAData_t;

void NMEAHandler(const uint8_t* package, const uint16_t size);

#endif /* NMEA_H */