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
#include "app_scheduler.h"


/* Default VL53L1X Address (7 bits are used)*/
#define VL53L1X_ADDR          (0x52U >> 1)

// Scheduler settings
#define SCHED_MAX_EVENT_DATA_SIZE   sizeof(nrfx_gpiote_pin_t)
#define SCHED_QUEUE_SIZE            10


void sensor_setup(void)
{
    VL53L1X_ERROR status;
    uint16_t      temp16;
    VL53L1X_GetSensorId(VL53L1X_ADDR, &temp16);
    NRF_LOG_INFO("[Module type][Model ID] - %x", temp16);

    /*Mandatory function to init sensor*/
    status  = VL53L1X_SensorInit(VL53L1X_ADDR);
    /*Optional function to sett distance threshold*/
    status  = VL53L1X_SetDistanceThreshold(VL53L1X_ADDR, 100, 100, 0, 1);
    /*Mandatory function to start ranging operation*/
    status  = VL53L1X_StartRanging(VL53L1X_ADDR);
    NRF_LOG_INFO("Started ranging");
    NRF_LOG_FLUSH();
}

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(false);

void gpiote_init(void)
{
    nrfx_gpiote_init();
    nrfx_gpiote_in_init(12, &config, in_event_handler);
    nrfx_gpiote_in_event_enable(12, true);
}

uint8_t       sensorState = 0;
int           status      = 0;
uint32_t      int_cnt     = 0;
VL53L1X_ERROR sensor_err;

void s_in_evt_handler(void * p_event_data, uint16_t event_size)
{
    NRF_LOG_INFO("TRIGGERED %d",int_cnt);
    int_cnt++;
    sensor_err = VL53L1X_ClearInterrupt(VL53L1X_ADDR);
    APP_ERROR_CHECK(sensor_err);
}

void in_event_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    app_sched_event_put(NULL, 0, s_in_evt_handler);
}

int main(void)
{
    //init and setup - GPIOTE: LOTOHI Transition on pin no.12
    gpiote_init();

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    twi_init(ARDUINO_SCL_PIN, ARDUINO_SDA_PIN);

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    //Check if device has booted
    while(sensorState==0)
        VL53L1X_BootState(VL53L1X_ADDR,&sensorState);

    NRF_LOG_INFO("Chip booted");
    NRF_LOG_FLUSH();
    
    sensor_setup();
    
    while (true)
    {
        app_sched_execute();
        NRF_LOG_FLUSH();
    }
}

