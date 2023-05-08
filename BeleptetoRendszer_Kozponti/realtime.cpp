/**
 ***************************************************************************************************
 * @file realtime.cpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief Implementation of realtime.h.
 ***************************************************************************************************
 */

#include "realtime.h"

#include <Arduino.h>

static uint32_t realtime = 0;
static unsigned long realtimeMillis = 0;

static bool isRealtimeSet = false;

static unsigned long lastMillis = 0;

/**
 * @brief Set the time in seconds.
 * @param time Time in seconds (UNIX time).
 */
void REALTIME_Set(uint32_t time)
{
    lastMillis = millis();
    realtime = time;
    realtimeMillis = time * 1000;
    isRealtimeSet = true;
}

/**
 * @brief Get the time in seconds.
 * @return Time in seconds (UNIX time).
 */
uint32_t REALTIME_Get(void)
{
    if (isRealtimeSet)
    {
        unsigned long currentMillis = millis();
        unsigned long elapsedMillis = currentMillis - lastMillis;
        lastMillis = currentMillis;

        realtimeMillis += elapsedMillis;
        realtime = (uint32_t)(realtimeMillis / 1000);
    }

    return realtime;
}

/**
 * @brief Check if the time is set.
 * @return True if the time is set, false otherwise.
 */
bool REALTIME_IsSet(void)
{
    return isRealtimeSet;
}
