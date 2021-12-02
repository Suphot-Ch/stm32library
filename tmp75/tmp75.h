/*
 * tmp75.h
 *
 *  Created on: 2 ธ.ค. 2564
 *      Author: TUF A15
 */

#ifndef LIB_INC_TMP75_H_
#define LIB_INC_TMP75_H_

#if !defined(_TMP75_I2C) && defined(hi2c1)
#define	_TMP75_I2C	&hi2c1
#else
#define	_TMP75_I2C	0//1
#endif
#define	TMP75OFFSET -3

enum {
	TEMP_REG, CONFIG_REG, TL_REG, TH_REG
};
typedef struct {
	uint16_t tick;
	uint16_t tick_limit;
	uint8_t time; //TIME500mSEC;
	int16_t Data;
	float 	temp_float;
	int16_t temp_int16;
	I2C_HandleTypeDef *i2c;
} Param_TMP75;

extern const struct TMP75Class {
	void (*init)(I2C_HandleTypeDef *, uint16_t);
//	void (*read)();
//	void (*write)();
//	void (*error)();

	void (*run_tick)();
	void (*run)();
	float (*get_temp_float)();
	int16_t (*get_temp_int16)();
};

extern const struct TMP75Class tmp75;

#endif /* LIB_INC_TMP75_H_ */
