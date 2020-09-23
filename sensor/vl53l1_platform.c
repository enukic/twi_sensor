
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

#include "vl53l1_platform.h"
#include <string.h>
#include <time.h>
#include <math.h>

#include "nrf_drv_twi.h"

uint8_t _I2CBuffer[256];

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Common addresses definition for temperature sensor. */
#define VL53L1X_ADDR          (0x52U >> 1)

//#define LM75B_REG_TEMP      0x00U
//#define LM75B_REG_CONF      0x01U
//#define LM75B_REG_THYST     0x02U
//#define LM75B_REG_TOS       0x03U

/* Mode for LM75B. */
//#define NORMAL_MODE 0U

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

extern nrf_drv_twi_t *p_twi;

/* TWI instance. */
//static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;


int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	return 0; // to be implemented
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	return 0; // to be implemented
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
        
        _I2CBuffer[0] = index>>8;
        _I2CBuffer[1] = index&0xFF;
        _I2CBuffer[2] = data;


        nrf_drv_twi_tx(p_twi, dev, _I2CBuffer,3, false);


	return 0; // to be implemented
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
        _I2CBuffer[0] = index>>8;
        _I2CBuffer[1] = index&0xFF;
        _I2CBuffer[2] = data >> 8;
        _I2CBuffer[3] = data & 0x00FF;


        nrf_drv_twi_tx(p_twi, dev, _I2CBuffer,4, false);
	return 0; // to be implemented
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	return 0; // to be implemented
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
        _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;

        nrf_drv_twi_tx(p_twi, VL53L1X_ADDR, _I2CBuffer, 2, false);
        nrf_drv_twi_rx(p_twi, VL53L1X_ADDR, data, 1);
	return 0; // to be implemented
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
        _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;

        nrf_drv_twi_tx(p_twi, VL53L1X_ADDR, _I2CBuffer, 2, false);
        nrf_drv_twi_rx(p_twi, VL53L1X_ADDR, data, 2);
	return 0; // to be implemented
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	return 0; // to be implemented
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	return 0; // to be implemented
}
