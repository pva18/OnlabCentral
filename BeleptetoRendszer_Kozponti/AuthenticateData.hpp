/**
 ***************************************************************************************************
 * @file AuthenticateData.hpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the definition of the AuthenticateData class.
 ***************************************************************************************************
 */

#pragma once

#include <cstdint>
#include <cstring>

/**
 * @brief This class represents the data needed for authentication.
 */
class AuthenticateData
{
private:
    const int uidSize = 10;
    const int nameSize = 16;

    uint8_t uid[10];
    char name[17];
    uint32_t interval_start;
    uint32_t interval_end;

public:
    /**
     * @brief Default constructor
     */
    AuthenticateData(void)
        : interval_start(0), interval_end(0)
    {
        for (int i = 0; i < uidSize; i++)
        {
            this->uid[i] = 0;
        }
        this->name[0] = '\0';
    }

    /**
     * @brief Constructor with data initialization
     * @param uid UID of an RFID tag
     * @param name Name of the RFID tag owner
     * @param interval_start Start of the authentication interval
     * @param interval_end End of the authentication interval
     */
    AuthenticateData(const uint8_t *uid, const char *name, uint32_t interval_start, uint32_t interval_end)
        : interval_start(interval_start), interval_end(interval_end)
    {
        for (int i = 0; i < uidSize; i++)
        {
            this->uid[i] = uid[i];
        }
        strncpy(this->name, name, nameSize);
        this->name[nameSize] = '\0';
    }

    /**
     * @brief Copy constructor
     * @param other Other AuthenticateData object
     */
    AuthenticateData(const AuthenticateData &other)
        : interval_start(other.interval_start), interval_end(other.interval_end)
    {
        for (int i = 0; i < uidSize; i++)
        {
            this->uid[i] = other.uid[i];
        }
        strncpy(this->name, other.name, nameSize);
        this->name[nameSize] = '\0';
    }

    /**
     * @brief Get the size of the UID
     * @return Size of the UID
     */
    int getUidSize(void) const
    {
        return uidSize;
    }

    /**
     * @brief Get the UID
     * @return UID
     */
    const uint8_t *getUid(void) const
    {
        return uid;
    }

    /**
     * @brief Get the name
     * @return Name
     */
    const char *getName(void) const
    {
        return name;
    }

    /**
     * @brief Get the start of the authentication interval
     * @return Start of the authentication interval
     */
    uint32_t getIntervalStart(void) const
    {
        return interval_start;
    }

    /**
     * @brief Get the end of the authentication interval
     * @return End of the authentication interval
     */
    uint32_t getIntervalEnd(void) const
    {
        return interval_end;
    }

    /**
     * @brief Set the interval start
     * @param interval_start Start of the authentication interval
     */
    void setIntervalStart(uint32_t interval_start)
    {
        this->interval_start = interval_start;
    }

    /**
     * @brief Set the interval end
     * @param interval_end End of the authentication interval
     */
    void setIntervalEnd(uint32_t interval_end)
    {
        this->interval_end = interval_end;
    }

    /**
     * @brief Assignment operator
     * @param other Other AuthenticateData object
     * @return Reference to this object
     */
    AuthenticateData &operator=(const AuthenticateData &other)
    {
        for (int i = 0; i < uidSize; i++)
        {
            this->uid[i] = other.uid[i];
        }
        strncpy(this->name, other.name, nameSize);
        this->name[nameSize] = '\0';
        this->interval_start = other.interval_start;
        this->interval_end = other.interval_end;
        return *this;
    }

    /**
     * @brief Equality operator
     * @param other Other AuthenticateData object
     * @return True if the objects are equal, false otherwise
     */
    bool operator==(const AuthenticateData &other) const
    {
        for (int i = 0; i < uidSize; i++)
        {
            if (this->uid[i] != other.uid[i])
            {
                return false;
            }
        }
        bool name_equal = (strcmp(this->name, other.name) == 0);
        bool interval_start_equal = (this->interval_start == other.interval_start);
        bool interval_end_equal = (this->interval_end == other.interval_end);

        return name_equal && interval_start_equal && interval_end_equal;
    }

    /**
     * @brief Convert the object to a string
     * @param str String to be filled
     * @note Format: "UID{20} NAME{16} INTERVAL_START{10} INTERVAL_END{10}"
     */
    void toString(char *str) const
    {
        sprintf(str, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X %16s %010u %010u",
                uid[0], uid[1], uid[2], uid[3], uid[4],
                uid[5], uid[6], uid[7], uid[8], uid[9],
                name, interval_start, interval_end);
    }
}; // AuthenticateData
