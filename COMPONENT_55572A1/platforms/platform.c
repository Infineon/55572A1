/*
 * Copyright 2016-2023, Cypress Semiconductor Corporation (an Infineon company) or
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
#include <wiced_hal_puart.h>
#include "wiced_bt_dev.h"
#include "wiced_rtos.h"
#include "wiced_transport.h"
#include "wiced_platform.h"
#include "hci_control_api.h"
#include "platform_mem.h"
#include "wiced_hal_nvram.h"
#include "cycfg_pins.h"

extern void     wiced_platform_nvram_delete(uint16_t vs_id, wiced_result_t *p_status);
extern uint16_t wiced_platform_nvram_read(uint16_t vs_id, uint16_t data_length, uint8_t *p_data, wiced_result_t *p_status);
extern uint16_t wiced_platform_nvram_write(uint16_t vs_id, uint16_t data_length, uint8_t *p_data, wiced_result_t *p_status);

typedef struct platform_virtual_nvram_t
{
    uint16_t                        data_length;
    struct platform_virtual_nvram_t *p_next;

    struct __attribute__((packed))
    {
        uint16_t                    vs_id;
        uint8_t                     data[0];
    } content;
} platform_virtual_nvram_t;

static struct
{
    struct
    {
        wiced_bool_t init;
        wiced_platform_transport_rx_data_handler *p_app_rx_data_handler;
    } transport;

    platform_virtual_nvram_t *p_virtual_nvram;
} platform_cb = {0};


/**
 * wiced_platform_i2c_init
 *
 * Initialize the I2C interface.
 */
void wiced_platform_i2c_init(void)
{
    /* TODO */
}

/**
 * wiced_platform_i2s_init
 *
 * Initialize the I2S interface.
 */
void wiced_platform_i2s_init(void)
{
    /* TODO */
}

/*
 * platform_transport_status_handler
 */
static void platform_transport_status_handler( wiced_transport_type_t type )
{
    wiced_transport_send_data(HCI_CONTROL_EVENT_DEVICE_STARTED, NULL, 0);
}

static void platform_transport_rx_data_handler_push_nvram_data(uint8_t *p_data, uint32_t data_len)
{
    uint16_t vs_id;
    uint32_t payload_len;
    platform_virtual_nvram_t *p_new;
    wiced_result_t status;

    /* Check parameter. */
    if ((p_data == NULL) ||
        (data_len == 0))
    {
        return;
    }

    /* Parse information. */
    STREAM_TO_UINT16(vs_id, p_data);
    payload_len = data_len - sizeof(vs_id);

    wiced_platform_nvram_write(vs_id, payload_len, p_data, &status);
    (void) status;
}

/*
 * platform_transport_rx_data_handler
 */
static uint32_t platform_transport_rx_data_handler(uint8_t *p_buffer, uint32_t length)
{
    uint16_t opcode;
    uint16_t payload_len;
    uint8_t *p_data = p_buffer;
    uint32_t sample_rate = 16000;
    uint8_t wiced_hci_status = 1;
    wiced_result_t status;
    uint8_t param8;

    /* Check parameter. */
    if (p_buffer == NULL)
    {
        return HCI_CONTROL_STATUS_INVALID_ARGS;
    }

    // Expected minimum 4 byte as the wiced header
    if (length < (sizeof(opcode) + sizeof(payload_len)))
    {
#ifndef NEW_DYNAMIC_MEMORY_INCLUDED
        wiced_transport_free_buffer(p_buffer);
#endif
        return HCI_CONTROL_STATUS_INVALID_ARGS;
    }

    STREAM_TO_UINT16(opcode, p_data);       // Get OpCode
    STREAM_TO_UINT16(payload_len, p_data);  // Gen Payload Length

    switch(opcode)
    {
    case HCI_CONTROL_HCI_AUDIO_COMMAND_PUSH_NVRAM_DATA:
        platform_transport_rx_data_handler_push_nvram_data(p_data, payload_len);
        break;
    default:
        if (platform_cb.transport.p_app_rx_data_handler)
        {
            (*platform_cb.transport.p_app_rx_data_handler)(opcode, p_data, payload_len);
        }
        break;
    }

#ifndef NEW_DYNAMIC_MEMORY_INCLUDED
    // Freeing the buffer in which data is received
    wiced_transport_free_buffer(p_buffer);
#endif

    return HCI_CONTROL_STATUS_SUCCESS;
}

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
wiced_bool_t wiced_platform_transport_init(wiced_platform_transport_rx_data_handler *p_rx_data_handler)
{
    wiced_transport_cfg_t cfg = {
        .type = WICED_TRANSPORT_UART,
        .cfg.uart_cfg = {
            .mode = WICED_TRANSPORT_UART_HCI_MODE,
            .baud_rate = HCI_UART_DEFAULT_BAUD,
        },
#ifdef NEW_DYNAMIC_MEMORY_INCLUDED
        .heap_config =
        {
            .data_heap_size = 1024 * 4 + 1500 * 2,
            .hci_trace_heap_size = 1024 * 2,
            .debug_trace_heap_size = 1024,
        },
#else
        .rx_buff_pool_cfg = {
            .buffer_size = 0,
            .buffer_count = 0,
        },
#endif
        .p_status_handler = platform_transport_status_handler,
        .p_data_handler = platform_transport_rx_data_handler,
        .p_tx_complete_cback = NULL,
    };

    wiced_result_t result;

    if (platform_cb.transport.init)
    {
        return WICED_FALSE;
    }

    /* Initialize the transport. */
    result = wiced_transport_init(&cfg);

    if (result == WICED_BT_SUCCESS)
    {
        platform_cb.transport.init = WICED_TRUE;
        platform_cb.transport.p_app_rx_data_handler = p_rx_data_handler;

        return WICED_TRUE;
    }

    return WICED_FALSE;
}

/**
 * wiced_platform_nvram_read
 *
 * Reads the data from NVRAM
 *
 * @param[in] vs_id         : Volatile Section Identifier. Application can use the VS IDs from
 *                            WICED_NVRAM_VSID_START to WICED_NVRAM_VSID_END
 * @param[in] data_length   : Length of the data to be read from NVRAM
 * @param[out] p_data       : Pointer to the buffer to which data will be copied
 * @param[out] p_status     : Pointer to location where status of the call is returned
 *
 * @return length of data that is read
 */
uint16_t wiced_platform_nvram_read(uint16_t vs_id, uint16_t data_length, uint8_t *p_data, wiced_result_t *p_status)
{
    platform_virtual_nvram_t *p_index;

    /* Check parameter. */
    if (p_status == NULL)
    {
        return 0;
    }

    *p_status = WICED_BADARG;

    if ((data_length == 0) ||
        (p_data == NULL))
    {
        return 0;
    }

    /* Check if the target vs_id exists. */
    p_index = platform_cb.p_virtual_nvram;
    while (p_index)
    {
        if (p_index->content.vs_id == vs_id)
        {
            /* Check the data length. */
            if (data_length < p_index->data_length)
            {
                return 0;
            }

            memcpy((void *) p_data, (void *) &p_index->content.data[0], p_index->data_length);

            break;
        }

        p_index = p_index->p_next;
    }

    if (p_index == NULL)
    {
        return 0;
    }

    *p_status = WICED_SUCCESS;

    return p_index->data_length;
}

/**
 * wiced_platform_nvram_write
 *
 * Reads the data to NVRAM
 *
 * @param[in] vs_id         : Volatile Section Identifier. Application can use the VS IDs from
 *                            WICED_NVRAM_VSID_START to WICED_NVRAM_VSID_END
 * @param[in] data_length   : Length of the data to be written to the NVRAM
 * @param[in] p_data        : Pointer to the data to be written to the NVRAM
 * @param[out] p_status     : Pointer to location where status of the call is returned
 *
 * @return number of bytes written, 0 on error
 */
uint16_t wiced_platform_nvram_write(uint16_t vs_id, uint16_t data_length, uint8_t *p_data, wiced_result_t *p_status)
{
    platform_virtual_nvram_t *p_index = NULL;
    wiced_bool_t update = WICED_FALSE;

    /* Check parameter. */
    if (p_status == NULL)
    {
        return 0;
    }

    *p_status = WICED_BADARG;

    if ((data_length == 0) ||
        (p_data == NULL))
    {
        return 0;
    }

    /* Check if the target vs_id exists. */
    p_index = platform_cb.p_virtual_nvram;
    while (p_index)
    {
        if (p_index->content.vs_id == vs_id)
        {
            wiced_result_t result;

            /* Check the data length. */
            if (data_length != p_index->data_length)
            {
                /* Delete this entry. */
                wiced_platform_nvram_delete(vs_id, p_status);

                /* Add a new entry. */
                return wiced_platform_nvram_write(vs_id, data_length, p_data, p_status);
            }

            /* Check if the data shall be updated. */
            if (memcmp((void *) p_data,
                       (void *) &p_index->content.data[0],
                       data_length) == 0)
            {
                *p_status = WICED_SUCCESS;
                return data_length;
            }

            /* Update data content. */
            memcpy((void *) &p_index->content.data[0], (void *) p_data, data_length);

            /* Inform Host device. */
            result = wiced_transport_send_data(HCI_CONTROL_HCI_AUDIO_EVENT_WRITE_NVRAM_DATA,
                                          (uint8_t *) &p_index->content,
                                          sizeof(p_index->content.vs_id) + p_index->data_length);
            if (result != WICED_SUCCESS)
            {
                // Ignore failure, transport may not be connected
            }

            *p_status = WICED_SUCCESS;
            return p_index->data_length;
        }

        p_index = p_index->p_next;
    }

    /* Acquire memory. */
    p_index = (platform_virtual_nvram_t *) platform_mem_allocate(sizeof(platform_virtual_nvram_t) - sizeof(uint8_t) + data_length);
    if (p_index == NULL)
    {
        *p_status = WICED_NO_MEMORY;
        return 0;
    }

    /* Write information. */
    p_index->content.vs_id = vs_id;
    p_index->data_length = data_length;
    memcpy((void *) &p_index->content.data[0], (void *) p_data, data_length);

    /* Add the new entry to the list. */
    p_index->p_next = platform_cb.p_virtual_nvram;
    platform_cb.p_virtual_nvram = p_index;

    /* Inform Host device. */
    if (wiced_transport_send_data(HCI_CONTROL_HCI_AUDIO_EVENT_WRITE_NVRAM_DATA,
                                  (uint8_t *) &p_index->content,
                                  sizeof(p_index->content.vs_id) + p_index->data_length) != WICED_SUCCESS)
    {
        // Ignore failure, transport may not be connected
    }

    *p_status = WICED_SUCCESS;
    return p_index->data_length;
}

/**
 * wiced_platform_nvram_delete
 *
 * Deletes data from NVRAM at specified VS id
 *
 * @param vs_id     : Volatile Section Identifier. Application can use the VS IDs from
 *                    WICED_NVRAM_VSID_START to WICED_NVRAM_VSID_END
 * @param p_status  : Pointer to location where status of the call is returned
 */
void wiced_platform_nvram_delete(uint16_t vs_id, wiced_result_t *p_status)
{
    platform_virtual_nvram_t *p_index = NULL;
    platform_virtual_nvram_t *p_pre = NULL;

    /* Check parameter. */
    if (p_status == NULL)
    {
        return;
    }

    *p_status = WICED_BADARG;

    p_index = platform_cb.p_virtual_nvram;
    p_pre = NULL;
    while (p_index)
    {
        if (p_index->content.vs_id == vs_id)
        {
            /* Inform Host device. */
            wiced_transport_send_data(HCI_CONTROL_HCI_AUDIO_EVENT_DELETE_NVRAM_DATA,
                                      (uint8_t *) &p_index->content.vs_id,
                                      sizeof(p_index->content.vs_id));

            /* Remove this entry from the list. */
            if (p_pre == NULL)
            {
                platform_cb.p_virtual_nvram = p_index->p_next;
            }
            else
            {
                p_pre->p_next = p_index->p_next;
            }

            platform_mem_free((void *) p_index);

            break;
        }

        p_pre = p_index;
        p_index = p_index->p_next;
    }
}
