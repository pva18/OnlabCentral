#pragma once

#include <cstdint>

#include "LogData.hpp"
#include "CircularBuffer.hpp"

#define LOG_LIST_MAX_SIZE (8192 / 2 / 14)

class LogList
{
private:
    CircularBuffer<LogData, LOG_LIST_MAX_SIZE> logList;

public:
    void add(const uint8_t *uid, uint32_t timestamp)
    {
        LogData logData(uid, timestamp);
        logList.enqueue(&logData);
    }

    void add(const LogData *logData)
    {
        logList.enqueue(logData);
    }

    void add(const char *uid, uint32_t timestamp)
    {
        uint8_t uid_bytes[10];
        sscanf(uid, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
               &uid_bytes[0], &uid_bytes[1], &uid_bytes[2], &uid_bytes[3],
               &uid_bytes[4], &uid_bytes[5], &uid_bytes[6], &uid_bytes[7],
               &uid_bytes[8], &uid_bytes[9]);

        add(uid_bytes, timestamp);
    }

    LogData *get(int index)
    {
        return logList[index];
    }

    const LogData *get(int index) const
    {
        return logList[index];
    }

    int size(void) const
    {
        return logList.size();
    }

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

    void remove(int index)
    {
        logList.remove(index);
    }

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

    void remove(const char *uid)
    {
        uint8_t uid_bytes[10];
        sscanf(uid, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
               &uid_bytes[0], &uid_bytes[1], &uid_bytes[2], &uid_bytes[3],
               &uid_bytes[4], &uid_bytes[5], &uid_bytes[6], &uid_bytes[7],
               &uid_bytes[8], &uid_bytes[9]);

        remove(uid_bytes);
    }

    void clear(void)
    {
        LogData logData;
        while(logList.dequeue(&logData))
        {
            // Do nothing
        }
    }
}; // LogList
