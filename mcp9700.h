/*
 * BSD 2-Clause License
 * 
 * Copyright (c) 2021, ESILV
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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