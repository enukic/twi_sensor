#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "vl53l1x_api.h"
#include "vl53l1_platform.h"

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Default VL53L1X Address (7 bytes are used)*/
#define VL53L1X_ADDR          (0x52U >> 1)

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
static nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Pointer used to reference to TWI instance*/
extern nrf_drv_twi_t *p_twi = &m_twi;


/**
 * @brief UART initialization.
 */
void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_lm75b_config = {
       .scl                = ARDUINO_SCL_PIN,       //27
       .sda                = ARDUINO_SDA_PIN,       //26yy
       .frequency          = NRF_DRV_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_lm75b_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

uint8_t       sensorState = 0;
int           status      = 0;
uint8_t       I2CBuffer[256];
uint16_t      index       = 0x00E5;  //BootState index


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\r\nTWI sensor example started.");
    NRF_LOG_FLUSH();
    twi_init();

    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    

    bsp_board_led_on(0);    //Just an indicator
    while(sensorState==0)
    {//Check if device has booted
        status=VL53L1X_BootState(0x52,&sensorState);
    }
    NRF_LOG_INFO("Chip booted");
    NRF_LOG_FLUSH();

    //Testing manual reading from sensor (not using driver functions)
    index = 0x010F;
    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    ret_code_t err_code = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, I2CBuffer, 2, false);
    APP_ERROR_CHECK(err_code);
    err_code= nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, &sensorState, 1);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("model id: %x",sensorState);
    NRF_LOG_FLUSH();

    index = 0x0110;
    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    err_code = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, I2CBuffer, 2, false);
    APP_ERROR_CHECK(err_code);
    err_code= nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, &sensorState, 1);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("model id: %x",sensorState);
    NRF_LOG_FLUSH();

    VL53L1X_SensorInit(VL53L1X_ADDR);
    
    nrf_delay_ms(50);
    VL53L1X_StartRanging(VL53L1X_ADDR);
    NRF_LOG_INFO("Started ranging");
    NRF_LOG_FLUSH();

    uint8_t     isdatardy;
    uint16_t    distance;

    while (true)
    {
        nrf_delay_ms(200);
        VL53L1X_CheckForDataReady(VL53L1X_ADDR, &isdatardy);
        if(isdatardy == 1)
        {
            VL53L1X_GetDistance(VL53L1X_ADDR,&distance);
        }
        VL53L1X_ClearInterrupt(VL53L1X_ADDR);

        NRF_LOG_INFO("DISTANCE: %d",distance);
        NRF_LOG_FLUSH();
    }
}

