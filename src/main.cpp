#define EEPROM_ADDRESS		0x50
#define EEPROM_SIZE 		512
#define EEPROM_PAGE_SIZE	16
#define EEPROM_PAGE_COUNT	32

#include <Arduino.h>
#include <EEPROM_24C04.h>

EEPROM_24C04 *eeprom = new EEPROM_24C04(EEPROM_ADDRESS);

uint8_t pattern[EEPROM_SIZE] = {0x0};
uint8_t buffer[EEPROM_SIZE] = {0};

uint16_t address = 0x00;
uint16_t itteration = 0x00;
uint16_t successes = 0x00;
uint16_t received = 0x00;

void setup() 
{
	Serial.begin(9600);
}

void scanner()
{	
	for(int i = 0; i < 128; i++)
	{
		Wire.beginTransmission(i);
		if(Wire.endTransmission() == 0x0)
		{
			Serial.print("Found device at address: 0x");
			Serial.println(i, HEX);
		}
	}
}

void rw_test()
{
	// reset counters
	received = 0x00;
	successes = 0x00;
	Serial.println("\nStarting the memory read/write test...");
	// write to EEPROM
	address = 0x00;
	itteration = 0x00;
	for(int i = 0; i < EEPROM_PAGE_COUNT; i++)
	{
		eeprom->set(address, pattern + address, EEPROM_PAGE_SIZE);
		address += EEPROM_PAGE_SIZE;
		itteration++;
	}
	address = 0x00;
	itteration = 0x00;
	// read from EEPROM and display in serial monitor
	for(int i = 0; i < EEPROM_PAGE_COUNT; i++)
	{
		eeprom->get(address, buffer + address, EEPROM_PAGE_SIZE);
		if(memcmp(pattern + address, buffer + address, EEPROM_PAGE_SIZE) == 0)
			successes++;

		address += EEPROM_PAGE_SIZE;
		itteration++;
		received++;
	}

	Serial.print("Received ");
	Serial.print(received);
	Serial.print(" | Successes ");
	Serial.print(successes);
	Serial.print(" | Failures ");
	Serial.print(received - successes);
	Serial.print(" | Success rate ");
	Serial.print((float)successes / (float)received * 100.0);
	Serial.print("%\n");
}

void loop() 
{
	// wait for serial monitor to open
	while(!Serial);
	// wait for the user to send any character
	while(!Serial.available());
	
	scanner();
	memset(pattern, 0x69, EEPROM_SIZE);
	rw_test();
	memset(pattern, 0x4, EEPROM_SIZE);
	rw_test();
	memset(pattern, 0x20, EEPROM_SIZE);
	rw_test();
	memset(pattern, 0x0, EEPROM_SIZE);
	rw_test();
	memset(pattern, 0x1, EEPROM_SIZE);
	rw_test();

	// end loop lmao
	while(1);
}