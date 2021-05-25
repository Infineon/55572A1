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

#include <wiced.h>
#include "wiced_platform.h"
#include "cycfg_pins.h"
#include "bt_types.h"

/* TODO: disallow to access last_seq_num */
uint32_t last_seq_num = 0;

/* TODO: temporary implementation for wiced_bt_utils APIs */
#include <wiced_bt_dev.h>

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

wiced_bool_t wiced_bt_utils_acl_connect(wiced_bt_device_address_t remote_bd_addr)
{
    return WICED_FALSE;
}

uint16_t wiced_bt_utils_acl_conn_handle_get(
        wiced_bt_device_address_t remote_bda, wiced_bt_transport_t transport)
{
    return 0xFFFF;
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

/* TODO: temporary implementation for wiced_hal_gpio APIs */
uint32_t wiced_hal_gpio_get_pin_input_status(uint32_t pin)
{
    return 0;
}


void wiced_hal_gpio_register_pin_for_interrupt(uint16_t pin,
        void (*userfn)(void*, uint8_t), void* userdata)
{
    return;
}
