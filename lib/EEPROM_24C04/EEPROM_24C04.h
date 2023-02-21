#pragma once

#include <Arduino.h>
#include <Wire.h>

#define EEPROM_PAGE_SIZE          16
#define EEPROM__RD_BUFFER_SIZE    BUFFER_LENGTH
#define EEPROM__WR_BUFFER_SIZE    (BUFFER_LENGTH - 1)
#define EEPROM_WRITE_CYCLE_TIME   5

class EEPROM_24C04
{
public:
    EEPROM_24C04(uint8_t device_address);
    uint8_t get(uint16_t target_address);
    void get(uint16_t target_address, uint8_t* data, uint8_t length);
    void set(uint16_t target_address, uint8_t data);
    void set(uint16_t target_address, uint8_t* data, uint8_t length);
private:
    uint8_t eeprom_addr; 
};