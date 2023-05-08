/**
 ***************************************************************************************************
 * @file wifi.h
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief Header file for the WiFi functionality.
 ***************************************************************************************************
 */

#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

bool WIFI_Init(void);

void WIFI_HandleClients(void);

#endif /* WIFI_H */
