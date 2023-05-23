/**
 ***************************************************************************************************
 * @file LogList.hpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the definition of the LogList class.
 ***************************************************************************************************
 */

#pragma once

#include <cstdint>

#include "LogData.hpp"
#include "CircularBuffer.hpp"

/**
 * @brief Maximum size of the log list
 */
#define LOG_LIST_MAX_SIZE 273

/**
 * @brief This class represents a list of logs.
 */
class LogList
{
private:
    CircularBuffer<LogData, LOG_LIST_MAX_SIZE> logList;

public:
    /**
     * @brief Add a new log to the list.
     * @param uid UID of an RFID tag
     * @param timestamp Timestamp of the log
     * @param auth Authentication state
     */
    void add(const uint8_t *uid, uint32_t timestamp, uint8_t auth)
    {
        LogData logData(uid, timestamp, auth);
        logList.enqueue(&logData);
    }

    /**
     * @brief Add a new log to the list.
     * @param logData LogData object
     */
    void add(const LogData *logData)
    {
        logList.enqueue(logData);
    }

    /**
     * @brief Add a new log to the list.
     * @param uid UID of an RFID tag
     * @param timestamp Timestamp of the log
     * @param auth Authentication state
     */
    void add(const char *uid, uint32_t timestamp, uint8_t auth)
    {
        uint8_t uid_bytes[10];
        sscanf(uid, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
               &uid_bytes[0], &uid_bytes[1], &uid_bytes[2], &uid_bytes[3],
               &uid_bytes[4], &uid_bytes[5], &uid_bytes[6], &uid_bytes[7],
               &uid_bytes[8], &uid_bytes[9]);

        add(uid_bytes, timestamp, auth);
    }

    /**
     * @brief Get a log from the list.
     * @param index Index of the log
     * @return LogData object
     */
    LogData *get(int index)
    {
        return logList[index];
    }

    /**
     * @brief Get a log from the list.
     * @param index Index of the log
     * @return LogData object
     */
    const LogData *get(int index) const
    {
        return logList[index];
    }

    /**
     * @brief Get the size of the list.
     * @return Size of the list
     */
    int size(void) const
    {
        return logList.size();
    }

    /**
     * @brief Find a log by UID.
     * @param uid UID of an RFID tag
     * @return Index of the log
     */
    int findByUid(const uint8_t *uid) const
    {
        for (int i = 0; i < logList.size(); i++)
        {
            const uint8_t *data_uid = logList[i]->getUid();
            bool match = true;
            for (int j = 0; j < logList[i]->getUidSize(); j++)
            {
                if (data_uid[j] != uid[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                return i;
            }
        }

        return -1;
    }

    /**
     * @brief Remove a log from the list.
     * @param index Index of the log
     */
    void remove(int index)
    {
        logList.remove(index);
    }

    /**
     * @brief Remove a log from the list.
     * @param uid UID of an RFID tag
     */
    void remove(const uint8_t *uid)
    {
        for (int i = 0; i < logList.size(); i++)
        {
            const uint8_t *data_uid = logList[i]->getUid();
            bool match = true;
            for (int j = 0; j < logList[i]->getUidSize(); j++)
            {
                if (data_uid[j] != uid[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                logList.remove(i);
                break;
            }
        }
    }

    /**
     * @brief Remove a log from the list.
     * @param uid UID of an RFID tag
     */
    void remove(const char *uid)
    {
        uint8_t uid_bytes[10];
        sscanf(uid, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
               &uid_bytes[0], &uid_bytes[1], &uid_bytes[2], &uid_bytes[3],
               &uid_bytes[4], &uid_bytes[5], &uid_bytes[6], &uid_bytes[7],
               &uid_bytes[8], &uid_bytes[9]);

        remove(uid_bytes);
    }

    /**
     * @brief Clear the list.
     */
    void clear(void)
    {
        LogData logData;
        while (logList.dequeue(&logData))
        {
            // Do nothing
        }
    }
}; // LogList
