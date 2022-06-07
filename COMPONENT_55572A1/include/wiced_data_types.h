/*
 * Copyright 2016-2022, Cypress Semiconductor Corporation (an Infineon company) or
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


#include <stdint.h>
#include <string.h>

/**
 *  @defgroup     gentypes   Common Bluetooth types
 *  @ingroup wicedbt
 *
 *  @{
 */

#ifndef WICED_FALSE
#define WICED_FALSE 0   /**< false */
#endif // !WICED_FALSE
#ifndef WICED_TRUE
#define WICED_TRUE  1   /**< true */
#endif // !WICED_TRUE

#ifndef FALSE
#define FALSE 0         /**< false */
#endif

#ifndef TRUE
#define TRUE  1         /**< true */
#endif

/** Surpress Warnings */
#define WICED_SUPPRESS_WARNINGS(m) if((m)){;}

/* Suppress unused variable warning */
#ifndef UNUSED_VARIABLE
/** Unused Variable */
#define UNUSED_VARIABLE(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

/* To prevent complier to optimize with LDM and STM instructions */
#define WICED_MEMCPY(a, b, c)       memcpy((void*)(a), (const void*)(b), c)     /**< Memory copy*/
#define WICED_MEMSET(a, b, c)       memset((void*)(a), b, c)                    /**< Memory set */
#define WICED_MEMMOVE(a, b, c)      memmove((void*)(a), (const void*)(b), c)    /**< Memory move*/
#define WICED_MEMCMP(a, b, c)       memcmp((void*)(a), (const void*)(b), c)     /**< Memory compare*/

/** Boolean */
typedef unsigned int   wiced_bool_t;

/** Function prototypes to lock and unlock (typically using a mutex). The context
** pointer may be NULL, depending on implementation.
*/
typedef struct {
    void* p_lock_context;                           /**< lock context pointer */
    void (*pf_lock_func)(void * p_lock_context);    /**< Lock function pointer */
    void (*pf_unlock_func)(void * p_lock_context);  /**< Unlock function pointer */
}wiced_bt_lock_t;

 /**
  * Exception callback :
  *
  * Called by stack in case of unhandled exceptions and critical errors.
  *
  * @param[in] code    : Exception code
  * @param[in] msg     : Exception string
  * @param[in] p_tr    : Pointer to the data (based on the exception)
  *
  * @return void
  */
typedef void (*pf_wiced_exception)(uint16_t code, char* msg, void* ptr);
/**@} gentypes */
