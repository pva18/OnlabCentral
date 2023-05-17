/**
 ***************************************************************************************************
 * @file BeleptetoRendszer_Kozponti.ino
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the main program of the central module.
 ***************************************************************************************************
 */

#include "BeleptetoRendszer_Kozponti.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LCD_I2C.h>

#include "DataListManager.hpp"
#include "realtime.h"
#include "eeprom.h"
#include "UiStateMachine.hpp"
#include "wifi.h"

#define DEBUG 0

#ifdef DEBUG
#define DEBUG_PRINT(str) Serial.print(str)
#else
#define DEBUG_PRINT(str)
#endif /* DEBUG */

/** @brief LCD display object. */
LCD_I2C lcd(0x27, 16, 2);
/** @brief Data list manager object. */
DataListManager dataListManager;
/** @brief UI state machine object. */
UiStateMachine uiStateMachine(&(dataListManager.authList), &lcd);

/**
 * @defgroup button_state_bits Button state bits
 * @brief Bits for button states.
 * @{
 */
#define BUTTON_E_PRESSED_BIT (1 << 0)
#define BUTTON_E_RELEASED_BIT (1 << 1)
#define BUTTON_1_PRESSED_BIT (1 << 2)
#define BUTTON_1_RELEASED_BIT (1 << 3)
#define BUTTON_2_PRESSED_BIT (1 << 4)
#define BUTTON_2_RELEASED_BIT (1 << 5)
#define BUTTON_3_PRESSED_BIT (1 << 6)
#define BUTTON_3_RELEASED_BIT (1 << 7)
/** @} */

/** @brief Stores the button states. */
uint32_t buttonSinglePressStates = 0;

/** @brief Begin marker of the message. */
const char messageBeginMarker = '<';
/** @brief End marker of the message. */
const char messageEndMarker = '>';
/** @brief Message buffer. */
char message[64 + 1];
/** @brief Index of the message buffer. */
int messageIndex = 0;
/** @brief Stores if the message has started. */
bool messageStarted = false;

void ioPinsInit(void);
void ioButtonSample(void);
uint32_t ioButtonGetSinglePressStates(uint32_t bitMask);

void processMessage(const char *msg);
void revceiveMessage(void);

/**
 * @brief Arduino setup function.
 */
void setup()
{
    Serial.begin(115200);
    delay(10);
    Serial.println();

    ioPinsInit();

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    EEPROM_Init();

#ifdef DEBUG
    for (uint16_t i = 0; i < EEPROM_GetSize(); i++)
    {
        uint8_t data;
        char buffer[4 + 1];
        EEPROM_Read(i, &data, 1);
        sprintf(buffer, "%02X", data);
        DEBUG_PRINT(buffer);
        if (i % 32 == 31)
        {
            DEBUG_PRINT("\r\n");
        }
        else
        {
            DEBUG_PRINT(" ");
        }
    }
#endif /* DEBUG */

    dataListManager.Initialize();

#ifdef DEBUG
    DEBUG_PRINT((dataListManager.authList).size());
    DEBUG_PRINT(" items in list:\r\n");
    for (int i = 0; i < dataListManager.authList.size(); i++)
    {
        char buffer[64 + 1];
        dataListManager.authList[i]->toString(buffer);
        DEBUG_PRINT(buffer);
        DEBUG_PRINT("\r\n");
    }
#endif /* DEBUG */

    if (!WIFI_Init())
    {
        DEBUG_PRINT("WiFi init failed!\r\n");
    }
    else
    {
        DEBUG_PRINT("WiFi init success!\r\n");
    }

    lcd.begin(false);
    lcd.backlight();
}

/**
 * @brief Arduino loop function.
 */
void loop()
{
    static uint8_t ledState = LED_OFF;
    static unsigned long lastMillisFast = 0;
    static unsigned long lastMillisSlow = 0;

    unsigned long current_millis = millis();

    if (current_millis - lastMillisFast > 50)
    {
        lastMillisFast = current_millis;

        // Sample buttons frequently

        ioButtonSample();

        digitalWrite(LED_1_PIN, ledState);
        digitalWrite(LED_2_PIN, ledState);
        ledState = !ledState;
    }

    if (current_millis - lastMillisSlow > 500)
    {
        lastMillisSlow = current_millis;

        // Handle user interface and EEPROM update infrequently to save CPU time

        if (ioButtonGetSinglePressStates(BUTTON_E_PRESSED_BIT))
        {
            uiStateMachine.Update(UiStateMachine::Button::ENTER, current_millis);
        }
        else if (ioButtonGetSinglePressStates(BUTTON_2_PRESSED_BIT))
        {
            uiStateMachine.Update(UiStateMachine::Button::BACK, current_millis);
        }
        else if (ioButtonGetSinglePressStates(BUTTON_1_PRESSED_BIT))
        {
            uiStateMachine.Update(UiStateMachine::Button::LEFT, current_millis);
        }
        else if (ioButtonGetSinglePressStates(BUTTON_3_PRESSED_BIT))
        {
            uiStateMachine.Update(UiStateMachine::Button::RIGHT, current_millis);
        }
        else
        {
            uiStateMachine.Update(UiStateMachine::Button::NONE, current_millis);
        }

        dataListManager.updateEepromFromList();
    }

    // Handle communication tasks the most frequently to avoid missing messages and timeouts
    WIFI_HandleClients();
    revceiveMessage();
}

/**
 * @brief Process the received message.
 * @param msg The received message.
 */
void processMessage(const char *msg)
{
    String message(msg);

    if (message[0] == 'A')
    {
        // Add
        String uid = message.substring(2, 22);
        String name = message.substring(23, 39);
        String intervalStart = message.substring(40, 50);
        String intervalEnd = message.substring(51, 61);

        (dataListManager.authList).remove(uid.c_str());
        (dataListManager.authList).add(uid.c_str(), name.c_str(), intervalStart.toInt(), intervalEnd.toInt());
    }
    else if (message[0] == 'R')
    {
        // Remove
        String uid = message.substring(2, 22);

        (dataListManager.authList).remove(uid.c_str());
    }
    else if (message[0] == 'T')
    {
        // Time
        String time = message.substring(2, 12);

        REALTIME_Set(time.toInt());
    }
    else if (message[0] == 'L')
    {
        // Get log list
        char buffer[32 + 1];

        Serial.print("<L ");
        Serial.print((dataListManager.logList).size());
        Serial.print("\n");
        for (int i = 0; i < (dataListManager.logList).size(); i++)
        {
            (dataListManager.logList).get(i)->toString(buffer);
            Serial.print(buffer);
            Serial.print("\n");
        }
        Serial.print(">");
    }
    else if (message[0] == 'Q')
    {
        // Get auth list
        char buffer[64 + 1];

        Serial.print("<Q ");
        Serial.print((dataListManager.authList).size());
        Serial.print("\n");
        for (int i = 0; i < (dataListManager.authList).size(); i++)
        {
            (dataListManager.authList).get(i)->toString(buffer);
            Serial.print(buffer);
            Serial.print("\n");
        }
        Serial.print(">");
    }
    else if (message[0] = 'C')
    {
        // Clear log list
        (dataListManager.logList).clear();
    }
}

/**
 * @brief Initialize the IO pins.
 */
void ioPinsInit(void)
{
    pinMode(BUTTON_1_PIN, INPUT);
    pinMode(BUTTON_2_PIN, INPUT);
    pinMode(BUTTON_3_PIN, INPUT);
    pinMode(BUTTON_E_PIN, INPUT);

    pinMode(LED_1_PIN, OUTPUT);
    pinMode(LED_2_PIN, OUTPUT);
    pinMode(LED_3_PIN, OUTPUT);

    digitalWrite(LED_1_PIN, LED_OFF);
    digitalWrite(LED_2_PIN, LED_OFF);
    digitalWrite(LED_3_PIN, LED_ON);
}

/**
 * @brief Sample the IO buttons.
 */
void ioButtonSample(void)
{
    static uint8_t button1Prev = !BUTTON_PRESSED;
    static uint8_t button2Prev = !BUTTON_PRESSED;
    static uint8_t button3Prev = !BUTTON_PRESSED;
    static uint8_t buttonEPrev = !BUTTON_PRESSED;

    uint8_t button1 = digitalRead(BUTTON_1_PIN);
    uint8_t button2 = digitalRead(BUTTON_2_PIN);
    uint8_t button3 = digitalRead(BUTTON_3_PIN);
    uint8_t buttonE = digitalRead(BUTTON_E_PIN);

    if (button1 != button1Prev)
    {
        if (button1 == BUTTON_PRESSED)
        {
            buttonSinglePressStates |= BUTTON_1_PRESSED_BIT;
        }
        else
        {
            buttonSinglePressStates |= BUTTON_1_RELEASED_BIT;
        }
    }
    if (button2 != button2Prev)
    {
        if (button2 == BUTTON_PRESSED)
        {
            buttonSinglePressStates |= BUTTON_2_PRESSED_BIT;
        }
        else
        {
            buttonSinglePressStates |= BUTTON_2_RELEASED_BIT;
        }
    }
    if (button3 != button3Prev)
    {
        if (button3 == BUTTON_PRESSED)
        {
            buttonSinglePressStates |= BUTTON_3_PRESSED_BIT;
        }
        else
        {
            buttonSinglePressStates |= BUTTON_3_RELEASED_BIT;
        }
    }
    if (buttonE != buttonEPrev)
    {
        if (buttonE == BUTTON_PRESSED)
        {
            buttonSinglePressStates |= BUTTON_E_PRESSED_BIT;
        }
        else
        {
            buttonSinglePressStates |= BUTTON_E_RELEASED_BIT;
        }
    }

    button1Prev = button1;
    button2Prev = button2;
    button3Prev = button3;
    buttonEPrev = buttonE;
}

/**
 * @brief Get the IO button states.
 * @param bitMask The bit mask of the buttons to get the states of.
 * @return The button states.
 * @note The returned states are cleared.
 */
uint32_t ioButtonGetSinglePressStates(uint32_t bitMask)
{
    uint32_t states = buttonSinglePressStates & bitMask;
    buttonSinglePressStates &= ~bitMask;
    return states;
}

/**
 * @brief Receive a message from the serial port.
 * @note This function is non-blocking.
 */
void revceiveMessage(void)
{
    while (Serial.available() > 0)
    {
        char c = Serial.read();
        if (!messageStarted)
        {
            if (c == messageBeginMarker)
            {
                messageStarted = true;
                messageIndex = 0;
            }
        }
        else
        {
            if (c == messageEndMarker)
            {
                message[messageIndex] = '\0';
                messageStarted = false;
                processMessage(message);
            }
            else
            {
                message[messageIndex] = c;
                messageIndex++;
                if (messageIndex >= 64)
                {
                    messageStarted = false;
                }
            }
        }
    }
}
