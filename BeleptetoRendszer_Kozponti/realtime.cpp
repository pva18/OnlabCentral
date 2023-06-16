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

static uint64_t realtimeMillis = 0;

static bool isRealtimeSet = false;

static unsigned long lastMillis = 0;

/**
 * @brief Set the time in seconds.
 * @param time Time in seconds (UNIX time).
 */
void REALTIME_Set(uint32_t time)
{
    lastMillis = millis();
    realtimeMillis = (uint64_t)time * 1000;

    isRealtimeSet = true;
}

/**
 * @brief Get the time in seconds.
 * @return Time in seconds (UNIX time).
 */
uint32_t REALTIME_Get(void)
{
    if (!isRealtimeSet)
    {
        return 0;
    }

    unsigned long currentMillis = millis();
    unsigned long elapsedMillis = currentMillis - lastMillis;
    lastMillis = currentMillis;

    realtimeMillis += elapsedMillis;

    return (uint32_t)(realtimeMillis / 1000);
}

/**
 * @brief Check if the time is set.
 * @return True if the time is set, false otherwise.
 */
bool REALTIME_IsSet(void)
{
    return isRealtimeSet;
}
