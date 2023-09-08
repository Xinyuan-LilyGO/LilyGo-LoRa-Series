/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      FT6X36Constants.h
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-01
 *
 */

#pragma once

#define FT3267_SLAVE_ADDRESS       (0x38)
#define FT5206_SLAVE_ADDRESS       (0x38)
#define FT6X36_SLAVE_ADDRESS       (0x38)

#define FT6X36_VENDID              (0x11)
#define FT3267_CHIPID              (0x33)
#define FT6206_CHIPID              (0x06)
#define FT6236_CHIPID              (0x36)
#define FT6236U_CHIPID             (0x64)
#define FT5206U_CHIPID             (0x64)

#define FT6X36_REG_MODE            (0x00)
#define FT6X36_REG_GEST            (0x01)
#define FT6X36_REG_STATUS          (0x02)
#define FT6X36_REG_TOUCH1_XH       (0x03)
#define FT6X36_REG_TOUCH1_XL       (0x04)
#define FT6X36_REG_TOUCH1_YH       (0x05)
#define FT6X36_REG_TOUCH1_YL       (0x06)
#define FT6X36_REG_THRESHHOLD      (0x80)
#define FT6X36_REG_MONITORTIME     (0x87)
#define FT6X36_REG_PERIODACTIVE    (0x88)
#define FT6X36_REG_PERIODMONITOR   (0x89)


#define FT6X36_REG_AUTO_CLB_MODE   (0xA0)
#define FT6X36_REG_LIB_VERSIONH    (0xA1)
#define FT6X36_REG_LIB_VERSIONL    (0xA2)
#define FT6X36_REG_INT_STATUS      (0xA4)
#define FT6X36_REG_POWER_MODE      (0xA5)
#define FT6X36_REG_FIRMVERS        (0xA6)
#define FT6X36_REG_CHIPID          (0xA3)
#define FT6X36_REG_VENDOR1_ID      (0xA8)
#define FT6X36_REG_ERROR_STATUS    (0xA9)