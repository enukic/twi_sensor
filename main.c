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
#include "mpu6050.h"
#include "mpu60x0_register_map.h"

#define TOF_SCL_PIN                     NRF_GPIO_PIN_MAP(0,27)    //SCL PIN
#define TOF_SDA_PIN                     NRF_GPIO_PIN_MAP(0,26)    //SDA PIN


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\r\nTWI sensor example started.");
    NRF_LOG_FLUSH();
    twi_init(TOF_SCL_PIN, TOF_SDA_PIN);
    
    uint8_t d_add = 0x68U;
    mpu6050_init(d_add);
    bsp_board_led_on(0);    //Just an indicator

    NRF_LOG_INFO("\r\nTWI sensor example init.");
    NRF_LOG_FLUSH();



    uint32_t ret;
    while (true)
    {
        nrf_delay_ms(200);

        uint8_t read_x, read_y;
        mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_XOUT_L,&read_y,2);
        int16_t tmpx = ((int16_t)read_x << 8) | read_y;
        tmpx+=120;
        mpu6050_register_read(MPU_REG_ACCEL_YOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,2);
        int16_t tmpy = ((int16_t)read_x << 8) | read_y;
        tmpy -=2050;
        mpu6050_register_read(MPU_REG_ACCEL_YOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,2);
        int16_t tmpz = ((int16_t)read_x << 8) | read_y;

        NRF_LOG_INFO("ACEL - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);

        
        NRF_LOG_FLUSH();




    }
}

