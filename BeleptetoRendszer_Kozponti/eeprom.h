/**
 ***************************************************************************************************
 * @file eeprom.h
 * @author Péter Varga
 * @date 2023. 05. 04.
 ***************************************************************************************************
 * @brief Header file for the EEPROM functionality.
 ***************************************************************************************************
 */

#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

/**
 * @brief Size of the EEPROM in bytes.
 */
#define EEPROM_SIZE 8192

void EEPROM_Init(void);

uint16_t EEPROM_GetSize(void);

const uint8_t *EEPROM_GetMemoryImage(void);

void EEPROM_Write(uint16_t address, uint8_t *data, uint16_t length);

void EEPROM_Read(uint16_t address, uint8_t *data, uint16_t length);

void EEPROM_MemoryImage_Update(void);

void EEPROM_MemoryImage_Commit(void);

#endif /* EEPROM_H */
