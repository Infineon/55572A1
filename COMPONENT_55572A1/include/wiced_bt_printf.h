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
/** @file
 *
 * This file provided functionality to provide debug traces
 *
 */


#pragma once

#include <stdarg.h>
#include <string.h>
#include "wiced.h"
#include "wiced_bt_types.h"
#include "wiced_bt_printf.h"

#ifndef WICEDX_WIN          /* In windows, this pulls in a lot of files that cause compile errors */
    #include "wiced_hal_puart.h"
#endif

//#define WICED_BT_TRACE_ENABLE
#ifdef WICED_BT_TRACE_ENABLE
#define WICED_SHIM_TRACE_ENABLE
#endif

/** Debug UARTs. Used when calling wiced_set_debug_uart.*/
typedef enum
{
    WICED_ROUTE_DEBUG_NONE  =  0x00, /**< No traces */
    WICED_ROUTE_DEBUG_TO_WICED_UART, /**< send debug strings in formatted AIROC HCI messages over HCI UART to ClientControl or MCU */
    WICED_ROUTE_DEBUG_TO_HCI_UART,   /**< send debug strings as plain text to HCI UART, used by default if wiced_set_debug_uart() not called */
    WICED_ROUTE_DEBUG_TO_DBG_UART,   /**< Deprecated */
    WICED_ROUTE_DEBUG_TO_PUART       /**< send debug strings as plain text to the peripheral uart (PUART) */
}wiced_debug_uart_types_t;

#ifdef WICED_SHIM_TRACE_ENABLE
#define WICED_SHIM_TRACE(...)        BTU_trace_debug(__VA_ARGS__)
#else
#define WICED_SHIM_TRACE(...)
#endif

void wiced_bt_trace_enable(void);

/**
 * Function         wiced_set_debug_uart
 *
 * To specify the UART to be used for the debug traces
 *
 * @param[in]      uart        : UART to be used
 *
 * @return          void
 *
 */
void wiced_set_debug_uart ( wiced_debug_uart_types_t uart );

/**
 * Function         wiced_set_debug_uart_baudrate
 *
 * It configures the value for wiced_set_debug_uart.
 *
 * @param[in]      baudrate        : baudrate to be used
 *
 * @return          void
 *
 * @note To reflect the configured baudrate, wiced_set_debug_uart API should be invoked
 */
void wiced_set_debug_uart_baudrate ( uint32_t baudrate );

/* @} */

extern void wiced_print_trace(char* p_trace_buf, int trace_buf_len, wiced_bt_trace_type_t trace_type);
extern void wiced_trace_array( const uint8_t* p_array, uint16_t len);
extern wiced_debug_uart_types_t wiced_get_debug_uart ( void );


extern void BTU_trace_debug(const char* p_fmt, ...);

extern void BTU_trace_error(const char* p_fmt, ...);

#define wiced_printf(p1,p2,...)  BTU_trace_debug(__VA_ARGS__)

extern int BTU_sprintf(char *buf, const char *fmt, ...);
