/*
 * mpu6050.c
 *
 *  Created on: Jul 29, 2026
 *      Author: emreb
 */

#include "mpu6050.h"
#include "main.h"

#define MPU6050_WHOAMI_REG (uint8_t)117
#define MPU6050_PWM1_REG (uint8_t)107
#define MPU6050_ACCEL_START_REG (uint8_t)59
#define MPU6050_CONFIG_REG (uint8_t)26
#define I2C_TIMEOUT 500UL
#define MPU6050_I2C_ADDR 105UL

//read 1 byte from the sensor
MPU6050_status mpu6050_read_byte(I2C_HandleTypeDef *hi2c,uint8_t reg_addr,uint8_t * data){

	if(HAL_I2C_Master_Transmit(hi2c,(MPU6050_I2C_ADDR << 1),&reg_addr, 1,I2C_TIMEOUT)!=HAL_OK)
		return MPU6050_ERR;
	if(HAL_I2C_Master_Receive(hi2c, (MPU6050_I2C_ADDR << 1), data, 1, I2C_TIMEOUT) != HAL_OK){
		return MPU6050_ERR;
	}

	return MPU6050_OK;

}

//read n bytes from the sensor
MPU6050_status mpu6050_read(I2C_HandleTypeDef *hi2c,uint8_t reg_addr,uint8_t * data,uint8_t len){

	if(HAL_I2C_Master_Transmit(hi2c,(MPU6050_I2C_ADDR << 1),&reg_addr, 1,I2C_TIMEOUT)!=HAL_OK)
		return MPU6050_ERR;
	if(HAL_I2C_Master_Receive(hi2c, (MPU6050_I2C_ADDR << 1), data, len, I2C_TIMEOUT) != HAL_OK){
		return MPU6050_ERR;
	}

	return MPU6050_OK;

}

//write 1 byte from the sensor
MPU6050_status mpu6050_write_byte(I2C_HandleTypeDef *hi2c,uint8_t reg_addr,uint8_t  data){

	uint8_t allData[2]={reg_addr,data};

	if(HAL_I2C_Master_Transmit(hi2c,(MPU6050_I2C_ADDR << 1),allData, 2,I2C_TIMEOUT)!=HAL_OK)
		return MPU6050_ERR;

	return MPU6050_OK;

}


MPU6050_status mpu6050_init(I2C_HandleTypeDef *hi2c){

	uint8_t response = 0;

	//send request to whoami reg
	if(mpu6050_read_byte(hi2c,MPU6050_WHOAMI_REG,&response) != MPU6050_OK)
		return MPU6050_ERR;

	printmsg("response: %d\r\n",response);
	if(response != 0x68 && response != 0x98)
		return MPU6050_ERR;

	//wake up from sleep mode
	if(mpu6050_write_byte(hi2c, MPU6050_PWM1_REG, 0x00)!= MPU6050_OK){
		return MPU6050_ERR;
	}


	return MPU6050_OK;

}

MPU6050_status mpu6050_Set_LPF(I2C_HandleTypeDef *hi2c,mpu6050_lpf_values lpf){

	uint8_t response = 0;

	//send request to config reg
	if(mpu6050_read_byte(hi2c,MPU6050_CONFIG_REG,&response) != MPU6050_OK)
		return MPU6050_ERR;

	printmsg("response: %d\r\n",response);

	//make first 3bit(DLPF_CFG) of value.
	response &= ~(0x07);
	//DLPF_CFG bits get value between 0 to 6
	response |= (uint8_t)lpf;

	//set lpf with wanted value by sending data to CONFIG_REG
	if(mpu6050_write_byte(hi2c, MPU6050_CONFIG_REG, response)!= MPU6050_OK){
		return MPU6050_ERR;
	}


	return MPU6050_OK;

}



MPU6050_status mpu6050_read_accelerometer_data(I2C_HandleTypeDef *hi2c,mpu6050_accel_data_t * accel_data){

	uint8_t accelData[6];
	MPU6050_status status=mpu6050_read(hi2c, MPU6050_ACCEL_START_REG, accelData, 6);
	if(status != MPU6050_OK)
		return status;

	accel_data->x = (accelData[0] << 8) | accelData[1];
	accel_data->y = (accelData[2] << 8) | accelData[3];
	accel_data->z = (accelData[4] << 8) | accelData[5];

	return status;
}


void mpu6050_calibrate_accelerometer_data(mpu6050_accel_data_t * accel_data,const mpu6050_accel_data_t * error_ofset){

	accel_data->x = accel_data->x - error_ofset->x;
	accel_data->y = accel_data->y - error_ofset->y;
	accel_data->z = accel_data->z - error_ofset->z;

}
