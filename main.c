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
#include "nrfx_gpiote.h"
#include "nrf_drv_gpiote.h"

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
nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);

uint8_t flg=0;

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    flg=1;
}

int main(void)
{
    ret_code_t err_code;

    //init and setup - GPIOTE: Transition on pin no.12
    nrfx_gpiote_init();
    nrfx_gpiote_in_init(12, &config, in_event_handler);
    nrfx_gpiote_in_event_enable(12, true);

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
    //--------------------------------------------------------------//
    index = 0x010F;
    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    err_code = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, I2CBuffer, 2, false);
    APP_ERROR_CHECK(err_code);
    err_code= nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, &sensorState, 1);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("model id: %x",sensorState);

    index = 0x0110;
    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    err_code = nrf_drv_twi_tx(&m_twi, VL53L1X_ADDR, I2CBuffer, 2, false);
    APP_ERROR_CHECK(err_code);
    err_code= nrf_drv_twi_rx(&m_twi, VL53L1X_ADDR, &sensorState, 1);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("module type: %x",sensorState);
    NRF_LOG_FLUSH();
    //--------------------------------------------------------------//
    
    /*Mandatory function to init sensor*/
    VL53L1X_SensorInit(VL53L1X_ADDR);
    /*Optional function to sett distance threshold*/
    VL53L1X_SetDistanceThreshold(VL53L1X_ADDR, 100, 100, 0, 1);
    /*Mandatory function to start ranging operation*/
    VL53L1X_StartRanging(VL53L1X_ADDR);
    NRF_LOG_INFO("Started ranging");
    NRF_LOG_FLUSH();

    uint8_t     isdatardy;
    uint16_t    distance;
    uint32_t    int_cnt   = 0;

    while (true)
    {
//        nrf_delay_ms(500);//delay used in polling mode
          ////Polling for data
//        VL53L1X_CheckForDataReady(VL53L1X_ADDR, &isdatardy);
//        if(isdatardy == 1)
//        {   //Reading ranging data
//            VL53L1X_GetDistance(VL53L1X_ADDR,&distance);
//        }
        //Clearing interrupt - if needed can be moved under following if statement
        VL53L1X_ClearInterrupt(VL53L1X_ADDR);
        if(flg==1)
        {
            NRF_LOG_INFO("TRIGGERED %d",int_cnt);
            int_cnt++;
            flg=0;
        }

//        NRF_LOG_INFO("DISTANCE: %d",distance);
        NRF_LOG_FLUSH();
    }
}

