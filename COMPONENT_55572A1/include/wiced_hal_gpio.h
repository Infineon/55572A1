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
* List of parameters and defined functions needed to access the
* General Purpose Input/Ouput (GPIO) driver.
*
*/

#ifndef __WICED_GPIO_H__
#define __WICED_GPIO_H__

#include "wiced.h"

/**  \addtogroup GPIODriver GPIO
* \ingroup HardwareDrivers
* @{
* Defines a driver to facilitate interfacing with the GPIO pins.
*
* Use this driver to control the behavior of any desired pin, such as
* driving a 1 or a 0, or as part of other drivers such as controlling
* the chip-select (CS) line for the SPI driver.
*
*/

/******************************************************************************
*** Parameters.
***
*** The following parameters are used to configure the driver or define
*** return status. They are not modifiable.
******************************************************************************/

/// Pin output config
typedef enum
{
    WICED_GPIO_PIN_OUTPUT_LOW,
    WICED_GPIO_PIN_OUTPUT_HIGH,
} WICED_GPIO_PIN_OUTPUT_CONFIG;

/// The following enum defines the constant values for the GPIO driver and
/// associated GPIOs, each of which has a set of configuration signals.
enum
{
    /// Trigger Type
    /// GPIO configuration bit 0, Interrupt type defines
    WICED_GPIO_EDGE_TRIGGER_MASK       = 0x0001,
    WICED_GPIO_EDGE_TRIGGER            = 0x0001,
    WICED_GPIO_LEVEL_TRIGGER           = 0x0000,

    /// Negative Edge Triggering
    /// GPIO configuration bit 1, Interrupt polarity defines
    WICED_GPIO_TRIGGER_POLARITY_MASK   = 0x0002,
    WICED_GPIO_TRIGGER_NEG             = 0x0002,

    /// Dual Edge Triggering
    /// GPIO configuration bit 2, single/dual edge defines
    WICED_GPIO_DUAL_EDGE_TRIGGER_MASK  = 0x0004,
    WICED_GPIO_EDGE_TRIGGER_BOTH       = 0x0004,
    WICED_GPIO_EDGE_TRIGGER_SINGLE     = 0x0000,


    /// Interrupt Enable
    /// GPIO configuration bit 3, interrupt enable/disable defines
    WICED_GPIO_INTERRUPT_ENABLE_MASK   = 0x0008,
    WICED_GPIO_INTERRUPT_ENABLE        = 0x0008,
    WICED_GPIO_INTERRUPT_DISABLE       = 0x0000,


    /// Interrupt Config
    /// GPIO configuration bit 0:3, Summary of Interrupt enabling type
    WICED_GPIO_EN_INT_MASK             = WICED_GPIO_EDGE_TRIGGER_MASK | WICED_GPIO_TRIGGER_POLARITY_MASK | WICED_GPIO_DUAL_EDGE_TRIGGER_MASK | WICED_GPIO_INTERRUPT_ENABLE_MASK,
    WICED_GPIO_EN_INT_LEVEL_HIGH       = WICED_GPIO_INTERRUPT_ENABLE | WICED_GPIO_LEVEL_TRIGGER,
    WICED_GPIO_EN_INT_LEVEL_LOW        = WICED_GPIO_INTERRUPT_ENABLE | WICED_GPIO_LEVEL_TRIGGER | WICED_GPIO_TRIGGER_NEG,
    WICED_GPIO_EN_INT_RISING_EDGE      = WICED_GPIO_INTERRUPT_ENABLE | WICED_GPIO_EDGE_TRIGGER,
    WICED_GPIO_EN_INT_FALLING_EDGE     = WICED_GPIO_INTERRUPT_ENABLE | WICED_GPIO_EDGE_TRIGGER | WICED_GPIO_TRIGGER_NEG,
    WICED_GPIO_EN_INT_BOTH_EDGE        = WICED_GPIO_INTERRUPT_ENABLE | WICED_GPIO_EDGE_TRIGGER | WICED_GPIO_EDGE_TRIGGER_BOTH,


    /// GPIO Output Buffer Control and Output Value Multiplexing Control
    /// GPIO configuration bit 4:5, and 14 output enable control and
    /// muxing control
    WICED_GPIO_INPUT_ENABLE            = 0x0000,
    WICED_GPIO_OUTPUT_DISABLE          = 0x0000,
    WICED_GPIO_OUTPUT_ENABLE           = 0x4000,
    WICED_GPIO_KS_OUTPUT_ENABLE        = 0x0001, //Keyscan Output enable
    WICED_GPIO_OUTPUT_FN_SEL_MASK      = 0x0000,
    WICED_GPIO_OUTPUT_FN_SEL_SHIFT     = 0,


    /// Global Input Disable
    /// GPIO configuration bit 6, "Global_input_disable" Disable bit
    /// This bit when set to "1" , P0 input_disable signal will control
    /// ALL GPIOs. Default value (after power up or a reset event) is "0".
    WICED_GPIO_GLOBAL_INPUT_ENABLE     = 0x0000,
    WICED_GPIO_GLOBAL_INPUT_DISABLE    = 0x0040,


    /// Pull-up/Pull-down
    /// GPIO configuration bit 9 and bit 10, pull-up and pull-down enable
    /// Default value is [0,0]--means no pull resistor.
    WICED_GPIO_PULL_UP_DOWN_NONE       = 0x0000,   //[0,0]
    WICED_GPIO_PULL_UP                 = 0x0400,   //[1,0]
    WICED_GPIO_PULL_DOWN               = 0x0200,   //[0,1]
    WICED_GPIO_INPUT_DISABLE           = 0x0600,   //[1,1] // input disables the GPIO

    /// Drive Strength
    /// GPIO configuration bit 11
    WICED_GPIO_DRIVE_SEL_MASK         = 0x0800,
    WICED_GPIO_DRIVE_SEL_LOWEST       = 0x0000,  // 2mA @ 1.8V
    WICED_GPIO_DRIVE_SEL_MIDDLE_0     = 0x0000,  // 4mA @ 3.3v
    WICED_GPIO_DRIVE_SEL_MIDDLE_1     = 0x0800,  // 4mA @ 1.8v
    WICED_GPIO_DRIVE_SEL_HIGHEST      = 0x0800,  // 8mA @ 3.3v


    /// Input Hysteresis
    /// GPIO configuration bit 13, hysteresis control
    WICED_GPIO_HYSTERESIS_MASK         = 0x2000,
    WICED_GPIO_HYSTERESIS_ON           = 0x2000,
    WICED_GPIO_HYSTERESIS_OFF          = 0x0000,
};

typedef enum wiced_bt_gpio_numbers
{
    BT_DEV_WAKE = 0,
    BT_HOST_WAKE = 1,
    BT_GPIO_2 = 2,
    BT_GPIO_3 = 3,
    BT_GPIO_4 = 4,
    BT_GPIO_5 = 5,
    BT_GPIO_6 = 6,
    BT_GPIO_7 = 7,
    BT_GPIO_8 = 8,
    BT_GPIO_9 = 9,
    BT_GPIO_10 = 10,
    BT_GPIO_11 = 11,
    BT_GPIO_12 = 12,
    BT_GPIO_13 = 13,
    BT_GPIO_14 = 14,
    BT_GPIO_15 = 15,
    BT_GPIO_16 = 16,
    BT_GPIO_17 = 17,
    BT_GPIO_18 = 18,
    BT_GPIO_19 = 19,
    BT_GPIO_20 = 20,
    BT_DMIC_CK = 21,
    BT_DMIC_DQ = 22,
    MAX_NUM_OF_GPIO,
} wiced_bt_gpio_numbers_t;

//! possible functions to be brought out through LHL GPIO's
typedef enum
{
    WICED_GPIO = 0,
    WICED_I2C_1_SCL,
    WICED_I2C_1_SDA,
    WICED_I2C_2_SCL,
    WICED_I2C_2_SDA,
    WICED_SPI_1_CLK,
    WICED_SPI_1_CS,
    WICED_SPI_1_MOSI,
    WICED_SPI_1_MISO,
    WICED_SPI_1_SLAVE_READY,
    WICED_SPI_1_IO2,
    WICED_SPI_1_IO3,
    WICED_SPI_1_INT,
    WICED_SPI_1_DCX,
    WICED_SPI_2_CLK,
    WICED_SPI_2_CS,
    WICED_SPI_2_MOSI,
    WICED_SPI_2_MISO,
    WICED_SPI_2_IO2,
    WICED_SPI_2_IO3,
    WICED_SPI_2_INT,
    WICED_SPI_2_DCX,
    WICED_SPI_3_CLK,
    WICED_SPI_3_CS,
    WICED_SPI_3_MOSI,
    WICED_SPI_3_MISO,
    WICED_SPI_3_INT,
    WICED_SWDCK,
    WICED_SWDIO,
    WICED_UART_1_TXD,
    WICED_UART_1_RXD,
    WICED_UART_1_CTS,
    WICED_UART_1_RTS,
    WICED_UART_2_TXD,
    WICED_UART_2_RXD,
    WICED_UART_2_CTS,
    WICED_UART_2_RTS,
    WICED_AOA_0,
    WICED_AOA_1,
    WICED_AOA_2,
    WICED_AOD_0,
    WICED_I2S_MASTER_CLK,
    WICED_I2S_MASTER_WS,
    WICED_I2S_MASTER_DO,
    WICED_I2S_MASTER_DI,
    WICED_I2S_SLAVE_CLK,
    WICED_I2S_SLAVE_WS,
    WICED_I2S_SLAVE_DO,
    WICED_I2S_SLAVE_DI,
    WICED_PCM_CLK,
    WICED_PCM_SYNC,
    WICED_PCM_OUT,
    WICED_PCM_IN,
    WICED_GCI_SECI_IN,
    WICED_GCI_SECI_OUT,
    WICED_ACLK_0,
    WICED_ACLK_1,
    WICED_KSO0,
    WICED_KSO1,
    WICED_KSO2,
    WICED_KSO3,
    WICED_KSO4,
    WICED_KSO5,
    WICED_KSO6,
    WICED_KSO7,
    WICED_KSO8,
    WICED_KSO9,
    WICED_KSO10,
    WICED_KSO11,
    WICED_KSO12,
    WICED_KSO13,
    WICED_KSO14,
    WICED_KSO15,
    WICED_KSO16,
    WICED_KSO17,
    WICED_KSO18,
    WICED_KSO19,
    WICED_TX_PD,
    WICED_TX_PD_TILDA,
    WICED_PA_RAMP,
    WICED_BT_GPIO_00,
    WICED_BT_GPIO_01,
    WICED_BT_GPIO_02,
    WICED_BT_GPIO_03,
    WICED_BT_GPIO_04,
    WICED_BT_GPIO_05,
    WICED_BT_GPIO_06,
    WICED_BT_GPIO_07,
    WICED_TX_FSM = 128,
    WICED_RX_FSM,
    WICED_RX_PU,
    WICED_UNAVAILABLE = 0xFF
} wiced_bt_gpio_function_t;

//! Possible return values from wiced_hal_gpio_select_function(...), Callers only need to check for the
//! GPIO_FAILURE case since any other status means success
typedef enum WICED_GPIO_STATUS_e
{
    WICED_GPIO_FAILURE, //!< The requested pin and function mapping is not supported by hardware
    WICED_GPIO_SUCCESS, //!< The requested pin and function mapping is complete, The pin was previously not used and the function was previously not mapped
    WICED_GPIO_REMAPPED,//!< The requested pin and function mapping is complete, The pin was previously used by another function, that function was disabled and the new function applied
    WICED_GPIO_MOVED    //!< The requested pin and function mapping is complete, The requested function was already mapped to a different pin, that pin was disabled and the function moved to the new pin
} wiced_bt_gpio_select_status_t;

/*! GPIO Active Level HIGH */
#define WICED_GPIO_ACTIVE_HIGH      1

/*! GPIO Active Level LOW */
#define WICED_GPIO_ACTIVE_LOW       0

/******************************************************************************
*** Function prototypes and defines.
******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
/// Initializes the GPIO driver and its private values.
/// Also programs all GPIOs to be ready for use. This must be invoked before
/// accessing any GPIO driver services, typically at boot.
/// This is independent of other drivers and must be one of the first to
/// be initialized.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_init(void);

///////////////////////////////////////////////////////////////////////////////
/// Configures a GPIO pin.
///
/// Note that the GPIO output value is programmed before
/// the GPIO is configured. This ensures that the GPIO will activate with the
/// correct external value. Also note that the output value is always
/// written to the output register regardless of whether the GPIO is configured
/// as input or output.
///
/// Enabling interrupts here isn't sufficient; you also want to
/// register with registerForMask().
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin    - pin id (0-39).
/// \param config - Gpio configuration. See Parameters section above for
/// possible values.
///
/// For example, to enable interrupts for all edges, with a pull-down,
/// you could use:
/// \verbatim
/// GPIO_EDGE_TRIGGER | GPIO_EDGE_TRIGGER_BOTH |
/// GPIO_INTERRUPT_ENABLE_MASK | GPIO_PULL_DOWN_MASK
/// \endverbatim
///
/// \param outputVal - output value.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_configure_pin(uint32_t pin, uint32_t config,
                                                uint32_t outputVal);

///////////////////////////////////////////////////////////////////////////////
/// Retrieve the current configuration of the specified pin.
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin  - pin id (0-39).
///
/// \return Configuration of specified pin.
///////////////////////////////////////////////////////////////////////////////
uint16_t wiced_hal_gpio_get_pin_config(uint32_t pin);

///////////////////////////////////////////////////////////////////////////////
/// Sets the output value of a pin. Note that the pin must already be
/// configured as output or this will have no visible effect.
///
/// All input parameter values must be in range or the function will
/// have no effect.
///
/// \param pin  - pin id (0-39).
/// \param val  - the output value
///    -   0        - pin will be set to 0
///    -   non-zero - pin will be set to 1
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_set_pin_output(uint32_t pin, uint32_t val);

///////////////////////////////////////////////////////////////////////////////
/// Get the programmed output value of a pin. Note that this does not return
/// the current pin value. It returns the value that the pin would attempt to
/// drive if it is configured as output.
///
/// \param pin  - pin id (0-39).
///
/// \return Returns 1 if output port of the pin is configured 1, likewise
/// for 0. Returns 0xFF if the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_gpio_get_pin_output(uint32_t pin);

///////////////////////////////////////////////////////////////////////////////
/// Read the current value at a pin. Note that for this to be valid, the pin
/// must be configured with input enabled.
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if the pin is high, 0 if the pin is low, and 0xFF if
/// the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_gpio_get_pin_input_status(uint32_t pin);

///////////////////////////////////////////////////////////////////////////////
/// Get the interrupt status of a pin
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if an interrupt (programmed edge) was detected at the
/// pin, 0 if not. Returns 0xFF if the input parameters are out of range.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_gpio_get_pin_interrupt_status(uint32_t pin);

///////////////////////////////////////////////////////////////////////////////
/// Register a function for notification of changes to a pin (via interrupt).
///
/// Note that this is independent of configuring the pin for
/// interrupts; a call to configurePin() is also required.
///
/// Also note that once registered, you CANNOT UNREGISTER; registration is
/// meant to be a startup activity. To stop receiving notifications,
/// re-configure the pin and disable the interrupt using
/// configurePin().
///
/// Example:
///
/// \verbatim
/// void gpioIntTestCb(void *data);
///
/// uint16_t gpioIntMasks[GPIO_NUM_PORTS] = {0};
/// wiced_hal_gpio_configurePin(27,
///     (GPIO_INPUT_ENABLE|GPIO_PULL_DOWN|GPIO_EN_INT_RISING_EDGE),
///      GPIO_PIN_OUTPUT_LOW);
/// gpioIntMasks[pin2port(27)] |= (1<<pin2pin(27));
/// wiced_hal_gpio_registerPinForInterrupt(gpioIntMasks, gpioIntTestCb, NULL);
/// wiced_hal_gpio_clearPinInterruptStatus(27);
/// \endverbatim
///
/// \param masks - Points to an array of NUM_PORTS.
///
/// \param userfn - Points to the function to call when the interrupt
/// comes .Below is the description of the arguments received by the call back.
///
///     void* user_data  - User data provided when interrupt is being registered
///                        using wiced_hal_gpio_register_pin_for_interrupt(...)
///
///     uint8_t value    - Number of the pin causing the interrupt
///
/// (!) Note that the function does not need to clear the interrupt
/// status; this will be done automatically.
///
/// (!) Note that the function will be called ONCE per interrupt, not once per
/// pin (this makes a difference if multiple pins toggle at the same time).
///
/// \param userdata - Will be passed back to userfn as-is. Typically NULL.
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_register_pin_for_interrupt(uint16_t pin,
                                               void (*userfn)(void*, uint8_t), void* userdata);

///////////////////////////////////////////////////////////////////////////////
/// Clear the interrupt status of a pin manually.
///
/// \param pin  - pin id (0-39).
///
/// \return  Returns 1 if successful, 0xFF if the input parameters are
/// out of range.
///////////////////////////////////////////////////////////////////////////////
uint32_t wiced_hal_gpio_clear_pin_interrupt_status(uint32_t pin);

///////////////////////////////////////////////////////////////////////////////
/// Configures all gpios to be INPUT DISABLED.
///
/// \param none
///
/// \return none
///////////////////////////////////////////////////////////////////////////////
void wiced_hal_gpio_disable_all_inputs(void);

/////////////////////////////////////////////////////////////////////////////////////
///  save the LHL GPIO configuration in AON memeory to reenforce in slimboot
///  This is a must if we want to wake up in SDS by external LHL GPIO interrupts
///  \param pin - pin id (0-39)
///  \param config - Gpio configuration.
///  \result: TRUE - successful save/update; FALSE - run out of entries, not able to save or invalid pin
/////////////////////////////////////////////////////////////////////////////////////
wiced_bool_t wiced_hal_gpio_slimboot_reenforce_cfg(uint8_t pin, uint16_t config);

/////////////////////////////////////////////////////////////////////////////////////
///  Configure a GPIO pin to have the chosen functionality.
///
///  \param pin (0-39) (Only LHL GPIO supported for now)
///  \param function Chosen functionality
///  \result: TRUE - successful save/update; FALSE - run out of entries, not able to save or invalid pin
/////////////////////////////////////////////////////////////////////////////////////
wiced_bt_gpio_select_status_t wiced_hal_gpio_select_function(wiced_bt_gpio_numbers_t pin, wiced_bt_gpio_function_t function);

/* @} */

#endif // __WICED_GPIO_H__
