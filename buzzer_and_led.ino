// led and buzzer
#define DDRB     ((volatile uint8_t)(0x24))
#define PORTB    ((volatile uint8_t)(0x25))
#define UBRR0H   ((volatile uint8_t)(0xC5))
#define UBRR0L   ((volatile uint8_t)(0xC4))
#define UCSR0A   ((volatile uint8_t)(0xC0))
#define UCSR0B   ((volatile uint8_t)(0xC1))
#define UDR0     ((volatile uint8_t)(0xC6))
#define TCCR2A   ((volatile uint8_t)(0xB0))
#define TCCR2B   ((volatile uint8_t)(0xB1))
#define OCR2A    ((volatile uint8_t)(0xB3))

// Pin Definitions (PORTB)
#define GREEN_LED  PB5  // Pin 13
#define RED_LED    PB4  // Pin 12
#define BUZZER     PB3  // Pin 11

void initUART(void) {
    UBRR0H = (F_CPU/16/9600-1) >> 8;
    UBRR0L = (F_CPU/16/9600-1);
    UCSR0B = (1<<RXEN0);
}

void initTimer(void) {
    TCCR2A = (1 << WGM21) | (1 << COM2A0);  // CTC mode, toggle OC2A
    TCCR2B = (1 << CS22) | (1 << CS21);      // Prescaler 256
}

void playTone(uint16_t frequency, uint16_t duration) {
    OCR2A = F_CPU/(2*256*frequency) - 1;
    TCCR2A |= (1 << COM2A0);
    _delay_ms(duration);
    TCCR2A &= ~(1 << COM2A0);
}

void playSuccessSound(void) {
    playTone(1000, 200);
    _delay_ms(200);
    playTone(1500, 200);
    _delay_ms(200);
    playTone(2000, 400);
}

void playEmergencySound(void) {
    for(uint8_t i = 0; i < 3; i++) {
        playTone(2000, 100);
        _delay_ms(150);
        playTone(1500, 100);
        _delay_ms(150);
        playTone(2000, 100);
        _delay_ms(150);
    }
}

void setup(void) {
    // Configure LED and Buzzer pins as outputs
    DDRB |= (1 << GREEN_LED) | (1 << RED_LED) | (1 << BUZZER);
    
    // Initialize all outputs to LOW
    PORTB &= ~((1 << GREEN_LED) | (1 << RED_LED) | (1 << BUZZER));
    
    initUART();
    initTimer();
}

void loop(void) {
    if (UCSR0A & (1 << RXC0)) {
        char status = UDR0;
        
        if (status == 'C') {
            PORTB |= (1 << GREEN_LED);
            PORTB &= ~(1 << RED_LED);
            playSuccessSound();
            _delay_ms(1000);
        }
        
        if (status == 'I') {
            PORTB &= ~(1 << GREEN_LED);
            PORTB |= (1 << RED_LED);
            playEmergencySound();
            _delay_ms(1000);
        }
        
        // Reset outputs
        PORTB &= ~((1 << GREEN_LED) | (1 << RED_LED));
    }
}