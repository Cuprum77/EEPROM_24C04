#include <EEPROM_24C04.h>


EEPROM_24C04::EEPROM_24C04(uint8_t device_address)
{
    this->eeprom_addr = device_address;
    Wire.begin();
    Wire.setClock(1000000);
}


uint8_t EEPROM_24C04::get(uint16_t target_address)
{
    uint8_t __device_address = (uint8_t)(this->eeprom_addr | ((target_address >> 8) & 0x07));
    
    Wire.beginTransmission(__device_address);
    Wire.write(target_address & 0xFF);
    Wire.endTransmission();

    Wire.requestFrom(__device_address, 0x1);
    while(Wire.available())
        return Wire.read();
}

void EEPROM_24C04::get(uint16_t target_address, uint8_t* data, uint8_t length)
{
    uint8_t __device_address = (uint8_t)(this->eeprom_addr) | ((target_address >> 8) & 0x07);
    
    Wire.beginTransmission(__device_address);
    Wire.write(target_address & 0xFF);
    Wire.endTransmission();

    Wire.requestFrom(__device_address, length);
    while(Wire.available())
        *data++ = Wire.read();
}


void EEPROM_24C04::set(uint16_t target_address, uint8_t data)
{
    Wire.beginTransmission((uint8_t)(this->eeprom_addr | ((target_address >> 8) & 0x07)));
    Wire.write(target_address & 0xFF);
    Wire.write(data);
    Wire.endTransmission();
    delay(EEPROM_WRITE_CYCLE_TIME); // write cycle time
}

void EEPROM_24C04::set(uint16_t target_address, uint8_t* data, uint8_t length)
{
    Wire.beginTransmission((uint8_t)(this->eeprom_addr | ((target_address >> 8) & 0x07)));
    Wire.write(target_address & 0xFF);
    for(int i = 0; i < length; i++)
        Wire.write(*data++);
        
    Wire.endTransmission();
    delay(EEPROM_WRITE_CYCLE_TIME * 2); // write cycle time
}