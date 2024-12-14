#include <EEPROM.h>

// SPI Register Definitions
#define SPCR_REG (*(volatile uint8_t *)0x2C)   // SPI Control Register
#define SPSR_REG (*(volatile uint8_t *)0x2D)   // SPI Status Register
#define SPDR_REG (*(volatile uint8_t *)0x2E)   // SPI Data Register
#define DDRB_REG (*(volatile uint8_t *)0x24)   // DDRB
#define PORTB_REG (*(volatile uint8_t *)0x25)  // PORTB
#define PINB_REG (*(volatile uint8_t *)0x23)   // PINB

// SPI Pins
#define SS 2   // PB2 - Slave Select
#define MISO 4 // PB4 - MISO

void setup() {
    // Initialize SPI as Slave using registers
    DDRB_REG |= (1 << MISO);    // Set MISO as output
    DDRB_REG &= ~(1 << SS);     // Ensure SS is an input
    SPCR_REG = (1 << SPE);      // Enable SPI in Slave mode

    // Initialize Serial for debugging
    Serial.begin(9600);
    Serial.write("EEPROM SPI Slave Initialized...\n");
}

void loop() {
    if (SPSR_REG & (1 << SPIF)) { // Check if SPI data is received
        char receivedData = SPDR_REG; // Read the received data

        // Write the data to EEPROM
        static int eepromAddress = 0; // Start address in EEPROM
        if (eepromAddress < EEPROM.length()) {
            EEPROM.write(eepromAddress++, receivedData); // Store data in EEPROM
            Serial.print("Data stored in EEPROM: ");
            Serial.println(receivedData);
        } else {
            Serial.println("EEPROM is full!");
        }
    }
}