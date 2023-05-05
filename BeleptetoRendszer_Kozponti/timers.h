/**
 ***************************************************************************************************
 * @file timers.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the timer functionality.
 ***************************************************************************************************
 */

#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

/**
 * @brief The type of the timer event handler function.
 * @param millis_real_period The real period of the timer event in milliseconds.
 */
typedef void timer_event_handler_t(uint32_t millis_real_period);

/**
 * @brief The timer event structure.
 */
typedef struct _timer_event
{
    unsigned long millis_start; /**< Value of millis() when the timer event was added. */
    unsigned long millis_period; /**< The period of the timer event in milliseconds. */
    timer_event_handler_t *handler; /**< The handler function of the timer event. */
} timer_event_t;

int TIMERS_GetMaxEventNumber(void);

bool TIMERS_AddEvent(const timer_event_t *event);

void TIMERS_HandleEvents(unsigned long millis_current);

#endif /* TIMERS_H */
