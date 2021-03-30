#include <stdlib.h>
#include "mcp9700.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static esp_adc_cal_characteristics_t *adc_chars; //pointer for ADC characterisitics
static mcp9700_t mcp9700;//create a static instance of the mcp9700 object (in this file)

void mcp9700_init(adc_unit_t unit, adc_channel_t channel)
{
    mcp9700.unit = unit;
    mcp9700.channel=channel;
    if (unit == ADC_UNIT_1) //ADC 1 
    {
        adc1_config_width(ADC_WIDTH);                  // the width of the ADC1 is 12 bits
        adc1_config_channel_atten(channel, ADC_ATTEN); // configures the attenuation of the channel to give full scale voltage of 1,1V
    }
    else //ADC 2 
    {
        adc2_config_channel_atten(channel, ADC_ATTEN); //same as ADC1 but the width of ADC 2 is defined automatically when there's a reading
    }
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t)); //place allocated in memory with all bits equal to 0 in the pointer adc_chars
    esp_adc_cal_characterize(unit, ADC_ATTEN, ADC_WIDTH, DEFAULT_VREF, adc_chars); //this function stores the ADC voltage curve based on the characterisitics of the ADC at adc_chars
    mcp9700.adc_chars = *adc_chars;
}

int32_t mcp9700_get_value()
{
    uint32_t adc_reading = 0;
    uint32_t voltage;
    uint32_t temperature;
    for (int i = 0; i < NO_OF_SAMPLES; i++) //here we have 64 samples
    {
        if (mcp9700.unit == ADC_UNIT_1) //if ADC1
        {
            adc_reading += adc1_get_raw(mcp9700.channel); //add the value of each reading to the sum
        }
        else //if ADC2
        {
            int raw; //the function for ADC 2 has more parameters and the value is stored in the "raw" variable
            adc2_get_raw(mcp9700.channel, ADC_WIDTH, &raw);
            adc_reading += raw;
        }
    }
    adc_reading /= NO_OF_SAMPLES;                                 //compute the average value by dividing by number of samples
    voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars); //converts the average value to an average voltage value in mV
    //printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);     //prints the voltage on the serial
    voltage = voltage - 500;                                       // we substract the offset
    temperature = voltage/10;                                           // we convert the voltage into temperature
    return temperature;
}
