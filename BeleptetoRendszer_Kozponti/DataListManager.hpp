/**
 ***************************************************************************************************
 * @file DataListManager.hpp
 * @author Péter Varga
 * @date 2023. 05. 08.
 ***************************************************************************************************
 * @brief This file contains the definition of the DataListManager class.
 ***************************************************************************************************
 */

#pragma once

#include <cstdint>

#include "AuthenticateList.hpp"
#include "LogList.hpp"
#include "eeprom.h"
#include "realtime.h"

/**
 * @brief This class manages the data lists.
 */
class DataListManager
{
public:
    /**
     * @brief List of authentication data.
     */
    AuthenticateList authList;
    /**
     * @brief List of log data.
     */
    LogList logList;

private:
    typedef struct _eeprom_header
    {
        uint16_t headerSize;
        uint16_t authenticateLength;
        uint16_t authenticateBaseAddress;
        uint16_t logLength;
        uint16_t logBaseAddress;
        uint32_t lastTimeUpdate;
    } eeprom_header_t;

    const uint16_t HEADER_SIZE_ADDRESS = 0;
    const uint16_t AUTHENTICATE_LENGTH_ADDRESS = 2;
    const uint16_t AUTHENTICATE_BASE_ADDRESS_ADDRESS = 4;
    const uint16_t LOG_LENGTH_ADDRESS = 6;
    const uint16_t LOG_BASE_ADDRESS_ADDRESS = 8;
    const uint16_t LAST_TIME_UPDATE_ADDRESS = 10;

    const uint16_t HEADER_SIZE = 14;
    const uint16_t AUTHENTICATE_BASE_ADDRESS;
    const uint16_t LOG_BASE_ADDRESS;

    eeprom_header_t local_header;

public:
    /**
     * @brief Default constructor
     */
    DataListManager(void)
        : AUTHENTICATE_BASE_ADDRESS(HEADER_SIZE),
          LOG_BASE_ADDRESS(EEPROM_SIZE / 2)
    {
    }

    /**
     * @brief Initializes the data lists.
     * @note This function must be called before using the data lists and after the EEPROM initialization.
     */
    void Initialize(void)
    {
        EEPROM_MemoryImage_Update();

        // Format: HEADER_SIZE{2} + AUTHENTICATE_LENGTH{2} + AUTHENTICATE_BASE_ADDRESS{2} + LOG_LENGTH{2} +
        // LOG_BASE_ADDRESS{2} + LAST_TIME_UPDATE{4} = 14

        const uint8_t *memory_image = EEPROM_GetMemoryImage();

        local_header.headerSize = ((uint16_t)(memory_image[HEADER_SIZE_ADDRESS]) << 8) |
                                  memory_image[HEADER_SIZE_ADDRESS + 1];
        local_header.authenticateBaseAddress = ((uint16_t)(memory_image[AUTHENTICATE_BASE_ADDRESS_ADDRESS]) << 8) |
                                               memory_image[AUTHENTICATE_BASE_ADDRESS_ADDRESS + 1];
        local_header.logBaseAddress = ((uint16_t)(memory_image[LOG_BASE_ADDRESS_ADDRESS]) << 8) |
                                      memory_image[LOG_BASE_ADDRESS_ADDRESS + 1];

        if ((local_header.headerSize != HEADER_SIZE) ||
            (local_header.authenticateBaseAddress != AUTHENTICATE_BASE_ADDRESS) ||
            (local_header.logBaseAddress != LOG_BASE_ADDRESS))
        {
            // EEPROM is not initialized

            local_header.headerSize = HEADER_SIZE;
            local_header.authenticateLength = 0;
            local_header.authenticateBaseAddress = AUTHENTICATE_BASE_ADDRESS;
            local_header.logLength = 0;
            local_header.logBaseAddress = LOG_BASE_ADDRESS;
            local_header.lastTimeUpdate = REALTIME_Get();

            return;
        }

        local_header.authenticateLength = ((uint16_t)(memory_image[AUTHENTICATE_LENGTH_ADDRESS]) << 8) |
                                          memory_image[AUTHENTICATE_LENGTH_ADDRESS + 1];

        local_header.logLength = ((uint16_t)(memory_image[LOG_LENGTH_ADDRESS]) << 8) |
                                 memory_image[LOG_LENGTH_ADDRESS + 1];

        local_header.lastTimeUpdate = ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS]) << 24) |
                                      ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS + 1]) << 16) |
                                      ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS + 2]) << 8) |
                                      memory_image[LAST_TIME_UPDATE_ADDRESS + 3];

        extractAuthenticateData(memory_image, EEPROM_SIZE, &local_header);
        extractLogData(memory_image, EEPROM_SIZE, &local_header);
    }

    /**
     * @brief Extract data from an EEPROM image and update the data lists with it.
     * @param memory_image Memory image of the EEPROM
     * @param size Size of the memory image
     */
    void extractListFromEepromImage(const uint8_t *memory_image, uint16_t size)
    {
        eeprom_header_t header;
        extractEepromHeader(memory_image, size, &header);
        if ((header.headerSize != HEADER_SIZE) ||
            (header.authenticateBaseAddress != AUTHENTICATE_BASE_ADDRESS) ||
            (header.logBaseAddress != LOG_BASE_ADDRESS))
        {
            return;
        }

        // extractAuthenticateData(memory_image, size, &header);
        extractLogData(memory_image, size, &header);
    }

    /**
     * @brief Update the EEPROM image with the data lists.
     */
    void updateEepromFromList(void)
    {
        updateEepromHeader();
        updateEepromAuthenticateData();
        updateEepromLogData();
        EEPROM_MemoryImage_Commit();
    }

private:
    void extractEepromHeader(const uint8_t *memory_image, uint16_t size, eeprom_header_t *header)
    {
        // Format: HEADER_SIZE{2} + AUTHENTICATE_LENGTH{2} + AUTHENTICATE_BASE_ADDRESS{2} + LOG_LENGTH{2} +
        // LOG_BASE_ADDRESS{2} + LAST_TIME_UPDATE{4} = 14
        header->headerSize = ((uint16_t)(memory_image[HEADER_SIZE_ADDRESS]) << 8) |
                             memory_image[HEADER_SIZE_ADDRESS + 1];

        header->authenticateLength = ((uint16_t)(memory_image[AUTHENTICATE_LENGTH_ADDRESS]) << 8) |
                                     memory_image[AUTHENTICATE_LENGTH_ADDRESS + 1];

        header->authenticateBaseAddress = ((uint16_t)(memory_image[AUTHENTICATE_BASE_ADDRESS_ADDRESS]) << 8) |
                                          memory_image[AUTHENTICATE_BASE_ADDRESS_ADDRESS + 1];

        header->logLength = ((uint16_t)(memory_image[LOG_LENGTH_ADDRESS]) << 8) |
                            memory_image[LOG_LENGTH_ADDRESS + 1];

        header->logBaseAddress = ((uint16_t)(memory_image[LOG_BASE_ADDRESS_ADDRESS]) << 8) |
                                 memory_image[LOG_BASE_ADDRESS_ADDRESS + 1];

        header->lastTimeUpdate = ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS]) << 24) |
                                 ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS + 1]) << 16) |
                                 ((uint32_t)(memory_image[LAST_TIME_UPDATE_ADDRESS + 2]) << 8) |
                                 memory_image[LAST_TIME_UPDATE_ADDRESS + 3];
    }

    void extractAuthenticateData(const uint8_t *memory_image, uint16_t size, const eeprom_header_t *header)
    {
        uint16_t address = header->authenticateBaseAddress;

        // Format: UID{10} + NAME{16} + BEGIN_HOUR{1} + BEGIN_MINUTE{1} + END_HOUR{1} + END_MINUTE{1} = 30
        uint8_t authenticate_data[30];

        for (uint16_t i = 0; i < header->authenticateLength / 30; i++, address += 30)
        {
            for (uint16_t j = 0; j < 30; j++)
            {
                authenticate_data[j] = memory_image[address + j];
            }

            uint8_t uid[10];
            char name[16 + 1];
            uint8_t begin_hour;
            uint8_t begin_minute;
            uint8_t end_hour;
            uint8_t end_minute;
            uint32_t interval_start;
            uint32_t interval_end;

            for (uint16_t j = 0; j < 10; j++)
            {
                uid[j] = authenticate_data[j];
            }

            for (uint16_t j = 0; j < 16; j++)
            {
                name[j] = authenticate_data[j + 10];
            }
            name[16] = '\0';

            begin_hour = authenticate_data[26];
            begin_minute = authenticate_data[27];
            end_hour = authenticate_data[28];
            end_minute = authenticate_data[29];

            interval_start = (begin_hour * 60 + begin_minute) * 60;
            interval_end = (end_hour * 60 + end_minute) * 60;

            if (authList.findByUid(uid) == -1)
            {
                authList.add(uid, name, interval_start, interval_end);
            }
        }
    }

    void extractLogData(const uint8_t *memory_image, uint16_t size, const eeprom_header_t *header)
    {
        uint16_t address = header->logBaseAddress;

        // Format: UID{10} + TIME{4} + AUTH{1} = 15
        uint8_t log_data[15];
        uint8_t uid[10];
        uint32_t time;
        uint8_t auth;

        for (uint16_t address = header->logBaseAddress;
             address < header->logBaseAddress + header->logLength;
             address += 15)
        {
            for (uint16_t j = 0; j < 15; j++)
            {
                log_data[j] = memory_image[address + j];
            }

            for (uint16_t j = 0; j < 10; j++)
            {
                uid[j] = log_data[j];
            }
            time = (log_data[10] << 24) | (log_data[11] << 16) | (log_data[12] << 8) | log_data[13];
            auth = log_data[14];

            logList.add(uid, time, auth);
        }
    }

    void updateEepromHeader(void)
    {
        // local_header.lastTimeUpdate = REALTIME_Get();
        local_header.authenticateLength = authList.size() * 30;
        local_header.logLength = logList.size() * 14;

        uint8_t buffer[4];
        uint16_t address = HEADER_SIZE_ADDRESS;

        buffer[0] = local_header.headerSize >> 8;
        buffer[1] = local_header.headerSize & 0xFF;
        EEPROM_Write(address, buffer, 2);
        address += 2;

        buffer[0] = local_header.authenticateLength >> 8;
        buffer[1] = local_header.authenticateLength & 0xFF;
        EEPROM_Write(address, buffer, 2);
        address += 2;

        buffer[0] = local_header.authenticateBaseAddress >> 8;
        buffer[1] = local_header.authenticateBaseAddress & 0xFF;
        EEPROM_Write(address, buffer, 2);
        address += 2;

        buffer[0] = local_header.logLength >> 8;
        buffer[1] = local_header.logLength & 0xFF;
        EEPROM_Write(address, buffer, 2);
        address += 2;

        buffer[0] = local_header.logBaseAddress >> 8;
        buffer[1] = local_header.logBaseAddress & 0xFF;
        EEPROM_Write(address, buffer, 2);
        address += 2;

        buffer[0] = local_header.lastTimeUpdate >> 24;
        buffer[1] = local_header.lastTimeUpdate >> 16 & 0xFF;
        buffer[2] = local_header.lastTimeUpdate >> 8 & 0xFF;
        buffer[3] = local_header.lastTimeUpdate & 0xFF;
        EEPROM_Write(address, buffer, 4);
    }

    void updateEepromAuthenticateData(void)
    {
        for (int i = 0; i < authList.size(); i++)
        {
            uint16_t address = local_header.authenticateBaseAddress + (i * 30);

            uint8_t begin_hour = authList.get(i)->getIntervalStart() % (60 * 60 * 24) / (60 * 60);
            uint8_t begin_minute = authList.get(i)->getIntervalStart() % (60 * 60) / 60;
            uint8_t end_hour = authList.get(i)->getIntervalEnd() % (60 * 60 * 24) / (60 * 60);
            uint8_t end_minute = authList.get(i)->getIntervalEnd() % (60 * 60) / 60;

            EEPROM_Write(address, authList.get(i)->getUid(), 10);
            EEPROM_Write(address + 10, (const uint8_t *)(authList.get(i)->getName()), 16);
            EEPROM_Write(address + 26, &begin_hour, 1);
            EEPROM_Write(address + 27, &begin_minute, 1);
            EEPROM_Write(address + 28, &end_hour, 1);
            EEPROM_Write(address + 29, &end_minute, 1);
        }
    }

    void updateEepromLogData(void)
    {
        uint16_t address = local_header.logBaseAddress;
        for (int i = 0; i < logList.size(); i++, address += 15)
        {
            uint32_t timestamp = logList.get(i)->getTimestamp();
            uint8_t buffer[4];
            buffer[0] = timestamp >> 24;
            buffer[1] = (timestamp >> 16) & 0xFF;
            buffer[2] = (timestamp >> 8) & 0xFF;
            buffer[3] = timestamp & 0xFF;
            uint8_t auth = logList.get(i)->getAuthentication();

            EEPROM_Write(address, logList.get(i)->getUid(), 10);
            EEPROM_Write(address + 10, buffer, 4);
            EEPROM_Write(address + 14, &auth, 1);
        }
    }
}; // DataListManager
