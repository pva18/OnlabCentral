/**
 ***************************************************************************************************
 * @file AuthenticateList.hpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the definition of the AuthenticateList class.
 ***************************************************************************************************
 */

#pragma once

#include <stdint.h>
#include <stdio.h>

#include "CircularBuffer.hpp"
#include "AuthenticateData.hpp"

/**
 * @brief Size of the authentication list.
 */
#define AUTH_LIST_SIZE 145

class AuthenticateList
{
private:
    /**
     * @brief List of the authentication data.
     */
    CircularBuffer<AuthenticateData, AUTH_LIST_SIZE> data_list;

public:
    /**
     * @brief Add an authentication data to the list.
     * @param data Pointer to the authentication data
     */
    void add(const AuthenticateData *data)
    {
        data_list.enqueue(data);
    }

    /**
     * @brief Add an authentication data to the list.
     * @param uid UID of an RFID tag
     * @param name Name of the RFID tag owner
     * @param interval_start Start of the authentication interval
     * @param interval_end End of the authentication interval
     */
    void add(const uint8_t *uid, const char *name, uint32_t interval_start, uint32_t interval_end)
    {
        AuthenticateData data(uid, name, interval_start, interval_end);
        data_list.enqueue(&data);
    }

    /**
     * @brief Add an authentication data to the list.
     * @param uid UID of an RFID tag
     * @param name Name of the RFID tag owner
     * @param interval_start Start of the authentication interval
     * @param interval_end End of the authentication interval
     */
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

    /**
     * @brief Get the authentication data at the given index.
     * @param index Index of the authentication data
     * @return Pointer to the authentication data
     */
    AuthenticateData *get(int index)
    {
        return data_list[index];
    }

    /**
     * @brief Get the authentication data at the given index.
     * @param index Index of the authentication data
     * @return Pointer to the authentication data
     */
    const AuthenticateData *get(int index) const
    {
        return data_list[index];
    }

    /**
     * @brief Remove the authentication data at the given index.
     * @param index Index of the authentication data
     */
    void remove(int index)
    {
        data_list.remove(index);
    }

    /**
     * @brief Remove the authentication data with the given UID.
     * @param uid UID of the authentication data
     */
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

    /**
     * @brief Remove the authentication data with the given UID.
     * @param uid UID of the authentication data
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
     * @brief Get the size of the authentication list.
     * @return Size of the authentication list
     */
    int size() const
    {
        return data_list.size();
    }

    /**
     * @brief Check if the given UID is in the authentication list.
     * @param uid UID of an RFID tag
     * @return True if the UID is in the authentication list, false otherwise
     */
    bool authenticate(const uint8_t *uid) const
    {
        return (findByUid(uid) != -1);
    }

    /**
     * @brief Find the authentication data with the given UID.
     * @param uid UID of an RFID tag
     * @return Index of the authentication data, -1 if not found
     */
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

    /**
     * @brief Index operator.
     * @param index Index of the authentication data
     * @return Pointer to the authentication data
     */
    const AuthenticateData *operator[](int index) const
    {
        if (index < 0 || index >= data_list.size())
        {
            return nullptr;
        }
        return data_list[index];
    }

    /**
     * @brief Index operator.
     * @param index Index of the authentication data
     * @return Pointer to the authentication data
     */
    AuthenticateData *operator[](int index)
    {
        if (index < 0 || index >= data_list.size())
        {
            return nullptr;
        }
        return data_list[index];
    }

    /**
     * @brief Clear the authentication list.
     */
    void clear()
    {
        AuthenticateData data;
        while (data_list.dequeue(&data))
        {
            // Do nothing
        }
    }
}; // AuthenticateList
