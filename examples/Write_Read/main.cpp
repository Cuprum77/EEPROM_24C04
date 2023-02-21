#define EEPROM_ADDRESS		0x50

#include <Arduino.h>
#include <EEPROM_24C04.h>

EEPROM_24C04 *eeprom = new EEPROM_24C04(EEPROM_ADDRESS, &Wire);

uint8_t buffer[512] = {0};

void setup() 
{
	// initialize serial monitor
	Serial.begin(9600);
}

void loop() 
{
	// wait for serial monitor to open
	while(!Serial);
	// wait for the user to send any character
	while(!Serial.available());

	// set the entire buffer to 0x5 so we can see if the eeprom is actually being written to
	memset(buffer, 0x69, sizeof(buffer));
	// write the entirety of the buffer to the eeprom
	eeprom->write_buffer(0x00, buffer, sizeof(buffer));

	// clear the buffer so we are sure the data is being read from the eeprom
	memset(buffer, 0x0, sizeof(buffer));
	// read the entirety of the eeprom into the buffer
	eeprom->read_buffer(0x00, buffer, sizeof(buffer));

	Serial.println("Buffer:");

	// print the buffer to the serial monitor
	for(uint16_t i = 0; i < sizeof(buffer); i++)
	{
		// print the byte in hex
		Serial.print("0x");
		// add a leading 0 if the byte is less than 0x10
		if(buffer[i] < 0x10) 
			Serial.print("0");
		// print the byte
		Serial.print(buffer[i], HEX);

		// newline every 16 bytes for readability
		if((i + 1) % 0x10 == 0)
			Serial.println();
		else
			Serial.print(" ");
	}

	// end loop lmao
	while(1);
}