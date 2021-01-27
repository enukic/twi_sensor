/**
 * Copyright (c) 2009 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdbool.h>
#include <stdint.h>

#include "twi_master.h"
#include "mpu6050.h"
#include "nrf_drv_twi.h"
#include "mpu60x0_register_map.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/*lint ++flb "Enter library region" */

#define MPU_ADDRESS               (0x68)
#define ADDRESS_WHO_AM_I          (0x75U) // !< WHO_AM_I register identifies the device. Expected value is 0x68.
#define ADDRESS_SIGNAL_PATH_RESET (0x68U) // !<

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

//#define TOF_SCL_PIN                     NRF_GPIO_PIN_MAP(0,27)    //SCL PIN
//#define TOF_SDA_PIN                     NRF_GPIO_PIN_MAP(0,26)    //SDA PIN

static const uint8_t expected_who_am_i = 0x68U; // !< Expected value to get from WHO_AM_I register.
static uint8_t       m_device_address;          // !< Device address in bits [7:1]

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


bool mpu6050_init(uint8_t device_address)
{
    bool transfer_succeeded = true;

    m_device_address = (uint8_t)(device_address << 1);

    // Do a reset on signal paths
    uint8_t reset_value = 0x04U | 0x02U | 0x01U; // Resets gyro, accelerometer and temperature sensor signal paths.
    transfer_succeeded &= mpu6050_register_write(ADDRESS_SIGNAL_PATH_RESET, reset_value);
    transfer_succeeded &= mpu6050_register_write(MPU_REG_PWR_MGMT_1, 1);
    transfer_succeeded &= mpu6050_register_write(MPU_REG_GYRO_CONFIG, (24 | 128));

    // Read and verify product ID
    transfer_succeeded &= mpu6050_verify_product_id();

    return transfer_succeeded;
}

bool mpu6050_verify_product_id(void)
{
    uint8_t who_am_i;

    uint8_t raw_values[6];
    NRF_LOG_INFO("\r\n INIT: %x",raw_values);
    
    mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,raw_values,6);
    NRF_LOG_INFO("\r\nDATA: %x",raw_values);
    uint8_t read_v;
    mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&read_v,2);
    NRF_LOG_INFO("\r\nDATA: %x",read_v);
        NRF_LOG_FLUSH();


    if (mpu6050_register_read(ADDRESS_WHO_AM_I, &who_am_i, 1))
    {
        if (who_am_i != expected_who_am_i)
        {
            NRF_LOG_INFO("\r\n%x",who_am_i);
            NRF_LOG_FLUSH();
            return false;
        }
        else
        {
            NRF_LOG_INFO("\r\n%x",who_am_i);
            NRF_LOG_FLUSH();
            return true;
        }
    }
    else
    {
        NRF_LOG_INFO("\r\n%x",who_am_i);
        NRF_LOG_FLUSH();
        return false;
    }
    
}

bool mpu6050_register_write(uint8_t register_address, uint8_t value)
{
    uint8_t w2_data[2];

    w2_data[0] = register_address;
    w2_data[1] = value;
    //return twi_master_transfer(m_device_address, w2_data, 2, TWI_ISSUE_STOP);
    uint32_t ret;

    ret = nrf_drv_twi_tx(&m_twi, MPU_ADDRESS, w2_data,2, false);
    APP_ERROR_CHECK(ret);
    NRF_LOG_INFO("\r\nTWI WRITE");
    NRF_LOG_FLUSH();
    return ret;
}

bool mpu6050_register_read(uint8_t register_address, uint8_t *destination, uint8_t number_of_bytes)
{
    //bool transfer_succeeded;
    //transfer_succeeded  = twi_master_transfer(m_device_address, &register_address, 1, TWI_DONT_ISSUE_STOP);
    //transfer_succeeded &= twi_master_transfer(m_device_address|TWI_READ_BIT, destination, number_of_bytes, TWI_ISSUE_STOP);
    //return transfer_succeeded;
    uint32_t ret;
    ret = nrf_drv_twi_tx(&m_twi, MPU_ADDRESS, &register_address, 1, false);
    APP_ERROR_CHECK(ret);
    if(NRF_SUCCESS != ret)
        return ret;

    ret = nrf_drv_twi_rx(&m_twi, MPU_ADDRESS, destination, number_of_bytes);
    APP_ERROR_CHECK(ret);
    return ret;

}

/*lint --flb "Leave library region" */
