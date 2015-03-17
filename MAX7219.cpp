/*
 
 MAX7219 class
 Author: Nick Gammon
 Date:   17 March 2015
 
 
 PERMISSION TO DISTRIBUTE
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 and associated documentation files (the "Software"), to deal in the Software without restriction, 
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in 
 all copies or substantial portions of the Software.
 
 
 LIMITATION OF LIABILITY
 
 The software is provided "as is", without warranty of any kind, express or implied, 
 including but not limited to the warranties of merchantability, fitness for a particular 
 purpose and noninfringement. In no event shall the authors or copyright holders be liable 
 for any claim, damages or other liability, whether in an action of contract, 
 tort or otherwise, arising from, out of or in connection with the software 
 or the use or other dealings in the software. 
 
*/

#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219.h>
#include <MAX7219_font.h>

/*

Wiring:

  Hardware SPI:

  Wire DIN (data) to the MOSI pin (D11 on a Uno)
  Wire CLK (clock) to the SCK pin (D13 on a Uno)
  Wire LOAD to the /SS (slave select) pin (D10 on a Uno)

  Make an instance of the class:

    MAX7219 myDisplay (1, 10);  // 1 chip, and then specify the LOAD pin only

  Bit-banged SPI:

  Wire LOAD, DIN, CLK to any pins of your choice.

  Make an instance of the class:

    MAX7219 myDisplay (2, 6, 7, 8);  // 2 chips, then specify the LOAD, DIN, CLK pins

Usage:

  Initialize:

    myDisplay.begin ();

  Shut down:

    myDisplay.end ();

  Write to display:

    myDisplay.sendString ("HELLO");

  Set the intensity (from 0 to 15):

    myDisplay.setIntensity (8);

  For the class to compile you need to include these three files:

      SPI.h
      bitBangedSPI.h
      MAX7219.h

  You can obtain the bitBangedSPI library from:

    https://github.com/nickgammon/bitBangedSPI

*/


  
// destructor
MAX7219::~MAX7219 ()
  {
  end ();
  } // end of destructor

void MAX7219::begin ()
  {
  pinMode (load_, OUTPUT);
  digitalWrite (load_, HIGH);
  
  // prepare SPI
  if (bitBanged_)
    {
    if (bbSPI_ == NULL)
      bbSPI_ = new bitBangedSPI (din_, bitBangedSPI::NO_PIN, clock_);
    bbSPI_->begin ();
    } // end of bit banged SPI
  else
    {  // hardware SPI
    SPI.begin ();
    }  // end of hardware SPI

  sendToAll (MAX7219_REG_SCANLIMIT, 7);     // show 8 digits
  sendToAll (MAX7219_REG_DECODEMODE, 0);    // use bit patterns
  sendToAll (MAX7219_REG_DISPLAYTEST, 0);   // no display test
  sendToAll (MAX7219_REG_INTENSITY, 15);    // character intensity: range: 0 to 15
  sendString ("");                          // clear display
  sendToAll (MAX7219_REG_SHUTDOWN, 1);      // not in shutdown mode (ie. start it up)
  } // end of MAX7219::begin

void MAX7219::end ()
  {
  sendToAll (MAX7219_REG_SHUTDOWN, 0);  // shutdown mode (ie. turn it off)

  if (bbSPI_ != NULL)
    {
    delete bbSPI_;
    bbSPI_ = NULL;
    }

  if (!bitBanged_)
    SPI.end ();

  } // end of MAX7219::end

void MAX7219::setIntensity (const byte amount)
  {
  sendToAll (MAX7219_REG_INTENSITY, amount & 0xF);     // character intensity: range: 0 to 15
  } // end of MAX7219::setIntensity

// send one byte to MAX7219
void MAX7219::sendByte (const byte reg, const byte data)
  {    
  if (bitBanged_)
    {
    if (bbSPI_ != NULL)
      {
      bbSPI_->transfer (reg);
      bbSPI_->transfer (data);
      }
    }
  else 
    {
    SPI.transfer (reg);
    SPI.transfer (data);
    }
  }  // end of sendByte

void MAX7219::sendToAll (const byte reg, const byte data)
  {    
  digitalWrite (load_, LOW);
  for (byte chip = 0; chip < chips_; chip++)
    sendByte (reg, data);
  digitalWrite (load_, HIGH); 
  }  // end of sendToAll

// send one character (data) to position (pos) with or without decimal place
// pos is 0 to 7
void MAX7219::sendChar (const byte pos, const char data, const bool dp)
  {
  byte converted = 0b0000001;    // hyphen as default

  // look up bit pattern if possible
  if (data >= ' ' && data <= 'z')
    converted = pgm_read_byte (&MAX7219_font [data - ' ']);
  // 'or' in the decimal point if required
  if (dp)
    converted |= 0b10000000;

  // start sending
  digitalWrite (load_, LOW);
  
  // segment is in range 1 to 8
  const byte segment = 8 - (pos % 8);
  // for each daisy-chained display we need an extra NOP
  const byte nopCount = pos / 8;
  // send extra NOPs to push the data out to extra displays
  for (byte i = 0; i < nopCount; i++)
    sendByte (MAX7219_REG_NOOP, MAX7219_REG_NOOP);
  // send the segment number and data
  sendByte (segment, converted);
  // end with enough NOPs so later chips don't update
  for (int i = 0; i < chips_ - nopCount - 1; i++)
    sendByte (MAX7219_REG_NOOP, MAX7219_REG_NOOP);

  // all done!
  digitalWrite (load_, HIGH); 
  }  // end of sendChar

// write an entire null-terminated string to the LEDs
void MAX7219::sendString (const char * s)
{
  byte pos;
  
  for (pos = 0; pos < (chips_ * 8) && *s; pos++)
    {
    boolean dp = s [1] == '.';
    sendChar (pos, *s++, dp);   // turn decimal place on if next char is a dot
    if (dp)  // skip dot
      s++;
    }
    
  // space out rest
  while (pos < (chips_ * 8))
    sendChar (pos++, ' ');


}  // end of sendString
