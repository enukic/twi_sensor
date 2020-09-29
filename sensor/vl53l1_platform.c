#include "vl53l1_platform.h"
#include <string.h>
#include <time.h>
#include <math.h>

#include "nrf_drv_twi.h"

uint8_t _I2CBuffer[256];

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* VL53L1X sensor address. */
#define VL53L1X_ADDR          (0x52U >> 1)

//extern nrf_drv_twi_t *p_twi;

/* TWI instance. */
static nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

void twi_init (uint32_t scl, uint32_t sda)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_lm75b_config = {
       .scl                = scl,       //27
       .sda                = sda,       //26
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_lm75b_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	return 0; // to be implemented
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	return 0; // to be implemented
}

uint32_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
        
        _I2CBuffer[0] = index>>8;
        _I2CBuffer[1] = index&0xFF;
        _I2CBuffer[2] = data;

        uint32_t ret;

        ret = nrf_drv_twi_tx(&m_twi, dev, _I2CBuffer,3, false);
        APP_ERROR_CHECK(ret);
	return ret;
}

uint32_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
        _I2CBuffer[0] = index>>8;
        _I2CBuffer[1] = index&0xFF;
        _I2CBuffer[2] = data >> 8;
        _I2CBuffer[3] = data & 0x00FF;

        uint32_t ret;
        ret = nrf_drv_twi_tx(&m_twi, dev, _I2CBuffer,4, false);
        APP_ERROR_CHECK(ret);
	return ret;
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	return 0; // to be implemented
}

uint32_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
        _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;
        uint32_t ret;

        ret = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, _I2CBuffer, 2, false);
        APP_ERROR_CHECK(ret);
        if(NRF_SUCCESS != ret)
            return ret;

        ret = nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, data, 1);
        APP_ERROR_CHECK(ret);
	return ret; // to be implemented
}

uint32_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
        _I2CBuffer[0] = index>>8;
	_I2CBuffer[1] = index&0xFF;
        uint32_t ret;

        ret = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, _I2CBuffer, 2, false);
        APP_ERROR_CHECK(ret);
        if(NRF_SUCCESS != ret)
            return ret;

        ret = nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, data, 2);
        APP_ERROR_CHECK(ret);
	return ret;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	return 0; // to be implemented
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	return 0; // to be implemented
}
