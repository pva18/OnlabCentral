/**
 ***************************************************************************************************
 * @file realtime.h
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the realtime functions.
 ***************************************************************************************************
 */

#ifndef REALTIME_H
#define REALTIME_H

#include <stdint.h>

void REALTIME_Set(uint32_t time);

uint32_t REALTIME_Get(void);

bool REALTIME_IsSet(void);

#endif /* REALTIME_H */
