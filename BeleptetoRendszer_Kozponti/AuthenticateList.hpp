#pragma once

#include <stdint.h>
#include <stdio.h>

#include "CircularBuffer.hpp"
#include "AuthenticateData.hpp"

#define DATA_LIST_SIZE 128

class AuthenticateList
{
private:
    CircularBuffer<AuthenticateData, DATA_LIST_SIZE> data_list;

public:
    void add(const AuthenticateData *data)
    {
        data_list.enqueue(data);
    }

    void add(const uint8_t *uid, const char *name, uint32_t interval_start, uint32_t interval_end)
    {
        AuthenticateData data(uid, name, interval_start, interval_end);
        data_list.enqueue(&data);
    }

    void add(const char *uid, const char *name, uint32_t interval_start, uint32_t interval_end)
    {
        uint8_t uid_bytes[10];
        sscanf(uid, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
               &uid_bytes[0], &uid_bytes[1], &uid_bytes[2], &uid_bytes[3],
               &uid_bytes[4], &uid_bytes[5], &uid_bytes[6], &uid_bytes[7],
               &uid_bytes[8], &uid_bytes[9]);

        AuthenticateData data(uid_bytes, name, interval_start, interval_end);
        data_list.enqueue(&data);
    }

    AuthenticateData *get(int index)
    {
        return data_list[index];
    }

    const AuthenticateData *get(int index) const
    {
        return data_list[index];
    }

    void remove(int index)
    {
        data_list.remove(index);
    }

    void remove(const uint8_t *uid)
    {
        for (int i = 0; i < data_list.size(); i++)
        {
            const uint8_t *data_uid = data_list[i]->getUid();
            bool match = true;
            for (int j = 0; j < data_list[i]->getUidSize(); j++)
            {
                if (data_uid[j] != uid[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                data_list.remove(i);
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

    int size() const
    {
        return data_list.size();
    }

    bool authenticate(const uint8_t *uid) const
    {
        return (findByUid(uid) != -1);
    }

    int findByUid(const uint8_t *uid) const
    {
        for (int i = 0; i < data_list.size(); i++)
        {
            const uint8_t *data_uid = data_list[i]->getUid();
            bool match = true;
            for (int j = 0; j < data_list[i]->getUidSize(); j++)
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

    const AuthenticateData *operator[](int index) const
    {
        if (index < 0 || index >= data_list.size())
        {
            return nullptr;
        }
        return data_list[index];
    }

    AuthenticateData *operator[](int index)
    {
        if (index < 0 || index >= data_list.size())
        {
            return nullptr;
        }
        return data_list[index];
    }

    void clear()
    {
        AuthenticateData data;
        while (data_list.dequeue(&data))
        {
            // Do nothing
        }
    }
}; // AuthenticateList
