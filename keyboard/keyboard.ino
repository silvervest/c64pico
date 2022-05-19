#include <Wire.h>

// setup 9-byte array for response, 8 for the matrix, one lonesome byte just for the RESTORE key
// msg[9] = { [8-byte state of matrix], [1 byte state of RESTORE key] }
// the matrix state is comprised of eight bytes, each byte carries the value of one column of keys
// the column bytes are ordered high to low LSB, ie the top eight bits are for column 1 and the bottom eight bits are column 8
// the rows are ordered low to high LSB, so the row 1 is bit 1, row 8 is bit 8
// so if you're looking for row 6 in col 2 you would want to read the msg bit at position 8 + 8 + 5 (skip RESTORE byte, skip first column, skip row 5)
// or alternatively you could just look at (msg[1] & 0b00100000) :-)
static byte msg[9];

// iterate the keyboard matrix and extract which keys are currently pressed
void read_matrix(void) {
  byte *pos = &msg[0];
  uint8_t colBit;
  uint8_t rowBit;

  for (int col = 0; col < 8; col++) {
    colBit = (1 << col);
    // sink the column pin to ground
    DDRB |= colBit;
    PORTB &= ~colBit;
    
    for (int row = 0; row < 8; row++) {
      rowBit = (1 << row);
      
      // pull the row pin high via internal pull-up
      DDRD &= ~rowBit;
      PORTD |= rowBit;

      // data sheet says 1/2 - 1 1/2 cpu cycles for port sync
      // but this doesn't work in practice so send a bunch - doubled until it was stable
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      
      // if we get our 5v pull up (HIGH), the button is not pressed
      // if we get ground (LOW), the button is pressed
      if (PIND & rowBit) {
        *pos &= ~rowBit;
      } else {
        *pos |= rowBit;
      }

      // disable the internal pull-up on this row
      DDRD &= ~rowBit;
      PORTD &= ~rowBit;
    }
    *pos++;

    // and now drive the column pin high also
    DDRB &= ~colBit;
    PORTB &= ~colBit;
  }

  // finally, get the restore pin and set the last byte
  *pos = PINC & (1<<PC0) ? 0x00 : 0xFF;
}

void i2c_respond(void) {
  Wire.write(msg, 9);
}

void setup() {
  // port config, yes I'm doing this via AVR constants, deal with it
  // PORTB (cols) will be input, and PORTD (rows) will be input with pull ups enabled
  DDRB  = 0b00000000;
  PORTB = 0b00000000;
  DDRD  = 0b00000000;
  PORTD = 0b11111111;

  // also port 0 on PORTC is input with pull up enabled
  DDRC |= (1<<DDC0);
  PORTC |= (1<<PC0);

  // setup i2c also - slave on address 8
  Wire.begin(8);
  Wire.setClock(400000);
  
  // respond to a request for data
  Wire.onRequest(i2c_respond);
}

// basically just read the matrix forever
void loop() {
  read_matrix();
  delay(2);
}
