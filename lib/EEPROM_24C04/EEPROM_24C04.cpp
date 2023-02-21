#include <EEPROM_24C04.h>

/**
 * Create a new instance of the EEPROM_24C04 class
 * 
 * @param device_address: The address of the EEPROM device.
 * @param wire: The TwoWire instance to use.
 * @param clock: The clock speed to use.
 * @param page_size: The size of the page to write to.
 * @param write_cycle_time: The time to wait between writes in milliseconds.
 * 
 * @example Create a new instance of the EEPROM_24C04 class
 *          EEPROM_24C04* eeprom = new eeprom(EEPROM_ADDRESS, Wire, 1000000);
 * 
 * @note The default clock speed is 1MHz, the default page size is 16 bytes and the default write cycle time is 5ms. 
 *       It is not recommended to change the write_cycle_time or the page_size if you are not familiar with the EEPROM device you are using.
*/
EEPROM_24C04::EEPROM_24C04(uint8_t device_address, TwoWire* wire, uint32_t clock, 
                        uint8_t page_size, uint16_t eeprom_size, uint8_t write_cycle_time)
{
    this->_eeprom_addr = device_address;
    this->_wire = wire;
    this->_page_size = page_size;
    this->_page_count = eeprom_size / page_size;
    this->_eeprom_size = eeprom_size;
    this->_write_cycle_time = write_cycle_time;

    this->_wire->begin();
    this->_wire->setClock(clock);
}

/**
 * Read a single byte from the EEPROM device
 * 
 * @param target_address: The address of the byte to read.
 * @return The byte read from the EEPROM.
 * 
 * @example Read a single byte from the EEPROM device
 *          uint8_t data = eeprom->read_byte(0);
*/
uint8_t EEPROM_24C04::read_byte(uint16_t target_address)
{
    uint8_t __device_address = (uint8_t)(this->_eeprom_addr | ((target_address >> 8) & 0x07));
    this->read_bytes(target_address, &__device_address, 1);
}

/**
 * Read a page of data from the EEPROM device
 * 
 * @param target_address: The address of the data to read.
 * @param data: The buffer to store the data in.
 * 
 * @example Read a page of data from the EEPROM device
 *          uint8_t data[16];
 *          eeprom->read_page(0, data);
 * 
 * @note This will return and do nothing if the buffer is not the correct size!
*/
void EEPROM_24C04::read_page(uint16_t target_address, uint8_t* data)
{
    // just make sure the user is providing the correct buffer size
    if(sizeof(data) > this->_page_size)
        return;

    this->read_bytes(target_address, data, this->_page_size);
}

/**
 * Read an arbitrary amount of data from the EEPROM device
 * 
 * @param target_address: The address of the data to read.
 * @param data: The buffer to store the data in.
 * @param length: The amount of data to read.
 * 
 * @example Read 32 bytes from the EEPROM device
 *          uint8_t data[32];
 *          eeprom->read_buffer(0, data, 32);
 * 
 * @note This splits the data into pages and reads them individually. This is due to a limitation of the Wire library.
*/
void EEPROM_24C04::read_buffer(uint16_t target_address, uint8_t* data, uint16_t length)
{
    // divide the length by the page size to get the number of pages to read
    uint8_t pages = length / this->_page_size;
    // get the remainder
    uint8_t remainder = length % this->_page_size;

    // determine if we are in the middle of a page
    uint8_t offset = target_address % this->_page_size;
    // if we are in the middle of a page, we need to read the remainder of the page
    if(offset > 0)
    {
        // read the remainder of the page
        this->read_bytes(target_address, data, this->_page_size - offset);
        // increment the target address
        target_address += this->_page_size - offset;
        // increment the data pointer
        data += this->_page_size - offset;
        // decrement the number of pages to read
        pages--;
    }

    // if the number of pages exceeds the total amount of pages from that address and beyond, drop the excess
    if(pages > (this->_page_count - (target_address / this->_page_size)))
        pages = (this->_page_count - (target_address / this->_page_size));

    // read the pages
    for(int i = 0; i < pages; i++)
    {
        this->read_bytes(target_address, data, this->_page_size);
        target_address += this->_page_size;
        data += this->_page_size;
    }

    // read the remainder
    if(remainder > 0)
        this->read_bytes(target_address, data, remainder);
}

/**
 * Write a single byte to the EEPROM device
 * 
 * @param target_address: The address of the byte to write.
 * @param data: The byte to write.
 * 
 * @example Write a single byte to the EEPROM device
 *          eeprom->write_byte(0, 0x01);
*/
void EEPROM_24C04::write_byte(uint16_t target_address, uint8_t data)
{
    this->write_bytes(target_address, &data, 1);
}

/**
 * Write a page of data to the EEPROM device
 * 
 * @param target_address: The address of the data to write.
 * @param data: The buffer to write.
 * 
 * @example Write a page of data to the EEPROM device
 *          uint8_t data[16];
 *          eeprom->write_page(0, data);
 * 
 * @note This will return and do nothing if the buffer is not the correct size!
*/
void EEPROM_24C04::write_page(uint16_t target_address, uint8_t* data)
{
    // just make sure the user is providing the correct buffer size
    if(sizeof(data) > this->_page_size)
        return;

    this->write_bytes(target_address, data, this->_page_size);
}

/**
 * Write an arbitrary amount of data to the EEPROM device
 * 
 * @param target_address: The address of the data to write.
 * @param data: The buffer to write.
 * @param length: The amount of data to write.
 * 
 * @example Write 32 bytes to the EEPROM device
 *          uint8_t data[32];
 *          eeprom->write_buffer(0, data, 32);
 * 
 * @note This splits the data into pages and writes them individually. This is due to a limitation of the Wire library.
*/
void EEPROM_24C04::write_buffer(uint16_t target_address, uint8_t* data, uint16_t length)
{
    // divide the length by the page size to get the number of pages to write
    uint8_t pages = length / this->_page_size;
    // get the remainder
    uint8_t remainder = length % this->_page_size;

    // determine if we are in the middle of a page
    uint8_t offset = target_address % this->_page_size;
    if(offset > 0)
    {
        // write the remainder of the page
        uint8_t remainder = this->_page_size - offset;
        this->write_bytes(target_address, data, remainder);
        target_address += remainder;
        data += remainder;
        pages--;
    }

    // if the number of pages exceeds the total amount of pages from that address and beyond, drop the excess
    if(pages > (this->_page_count - (target_address / this->_page_size)))
        pages = (this->_page_count - (target_address / this->_page_size));

    // write the pages
    for(int i = 0; i < pages; i++)
    {
        this->write_bytes(target_address, data, this->_page_size);
        target_address += this->_page_size;
        data += this->_page_size;
    }

    // write the remainder
    if(remainder > 0)
        this->write_bytes(target_address, data, remainder);
}

/**
 * Get the page size of the EEPROM device
 * 
 * @return The page size of the EEPROM device in bytes
*/
uint8_t EEPROM_24C04::page_size()
{
    return this->_page_size;
}

/**
 * Get the total size of the EEPROM device
 * 
 * @return The total size of the EEPROM device in bytes
*/
uint16_t EEPROM_24C04::eeprom_size()
{
    return this->_page_size * this->_page_count;
}

/**
 * @private
 * Read an arbitrary amount of data from the EEPROM device
 * 
 * @param target_address: The address of the data to read.
 * @param data: The buffer to store the data in.
 * @param length: The amount of data to read.
 * 
 * @example Read 32 bytes from the EEPROM device
 *          uint8_t data[32];
 *          eeprom->read_bytes(0, data, 32);
*/
void EEPROM_24C04::read_bytes(uint16_t target_address, uint8_t* data, uint8_t length)
{
    uint8_t __device_address = (uint8_t)(this->_eeprom_addr) | ((target_address >> 8) & 0x07);
    
    _wire->beginTransmission(__device_address);
    _wire->write(target_address & 0xFF);
    _wire->endTransmission();

    _wire->requestFrom(__device_address, this->_page_size);
    while(Wire.available())
        *data++ = Wire.read();
}

/**
 * @private
 * Write an arbitrary amount of data to the EEPROM device
 * 
 * @param target_address: The address of the data to write.
 * @param data: The buffer to write.
 * @param length: The amount of data to write.
 * 
 * @example Write 32 bytes to the EEPROM device
 *          uint8_t data[32];
 *          eeprom->write_bytes(0, data, 32);
*/
void EEPROM_24C04::write_bytes(uint16_t target_address, uint8_t* data, uint8_t length)
{
    _wire->beginTransmission((uint8_t)(this->_eeprom_addr | ((target_address >> 8) & 0x07)));
    _wire->write(target_address & 0xFF);
    for(int i = 0; i < this->_page_size; i++)
        _wire->write(*data++);
        
    _wire->endTransmission();
    delay(this->_write_cycle_time * 2); // write cycle time
}
