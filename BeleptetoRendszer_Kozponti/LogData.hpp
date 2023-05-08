#pragma once

#include <cstdint>

class LogData
{
private:
    const int uidSize = 10;

    uint8_t uid[10];
    uint32_t timestamp;

public:
    /**
     * @brief Default constructor
     */
    LogData(void)
    {
        for (int i = 0; i < uidSize; i++)
        {
            uid[i] = 0;
        }
        timestamp = 0;
    }

    /**
     * @brief Constructor with data initialization
     * @param uid UID of an RFID tag
     * @param timestamp Timestamp of the log
     */
    LogData(const uint8_t *uid, uint32_t timestamp)
    {
        for (int i = 0; i < uidSize; i++)
        {
            this->uid[i] = uid[i];
        }
        this->timestamp = timestamp;
    }

    /**
     * @brief Copy constructor
     * @param other Other LogData object
     */
    LogData(const LogData &logData)
    {
        for (int i = 0; i < uidSize; i++)
        {
            uid[i] = logData.uid[i];
        }
        timestamp = logData.timestamp;
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
    uint8_t *getUid(void) const
    {
        return (uint8_t *)uid;
    }

    /**
     * @brief Get the timestamp
     * @return Timestamp
     */
    uint32_t getTimestamp() const
    {
        return timestamp;
    }

    /**
     * @brief Assignment operator
     * @param other Other LogData object
     * @return Reference to this object
     */
    LogData &operator=(const LogData &logData)
    {
        for (int i = 0; i < uidSize; i++)
        {
            uid[i] = logData.uid[i];
        }
        timestamp = logData.timestamp;
        return *this;
    }

    /**
     * @brief Equality operator
     * @param other Other LogData object
     * @return True if the two objects are equal
     */
    bool operator==(const LogData &logData) const
    {
        for (int i = 0; i < uidSize; i++)
        {
            if (uid[i] != logData.uid[i])
            {
                return false;
            }
        }
        return (timestamp == logData.timestamp);
    }

    /**
     * @brief Convert the object to a string
     * @param str String to be filled
     * @note Format: "UID{20} TIMESTAMP{10}"
     */
    void toString(char *str) const
    {
        sprintf(str, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %010u",
                uid[0], uid[1], uid[2], uid[3], uid[4],
                uid[5], uid[6], uid[7], uid[8], uid[9],
                timestamp);
    }
}; // LogData
