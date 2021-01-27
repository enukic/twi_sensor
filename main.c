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

/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Default VL53L1X Address (7 bytes are used)*/
#define VL53L1X_ADDR          (0x52U >> 1)

#define TOF_SCL_PIN                     NRF_GPIO_PIN_MAP(0,27)    //SCL PIN
#define TOF_SDA_PIN                     NRF_GPIO_PIN_MAP(0,26)    //SDA PIN

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
//static nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Pointer used to reference to TWI instance*/
//extern nrf_drv_twi_t *p_twi = &m_twi;


/**
 * @brief UART initialization.
 */
//void twi_init (void)
//{
//    ret_code_t err_code;

//    const nrf_drv_twi_config_t twi_lm75b_config = {
//       .scl                = ARDUINO_SCL_PIN,       //27
//       .sda                = ARDUINO_SDA_PIN,       //26yy
//       .frequency          = NRF_DRV_TWI_FREQ_100K,
//       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
//       .clear_bus_init     = false
//    };

//    err_code = nrf_drv_twi_init(&m_twi, &twi_lm75b_config, NULL, NULL);
//    APP_ERROR_CHECK(err_code);

//    nrf_drv_twi_enable(&m_twi);
//}

uint8_t       sensorState = 0;
int           status      = 0;
uint8_t       I2CBuffer[256];
uint16_t      index       = 0x00E5;  //BootState index


//    void mpu_init(void)
//{
//    ret_code_t ret_code;
//    // Initiate MPU driver
//    ret_code = app_mpu_init();
//    APP_ERROR_CHECK(ret_code); // Check for errors in return value
    
//    // Setup and configure the MPU with intial values
//    app_mpu_config_t p_mpu_config = MPU_DEFAULT_CONFIG(); // Load default values
//    p_mpu_config.smplrt_div = 19;   // Change sampelrate. Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV). 19 gives a sample rate of 50Hz
//    p_mpu_config.accel_config.afs_sel = AFS_2G; // Set accelerometer full scale range to 2G
//    ret_code = app_mpu_config(&p_mpu_config); // Configure the MPU with above values
//    APP_ERROR_CHECK(ret_code); // Check for errors in return value 
//}


int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    NRF_LOG_INFO("\r\nTWI sensor example started.");
    NRF_LOG_FLUSH();
    twi_init(TOF_SCL_PIN, TOF_SDA_PIN);

    I2CBuffer[0] = index>>8;
    I2CBuffer[1] = index&0xFF;
    
    uint8_t d_add = 0x68U;
    mpu6050_init(d_add);
    bsp_board_led_on(0);    //Just an indicator

    NRF_LOG_INFO("\r\nTWI sensor example init.");
    NRF_LOG_FLUSH();



    uint8_t raw_values[6];
    uint32_t ret;
    while (true)
    {
        nrf_delay_ms(100);

        uint8_t read_x, read_y, read_z;
        //mpu6050_register_read(MPU_REG_ACCEL_XOUT_L,&read_x,1);
        //mpu6050_register_read(MPU_REG_ACCEL_YOUT_L,&read_y,1);
        //mpu6050_register_read(MPU_REG_ACCEL_ZOUT_L,&read_z,1);
        //NRF_LOG_INFO("\r\n ACCEL:: %x %x %x",read_x, read_y, read_z);
        //mpu6050_register_read(MPU_REG_WHO_AM_I,&read_y,1);
        //NRF_LOG_INFO("\r\n WHO AM I: %x",read_y);
        mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&read_x,2);
        mpu6050_register_read(MPU_REG_ACCEL_XOUT_L,&read_y,2);
        //mpu6050_register_read(MPU_REG_GYRO_YOUT_H,&read_y,2);
        //mpu6050_register_read(MPU_REG_GYRO_ZOUT_H,&read_z,2);
        //NRF_LOG_INFO("GYRO: %x %x %x",read_x, read_y, read_z);
        uint16_t tmp = ((uint16_t)read_x << 8) | read_y;

        NRF_LOG_INFO("GYRO: %x",tmp);

        
        //ret = mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,&raw_values[0],6);
        //APP_ERROR_CHECK(ret);
        //NRF_LOG_INFO("\r\nDATA: %x",&raw_values);
        //ret = mpu6050_register_read(MPU_REG_GYRO_XOUT_H,&raw_values,6);
        //APP_ERROR_CHECK(ret);
        //NRF_LOG_INFO("\r\nGYROA: %x",&raw_values);
        NRF_LOG_FLUSH();




    }
}

