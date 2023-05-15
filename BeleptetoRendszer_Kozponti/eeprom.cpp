/**
 ***************************************************************************************************
 * @file eeprom.cpp
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Implementation of eeprom.h.
 ***************************************************************************************************
 */

#include "eeprom.h"

#include "EEPROM_24LC64.h"

/**
 * @brief The lower 3 bits of the EEPROM device address.
 */
#define EEPROM_ADDRESS_LOWER_3_BITS 0b111

/**
 * @brief Object for the EEPROM.
 */
EEPROM_24LC64 eeprom(Wire, EEPROM_ADDRESS_LOWER_3_BITS);

/**
 * @brief The memory image of the EEPROM.
 */
static uint8_t memoryImage[EEPROM_24LC64_SIZE];

/**
 * @brief The updated pages of the EEPROM.
 */
static bool updatedPage[EEPROM_24LC64_SIZE_IN_PAGES];

/**
 * @brief Initialize the EEPROM.
 */
void EEPROM_Init()
{
    EEPROM_MemoryImage_Update();
}

/**
 * @brief Get the size of the EEPROM.
 * @return The size of the EEPROM in bytes.
 */
uint16_t EEPROM_GetSize(void)
{
    return EEPROM_24LC64_SIZE;
}

/**
 * @brief Get the memory image of the EEPROM.
 * @return Pointer to the memory image of the EEPROM.
 */
const uint8_t *EEPROM_GetMemoryImage(void)
{
    return memoryImage;
}

/**
 * @brief Write data to the EEPROM.
 * @note The data is written to the memory image, not to the EEPROM. Commit the memory image to write the data to the EEPROM.
 * @param address The address to write to.
 * @param data The data to write.
 * @param length The length of the data.
 */
void EEPROM_Write(uint16_t address, const uint8_t *data, uint16_t length)
{
    if (address + length > EEPROM_24LC64_SIZE)
    {
        // Trying to write outside of the EEPROM
        return;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        if (memoryImage[address + i] == data[i])
        {
            // The data is the same as in the memory image
            continue;
        }
        memoryImage[address + i] = data[i];
        updatedPage[(address + i) / EEPROM_24LC64_PAGE_SIZE] = true;
    }
}

/**
 * @brief Read data from the EEPROM.
 * @note The data is read from the memory image, not from the EEPROM. Update the memory image to read the data actually stored in the EEPROM.
 * @param address The address to read from.
 * @param data The data to read.
 * @param length The length of the data.
 */
void EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length)
{
    if (address + length > EEPROM_24LC64_SIZE)
    {
        // Trying to read outside of the EEPROM
        return;
    }

    for (uint16_t i = 0; i < length; i++)
    {
        data[i] = memoryImage[address + i];
    }
}

/**
 * @brief Update the EEPROM memory image.
 */
void EEPROM_MemoryImage_Update(void)
{
    // Read the whole EEPROM in page size chunks
    for (uint16_t i = 0; i < EEPROM_24LC64_SIZE_IN_PAGES; i++)
    {
        eeprom.readMultiBytes(i * EEPROM_24LC64_PAGE_SIZE,
                              &(memoryImage[i * EEPROM_24LC64_PAGE_SIZE]),
                              EEPROM_24LC64_PAGE_SIZE);
    }
}

/**
 * @brief Commit the EEPROM memory image.
 */
void EEPROM_MemoryImage_Commit(void)
{
    // Note: At each write operation, the EEPROM updates the whole page that contains the write
    // address. Therefore, it is most efficient to write data in page size chunks.

    // Make sure the EEPROM is not busy
    delay(EEPROM_24LC64_WRITE_DELAY_MS);

    // Write all updated pages to the EEPROM
    for (uint16_t i = 0; i < EEPROM_24LC64_SIZE_IN_PAGES; i++)
    {
        if (!updatedPage[i])
        {
            // Only write pages that have been updated
            continue;
        }
        updatedPage[i] = false;
        eeprom.writePage(i * EEPROM_24LC64_PAGE_SIZE,
                         &(memoryImage[i * EEPROM_24LC64_PAGE_SIZE]),
                         EEPROM_24LC64_PAGE_SIZE);
        delay(EEPROM_24LC64_WRITE_DELAY_MS);
    }
}
