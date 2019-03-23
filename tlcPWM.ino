#include <SPI.h> // Include SPI to establish a coomunication between tlc5940.
#define NUM_OF_BYTES 24 // Set the number of bites to send via SPI.
uint8_t REG_val[NUM_OF_BYTES];  // Array which is used to store the data to be sent.
void writePWM(uint8_t Channel, uint16_t Value); // tlc5940 PWM set function.
void setup() { //Setup all the parameters
  for (int i = 0; i < NUM_OF_BYTES; i++) // initialize all the data to zero.
    REG_val[i] = 0x00;
  cli();// Stop all interrupts at beginning.
  DDRD |= (1 << PIND3); //Set PIND3 as OUTPUT.
  DDRB |= (1 << PINB1) | (1 << PINB2) | (1 << PINB5); //Set PIND1, PIND2 and PIND5 as OUTPUTs.
  pinMode(11, OUTPUT);//MOSI DATA.
  pinMode(13, OUTPUT);//SPI Clock.
  SPI.setBitOrder(MSBFIRST); // SPI configure as Most Significant Bit First.
  SPI.setDataMode(SPI_MODE0); // SPI mode set to zero(tcl5940 uses the rising edge of the clock pulse for data shifting).
  SPI.setClockDivider(SPI_CLOCK_DIV4); // SPI clock set to 4MHz which is quite stable.
  TCCR2A = 0B00010010; // Set Timer 2 bits (Go through the data sheet of tcl5490 and Atmega328p ICs).
  TCCR2B = 0B00000001; // Set Timer 2 bits (Go through the data sheet of tcl5490 and Atmega328p ICs).
  OCR2A = 38; // Set Timer2 compare velue to generate the desired frequency.
  TCNT2 = 0; // Clear Timer Counter 2
  TCCR1A = 0B00000000;// Set Timer 1 bits (Go through the data sheet of tcl5490 and Atmega328p ICs).
  TCCR1B = 0B00001100;// Set Timer 1 bits (Go through the data sheet of tcl5490 and Atmega328p ICs).
  OCR1A = 1249; // To genarate 50Hz frequency.
  TIMSK1 = 0B00000010; // Set Timer 1 Mask according to your desired frequency.
  sei(); // Enable global interrupts.
}

void loop() {
  // This is to check the code with servos on 2 pins 2, 3 on tcl5940.
  writePWM(2, 3640);
  writePWM(3, 3980);
  delay(500);
  writePWM(2, 3820);
  writePWM(3, 3820);
  delay(500);
  writePWM(2, 3980);
  writePWM(3, 3640);
  delay(500);
//  for (int i = 0; i < 100; i++) {
//    writePWM(1, i*40);
//    delay(50);
//  }
//  for (int i = 0; i < 100; i++) {
//    writePWM(2, i*40);
//    delay(50);
//  }
//  for (int i = 0; i < 100; i++) {
//    writePWM(3, i*40);
//    delay(50);
//  }
}

void writePWM(uint8_t Channel, uint16_t Value) { //tcl5940 PWM data send function.

    /*This process has to be done because each channel in tcl5940 needs 12bits as the data per channel
    But SPI can only transfer 8bits through the communication process.
    To fulfill that requirement, the data to be sent is re-arrange in a new form by deviding in to
    8 bit parts.
  */
  
  Channel = 17 - Channel; //Channel Selection
  delay(1); // Small delay to stabilize the system
  switch (Channel % 2) { // data is separated as the index being even or odd
    case 0:
      { uint8_t even = (3 * Channel - 4) / 2; // location for each data is selected by this function.
        REG_val[even] &= B11110000; // then data will put into coresponding indexes.
        REG_val[even] |= Value >> 8; // Bit shifting for data manipulation process.
        REG_val[even + 1] = Value; // setting up the whole data to be sent.
      }
      break;
    case 1:
      { uint8_t odd = (3 * Channel - 3) / 2; // location selection for odd channel numbers.
        REG_val[odd] = Value >> 4;
        REG_val[odd + 1] &= B00001111;
        REG_val[odd + 1] |= Value << 4;
      }
      break;
  }
}

ISR(TIMER1_COMPA_vect) {
  PORTB |= (1 << PINB2);//BLANK = 1 ---> Stop PWM
  PORTB |= (1 << PINB1);//XLAT = 1 ---> Shift data to GS REG
  PORTB &= ~(1 << PINB1);//XLAT = 0 ---> Shifting over and ready to go
  PORTB &= ~(1 << PINB2);//BLANK = 0 ---> Start PWM
  PORTB |= (1 << PINB5);//Extra High Pulse for CLK to get the 193 pulses. (***Refer data sheet clock diagram to see the 193rd pulse***)
  PORTB &= ~(1 << PINB5);//Return that pulse to Zero again.
  SPI.begin();
  for (int trans = 0; trans < NUM_OF_BYTES; trans++)
    SPI.transfer(REG_val[trans]);
  SPI.end();
}
