/*
 * mpu6050.h
 *
 *  Created on: Jul 29, 2026
 *      Author: emreb
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_
#include "main.h"

typedef enum{
	MPU6050_OK,
	MPU6050_ERR
}MPU6050_status;

typedef enum{
	MPU6050_BANDWITH_256,
	MPU6050_BANDWITH_188,
	MPU6050_BANDWITH_98,
	MPU6050_BANDWITH_42,
	MPU6050_BANDWITH_20,
	MPU6050_BANDWITH_10,
	MPU6050_BANDWITH_5
}mpu6050_lpf_values;

typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
}mpu6050_accel_data_t;

MPU6050_status mpu6050_init(I2C_HandleTypeDef *);
MPU6050_status mpu6050_read_byte(I2C_HandleTypeDef *,uint8_t,uint8_t * );
MPU6050_status mpu6050_read(I2C_HandleTypeDef *,uint8_t,uint8_t *,uint8_t );
MPU6050_status mpu6050_read_accelerometer_data(I2C_HandleTypeDef *,mpu6050_accel_data_t *);
void mpu6050_calibrate_accelerometer_data(mpu6050_accel_data_t *,const mpu6050_accel_data_t *);
MPU6050_status mpu6050_Set_LPF(I2C_HandleTypeDef *,mpu6050_lpf_values);

#endif /* INC_MPU6050_H_ */
