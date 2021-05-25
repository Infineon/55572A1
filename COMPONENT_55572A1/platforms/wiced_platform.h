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

#pragma once

#ifndef __WICED_PLATFORM_H__
#define __WICED_PLATFORM_H__

#include <wiced_hal_gpio.h>
#include "platform_target.h"

/*! configuration settings for button, x can be GPIO_EN_INT_RISING_EDGE or GPIO_EN_INT_FALLING_EDGE or GPIO_EN_INT_BOTH_EDGE */
#define WICED_GPIO_BUTTON_SETTINGS(x) (GPIO_INPUT_ENABLE | GPIO_PULL_UP | (x))

//! Possible interrupt configuration for platform buttons
typedef enum
{
    WICED_PLATFORM_BUTTON_BOTH_EDGE = WICED_GPIO_EN_INT_BOTH_EDGE,   //!< indicates that app. should receive interrupt on both edges
    WICED_PLATFORM_BUTTON_RISING_EDGE = WICED_GPIO_EN_INT_RISING_EDGE, //!< indicates that app. should receive interrupt only for rising edge
    WICED_PLATFORM_BUTTON_FALLING_EDGE = WICED_GPIO_EN_INT_FALLING_EDGE,//!< indicates that app. should receive interrupt only for falling edge
} wiced_platform_button_interrupt_edge_t;

/**
 * configuration for the platform GPIOs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t gpio_pin; /**< WICED GPIO pin */
    wiced_bt_gpio_function_t functionality; /**< chosen functionality for the pin */
}
wiced_platform_gpio_t;

/**
 * Configuration for platform LEDs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
}
wiced_platform_led_config_t;

/**
 * Configuration for platform Buttons
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., interrupt is configured through wiced_platform_register_button_callback(...) */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
    uint32_t button_pressed_value; /**< Button pressed value */
}
wiced_platform_button_config_t;

/**
 * Configuration for platform GPIOs
 */
typedef struct
{
    wiced_bt_gpio_numbers_t* gpio; /**< WICED GPIO pin */
    uint32_t config; /**< configuration like GPIO_PULL_DOWN,GPIO_PULL_UP etc., interrupt is configured through wiced_platform_register_button_callback(...) */
    uint32_t default_state; /**< GPIO_PIN_OUTPUT_HIGH/GPIO_PIN_OUTPUT_LOW */
}
wiced_platform_gpio_config_t;


/*! pin for buttons */
#define WICED_BUTTON1 BUTTON_USER


/** \addtogroup Platfrom config - Peripherals pin configuration
*   \ingroup HardwareDrivers
*/
/*! @{ */

/**
 * wiced_platform_transport_rx_data_handler
 *
 * Callback registered by the application to receive the incoming HCI UART data.
 *
 * @param[in] op_code   : operation code for the incoming HCI data (refer to hci_control_api.h)
 * @param[in] p_data    : Pointer to the received data for the op_code
 * @param[in] data_len  : length of the data pointed to by p_data in bytes
 */
typedef void (wiced_platform_transport_rx_data_handler)(uint16_t op_code, uint8_t *p_data, uint32_t data_len);

void wiced_platform_init(void);

/**
 * wiced_platform_i2c_init
 *
 * Initialize the I2C interface.
 */
/* utility functions */
void     wiced_platform_register_button_callback(wiced_platform_button_number_t button, void (*userfn)(void*, uint8_t), void* userdata, wiced_platform_button_interrupt_edge_t trigger_edge);
void wiced_platform_i2c_init(void);

/**
 * wiced_platform_i2s_init
 *
 * Initialize the I2S interface.
 */
void wiced_platform_i2s_init(void);

/**
 * wiced_platform_transport_init
 *
 * Initialize the WICED HCI Transport interface.
 *
 * @param[in] p_rx_data_handler : user callback for incoming HCI data.
 *
 * @return  WICED_TRUE - success
 *          WICED_FALSE - fail
 */
wiced_bool_t wiced_platform_transport_init(wiced_platform_transport_rx_data_handler *p_rx_data_handler);

/**
 * wiced_platform_puart_init
 *
 * @param[in] baud_rate : debug PUART baudrate
 */
void wiced_platform_puart_init(uint32_t baud_rate);

/**
 * Initialize the target PUART.
 */
void wiced_platform_target_puart_init(void);

#endif

/* @} */
