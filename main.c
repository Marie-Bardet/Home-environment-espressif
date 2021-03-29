#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
//#include "led.h"
#include "mcp9700.h"
#include "vma311.h"
#include "bme680.h"
#include "wifi.h"
#include "aio.h"
#include "mqtt.h"


//These macros define characteristics of devices
#define MCP9700_ADC_UNIT    ADC_UNIT_1
#define MCP9700_ADC_CHANNEL ADC_CHANNEL_4
#define VMA311_GPIO         GPIO_NUM_5
#define SPI_BUS HSPI_HOST
#define SPI_SCK_GPIO 18
#define SPI_MOSI_GPIO 23
#define SPI_MISO_GPIO 19
#define SPI_CS_GPIO 15

//bme680 initialization
static bme680_sensor_t *sensor = 0;
static void bme680_init()
{
    spi_bus_init(SPI_BUS, SPI_SCK_GPIO, SPI_MISO_GPIO, SPI_MOSI_GPIO);
    // init the sensor connected to SPI_BUS with SPI_CS_GPIO as chip select.
    sensor = bme680_init_sensor(SPI_BUS, 0, SPI_CS_GPIO);
    if (sensor)
    {
        printf("Bme initialization success\n");
    }
    else
    {
        printf("Could not initialize BME680 sensor\n");
    }
}



//This function is the main function of the application.
void app_main()
{
    //attribution of variables for the different parameters 
    char value_temp[8]; // value of the temperature from mcp9700
    char value_temp_vma[8]; // value of the temperature from vma
    char value_hum_vma[8]; // value of the humidity from vma*/
    char value_temp_bme[8]; // value of the temperature from bme680
    char value_hum_bme[8]; // value of the humidity from bme680
    char value_pressure_bme[8]; // value of the pressure from bme680
    char value_gas_bme[8]; // value of the gas from bme680

    //definition of variables to get the data
    uint32_t mcp_temp;
    vma311_data_t vma311_data;
    bme680_values_float_t bmeValues;
 

    //connection to the wifi
    wifi_init("Telephone Mi", "Lecheval12!");

    /* Device initialization */
    mcp9700_init(MCP9700_ADC_UNIT, MCP9700_ADC_CHANNEL);
    vma311_init(VMA311_GPIO);
    bme680_init();
    uint32_t duration = bme680_get_measurement_duration(sensor);


    //connction to adafruit
    aio_init("MarieBardet","aio_eqml05yCEBD9baSJBAuTQg3DxXKx");

    //creation of dashboard in adafruit
    aio_create_group("envmon");

    //creation of different feeds in adafruit
    aio_create_feed("mcp9700", "envmon");
    aio_create_feed("vma_hum", "envmon");
    aio_create_feed("vma_temp", "envmon");
    aio_create_feed("bme_temp", "envmon");
    aio_create_feed("bme_hum", "envmon");
    aio_create_feed("bme_pressure", "envmon");
    aio_create_feed("bme_gas", "envmon");

    //connection to mqtt
    mqtt_init("mqtts://mb170639@iot.devinci.online","mb170639","XdC1XVfc");

    //we collecte the data and send them in real time
    while(1)
    {
        //from mcp9700
        mcp_temp = mcp9700_get_value(); // we get the temperature
        sprintf(value_temp, "%d", mcp_temp); // we convert in char 
        aio_create_data(value_temp,"envmon.mcp9700"); // we send to adafruit 
        mqtt_publish("mb170639/mcp9700/temp",value_temp); // we send to mqtt

        if (bme680_force_measurement(sensor))
        {
            // passive waiting until measurement results are available
            vTaskDelay(duration);
            // get the results and do something with them
            
            if (bme680_get_results_float(sensor, &bmeValues))
            {
                sprintf(value_temp_bme, "%d", (int)bmeValues.temperature);
                sprintf(value_hum_bme, "%d", (int)bmeValues.humidity);
                sprintf(value_pressure_bme, "%d", (int)bmeValues.pressure);
                sprintf(value_gas_bme, "%d", (int)bmeValues.gas_resistance);
                aio_create_data(value_temp_bme,"envmon.bme-temp");
                aio_create_data(value_hum_bme,"envmon.bme-hum");
                aio_create_data(value_pressure_bme,"envmon.bme-pressure");
                aio_create_data(value_gas_bme,"envmon.bme-gas");
                mqtt_publish("mb170639/bme/temp",value_temp_bme);
                mqtt_publish("mb170639/bme/hum",value_hum_bme);
                mqtt_publish("mb170639/bme/pressure",value_pressure_bme);
                mqtt_publish("mb170639/bme/gas",value_gas_bme);

            }
        }

        

        //from vma
        vma311_data = vma311_get_values();
        if (vma311_data.status == VMA311_OK)
        {
          sprintf(value_temp_vma, "%d", vma311_data.t_int);
          sprintf(value_hum_vma, "%d", vma311_data.rh_int);
          aio_create_data(value_temp_vma,"envmon.vma-temp");
          aio_create_data(value_hum_vma,"envmon.vma-hum");
          mqtt_publish("mb170639/vma/temp",value_temp_vma);
          mqtt_publish("mb170639/vma/hum",value_hum_vma);
        }
        else
        {
            printf("vma311:error\n");
        }

        vTaskDelay(10000 / portTICK_PERIOD_MS);//we wait 10 sec
    }
}
