/**
 ***************************************************************************************************
 * @file timers.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of timers.h.
 ***************************************************************************************************
 */

#include "timers.h"

#include "CircularBuffer.hpp"

/**
 * @brief The maximum number of timer events.
 */
#define TIMER_MAX_EVENT_NUMBER 16

/**
 * @brief The timer event buffer.
 */
CircularBuffer<timer_event_t, TIMER_MAX_EVENT_NUMBER> timer_events;

/**
 * @brief Get the maximum number of timer events.
 * @return The maximum number of timer events.
 */
int TIMERS_GetMaxEventNumber(void)
{
    return TIMER_MAX_EVENT_NUMBER;
}

/**
 * @brief Add a timer event.
 * @param event The timer event to add.
 * @return True if the timer event was added successfully, false otherwise.
 */
bool TIMERS_AddEvent(const timer_event_t *event)
{
    return timer_events.enqueue(*event);
}

/**
 * @brief Handle the timer events.
 * @param millis_current The current value of millis().
 */
void TIMERS_HandleEvents(unsigned long millis_current)
{
    timer_event_t event;
    while (timer_events.dequeue(&event))
    {
        unsigned long millis_real_period = millis_current - event.millis_start;
        if (millis_real_period >= event.millis_period)
        {
            event.handler(millis_real_period);
        }
        else
        {
            timer_events.enqueue(event);
        }
    }
}
