/**
 ***************************************************************************************************
 * @file BeleptetoRendszer_Kozponti.h
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the configuration of the central module.
 ***************************************************************************************************
 */

#ifndef BELEPTETO_RENDSZER_KOZPONTI_H
#define BELEPTETO_RENDSZER_KOZPONTI_H

/**
 * @defgroup led_pin_config LED pin configuration
 * @brief Configuration of the LED pins.
 * @{
 */
#define LED_1_PIN 12
#define LED_2_PIN 15
#define LED_3_PIN 0
#define LED_OFF HIGH
#define LED_ON LOW
/** @} */

/**
 * @defgroup i2c_config I2C configuration
 * @brief Configuration of the I2C bus.
 * @{
 */
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 14
/** @} */

/**
 * @defgroup button_pin_config Button pin configuration
 * @brief Configuration of the button pins.
 * @{
 */
#define BUTTON_1_PIN 16
#define BUTTON_2_PIN 4
#define BUTTON_3_PIN 5
#define BUTTON_E_PIN 13
#define BUTTON_PRESSED LOW
/** @} */

#endif /* BELEPTETO_RENDSZER_KOZPONTI_H */
