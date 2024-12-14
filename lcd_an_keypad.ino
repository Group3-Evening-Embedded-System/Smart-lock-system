//LCD AND KEYPAD
#define DDRD     ((volatile uint8_t)(0x2A))
#define PORTD    ((volatile uint8_t)(0x2B))
#define PIND     ((volatile uint8_t)(0x29))
#define DDRB     ((volatile uint8_t)(0x24))
#define PORTB    ((volatile uint8_t)(0x25))
#define PINB     ((volatile uint8_t)(0x23))
#define TWBR     ((volatile uint8_t)(0xB8))
#define TWSR     ((volatile uint8_t)(0xB9))
#define TWDR     ((volatile uint8_t)(0xBB))
#define TWCR     ((volatile uint8_t)(0xBC))

// Constants
#define LCD_ADDR 0x27
#define ROWS 4
#define COLS 4
#define PASSCODE "1234"

// Pin Definitions
const uint8_t ROW_PINS[ROWS] = {9, 8, 7, 6};  // PB1, PB0, PD7, PD6
const uint8_t COL_PINS[COLS] = {5, 4, 3, 2};  // PD5, PD4, PD3, PD2

const char KEYMAP[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char enteredCode[5];
uint8_t codeIndex = 0;

void i2cInit(void) {
    TWBR = ((F_CPU/100000UL)-16)/2;
    TWSR = 0;
    TWCR = (1<<TWEN);
}

void i2cStart(void) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
}

void i2cWrite(uint8_t data) {
    TWDR = data;
    TWCR = (1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT)));
}

void lcdCommand(uint8_t cmd) {
    i2cStart();
    i2cWrite(LCD_ADDR << 1);
    i2cWrite(0x00);
    i2cWrite(cmd);
    i2cStop();
}

void lcdInit(void) {
    i2cInit();
    _delay_ms(50);
    lcdCommand(0x38);  // 2 lines, 5x7 matrix
    lcdCommand(0x0C);  // Display ON, cursor OFF
    lcdCommand(0x01);  // Clear display
    _delay_ms(2);
}

char scanKeypad(void) {
    for(uint8_t r = 0; r < ROWS; r++) {
        // Set current row low
        if(ROW_PINS[r] >= 8)
            PORTB &= ~(1 << (ROW_PINS[r]-8));
        else
            PORTD &= ~(1 << ROW_PINS[r]);
        
        for(uint8_t c = 0; c < COLS; c++) {
            if(!(PIND & (1 << COL_PINS[c]))) {
                _delay_ms(20);  // Debounce
                return KEYMAP[r][c];
            }
        }
        
        // Reset row
        if(ROW_PINS[r] >= 8)
            PORTB |= (1 << (ROW_PINS[r]-8));
        else
            PORTD |= (1 << ROW_PINS[r]);
    }
    return 0;
}

void setup(void) {
    // Configure row pins as outputs
    for(uint8_t i = 0; i < ROWS; i++) {
        if(ROW_PINS[i] >= 8) {
            DDRB |= (1 << (ROW_PINS[i]-8));
            PORTB |= (1 << (ROW_PINS[i]-8));
        } else {
            DDRD |= (1 << ROW_PINS[i]);
            PORTD |= (1 << ROW_PINS[i]);
        }
    }
    
    // Configure column pins as inputs with pull-ups
    for(uint8_t i = 0; i < COLS; i++) {
        DDRD &= ~(1 << COL_PINS[i]);
        PORTD |= (1 << COL_PINS[i]);
    }
    
    lcdInit();
    displayMessage("Enter Code:");
}

void loop(void) {
    char key = scanKeypad();
    if(key) {
        enteredCode[codeIndex++] = key;
        displayAsterisks(codeIndex);
        
        if(codeIndex == 4) {
            enteredCode[4] = '\0';
            if(strcmp(enteredCode, PASSCODE) == 0) {
                displayMessage("Correct!");
            } else {
                displayMessage("Wrong!");
            }
            _delay_ms(2000);
            codeIndex = 0;
            displayMessage("Enter Code:");
        }
    }
}