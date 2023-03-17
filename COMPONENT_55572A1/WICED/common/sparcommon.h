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
 * All common definitions for this SPAR
 */
#pragma once

#define APPLICATION_START() __attribute__((section(".app_init_code"))) \
    void application_start(void)

void application_start(void);

/* ACL config */
#ifndef APP_CFG_BT_ACL_HOST_TO_DEVICE_COUNT
#define APP_CFG_BT_ACL_HOST_TO_DEVICE_COUNT         8 //BT_ACL_HOST_TO_DEVICE_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_ACL_DEVICE_TO_HOST_COUNT
#define APP_CFG_BT_ACL_DEVICE_TO_HOST_COUNT         12 //BT_ACL_DEVICE_TO_HOST_DEFAULT_COUNT
#endif

/* LE ACL config */
// ((BT_BLE_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE + BLE_ACL_BUF_SIZE_OVERHEAD) & ~0x3) + (((BT_BLE_HOST_CLAIM_HOST_TO_DEVICE_DEFAULT_SIZE + BLE_ACL_BUF_SIZE_OVERHEAD) & 0x3)?4:0)
#define APP_BT_BLE_HOST_TO_DEVICE_DEFAULT_SIZE      264
//((BT_BLE_DEVICE_CLAIM_DEVICE_TO_HOST_DEFAULT_SIZE + BLE_ACL_BUF_SIZE_OVERHEAD) & ~0x3) + (((BT_BLE_DEVICE_CLAIM_DEVICE_TO_HOST_DEFAULT_SIZE + BLE_ACL_BUF_SIZE_OVERHEAD) & 0x3)?4:0)
#define APP_BT_BLE_DEVICE_TO_HOST_DEFAULT_SIZE      264

#ifndef APP_CFG_BT_BLE_HOST_TO_DEVICE_COUNT
#define APP_CFG_BT_BLE_HOST_TO_DEVICE_COUNT         16 //BT_BLE_HOST_TO_DEVICE_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_BLE_DEVICE_TO_HOST_COUNT
#define APP_CFG_BT_BLE_DEVICE_TO_HOST_COUNT         16 //BT_BLE_DEVICE_TO_HOST_DEFAULT_COUNT
#endif

/* Dynamic memory config */
#ifndef APP_CFG_DYNMEM_SIZE_1
#define APP_CFG_DYNMEM_SIZE_1                       DEFAULT_DYNMEM_SIZE_1
#endif
#ifndef APP_CFG_DYNMEM_COUNT_1
#define APP_CFG_DYNMEM_COUNT_1                      DEFAULT_DYNMEM_COUNT_1
#endif
#ifndef APP_CFG_DYNMEM_DIE_RESERVE_1
#define APP_CFG_DYNMEM_DIE_RESERVE_1                DEFAULT_DYNMEM_DIE_RESERVE_1
#endif
#ifndef APP_CFG_DYNMEM_SIZE_2
#define APP_CFG_DYNMEM_SIZE_2                       DEFAULT_DYNMEM_SIZE_2
#endif
#ifndef APP_CFG_DYNMEM_COUNT_2
#define APP_CFG_DYNMEM_COUNT_2                      DEFAULT_DYNMEM_COUNT_2
#endif
#ifndef APP_CFG_DYNMEM_DIE_RESERVE_2
#define APP_CFG_DYNMEM_DIE_RESERVE_2                DEFAULT_DYNMEM_DIE_RESERVE_2
#endif
#ifndef APP_CFG_DYNMEM_SIZE_3
#define APP_CFG_DYNMEM_SIZE_3                       DEFAULT_DYNMEM_SIZE_3
#endif
#ifndef APP_CFG_DYNMEM_COUNT_3
#define APP_CFG_DYNMEM_COUNT_3                      DEFAULT_DYNMEM_COUNT_3
#endif
#ifndef APP_CFG_DYNMEM_DIE_RESERVE_3
#define APP_CFG_DYNMEM_DIE_RESERVE_3                DEFAULT_DYNMEM_DIE_RESERVE_3
#endif
#ifndef APP_CFG_DYNMEM_SIZE_4
#define APP_CFG_DYNMEM_SIZE_4                       DEFAULT_DYNMEM_SIZE_4
#endif
#ifndef APP_CFG_DYNMEM_COUNT_4
#define APP_CFG_DYNMEM_COUNT_4                      8 //DEFAULT_DYNMEM_COUNT_4
#endif
#ifndef APP_CFG_DYNMEM_DIE_RESERVE_4
#define APP_CFG_DYNMEM_DIE_RESERVE_4                DEFAULT_DYNMEM_DIE_RESERVE_4
#endif
#ifndef APP_CFG_DYNMEM_SIZE_5
#define APP_CFG_DYNMEM_SIZE_5                       DEFAULT_DYNMEM_SIZE_5
#endif
#ifndef APP_CFG_DYNMEM_COUNT_5
#define APP_CFG_DYNMEM_COUNT_5                      2 //DEFAULT_DYNMEM_COUNT_5
#endif
#ifndef APP_CFG_DYNMEM_DIE_RESERVE_5
#define APP_CFG_DYNMEM_DIE_RESERVE_5                DEFAULT_DYNMEM_DIE_RESERVE_5
#endif

/* ble_con_max */
#ifndef APP_CFG_ULP_MAX_CONNECTION
#define APP_CFG_ULP_MAX_CONNECTION                  4 //ULP_MAX_CONNECTION
#endif

/* LE ISO buffer */
#ifndef APP_CFG_BT_BLE_ISOC_HOST_TO_DEVICE_COUNT
#define APP_CFG_BT_BLE_ISOC_HOST_TO_DEVICE_COUNT    1 //BT_BLE_ISOC_HOST_TO_DEVICE_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_BLE_ISOC_DEVICE_TO_HOST_COUNT
#define APP_CFG_BT_BLE_ISOC_DEVICE_TO_HOST_COUNT    1 //BT_BLE_ISOC_DEVICE_TO_HOST_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_BLE_ISOC_DEVICE_TX_PDU_COUNT
#define APP_CFG_BT_BLE_ISOC_DEVICE_TX_PDU_COUNT     1 //BT_BLE_ISOC_DEVICE_TX_PDU_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_BLE_ISOC_DEVICE_RX_PDU_COUNT
#define APP_CFG_BT_BLE_ISOC_DEVICE_RX_PDU_COUNT     1 //BT_BLE_ISOC_DEVICE_RX_PDU_DEFAULT_COUNT
#endif
#ifndef APP_CFG_BT_BLE_ISOC_DEVICE_PDU_RB_ELEMENT_COUNT
#define APP_CFG_BT_BLE_ISOC_DEVICE_PDU_RB_ELEMENT_COUNT     1 //BT_BLE_ISOC_DEVICE_PDU_RB_ELEMENT_COUNT
#endif

/* Audio Enable */
#ifndef APP_CFG_ENABLE_BR_AUDIO
#define APP_CFG_ENABLE_BR_AUDIO                     0
#endif
#ifndef APP_CFG_BR_AUDIO_CFG
#define APP_CFG_BR_AUDIO_CFG                        NULL
#endif
