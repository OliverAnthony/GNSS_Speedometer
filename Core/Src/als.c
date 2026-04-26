#include "als.h"

void ALS_Init(void)
{
    HAL_I2C_Mem_Write(&I2C_ALS, ALS_ADDR, ALS_MAIN_CTRL, 1, (uint8_t[]){0x02}, 1, 100); // ALS on
    HAL_I2C_Mem_Write(&I2C_ALS, ALS_ADDR, ALS_GAIN, 1, (uint8_t[]){0x04}, 1, 100);  // Gain 18x
    return;
}

void ALS_Read(const uint8_t reg, uint8_t *data, const uint16_t size)
{
    HAL_I2C_Mem_Read_DMA(&I2C_ALS, ALS_ADDR, reg, 1, data, size);
    return;
}