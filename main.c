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
#include "nrfx_gpiote.h"
#include "nrf_drv_gpiote.h"

#define TOF_SCL_PIN                     NRF_GPIO_PIN_MAP(0,27)    //SCL PIN
#define TOF_SDA_PIN                     NRF_GPIO_PIN_MAP(0,26)    //SDA PIN
#define TOF_INT_PIN                     NRF_GPIO_PIN_MAP(0,31)     //INT PIN

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(false);

void gpiote_init(void)
{
    nrfx_gpiote_init();
    nrfx_gpiote_in_init(TOF_INT_PIN, &config, in_event_handler);
    nrfx_gpiote_in_event_enable(TOF_INT_PIN, true);
}

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    //app_sched_event_put(NULL, 0, s_in_evt_handler);
    //uint8_t read_x, read_y;
    //mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_ACCEL_XOUT_L,&read_y,2);
    //int16_t tmpx = ((int16_t)read_x << 8) | read_y;
    //tmpx+=120;
    //mpu6050_register_read(MPU_REG_ACCEL_YOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,2);
    //int16_t tmpy = ((int16_t)read_x << 8) | read_y;
    //tmpy -=2050;
    //mpu6050_register_read(MPU_REG_ACCEL_YOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,2);
    //int16_t tmpz = ((int16_t)read_x << 8) | read_y;

    //NRF_LOG_INFO("ACEL - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);
    
    //mpu6050_register_read(MPU_REG_GYRO_XOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_GYRO_XOUT_L,&read_y,2);
    //tmpx = ((int16_t)read_x << 8) | read_y;
    //tmpx+=120;
    //mpu6050_register_read(MPU_REG_GYRO_YOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_GYRO_YOUT_L,&read_y,2);
    //tmpy = ((int16_t)read_x << 8) | read_y;
    //tmpy -=2050;
    //mpu6050_register_read(MPU_REG_GYRO_YOUT_H,&read_x,2);
    //mpu6050_register_read(MPU_REG_GYRO_YOUT_L,&read_y,2);
    //tmpz = ((int16_t)read_x << 8) | read_y;

    //NRF_LOG_INFO("GYRO - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);
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
    
    uint8_t d_add = 0x68U;
    mpu6050_init(d_add);
    bsp_board_led_on(0);    //Just an indicator

    NRF_LOG_INFO("\r\nTWI sensor example init.");
    NRF_LOG_FLUSH();
    //gpiote_init();


    uint32_t ret;
    while (true)
    {
        nrf_delay_ms(500);
        uint8_t read_x, read_y;
        mpu6050_register_read(MPU6050_REG_ACCEL_XOFFS_H,&read_x,2);
        mpu6050_register_read(MPU6050_REG_ACCEL_XOFFS_L,&read_y,2);
        int16_t tmpx = ((int16_t)read_x << 8) | read_y;

        mpu6050_register_read(MPU6050_REG_ACCEL_YOFFS_H,&read_x,2);
        mpu6050_register_read(MPU6050_REG_ACCEL_YOFFS_L,&read_y,2);
        int16_t tmpy = ((int16_t)read_x << 8) | read_y;

        mpu6050_register_read(MPU6050_REG_ACCEL_ZOFFS_H,&read_x,2);
        mpu6050_register_read(MPU6050_REG_ACCEL_ZOFFS_L,&read_y,2);
        int16_t tmpz = ((int16_t)read_x << 8) | read_y;

        NRF_LOG_INFO("OFFS - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);

        mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_XOUT_L,&read_y,2);
        tmpx = ((int16_t)read_x << 8) | read_y;

        mpu6050_register_read(MPU_REG_ACCEL_YOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,2);
        tmpy = ((int16_t)read_x << 8) | read_y;

        mpu6050_register_read(MPU_REG_ACCEL_ZOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_ZOUT_L,&read_y,2);
        tmpz = ((int16_t)read_x << 8) | read_y;

        NRF_LOG_INFO("ACEL - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);

        //mpu6050_register_write(MPU6050_REG_ACCEL_XOFFS_H, 0x00);
        //mpu6050_register_write(MPU6050_REG_ACCEL_XOFFS_L, 0x00);
        //mpu6050_register_write(MPU6050_REG_ACCEL_YOFFS_H, 0xF5);
        //mpu6050_register_write(MPU6050_REG_ACCEL_YOFFS_L, 0x00);
        //mpu6050_register_write(MPU6050_REG_ACCEL_ZOFFS_H, 0x00);
        //mpu6050_register_write(MPU6050_REG_ACCEL_ZOFFS_L, 0x00);

        //mpu6050_register_write(MPU_REG_PWR_MGMT_1, 0x00);
        //mpu6050_register_write(MPU_REG_PWR_MGMT_1, 1<<7);
    
        //mpu6050_register_read(MPU_REG_GYRO_XOUT_H,&read_x,2);
        //mpu6050_register_read(MPU_REG_GYRO_XOUT_L,&read_y,2);
        //tmpx = ((int16_t)read_x << 8) | read_y;
        //tmpx+=45;
        //mpu6050_register_read(MPU_REG_GYRO_YOUT_H,&read_x,2);
        //mpu6050_register_read(MPU_REG_GYRO_YOUT_L,&read_y,2);
        //tmpy = ((int16_t)read_x << 8) | read_y;
        //tmpy -=245;
        //mpu6050_register_read(MPU_REG_GYRO_YOUT_H,&read_x,2);
        //mpu6050_register_read(MPU_REG_GYRO_YOUT_L,&read_y,2);
        //tmpz = ((int16_t)read_x << 8) | read_y;
        //tmpz -=243;

        //NRF_LOG_INFO("GYRO - X:%d Y:%d Z:%d",tmpx,tmpy,tmpz);

        NRF_LOG_FLUSH();
    }
}

