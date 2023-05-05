/**
 ***************************************************************************************************
 * @file EEPROM_24LC64.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Hardware abstraction layer for the 24LC64 EEPROM.
 ***************************************************************************************************
 */

#ifndef EEPROM_24LC64_H
#define EEPROM_24LC64_H

#include <Wire.h>

/** @brief Size of the 24LC64 EEPROM in bytes. */
#define EEPROM_24LC64_SIZE 8192
/** @brief Size of one page of the 24LC64 EEPROM in bytes. */
#define EEPROM_24LC64_PAGE_SIZE 32
/** @brief Number of pages in the 24LC64 EEPROM. */
#define EEPROM_24LC64_SIZE_IN_PAGES (EEPROM_24LC64_SIZE / EEPROM_24LC64_PAGE_SIZE)
/** @brief Maximum number of bytes that can be written in one write. */
#define EEPROM_24LC64_MAX_WRITE_LENGTH 32
/** @brief Delay in milliseconds between writes. */
#define EEPROM_24LC64_WRITE_DELAY_MS 5
/** @brief Base address of the 24LC64 EEPROM. */
#define EEPROM_24LC64_DEVICE_BASE_ADDRESS (0b10100000 >> 1)

class EEPROM_24LC64
{
private:
    TwoWire &_i2c;
    uint8_t _device_address;

public:
    EEPROM_24LC64(TwoWire &i2c, uint8_t device_address_lower_3_bits);
    ~EEPROM_24LC64();

    void writeByte(uint16_t address, uint8_t data);
    void writePage(uint16_t address, uint8_t *data, uint8_t length);

    uint8_t readByte(uint16_t address);
    uint16_t readMultiBytes(uint16_t address, uint8_t *data, uint16_t length);
    uint8_t readNextByte();
    uint16_t readNextMultiBytes(uint8_t *data, uint16_t length);
};

#endif /* EEPROM_24LC64_H */
