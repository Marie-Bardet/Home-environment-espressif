#ifndef __MCP9700_H__
#define __MCP9700_H__

#include "driver/adc.h"
#include "esp_adc_cal.h"

/* macro defintions */
#define ADC_WIDTH      ADC_WIDTH_BIT_12
#define ADC_ATTEN      ADC_ATTEN_DB_0
#define DEFAULT_VREF   1100
#define NO_OF_SAMPLES  64

/* structure definitions */
typedef struct mcp9700
{
    adc_unit_t unit;
    adc_channel_t channel;
    esp_adc_cal_characteristics_t adc_chars;
} mcp9700_t;

/* function prototypes */
void    mcp9700_init(adc_unit_t, adc_channel_t);
int32_t mcp9700_get_value();

#endif /* __MPC977_H__ */ 
