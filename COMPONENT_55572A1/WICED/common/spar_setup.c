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
/**
 * The Stackable Patch and Application Runtime
 * C-runtime setup of this SPAR tier
 */
#include <wiced_app_pre_init_cfg.h>
#include <wiced_platform.h>
#include "sparcommon.h"


extern void* app_iram_bss_begin;
extern void* app_iram_bss_length;
extern void* app_iram_data_begin;
extern void* app_iram_data_length;
extern void* app_irom_data_begin;


void SPAR_CRT_SETUP(void);


wiced_pre_init_cfg_t pre_init_cfg = {
    /* ACL Configuration - Default value */
    .acl_config     = {
        .host_claim_host_to_device_size = BT_ACL_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE,
        .host_to_device_size = BT_ACL_HOST_TO_DEVICE_DEFAULT_SIZE,
        .device_claim_device_to_host_size = 0, // this currently is a dont-care for Bluetooth
        .device_to_host_size = BT_ACL_DEVICE_TO_HOST_DEFAULT_SIZE,
        .host_claim_host_to_device_count = BT_ACL_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_COUNT,
        .host_to_device_count = APP_CFG_BT_ACL_HOST_TO_DEVICE_COUNT,
        .device_to_host_count = APP_CFG_BT_ACL_DEVICE_TO_HOST_COUNT,
    },

    /* LE ACL Configuration - Default Value */
    .ble_acl_config = {
        .host_claim_host_to_device_size = BT_BLE_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE,
        .host_to_device_size = APP_BT_BLE_HOST_TO_DEVICE_DEFAULT_SIZE,
        .device_claim_device_to_host_size = BT_BLE_DEVICE_CLAIM_DEVICE_TO_HOST_DEFAULT_SIZE,
        .device_to_host_size = APP_BT_BLE_DEVICE_TO_HOST_DEFAULT_SIZE,
        .host_claim_host_to_device_count = BT_BLE_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_COUNT,
        .host_to_device_count = APP_CFG_BT_BLE_HOST_TO_DEVICE_COUNT,
        .device_to_host_count = APP_CFG_BT_BLE_DEVICE_TO_HOST_COUNT,
    },

    /* Dynamic Memory Configuration - Default Value */
    .dynamic_mem_config = {
        .num_pools = DYNAMIC_MEMORY_NUM_POOLS,
        .pools = {
            [0] = {
                .size = APP_CFG_DYNMEM_SIZE_1,
                .count = APP_CFG_DYNMEM_COUNT_1,
                .die_reserve = APP_CFG_DYNMEM_DIE_RESERVE_1,
            },
            [1] = {
                .size = APP_CFG_DYNMEM_SIZE_2,
                .count = APP_CFG_DYNMEM_COUNT_2,
                .die_reserve = APP_CFG_DYNMEM_DIE_RESERVE_2,
            },
            [2] = {
                .size = APP_CFG_DYNMEM_SIZE_3,
                .count = APP_CFG_DYNMEM_COUNT_3,
                .die_reserve = APP_CFG_DYNMEM_DIE_RESERVE_3,
            },
            [3] = {
                .size = APP_CFG_DYNMEM_SIZE_4,
                .count = APP_CFG_DYNMEM_COUNT_4,
                .die_reserve = APP_CFG_DYNMEM_DIE_RESERVE_4,
            },
            [4] = {
                .size = APP_CFG_DYNMEM_SIZE_5,
                .count = APP_CFG_DYNMEM_COUNT_5,
                .die_reserve = APP_CFG_DYNMEM_DIE_RESERVE_5,
            },
        },
    },

    /* Entry Function */
    .wiced_app_entry_fn = SPAR_CRT_SETUP,

    /* Use default stack size */
    .app_thread_stack_size = 0,

    /* WICED_APP Scatter Information */
    .app_iram_bss_begin = &app_iram_bss_begin,
    .app_iram_bss_length = (uint32_t)&app_iram_bss_length,
    .app_iram_data_begin = &app_iram_data_begin,
    .app_iram_data_length = (uint32_t)&app_iram_data_length,
    .app_irom_data_begin = &app_irom_data_begin,

    /* Maximum LE connection number - Default */
    .ble_con_max = APP_CFG_ULP_MAX_CONNECTION,

    /* LE ISO Buffer Configuration - Default Value */
    .iso_buf_cfg = {
        .host_claim_host_to_device_size = BT_BLE_ISOC_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE,
        .host_to_device_size = BT_BLE_ISOC_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE,
        .device_claim_device_to_host_size = BT_BLE_ISOC_DEVICE_CLAIM_DEVICE_TO_HOST_DEFAULT_SIZE,
        .device_to_host_size = BT_BLE_ISOC_DEVICE_CLAIM_DEVICE_TO_HOST_DEFAULT_SIZE,
        .host_to_device_count = APP_CFG_BT_BLE_ISOC_HOST_TO_DEVICE_COUNT,
        .device_to_host_count = APP_CFG_BT_BLE_ISOC_DEVICE_TO_HOST_COUNT,
        .device_iso_pdu_size = (BT_BLE_ISOC_DEVICE_PDU_DEFAULT_SIZE + 3) & ~0x3,
        .device_tx_pdu_count = APP_CFG_BT_BLE_ISOC_DEVICE_TX_PDU_COUNT,
        .device_rx_pdu_count = APP_CFG_BT_BLE_ISOC_DEVICE_RX_PDU_COUNT,
        .device_iso_ring_buff_element_size = (BT_BLE_ISOC_DEVICE_PDU_RB_ELEMENT_SIZE + 3) & ~0x3,
        .device_iso_ring_buff_element_count = APP_CFG_BT_BLE_ISOC_DEVICE_PDU_RB_ELEMENT_COUNT,
    },

    /* Process Classic Audio - Disable */
    .enable_br_audio = APP_CFG_ENABLE_BR_AUDIO,
    .p_br_audio_cfg = APP_CFG_BR_AUDIO_CFG,
};

const wiced_pre_init_cfg_t *const pre_init_cfg_addr __attribute__((section(".pre_init_cfg"))) = &pre_init_cfg;


__attribute__((section(".app_entry")))
void SPAR_CRT_SETUP(void)
{
    wiced_platform_init();
    application_start();
}
