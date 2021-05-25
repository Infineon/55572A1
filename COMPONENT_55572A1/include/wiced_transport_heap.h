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
 * API Defines the transport utilities using dynamic memory allocation for sending/receiving the data.
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "wiced_data_types.h"
#include "wiced_result.h"

/*
*  Define structure to configure dynamic heap for transport
*/
typedef struct wiced_transport_heap_cfg_s
{
    /*Data heap size requirement of application, this heap will be used for data tx and Rx*/
    uint32_t      data_heap_size;

    /*Data heap size requirement of application, this heap will be used for logging hci trace*/
    uint32_t      hci_trace_heap_size;

    /*Data heap size requirement of application, this heap will be used for logging debug trace*/
    uint32_t      debug_trace_heap_size;
}wiced_transport_heap_cfg_t;

/**
* Function         wiced_trans_create_heap
*
*                  Creates dynamic memory area. Application reserves an area for dynamic
*                  variable memory allocations with this call. Application can now allocate
*                  variable sized buffers.
*
* @param[in]       Size of heap to be allocated and related configuration defined by config
*
* @return          wiced_result_t
*/
wiced_result_t wiced_trans_create_heap (wiced_transport_heap_cfg_t * heap_config);

/**
* Function         wiced_trans_malloc_data
*
*                  Allocates memory from the transport data heap
*
* @param[in]       Size of heap to be allocated and heap type
*
* @return          void
*/
void * wiced_trans_malloc_data(uint32_t length);

/**
* Function         wiced_trans_malloc_hci_trace
*
*                  Allocates memory from the transport hci trace heap
*
* @param[in]       Size of heap to be allocated and heap type
*
* @return          void
*/
void* wiced_trans_malloc_hci_trace(uint32_t length);

/**
* Function         wiced_trans_malloc_debug
*
*                  Allocates memory from the transport debug heap
*
* @param[in]       Size of heap to be allocated and heap type
*
* @return          void
*/
void* wiced_trans_malloc_debug(uint32_t length);


/**
* Function         wiced_trans_free
*
*                  Frees memory from the heap specified by type
*
* @param[in]       pointer to the heap memory.
*
* @return          void
*/
void wiced_trans_free (void *pBuf);
