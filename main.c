#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_delay.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

//#include "vl53l1x_api.h"
//#include "vl53l1_platform.h"
//#include "mpu6050.h"
//#include "mpu60x0_register_map.h"
#include "nrfx_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "bno055.h"

#define TOF_SCL_PIN                     NRF_GPIO_PIN_MAP(0,27)    //SCL PIN
#define TOF_SDA_PIN                     NRF_GPIO_PIN_MAP(0,26)    //SDA PIN
#define TOF_INT_PIN                     NRF_GPIO_PIN_MAP(0,31)     //INT PIN

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);

void gpiote_init(void)
{
    config.pull = NRF_GPIO_PIN_NOPULL;
    //config.pull = NRF_GPIO_PIN_PULLDOWN;
    //config.pull = NRF_GPIO_PIN_PULLUP;
    nrfx_gpiote_init();
    nrfx_gpiote_in_init(TOF_INT_PIN, &config, in_event_handler);
    nrfx_gpiote_in_event_enable(TOF_INT_PIN, true);
}

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    NRF_LOG_INFO("TRIGGERED");
    bno055_set_intr_rst(1);
}


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\r\nTWI sensor example started.");
    NRF_LOG_FLUSH();

    //gpiote_init();
    twi_init(TOF_SCL_PIN, TOF_SDA_PIN);
    
    bsp_board_led_on(0);    //Just an indicator

    NRF_LOG_INFO("\r\nTWI sensor example init.");
    NRF_LOG_FLUSH();
    gpiote_init();
    I2C_routine();
    s32 comres = BNO055_ERROR;
    //comres = bno055_init(&bno055);

    comres += bno055_set_power_mode(BNO055_POWER_MODE_NORMAL);
    comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_CONFIG);
    struct bno055_accel_t accel_xyz;
    struct bno055_gyro_t gyro_xyz;
    struct bno055_mag_t mag_xyz;

    bno055_set_gyro_any_motion_axis_enable(0, BNO055_BIT_ENABLE);
    bno055_set_gyro_any_motion_axis_enable(1, BNO055_BIT_ENABLE);
    bno055_set_gyro_any_motion_axis_enable(2, BNO055_BIT_ENABLE);
    bno055_set_gyro_any_motion_filter(BNO055_GYRO_UNFILTERED_CONFIG);
    //bno055_set_gyro_any_motion_filter(BNO055_GYRO_FILTERED_CONFIG);


    bno055_set_gyro_any_motion_thres(20);

    bno055_set_gyro_any_motion_slope_samples(0);

    bno055_set_gyro_any_motion_awake_durn(0);

    bno055_set_intr_gyro_any_motion(BNO055_BIT_ENABLE);
    bno055_set_intr_mask_gyro_any_motion(BNO055_BIT_ENABLE);
        comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_AMG);
            bno055_set_intr_rst(1);



    uint32_t ret;
    while (true)
    {
        nrf_delay_ms(500);
        bno055_read_accel_xyz(&accel_xyz);
        NRF_LOG_INFO("ACC: X[%d] Y[%d] Z[%d]", accel_xyz.x, accel_xyz.y, accel_xyz.z);
        bno055_read_gyro_xyz(&gyro_xyz);
        NRF_LOG_INFO("GYR: X[%d] Y[%d] Z[%d]", gyro_xyz.x, gyro_xyz.y, gyro_xyz.z);
        bno055_read_mag_xyz(&mag_xyz);
        NRF_LOG_INFO("MAG: X[%d] Y[%d] Z[%d]", mag_xyz.x, mag_xyz.y, mag_xyz.z);
        if(nrfx_gpiote_in_is_set(TOF_INT_PIN))
        {
            NRF_LOG_INFO("PIN IS SET");
            
        }

        NRF_LOG_FLUSH();
    }
}

