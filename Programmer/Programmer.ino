/* Ports for soft UART */
#define UART_PORT   PORTB
#define UART_PIN    PINB
#define UART_DDR    DDRB
#define UART_TX_BIT PB0
#define UART_RX_BIT PB0

#define STK_OK 0x10

#define BAUD_RATE 115200

#define SERIAL_TIMEOUT 15
#define SERIAL_TIMEOUT_LONG 200

uint8_t buf[300];
uint8_t ch;

void setup() {
  Serial.begin(115200);
  /* Set TX pin as output */
  UART_DDR |= _BV(UART_TX_BIT);
  /* Set TX pin as input = default (tri state pin?) */
  //UART_DDR &= ~_BV(UART_TX_BIT);
  /* SET_UART_PIN (line low) */
  //UART_PORT &= ~_BV(UART_TX_BIT);
  /* CLEAR_UART_PIN (set high) */
  UART_PORT |= _BV(UART_TX_BIT);
  /* READ_UART_PIN */
  //UART_PIN & _BV(UART_RX_BIT);
  /* Call uartdelay to have delayfuntion included in code */
  uartDelay();
}


void loop() {
  if (Serial.available()) {      // If anything comes in Serial (USB),
    int buflen = 0;
    buf[buflen++] = Serial.read();
    // Temporarily set timer2 to count ticks/128
    TCNT2 = 0;
    TCCR2B = (1<<CS22) | (1 << CS20);
    // Buffer data until the serial timeout
    while(TCNT2 < SERIAL_TIMEOUT){
      if(Serial.available()){
        buf[buflen++] = Serial.read();
        TCNT2 = 0;
      }
    }
    // Set timer2 back to normal
    TCCR2B &= 0b11111000;
    cli();
    putch(buf[0]);
    for(int i=1; i<buflen; i++){
      putch(buf[i]);
    }
    TCNT2 = 0;
    TCCR2B = (1<<CS22) | (1 << CS20);
    UART_DDR &= ~_BV(UART_TX_BIT);
    // Buffer data until the serial timeout
    while(TCNT2 < SERIAL_TIMEOUT){
      if(!(UART_PIN & _BV(UART_RX_BIT))){
        ch = getch();
        Serial.write(ch);
        TCNT2 = 0;
      }
    }
    /* during programming the STK_OK messages comes after a delay, catch this here but allow timeout if not coming */
    if(ch != STK_OK){
      TCCR2B = (1<<CS22) | (1<<CS21) | (1 << CS20);
      TCNT2 = 0;
      while(TCNT2 < SERIAL_TIMEOUT_LONG){
        if(!(UART_PIN & _BV(UART_RX_BIT))){
          ch = getch();
          Serial.write(ch);
          TCNT2 = 255; //immediate timeout
        }
      }     
    }
    // Set timer2 back to normal
    TCCR2B &= 0b11111000;
    UART_DDR |= _BV(UART_TX_BIT);
    sei();
  }
}

// AVR305 equation: #define UART_B_VALUE (((F_CPU/BAUD_RATE)-23)/6)
// Adding 3 to numerator simulates nearest rounding for more accurate baud rates
#define UART_B_VALUE (((F_CPU/BAUD_RATE)-20)/6)
#if UART_B_VALUE > 255
#error Baud rate too slow for soft UART
#endif

#if UART_B_VALUE < 6
// (this value is a "guess" at when loop/call overhead might become too
//  significant for the soft uart to work.  It tests OK with the popular
//  "ATtinycore" chips that need SOFT_UART, at the usual clock/baud combos.)
#error Baud rate too high for soft UART
#endif

int uartbval(void){
  return UART_B_VALUE;
}

void putch(char ch) {
  //Serial.write(ch);
  //watchdogReset();
  __asm__ __volatile__ (
    //"   rcall uartDelay\n"
    //"   rcall uartDelay\n"
    "   com %[ch]\n" // ones complement, carry set
    "   sec\n"
    "1: brcc 2f\n"
    "   cbi %[uartPort],%[uartBit]\n"
    "   rjmp 3f\n"
    "2: sbi %[uartPort],%[uartBit]\n"
    "   nop\n"
    "3: rcall uartDelay\n"
    "   rcall uartDelay\n"
    "   lsr %[ch]\n"
    "   dec %[bitcnt]\n"
    "   brne 1b\n"
    :
    :
      [bitcnt] "d" (10),
      [ch] "r" (ch),
      [uartPort] "I" (_SFR_IO_ADDR(UART_PORT)),
      [uartBit] "I" (UART_TX_BIT)
    :
      "r25"
  );
}

uint8_t getch(void) {
  uint8_t ch;
  //#ifdef LED_DATA_FLASH
  //  LED_PORT ^= _BV(LED);
  //#endif
  //watchdogReset();
  __asm__ __volatile__ (
    "   rcall uartDelay\n"          // Get to middle of start bit
    "2: rcall uartDelay\n"              // Wait 1 bit period
    "   rcall uartDelay\n"              // Wait 1 bit period
    "   clc\n"
    "   sbic  %[uartPin],%[uartBit]\n"
    "   sec\n"
    "   dec   %[bitCnt]\n"
    "   breq  3f\n"
    "   ror   %[ch]\n"
    "   rjmp  2b\n"
    "3:\n"
    :
      [ch] "=r" (ch)
    :
      [bitCnt] "d" (9),
      [uartPin] "I" (_SFR_IO_ADDR(UART_PIN)),
      [uartBit] "I" (UART_RX_BIT)
    :
      "r25"
  );
  //#ifdef LED_DATA_FLASH
  //  LED_PORT ^= _BV(LED);
  //#endif
  return ch;
}

void uartDelay(){
  __asm__ __volatile__ (
    //"   rcall uartDelay\n"          // Get to middle of start bit
    "   rjmp  2f\n"
    "uartDelay: ldi r25,%[count]\n"
    "1: dec r25\n"
    "   brne 1b\n"
    "   ret\n"
    "2:\n"
    :
    :
      [count] "M" (UART_B_VALUE)
    //:
    //  "r25"
  );
}


