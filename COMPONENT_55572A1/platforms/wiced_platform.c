/*
 * Copyright 2016-2025, Cypress Semiconductor Corporation (an Infineon company) or
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

#include <wiced.h>
#include "wiced_platform.h"
#include "platform_mem.h"
#include "cycfg_pins.h"
#include "bt_types.h"

/* TODO: disallow to access last_seq_num */
__attribute__((weak)) uint32_t last_seq_num = 0;

/* TODO: temporary implementation for wiced_bt_utils APIs */
#include <wiced_bt_dev.h>

extern void wiced_app_hal_init(void );

#define ARIP_I2S_PIN_OUT_AUD3    0x4
extern void wiced_audio_sink_set_arip_I2SPinOut( uint8_t arip_I2SPinOut );

/**
 *  \brief Provide utility function for application to register for cb upon button interrupt
 *
 *  \param [in] button select a button from wiced_platform_button_number_t
 *  \param [in] userfn Provide the call back function
 *  \param [in] userdata Data to be provided with the call back
 *  \param [in] trigger_edge To configure interrupt on rising/falling/dual edge
 *
 *  \return none
 *
 */
void wiced_platform_register_button_callback(wiced_platform_button_number_t button, void (*userfn)(void*, UINT8), void* userdata,
                wiced_platform_button_interrupt_edge_t trigger_edge)
{
    if(button < button_count)
    {
        wiced_hal_gpio_register_pin_for_interrupt(*platform_button[button].gpio, userfn, userdata);
        wiced_hal_gpio_configure_pin(*platform_button[button].gpio, (platform_button[button].config | trigger_edge), platform_button[button].default_state);
    }
}

/**
 *  \brief Return state of the pin when button is pressed
 *
 *  \param [in] button select a button from wiced_platform_button_number_t
 *
 *  \return button pressed value
 *
 */
uint32_t wiced_platform_get_button_pressed_value(wiced_platform_button_number_t button)
{
	return platform_button[button].button_pressed_value;
}

wiced_bool_t wiced_bt_utils_acl_connect(wiced_bt_device_address_t remote_bd_addr)
{
    return WICED_FALSE;
}

wiced_bool_t wiced_bt_utils_acl_start_authentication(
        wiced_bt_device_address_t bdaddr, uint16_t hci_handle)
{
    return WICED_FALSE;
}

wiced_bool_t wiced_bt_utils_acl_start_encryption(
        wiced_bt_device_address_t bdaddr)
{
    return WICED_FALSE;
}

/* TODO: temporary implementation for wiced_bt_utils APIs */
#include <wiced_bt_ble.h>
wiced_result_t wiced_bt_utils_ble_private_device_address_resolution(
        wiced_bt_device_address_t rpa, BT_OCTET16 irk)
{
    return WICED_BT_ERROR;
}

/* TODO: temporary implementation for wiced_bt_utils APIs */
#include <wiced_bt_sco.h>
#if 0
wiced_bt_dev_status_t wiced_bt_utils_sco_create_as_acceptor_with_specific_ag(
        wiced_bt_device_address_t ag_addr, uint16_t *p_sco_index)
{
    return WICED_BT_UNKNOWN_ADDR;
}

/* TODO: temporary implementation for wiced_bt_utils_system_time_microsecond64_get */
uint64_t wiced_bt_utils_system_time_microsecond64_get(void)
{
    return 0;
}
#endif

#define cr_pad_fcn_ctl_adr5 (0x8fa30294)
#define cr_pad_fcn_ctl_adr6 (0x8fa30298)

#define AUD_I2S_SELECT_A_GPIO (3)

#if defined(WICED_AUDIO_I2S_CONTROLLER)
    #define AUD_I2S_SELECT_CLK (1)
    #define AUD_I2S_SELECT_WS (1)
    #define AUD_I2S_SELECT_DI (1)
    #define AUD_I2S_SELECT_DO (1)
#elif defined(WICED_AUDIO_I2S_TARGET)
    #define AUD_I2S_SELECT_CLK (2)
    #define AUD_I2S_SELECT_WS (2)
    #define AUD_I2S_SELECT_DI (2)
    #define AUD_I2S_SELECT_DO (2)
#elif defined(WICED_AUDIO_PCM)
    #define AUD_I2S_SELECT_CLK (4)
    #define AUD_I2S_SELECT_WS (4)
    #define AUD_I2S_SELECT_DI (4)
    #define AUD_I2S_SELECT_DO (4)
#endif

wiced_bt_gpio_select_status_t aud_i2s_gpio_select_function(wiced_bt_gpio_numbers_t pin, wiced_bt_gpio_function_t function)
{
    wiced_bt_gpio_select_status_t status = WICED_GPIO_FAILURE;
    uint32_t shift = 0, value = 0, addr = 0;
    switch(pin)
    {
        case WICED_GPIO_24: // AUD_I2S_SCK
            addr = cr_pad_fcn_ctl_adr6;
            shift = 4;
            if(function == WICED_ARM_A_GPIO_4)
            {
                value = AUD_I2S_SELECT_A_GPIO;
            }
            else if(function == WICED_AUDIO_CLK)
            {
                value = AUD_I2S_SELECT_CLK;
            }
            break;
        case WICED_GPIO_25: // AUD_I2S_MCK
            addr = cr_pad_fcn_ctl_adr6;
            shift = 0;
            if(function == WICED_ARM_A_GPIO_4)
            {
                value = AUD_I2S_SELECT_A_GPIO;
            }
            else if(function == WICED_AUDIO_CLK)
            {
                value = AUD_I2S_SELECT_CLK;
            }
            break;
        case WICED_GPIO_26: // AUD_I2S_DO
            addr = cr_pad_fcn_ctl_adr5;
            shift = 24;
            if(function == WICED_ARM_A_GPIO_5)
            {
                value = AUD_I2S_SELECT_A_GPIO;
            }
            else if(function == WICED_AUDIO_DO_OUT)
            {
                value = AUD_I2S_SELECT_DO;
            }
            break;
        case WICED_GPIO_27: // AUD_I2S_DI
            addr = cr_pad_fcn_ctl_adr5;
            shift = 28;
            if(function == WICED_ARM_A_GPIO_6)
            {
                value = AUD_I2S_SELECT_A_GPIO;
            }
            else if(function == WICED_AUDIO_DI_IN)
            {
                value = AUD_I2S_SELECT_DI;
            }
            break;
        case WICED_GPIO_28: // AUD_I2S_IRCK
            addr = cr_pad_fcn_ctl_adr6;
            shift = 8;
            if(function == WICED_ARM_A_GPIO_7)
            {
                value = AUD_I2S_SELECT_A_GPIO;
            }
            else if(function == WICED_AUDIO_WS_SYNC)
            {
                value = AUD_I2S_SELECT_WS;
            }
            break;
        default:
            break;
    }
    if(addr != 0)
    {
        *((volatile uint32_t *)(addr)) &= ~(0xf << shift);
        *((volatile uint32_t *)(addr)) |= value << shift;
        status = WICED_GPIO_SUCCESS;
    }
    return status;
}

#define cr_pad_config_adr13 (0x8fa30284)
#define cr_pad_config_adr14 (0x8fa30288)

void aud_i2s_gpio_configure_pin(uint32_t pin, uint32_t config, uint32_t outputVal)
{
    uint32_t shift = 0, value = 0, addr = 0;
    switch(pin)
    {
        case WICED_GPIO_24: // AUD_I2S_SCK
            addr = cr_pad_config_adr13;
            shift = 8;
            break;
        case WICED_GPIO_25: // AUD_I2S_MCK
            addr = cr_pad_config_adr13;
            shift = 0;
            break;
        case WICED_GPIO_28: // AUD_I2S_IRCK
            addr = cr_pad_config_adr13;
            shift = 16;
            break;
        case WICED_GPIO_26: // AUD_I2S_DO
            addr = cr_pad_config_adr14;
            shift = 16;
            break;
        case WICED_GPIO_27: // AUD_I2S_DI
            addr = cr_pad_config_adr14;
            shift = 24;
            break;
        default:
            break;
    }
    if(addr != 0)
    {
        if(GPIO_INPUT_DISABLE == (GPIO_INPUT_DISABLE & config)) // 2bits here
        {
            value |= 0x40;
        }
        if(config & GPIO_OUTPUT_ENABLE)
        {
            value |= 1;
        }
        if(config & GPIO_PULL_UP)
        {
            value |= 2;
        }
        if(config & GPIO_PULL_DOWN)
        {
            value |= 4;
        }
        if(config & GPIO_HYSTERESIS_ON)
        {
            value |= 8;
        }
        *((volatile uint32_t *)(addr)) &= 0xff << shift;
        *((volatile uint32_t *)(addr)) |= value << shift;
    }
}

void wiced_platform_init(void)
{
    uint32_t i = 0;

    wiced_app_hal_init();

    /* Configure pins available on the platform with the chosen functionality */
    for (i = 0; i < platform_gpio_pin_count; i++)
    {
        if(platform_gpio_pins[i].gpio_pin <= WICED_GPIO_23)
        {
            wiced_hal_gpio_select_function(platform_gpio_pins[i].gpio_pin, platform_gpio_pins[i].functionality);
        }
        else
        {
            aud_i2s_gpio_select_function(platform_gpio_pins[i].gpio_pin, platform_gpio_pins[i].functionality);
        }
    }

    /* Initialize LEDs and turn off by default */
    for (i = 0; i < led_count; i++)
    {
        if(*platform_led[i].gpio <= WICED_GPIO_23)
        {
            wiced_hal_gpio_configure_pin(*platform_led[i].gpio, platform_led[i].config, platform_led[i].default_state);
        }
        else
        {
            aud_i2s_gpio_configure_pin(*platform_led[i].gpio, platform_led[i].config, platform_led[i].default_state);
        }
    }

    /* Initialize buttons with the default configuration */
    for (i = 0; i < button_count; i++)
    {
        if(*platform_button[i].gpio <= WICED_GPIO_23)
        {
            wiced_hal_gpio_configure_pin(*platform_button[i].gpio, platform_button[i].config, platform_button[i].default_state);
        }
        else
        {
            aud_i2s_gpio_configure_pin(*platform_button[i].gpio, platform_button[i].config, platform_button[i].default_state);
        }
    }

    /* Initialize GPIOs with the default configuration */
    for (i = 0; i < gpio_count; i++)
    {
        if(*platform_gpio[i].gpio <= WICED_GPIO_23)
        {
            wiced_hal_gpio_configure_pin(*platform_gpio[i].gpio, platform_gpio[i].config, platform_gpio[i].default_state);
        }
        else
        {
            aud_i2s_gpio_configure_pin(*platform_gpio[i].gpio, platform_gpio[i].config, platform_gpio[i].default_state);
        }
    }

    wiced_platform_target_puart_init();

    /* I2S PinOut selection */
    wiced_audio_sink_set_arip_I2SPinOut(ARIP_I2S_PIN_OUT_AUD3);

    /* Platform memory for Bluetooth key info */
    platform_mem_init();
}
