/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
/*******************************************************************************
* File Name: cycfg_pins.c
*
* Description:
* Pin configuration
* This file will remove when cycfg_pins.c is correct
*
********************************************************************************
* Copyright 2021 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
********************************************************************************/

#include "cycfg_pins.h"

#define BUTTON_CUSTOM_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_0].gpio_pin, \
    .config = GPIO_INPUT_ENABLE | GPIO_PULL_UP | GPIO_EN_INT_BOTH_EDGE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
    .button_pressed_value = GPIO_PIN_OUTPUT_LOW, \
}

#define BUTTON_VOL_DN_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_5].gpio_pin, \
    .config = GPIO_INPUT_ENABLE | GPIO_PULL_UP | GPIO_EN_INT_BOTH_EDGE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
    .button_pressed_value = GPIO_PIN_OUTPUT_LOW, \
}

#define BUTTON_VOL_UP_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_6].gpio_pin, \
    .config = GPIO_INPUT_ENABLE | GPIO_PULL_UP | GPIO_EN_INT_BOTH_EDGE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
    .button_pressed_value = GPIO_PIN_OUTPUT_LOW, \
}

#define SPI_CS_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_1].gpio_pin, \
    .config = GPIO_ARM_DRIVE_SEL_16MA | GPIO_PULL_UP | GPIO_HYSTERESIS_ON | GPIO_OUTPUT_ENABLE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
}

#define SPI_MISO_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_4].gpio_pin, \
    .config = GPIO_ARM_DRIVE_SEL_16MA | GPIO_PULL_UP | GPIO_HYSTERESIS_ON, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
}

#define SPI_MOSI_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_3].gpio_pin, \
    .config = GPIO_ARM_DRIVE_SEL_16MA | GPIO_PULL_UP | GPIO_HYSTERESIS_ON | GPIO_OUTPUT_ENABLE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
}

#define SPI_CLK_config \
{\
    .gpio = (wiced_bt_gpio_numbers_t*)&platform_gpio_pins[PLATFORM_GPIO_2].gpio_pin, \
    .config = GPIO_ARM_DRIVE_SEL_16MA | GPIO_PULL_UP | GPIO_HYSTERESIS_ON | GPIO_OUTPUT_ENABLE, \
    .default_state = GPIO_PIN_OUTPUT_HIGH, \
}

const wiced_platform_gpio_t platform_gpio_pins[] =
{
    [PLATFORM_GPIO_0] = { .gpio_pin = WICED_GPIO_10, .functionality = WICED_ARM_A_GPIO_6, },
    [PLATFORM_GPIO_1] = { .gpio_pin = WICED_GPIO_18, .functionality = WICED_ARM_SCB1_SPI_SELECT0, },
    [PLATFORM_GPIO_2] = { .gpio_pin = WICED_GPIO_19, .functionality = WICED_ARM_SCB1_SPI_CLK, },
    [PLATFORM_GPIO_3] = { .gpio_pin = WICED_GPIO_16, .functionality = WICED_ARM_SCB1_SPI_MOSI, },
    [PLATFORM_GPIO_4] = { .gpio_pin = WICED_GPIO_17, .functionality = WICED_ARM_SCB1_SPI_MISO, },
    [PLATFORM_GPIO_5] = { .gpio_pin = WICED_GPIO_07, .functionality = WICED_ARM_GPIO_7, },
    [PLATFORM_GPIO_6] = { .gpio_pin = WICED_GPIO_08, .functionality = WICED_ARM_A_GPIO_4, },
};
const size_t platform_gpio_pin_count = (sizeof(platform_gpio_pins) / sizeof(wiced_platform_gpio_t));
const wiced_platform_led_config_t platform_led[] =
{
};
const size_t led_count = (sizeof(platform_led) / sizeof(wiced_platform_led_config_t));
const wiced_platform_button_config_t platform_button[] =
{
    [WICED_PLATFORM_BUTTON_1] = BUTTON_CUSTOM_config,
    [WICED_PLATFORM_BUTTON_3] = BUTTON_VOL_DN_config,
    [WICED_PLATFORM_BUTTON_2] = BUTTON_VOL_UP_config,
};
const size_t button_count = (sizeof(platform_button) / sizeof(wiced_platform_button_config_t));
const wiced_platform_gpio_config_t platform_gpio[] =
{
    [WICED_PLATFORM_GPIO_1] = SPI_CS_config,
    [WICED_PLATFORM_GPIO_2] = SPI_CLK_config,
    [WICED_PLATFORM_GPIO_3] = SPI_MOSI_config,
    [WICED_PLATFORM_GPIO_4] = SPI_MISO_config,
};
const size_t gpio_count = (sizeof(platform_gpio) / sizeof(wiced_platform_gpio_config_t));
