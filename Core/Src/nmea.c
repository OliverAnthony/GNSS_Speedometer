#include "nmea.h"

NMEAData_t NMEAData;

void NMEAHandler(const uint8_t* package, const uint16_t size)
{
  NMEAData.hasRMC = 0;
  NMEAData.hasGGA = 0;
  NMEAData.hasGSA = 0;
  NMEAData.hasSpeed = 0;
  NMEAData.hasCourse = 0;
  NMEAData.hasTime = 0;
  NMEAData.hasAltitude = 0;
  uint16_t index = 0;
  while (index < size && (!NMEAData.hasRMC || !NMEAData.hasGGA || !NMEAData.hasGSA))
  {
    index = FindNextHeader(package, index, size);
    if (index < size)
    {
      if (package[index + 2] == 'R' && package[index + 3] == 'M' && package[index + 4] == 'C' && !NMEAData.hasRMC)
      {
        NMEAData.hasRMC = 1;
        index = OffsetField(package, index, 2, size);
        if (index < size && package[index] != ',')
        {
          NMEAData.status = package[index++];
        }
        index = OffsetField(package, index, 5, size);
        if (index < size && package[index] != ',')
        {
          NMEAData.hasSpeed = 1;
          NMEAData.speed_knot = Str2Dec(package, index, size);
          uint32_t speed_kmh_tmp;
          uint16_t pow10 = 1000;
          speed_kmh_tmp = NMEAData.speed_knot.integer * 1852 + NMEAData.speed_knot.decimal * 1852 / pow10;
          NMEAData.speed_kmh.integer = speed_kmh_tmp / pow10;
          NMEAData.speed_kmh.decimal = speed_kmh_tmp % pow10 / 10;
          NMEAData.speed_kmh.precision = 2;
        }
        index = OffsetField(package, index, 1, size);
        if (index < size && package[index] != ',')
        {
          NMEAData.hasCourse = 1;
          NMEAData.course = Str2Dec(package, index, size);
        }
      }
      else if (package[index + 2] == 'G' && package[index + 3] == 'G' && package[index + 4] == 'A' && !NMEAData.hasGGA)
      {
        NMEAData.hasGGA = 1;
        index = OffsetField(package, index, 1, size);
        if (index < size && package[index] != ',')
        {
          NMEAData.hasTime = 1;
          NMEAData.hourChr[0] = package[index++];
          NMEAData.hourChr[1] = package[index++];
          NMEAData.minuteChr[0] = package[index++];
          NMEAData.minuteChr[1] = package[index++];
          NMEAData.secondChr[0] = package[index++];
          NMEAData.secondChr[1] = package[index++];
          NMEAData.hour = ((NMEAData.hourChr[0] - '0') * 10 + (NMEAData.hourChr[1] - '0' + 8)) % 24;
          NMEAData.minute = (NMEAData.minuteChr[0] - '0') * 10 + (NMEAData.minuteChr[1] - '0');
          NMEAData.second = (NMEAData.secondChr[0] - '0') * 10 + (NMEAData.secondChr[1] - '0');
        }
        index = OffsetField(package, index, 5, size);
        if (index < size)
        {
          NMEAData.quality = package[index++];
        }
        index = OffsetField(package, index, 1, size);
        if (index < size)
        {
          NMEAData.satelliteCountChr[0] = package[index++];
          NMEAData.satelliteCountChr[1] = package[index++];
          NMEAData.satelliteCount = (NMEAData.satelliteCountChr[0] - '0') * 10 + (NMEAData.satelliteCountChr[1] - '0');
        }
        index = OffsetField(package, index, 2, size);
        if (index < size && package[index] != ',')
        {
          NMEAData.hasAltitude = 1;
          NMEAData.altitude = Str2Dec(package, index, size);
        }
      }
      else if (package[index + 2] == 'G' && package[index + 3] == 'S' && package[index + 4] == 'A' && !NMEAData.hasGSA)
      {
        NMEAData.hasGSA = 1;
        index = OffsetField(package, index, 16, size);
        if (index < size)
        {
          NMEAData.hdop = Str2Dec(package, index, size);
        }
        index = OffsetField(package, index, 1, size);
        if (index < size)
        {
          NMEAData.vdop = Str2Dec(package, index, size);
        }
      }
    }
  }
  return;
}

__attribute__((always_inline))
static inline uint16_t FindNextHeader(const uint8_t* package, uint16_t index, const uint16_t size)
{
  while (package[index++] != '$')
  {
    if (index >= size)
    {
      break;
    }
  }
  return index;
}

__attribute__((always_inline))
static inline uint16_t OffsetField(const uint8_t* package, uint16_t index, const uint8_t number, const uint16_t size)
{
  for (uint8_t i = 0; i <= number - 1; i++)
  {
    while (package[index++] != ',')
    {
      if (index >= size)
      {
        break;
      }
    }
  }
  return index;
}

__attribute__((always_inline))
static inline decimal_t Str2Dec(const uint8_t* str, uint16_t index, const uint16_t size)
{
  decimal_t dec;
  dec.integer = 0;
  dec.decimal = 0;
  dec.precision = 0;
  while (str[index] >= '0' && str[index] <= '9' && index < size)
  {
    dec.integer = dec.integer * 10 + (str[index++] - '0');
  }
  if (str[index++] == '.')
  {
    while (str[index] >= '0' && str[index] <= '9' && index < size)
    {
      dec.decimal = dec.decimal * 10 + (str[index++] - '0');
      dec.precision++;
    }
  }
  return dec;
}