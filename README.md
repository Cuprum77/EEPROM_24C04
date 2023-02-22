
# EEPROM_24C04
This library is built to interface with the cheap $1 24C04 EEPROM chips

## Parts 
It is not as easy to find this chip today, as several manufacturers have discontinued its production. And as a result, only a few stores carry it or similar ICs.

[Digikey](https://www.digikey.no/no/products/detail/fremont-micro-devices-ltd/FT24C04A-USG-B/3515962)
[Mouser](https://no.mouser.com/ProductDetail/onsemi/CAV24C04WE-GT3?qs=y%252BJdrdj3vZpSQMFoxfj%2FNQ%3D%3D) (not verified!)

## Installation
To install this library, simply download the zip file or clone this repository to your Arduino libraries folder. The default location for the libraries folder is in the Arduino installation directory, under the "libraries" folder.

## Basics of EEPROM
An EEPROM is an Electronic Erasable Programmable Memory, and is a non-volatile memory. This means it will retain its data after losing power, with modern chips having a data-retantion rate of over a hundred years!

Typically an EEPROM is divided up in multiple parts, first you have a block of memory. On the 24C04A that block happens to be 256 bytes large, and it has two of them, giving a total of 512 bytes worth of memory.

Then comes the pages, this is a division of a block into segments of memory, on the 24C04A this happens to be 16 bytes, giving us a total of 32 pages.
This number is not random, it gives us the ability to easily select a byte within a page by using the last 4 bits in an address, while also letting us select a page with the remainder bits. 
For example, at memory location `0x23` we can quickly see that we want to access the third byte on the second page!

For more information on this EEPROM, check out the [datasheet](datasheet/24C04A,8A,16A.pdf)

### Address
The EEPROM is divided into two seperate devices on the I2C bus, all while only taking up a single package. This is configured using the A0, A1 and A2 pins of the chip. 

Address of the device is decided on how you wire the A0-A2 pins
|    |A0 |A1 |A2 |
|--- |---|---|---|
|0x50| x | 0 | 0 |
|0x52| x | 1 | 0 |
|0x54| x | 0 | 1 |
|0x56| x | 1 | 1 |
A0 is not connected, and can simply be grounded

## Usage
To use this library you need to include the `EEPROM_24C04.h` header file in your Arduino sketch

```cpp
#include  <EEPROM_24C04.h>
#include  <Wire.h>
#define EEPROM_ADDRESS 0x50

EEPROM_24C04 *eeprom = new  EEPROM_24C04(EEPROM_ADDRESS, &Wire);
```

### Writing to EEPROM
#### Writing a single byte
To write a single byte to the EEPROM, one can use the `write_byte(address, byte)` function.
```cpp
// write 0x1 to location 0x0
eeprom->write_byte(0x0, 0x1);
```
#### Writing a page
To write a page's worth of bytes to the EEPROM, there is the `write_page(address, byte array)` function.
```cpp
// create an array filled with 0
byte data[16] = { 0x0 };
// write that page to the eeprom, at location 0x0
eeprom->write_page(0x0, data);
```
Note, this function will not run if the input array is not exactly the same bytes as the page size (default 16 bytes).
#### Writing a buffer
If there is the need to transfer an entire buffer, or just any arbitrary number of bytes to the EEPROM, the function`write_buffer(address, byte array, array size)` should be used.
```cpp
#define ARRAY_SIZE 36
// create an arbitrarily large array
byte data[ARRAY_SIZE];
// fill it with 0x5
memset(data, 0x5, sizeof(data));
// write the array to the eeprom at 0x20
eeprom->write_buffer(0x20, data, ARRAY_SIZE );
```
Note, this will only write it to the memory, page by page. However, it will start off from any arbitrary point on the memory, and allign itself to the memory

### Reading from EEPROM
#### Reading a single byte
To read a single byte, the function `read_byte(address)` should be used.
```cpp
// read the byte located at 0x20
byte data = eeprom->read_byte(0x20);
```
#### Reading a page
To read a page from the device, the function `read_page(address, byte array)` should be used. It reads a page worth of memory and dumps it into the byte array given.
This expects a byte array thats the same size as the EEPROM page size (default is 16 bytes).
```cpp
// create an empty buffer to store the page in
byte page[16] = { 0x0 };
// read a page from address 0x30
eeprom->read_page(0x30, page);
```
Note, this will only reads the number of bytes in a page! Default is 16 bytes.
#### Reading a buffer
However, if there is need for more data at once than a page's worth, one can batch read multiple bytes worth using `read_buffer(address, byte array, array length)`. This will read any arbitrary number of bytes from the device.
```cpp
#define ARRAY_SIZE 36
// create an empty byte array with the size ARRAY_SIZE
byte data[ARRAY_SIZE] = { 0x0 };
// fill the contents of the array from the eeprom,
// starting from address 0x5
eeprom->read_buffer(0x5, data, ARRAY_SIZE);
```

### Miscellaneous functions
#### Getting the page size
It might be useful to know how big the page size is for certain programs.
It is accessible through the function `page_size()`, which returns the number of bytes that a page is configured to.
```cpp
byte page_size = eeprom->page_size();
```
#### Getting the EEPROM size
Just as with the page size, there might be the need to get the EEPROM size as well, this can be accessed through `eeprom_size()`, which returns the number of bytes the EEPROM can store.
```cpp
unsigned short eeprom_size = eeprom->eeprom_size();
```

### Advanced Setup
There might be the need to customize the device a bit further. Maybe there is a desire to increase page size or attempt to make it work with similar EEPROM devices?

Luckily the constructor supports multiple arguements, listed below
|||
|---|---|
| Clock speed | Set the I²C clock speed |
| Page size | How many bytes per page |
| EEPROM size | How many bytes the EEPROM contains |
| Write cycle time | How much delay between each page write, in ms |

```cpp
#include  <EEPROM_24C04.h>
#include  <Wire.h>
#define ADDRESS 	0x50
#define PAGE_SIZE 	0x10	// 16 bytes
#define SIZE 	 	0x200	// 512 bytes

#define CLK_SPEED 	1000000	// 1MHz
#define WRITE_CYCLE	5		// 5 ms

EEPROM_24C04 *eeprom = 
	new EEPROM_24C04(ADDRESS, &Wire, CLK_SPEED, PAGE_SIZE, SIZE, WRITE_CYCLE);
```
Please note that the write_cycle should not be lower than 5 ms, as this will not give the chip enough time to write to memory!

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for more information
