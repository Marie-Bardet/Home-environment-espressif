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

#ifndef __VMA311_H__
#define __VMA311_H__

#include "driver/gpio.h"

/* type definitions */
typedef enum vma311_status
{
    VMA311_CRC_ERROR = -2,
    VMA311_TIMEOUT_ERROR,
    VMA311_OK
} vma311_status_t;

typedef struct vma311_data
{
    vma311_status_t status;
    int8_t rh_int;
    int8_t rh_dec;
    int8_t t_int;
    int8_t t_dec;
} vma311_data_t;

typedef struct vma311
{
    gpio_num_t num;
    int64_t last_read_time;
    vma311_data_t data;
} vma311_t;

/* function prototypes */
void          vma311_init(gpio_num_t);
vma311_data_t vma311_get_values();

#endif /* __VMA311_H__ */