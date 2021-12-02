/*
 * tmp75.c
 *
 *  Created on: 2 ธ.ค. 2564
 *      Author: TUF A15
 */
#include "main.h"
#include "tmp75.h"
#if _TMP75_I2C
#include "i2c.h"
#endif

Param_TMP75 tmp;

HAL_StatusTypeDef tmp_master_transfer(uint16_t DevAddress, uint8_t *pData,
        uint16_t Size, uint32_t Timeout) {
//#if _TMP75_I2C
	return HAL_I2C_Master_Transmit(tmp.i2c, DevAddress, pData, Size, Timeout);
//#endif
	return HAL_TIMEOUT;
}
HAL_StatusTypeDef tmp_mem_read(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize,
		uint8_t *pData) {
//#if _TMP75_I2C
	return HAL_I2C_Mem_Read(tmp.i2c, DevAddress, (uint16_t) MemAddress, MemAddSize, pData, 2, 100);
//#endif
	return HAL_TIMEOUT;
}
HAL_StatusTypeDef tmp_mem_write(uint16_t DevAddress, uint16_t MemAddress,
        uint16_t MemAddSize, uint8_t *pData) {
//#if _TMP75_I2C
	return HAL_I2C_Mem_Write(tmp.i2c, DevAddress, MemAddress,
			MemAddSize, pData, 2, 100);
//#endif
	return HAL_TIMEOUT;
}

uint16_t tmp75_read(uint8_t addr) {
	uint8_t temp[2];
	union {
		uint16_t word;
		struct {
			uint8_t low;
			uint8_t high;
		} byte;
	} value;
	tmp_mem_read(addr, (uint16_t) TEMP_REG, I2C_MEMADD_SIZE_8BIT, temp);
	value.byte.high = temp[0];
	value.byte.low = temp[1];
	value.word >>= 4;
	return value.word;
}

void tmp75_write(uint8_t register_pointer, uint16_t register_value) {
	HAL_StatusTypeDef status = HAL_OK;

	status = tmp_mem_write(0xA0, (uint16_t) register_pointer,
	I2C_MEMADD_SIZE_8BIT, (uint8_t*) (&register_value));

	if (status != HAL_OK) {

	}
}

void tmp75_start(void) {
//	if(!thermocouple_input()) return;
	tmp75_write(0x01, 0x60);
}

void tmp75_error_check(void) {
	if (tmp.temp_int16 > 1250 || tmp.temp_int16 < -400) {
		tmp_master_transfer(0x00, (uint8_t*) 0x06, 1, 500);
		// Reinitialize TMP75
		tmp75_start();
	}
}

void tmp75_init(I2C_HandleTypeDef *i2c, uint16_t time_tick) {
	tmp.i2c = i2c;
	tmp.tick_limit = time_tick;
	tmp.time = 500;

	tmp75_start();
}

void tmp75_temp_run_tick(void)
{
	if (tmp.tick < 65535) tmp.tick++;
}
void tmp75_run(void) {
	short int DATATMP75;

	if (tmp.tick < tmp.tick_limit)
		return;
	tmp.tick = 0;

	tmp.time = 0;
	DATATMP75 = tmp75_read((uint8_t) 0x90);
	if (DATATMP75 == 0x0FFF || DATATMP75 == 0x0000) {
		DATATMP75 = tmp.Data;
	}
	tmp.Data = DATATMP75;
	if (DATATMP75 & 0x800) {
		DATATMP75 ^= 0xFFF;
		DATATMP75 += 1;
		DATATMP75 = -DATATMP75;
	}
	tmp.temp_float = (float) (DATATMP75 * 0.0625);
	tmp.temp_int16 = (int16_t) (tmp.temp_float * 10);
}

int16_t get_temp_cojunction() {
	return tmp.temp_int16;
}

float get_temp_float_cojuction() {
	return tmp.temp_float;
}

const struct TMP75Class tmp75 = {
		.run = &tmp75_run,
		.run_tick = &tmp75_temp_run_tick,
		.get_temp_float = &get_temp_float_cojuction,
		.get_temp_int16 = &get_temp_cojunction,
		.init = &tmp75_init,
};
