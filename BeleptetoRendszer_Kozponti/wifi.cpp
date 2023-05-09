/**
 ***************************************************************************************************
 * @file wifi.cpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief Implementation of wifi.h.
 ***************************************************************************************************
 */

#include "wifi.h"

#include <Arduino.h>
#include "eeprom.h"
#include "realtime.h"

#include "DataListManager.hpp"

extern DataListManager dataListManager;

/**
 * @defgroup wifi_time_constants WiFi time constants
 * @brief Constants for timing the WiFi communication.
 * @{
 */
#define WIFI_CONNECT_TIMEOUT_MS 10000
#define CLIENT_TIMEOUT_MS 5000
/** @} */

/**
 * @defgroup wifi_constants WiFi constants
 * @brief Constants for the communication with the central module.
 * @{
 */
#define WIFI_CENTRAL_SSID "belepteto_rendszer"
#define WIFI_CENTRAL_PASS "0123456789abcdef"
#define WIFI_CENTRAL_IP "192.168.4.1"
#define WIFI_CENTRAL_PORT 80
/** @} */

/** @brief The SSID of the WiFi network. */
static const char *ssid = WIFI_CENTRAL_SSID;
/** @brief The password of the WiFi network. */
static const char *password = WIFI_CENTRAL_PASS;
/** @brief The IP address of the central server. */
const char *host = WIFI_CENTRAL_IP;
/** @brief The port of the central server. */
const uint16_t port = WIFI_CENTRAL_PORT;

uint8_t memoryImageReceived[EEPROM_SIZE];

bool isWifiInitialized = false;

WiFiServer server(80);

void sendMemory(WiFiClient &client, int size);
void sendTime(WiFiClient &client, int size);
void receiveMemory(WiFiClient &client, int size);
void processRemoteData(WiFiClient &client);

/**
 * @brief Initialize the WiFi module.
 * @return True if the initialization was successful, false otherwise.
 */
bool WIFI_Init(void)
{
    if (!WiFi.mode(WIFI_AP))
    {
        return false;
    }

    if (!WiFi.softAP(ssid, password, 1, 1, 4))
    {
        return false;
    }

    server.begin();

    isWifiInitialized = true;

    return true;
}

/**
 * @brief Handle the WiFi clients.
 */
void WIFI_HandleClients(void)
{
    if (!isWifiInitialized)
    {
        return;
    }

    WiFiClient client = server.available();
    if (!client)
    {
        return;
    }

    processRemoteData(client);
}

void sendMemory(WiFiClient &client, int size)
{
    if (size > EEPROM_GetSize())
    {
        client.print("0\n");
        return;
    }

    uint8_t data[8];

    while (size > 0)
    {
        EEPROM_Read(size, data, 8);
        client.write(data, 8);
        size -= 8;
    }
}

void sendTime(WiFiClient &client, int size)
{
    if (REALTIME_IsSet())
    {
        client.print("0\n");
    }
    else
    {
        client.print(REALTIME_Get());
        client.print('\n');
    }
}

void receiveMemory(WiFiClient &client, int size)
{
    if (size > EEPROM_GetSize())
    {
        return;
    }

    int i = 0;
    while (size > i)
    {
        int readSize = client.readBytes(memoryImageReceived, size > 8 ? 8 : size);
        i += readSize;
    }

    dataListManager.extractListFromEepromImage(memoryImageReceived, i);
}

void processRemoteData(WiFiClient &client)
{
    // read data from the remote module
    // process data
    // send response to the remote module

    if (!client.available())
    {
        delay(5);
    }
    if (!client.available())
    {
        return;
    }

    char type = client.read();
    client.read(); // skip the whitespace
    int size = client.parseInt();
    client.read(); // skip the whitespace

    if (type == 'N')
    {
        sendMemory(client, size);
    }
    else if (type == 'T')
    {
        sendTime(client, size);
    }
    else if (type == 'M')
    {
        receiveMemory(client, size);
    }
}

/**
 * @brief Wait for the client response.
 * @param client The client to use for communication.
 * @param timeout The timeout in milliseconds.
 * @return True if the response was received, false otherwise.
 */
bool WIFI_ClientWaitForResponse(WiFiClient &client, unsigned long timeout)
{
    unsigned long startMillis = millis();
    while (client.available() == 0)
    {
        if ((millis() - startMillis) > timeout)
        {
            return false;
        }
    }

    return true;
}
