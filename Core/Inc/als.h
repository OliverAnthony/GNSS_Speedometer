/* LTR-308ALS */
#ifndef ALS_H
#define ALS_H

#include "main.h"

#define ALS_ADDR 0xA6
#define ALS_MAIN_CTRL 0x00
#define ALS_MEAS_RATE 0x04
#define ALS_GAIN 0x05
#define ALS_DATA_0 0x0D

void ALS_Init(void);
void ALS_Read(const uint8_t reg, uint8_t *data, const uint16_t size);

#endif /* ALS_H */