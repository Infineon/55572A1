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

/*******************************************************************************
*  Include
*******************************************************************************/
#include "wiced.h"
#include "wiced_memory.h"
#include "wiced_rtos.h"

/*******************************************************************************
*  Global Variable
*******************************************************************************/

/*******************************************************************************
*  Private Macro and Literal Definition
*******************************************************************************/
#define __num_of(a) (sizeof(a)/sizeof(a[0]))

/*******************************************************************************
*  Private Typedef and Enum
*******************************************************************************/
#define BIT_NUM_OF_BYTE 8
#define PLATFORM_HEAP_SIZE 1024 * 2

/*******************************************************************************
*  Private structure
*******************************************************************************/
typedef struct
{
    uint16_t size;
    uint16_t num;
} platform_mem_buf_cfg_t;

typedef struct
{
    uint16_t    free_num;
    uint8_t     *p_first;
} platform_mem_ctrl_t;

typedef struct
{
    uint8_t *p_next;
} platform_mem_buf_ctrl_t;

/*******************************************************************************
*  Static Variable
*******************************************************************************/
static platform_mem_buf_cfg_t platform_mem_cfg[] =
{
    // size,    num
    {32,        2},
    {80,        2},
    {284,       2},
    {572,       1},
};

static struct
{
    platform_mem_ctrl_t cat[__num_of(platform_mem_cfg)];
    uint8_t             *p_memory_min;
    uint8_t             *p_use_map;
    wiced_mutex_t       *p_mutex;
} platform_mem_info = {0};

wiced_bt_heap_t *p_platform_heap = NULL;

/*******************************************************************************
*  Declaration of Static Functions
*******************************************************************************/

/*******************************************************************************
*  Global Function for Outside
*******************************************************************************/

wiced_bool_t platform_mem_init(void)
{
    uint32_t total_memory_size = 0;
    uint16_t i, j;
    uint32_t num_of_use_map = 0;
    uint16_t num_of_buf = 0;
    uint8_t *p_index = NULL;
    platform_mem_buf_ctrl_t *p_buf_ctrl = NULL;

    /* Create a platform heap */
    p_platform_heap = wiced_bt_create_heap("platform_heap", NULL, PLATFORM_HEAP_SIZE, NULL,
            WICED_FALSE);
    if (p_platform_heap == NULL)
    {
        return WICED_FALSE;
    }

    /* Guarantee that the size of larger category must be strictly larger */
    for (i = 0 ; i < __num_of(platform_mem_cfg) - 1 ; i++)
    {
        if (platform_mem_cfg[i + 1].size <= platform_mem_cfg[i].size)
        {
            return WICED_FALSE;
        }
    }

    /* Calculate the total memory size. */
    for (i = 0 ; i < __num_of(platform_mem_cfg) ; i++)
    {
        platform_mem_info.cat[i].free_num = platform_mem_cfg[i].num;
        total_memory_size += (platform_mem_cfg[i].num * (sizeof(platform_mem_buf_ctrl_t) + platform_mem_cfg[i].size));

        num_of_buf += (platform_mem_cfg[i].num);
    }

    /* Create mutex. */
    platform_mem_info.p_mutex = wiced_rtos_create_mutex();

    if (!platform_mem_info.p_mutex)
    {
        return WICED_FALSE;
    }

    /* Initialize the mutex used for MIC data control. */
    if (wiced_rtos_init_mutex(platform_mem_info.p_mutex) != WICED_BT_SUCCESS)
    {
        return WICED_FALSE;
    }

    /* Request a permanent memory from MPAF. */
    num_of_use_map = ((uint32_t) num_of_buf + BIT_NUM_OF_BYTE - 1) / BIT_NUM_OF_BYTE;
    platform_mem_info.p_use_map = wiced_bt_get_buffer_from_heap(p_platform_heap, num_of_use_map);
    platform_mem_info.p_memory_min = wiced_bt_get_buffer_from_heap(p_platform_heap, total_memory_size);

    if ((platform_mem_info.p_use_map == NULL) ||
        (platform_mem_info.p_memory_min == NULL))
    {
        //wiced_rtos_deinit_mutex(platform_mem_info.p_mutex);
        return WICED_FALSE;
    }

    memset((void *) platform_mem_info.p_use_map, 0, num_of_use_map);
    memset((void *) platform_mem_info.p_memory_min, 0, total_memory_size);

    // Fill in p_next in the buffer control field of every buffer
    p_index = platform_mem_info.p_memory_min;
    for (i = 0 ; i < __num_of(platform_mem_cfg) ; i++)
    {
        platform_mem_info.cat[i].p_first = p_index;

        for (j = 0 ; j < platform_mem_cfg[i].num ; j++)
        {
            p_buf_ctrl = (platform_mem_buf_ctrl_t *) p_index;
            p_index += (sizeof(platform_mem_buf_ctrl_t) + platform_mem_cfg[i].size);
            p_buf_ctrl->p_next = p_index;
        }

        p_buf_ctrl->p_next = NULL;
    }

    return WICED_TRUE;
}

void *platform_mem_allocate(uint32_t size)
{
    uint16_t i;
    uint8_t *p_target_cat_base = NULL;
    uint8_t *p_target_base = NULL;
    uint16_t use_map_shift = 0;
    platform_mem_buf_ctrl_t *p_buf_ctrl = NULL;
    uint8_t *p_target = NULL;

    wiced_rtos_lock_mutex(platform_mem_info.p_mutex);

    p_target_cat_base = platform_mem_info.p_memory_min;
    for (i = 0 ; i < __num_of(platform_mem_cfg) ; i++)
    {
        if ((platform_mem_cfg[i].size >= size) &&
            (platform_mem_info.cat[i].free_num > 0))
        {
            break;
        }
        else
        {
            use_map_shift += platform_mem_cfg[i].num;
            p_target_cat_base += (platform_mem_cfg[i].num *
                                  (sizeof(platform_mem_buf_ctrl_t) + platform_mem_cfg[i].size));
        }
    }

    if (i == __num_of(platform_mem_cfg))
    {
        goto PLATFORM_MEM_ALLOCATE_END;
    }

    if (platform_mem_info.cat[i].p_first != NULL)
    {
        p_target_base = platform_mem_info.cat[i].p_first;
        p_buf_ctrl = (platform_mem_buf_ctrl_t *) p_target_base;

        platform_mem_info.cat[i].p_first = p_buf_ctrl->p_next;
        platform_mem_info.cat[i].free_num--;

        use_map_shift += ((p_target_base - p_target_cat_base) /
                          (sizeof(platform_mem_buf_ctrl_t) + platform_mem_cfg[i].size));

        platform_mem_info.p_use_map[use_map_shift / BIT_NUM_OF_BYTE] |= (1 << (use_map_shift % BIT_NUM_OF_BYTE));

        p_target = p_target_base + sizeof(platform_mem_buf_ctrl_t);

        memset((void *) p_target, 0 , platform_mem_cfg[i].size);
    }
    else
    {
        // Unexpected
    }

PLATFORM_MEM_ALLOCATE_END:

    wiced_rtos_unlock_mutex(platform_mem_info.p_mutex);

    return (void *) p_target;
}

void platform_mem_free(void *p_target)
{
    uint8_t *p_target_cat_base = NULL;
    uint8_t *p_target_cat_max = NULL;
    uint8_t *p_target_to_be_free = (uint8_t *) p_target;
    uint16_t use_map_shift = 0;
    uint8_t *p_target_block = NULL;
    uint16_t i;
    platform_mem_buf_ctrl_t *p_buf_ctrl = NULL;
    uint16_t tmp1 , tmp2;

    wiced_rtos_lock_mutex(platform_mem_info.p_mutex);

    p_target_cat_base = platform_mem_info.p_memory_min;
    for (i = 0 ; i < __num_of(platform_mem_cfg) ; i++)
    {
        p_target_cat_max = p_target_cat_base +
                           platform_mem_cfg[i].num * (sizeof(platform_mem_buf_ctrl_t) + platform_mem_cfg[i].size);

        if ((p_target_to_be_free >= p_target_cat_base) &&
            (p_target_to_be_free < p_target_cat_max))
        {
            break;
        }
        else
        {
            use_map_shift += platform_mem_cfg[i].num;
            p_target_cat_base = p_target_cat_max;
        }
    }

    if (i >= __num_of(platform_mem_cfg))
    {
        goto PLATFORM_MEM_FREE_END;
    }

    tmp1 = (p_target_to_be_free - p_target_cat_base) / (platform_mem_cfg[i].size + sizeof(platform_mem_buf_ctrl_t));
    tmp2 = (p_target_to_be_free - p_target_cat_base) % (platform_mem_cfg[i].size + sizeof(platform_mem_buf_ctrl_t));

    if (tmp2 == sizeof(platform_mem_buf_ctrl_t))
    {
        use_map_shift += tmp1;
        p_target_block = p_target_cat_base + tmp1 * (platform_mem_cfg[i].size + sizeof(platform_mem_buf_ctrl_t));

        p_buf_ctrl = (platform_mem_buf_ctrl_t*) p_target_block;

        /* Check address consistence. */
        if (p_target_to_be_free != p_target_block + sizeof(platform_mem_buf_ctrl_t))
        {
            goto PLATFORM_MEM_FREE_END;
        }

        /* Check if the target block has already been allocated. */
        if (platform_mem_info.p_use_map[use_map_shift / BIT_NUM_OF_BYTE] & (1 << (use_map_shift % BIT_NUM_OF_BYTE)))
        {
            goto PLATFORM_MEM_FREE_END;
        }

        /* Reset the allocation flag. */
        platform_mem_info.p_use_map[use_map_shift / BIT_NUM_OF_BYTE] &= ~(1 << (use_map_shift % BIT_NUM_OF_BYTE));

        /* Add the target block back to the head of the available block list. */
        p_buf_ctrl->p_next = platform_mem_info.cat[i].p_first;
        platform_mem_info.cat[i].p_first = p_target_block;
        platform_mem_info.cat[i].free_num++;
    }

PLATFORM_MEM_FREE_END:

    wiced_rtos_unlock_mutex(platform_mem_info.p_mutex);
}

/*******************************************************************************
*  Static Function
*******************************************************************************/

/*******************************************************************************
*  End of File
*******************************************************************************/
