#include <stdio.h>// declare the macros, constants and definition of the functions for input/ouput operations
#include "freertos/FreeRTOS.h" // to run the freeRTOS in ESP32
#include "freertos/task.h" //provides the multitasking functionalities
#include "sdkconfig.h"//provide ESP specific API / libraries
#include "mcp9700.h"
#include "vma311.h"
#include "bme680.h"
#include "wifi.h"
#include "aio.h"
#include "mqtt.h"


//These macros define characteristics of devices
#define MCP9700_ADC_UNIT    ADC_UNIT_1 //ADC 1 used for MCP9700
#define MCP9700_ADC_CHANNEL ADC_CHANNEL_4 //channel 4 used for MCP9700
#define VMA311_GPIO         GPIO_NUM_5 // GPIO 5 for VMA311
#define SPI_BUS HSPI_HOST  // definition of the bus communication of the SPI
//definition of the GPIO of the ESP32 used for the SPI interface with the BME680
#define SPI_SCK_GPIO 18    
#define SPI_MOSI_GPIO 23
#define SPI_MISO_GPIO 19
#define SPI_CS_GPIO 15

//bme680 initialization
static bme680_sensor_t *sensor = 0; //pointer that will point to the place in memory where the bme sensor is stored
static void bme680_init() //intialization of the bme680
{
    spi_bus_init(SPI_BUS, SPI_SCK_GPIO, SPI_MISO_GPIO, SPI_MOSI_GPIO);
    // init the sensor connected to SPI_BUS with SPI_CS_GPIO as chip select.
    sensor = bme680_init_sensor(SPI_BUS, 0, SPI_CS_GPIO); //0 for using a SPI interface 
    if (sensor) //if the initialization is successful 
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
    vma311_data_t vma311_data; //creates a a static instance for vma values
    bme680_values_float_t bmeValues; //creates a a static instance for bme values
 

    //connection to the wifi
    wifi_init("Telephone Mi", "Lecheval12!");

    /* Device initialization */
    mcp9700_init(MCP9700_ADC_UNIT, MCP9700_ADC_CHANNEL); // initialize the mcp9700 with ADC1 and channel 4 
    vma311_init(VMA311_GPIO); // inilialize vma to GPIO5
    bme680_init();
    uint32_t duration = bme680_get_measurement_duration(sensor); // time during which the BME is measuring


    //connection to adafruit with username and key
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
    while(1)//endless loop
    {
        //from mcp9700
        mcp_temp = mcp9700_get_value(); // we get the temperature adn store in mcp_temp
        sprintf(value_temp, "%d", mcp_temp); // we convert in char to send it in adafruit
        aio_create_data(value_temp,"envmon.mcp9700"); // we send to adafruit 
        mqtt_publish("mb170639/mcp9700/temp",value_temp); // we send to mqtt in the feed "mcp9700/temp"

        if (bme680_force_measurement(sensor))//we start one measurement
        {
            // passive waiting until measurement results are available
            vTaskDelay(duration);
            // get the results and do something with them
            
            if (bme680_get_results_float(sensor, &bmeValues))//if any results are get, we convert them into char and send them on adafruit and mqtt
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
        vma311_data = vma311_get_values(); //we put the value get from the sensor in vam311_data
        if (vma311_data.status == VMA311_OK)//if values are return in a certain duration, we convert value in char and send them in adafruit and mqtt
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

        vTaskDelay(10000 / portTICK_PERIOD_MS);//we wait 10 seconds
    }
}
