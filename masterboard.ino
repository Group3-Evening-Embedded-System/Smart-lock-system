//master
#define UBRR0H   ((volatile uint8_t)(0xC5))
#define UBRR0L   ((volatile uint8_t)(0xC4))
#define UCSR0A   ((volatile uint8_t)(0xC0))
#define UCSR0B   ((volatile uint8_t)(0xC1))
#define UDR0     ((volatile uint8_t)(0xC6))

#define CORRECT_CODE "1234"
#define CODE_LENGTH 4

char receivedCode[5];  // Extra byte for null terminator
uint8_t codeIndex = 0;

void initUART(void) {
    // Configure UART for 9600 baud
    UBRR0H = (F_CPU/16/9600-1) >> 8;
    UBRR0L = (F_CPU/16/9600-1);
    
    // Enable RX and TX
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
}

void transmitByte(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

char receiveByte(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void setup(void) {
    initUART();
}

void loop(void) {
    if (UCSR0A & (1 << RXC0)) {
        char key = UDR0;
        receivedCode[codeIndex++] = key;
        
        if (codeIndex == CODE_LENGTH) {
            receivedCode[codeIndex] = '\0';
            
            // Compare and send result
            char status = (strcmp(receivedCode, CORRECT_CODE) == 0) ? 'C' : 'I';
            transmitByte(status);
            
            // Reset for next code
            codeIndex = 0;
        }
    }
}