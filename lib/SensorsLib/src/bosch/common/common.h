/**
 * Copyright (c) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * BSD-3-Clause
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @file    common.h
 * @brief   Common header file for the BHI260/BHA260 examples
 * @note    Adapting Arduino based on BHY2-Sensor-API by lewis
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "bosch/bhy2.h"
#include "bosch/bhi3.h"
#include "bosch/bhi3_multi_tap.h"
#include "bosch/bhy2_klio.h"
#include "bosch/bhy2_swim.h"
#include "bosch/bhy2_bsec.h"
#include "bosch/bhy2_head_tracker.h"



typedef struct bhy_config {
    enum bhy2_intf intf;
    struct bhy2_dev bhy2;
    int irq;
    int rst;
    union   __ {
        struct  {
            int sda;
            int scl;
            int addr;
            TwoWire *wire;
        } i2c_dev;
        struct  {
            int cs;
            int miso;
            int mosi;
            int sck;
#if defined(ARDUINO_ARCH_RP2040)
            SPIClassRP2040 *spi;
#else
            SPIClass *spi;
#endif
        } spi_dev;
    } u  ;
} bhy_config_t;

#define BHY2_WORK_BUFFER_SIZE       2048
#define BHY2_RD_WR_LEN              64    /* MCU maximum read write length */
#define BHY2_ASSERT(x)             if (x) check_bhy2_api(__LINE__, __FUNCTION__, x)

const char *get_coines_error(int16_t rslt);
const char *get_api_error(int8_t error_code);
const char *get_sensor_error_text(uint8_t sensor_error);
const char *get_sensor_name(uint8_t sensor_id);
float get_sensor_default_scaling(uint8_t sensor_id);
const char *get_sensor_parse_format(uint8_t sensor_id);
const char *get_sensor_axis_names(uint8_t sensor_id);

bool setup_interfaces(bool reset_power, bhy_config_t config);
void close_interfaces(enum bhy2_intf intf);
int8_t bhy2_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t length, void *intf_ptr);
int8_t bhy2_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length, void *intf_ptr);
void bhy2_delay_us(uint32_t us, void *private_data);
void check_bhy2_api(unsigned int line, const char *func, int8_t val);
void time_to_s_ns(uint64_t time_ticks, uint32_t *s, uint32_t *ns, uint64_t *tns);

#define PRINT(format, ...)      Serial.printf(format,##__VA_ARGS__)
#define INFO(format, ...)       Serial.printf("[I]" format,##__VA_ARGS__)
#define PRINT_I(format, ...)    Serial.printf(format,##__VA_ARGS__)
#define WARNING(format, ...)    Serial.printf("[W]" format,##__VA_ARGS__)
#define PRINT_W(format, ...)    Serial.printf(format,##__VA_ARGS__)
#define ERROR(format, ...)      Serial.printf("[E]" format,##__VA_ARGS__)
#define PRINT_E(format, ...)    Serial.printf(format,##__VA_ARGS__)


#endif /* _COMMON_H_ */
