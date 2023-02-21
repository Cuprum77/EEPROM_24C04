#pragma once

#include <Arduino.h>
#include <Wire.h>

class EEPROM_24C04
{
public:
    EEPROM_24C04(uint8_t device_address, TwoWire* wire, uint32_t clock = 1000000, 
                uint8_t page_size = 16, uint16_t eeprom_size = 512, uint8_t write_cycle_time = 5);
    
    uint8_t read_byte(uint16_t target_address);
    void read_page(uint16_t target_address, uint8_t* data);
    void read_buffer(uint16_t target_address, uint8_t* data, uint16_t length);

    void write_byte(uint16_t target_address, uint8_t data);
    void write_page(uint16_t target_address, uint8_t* data);
    void write_buffer(uint16_t target_address, uint8_t* data, uint16_t length);

    uint8_t page_size();
    uint16_t eeprom_size();
    
private:
    uint8_t _page_size;
    uint8_t _page_count;
    uint16_t _eeprom_size;
    uint16_t _write_cycle_time;
    TwoWire* _wire;
    uint8_t _eeprom_addr; 

    void read_bytes(uint16_t target_address, uint8_t* data, uint8_t length);
    void write_bytes(uint16_t target_address, uint8_t* data, uint8_t length);
};