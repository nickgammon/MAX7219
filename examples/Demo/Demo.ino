// Demo of MAX7219 library
// Author: Nick Gammon
// Date: 17 March 2015

#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219.h>

const byte chips = 1;

// 1 chip, bit banged SPI on pins 6, 7, 8
MAX7219 display (chips, 6, 7, 8);  // Chips / LOAD / DIN / CLK

void setup ()
  {
  display.begin ();
  display.sendString ("HELLO   _-_-_-_-");
  delay (2000);
  display.setIntensity (2);
  display.sendString ("Lo Light");
  delay (2000);
  display.setIntensity (6);
  }  // end of setup

// scrolling display of all available characters
void loop ()
  {
  char buf [(chips * 8) + 1];
  for (char i = '0'; i < 'z'; i++)
    {
    for (byte j = 0; j < (chips * 8); j++)
      buf [j] = i + j;
    buf [chips * 8] = 0;
    display.sendString (buf);
    delay (1000);
    }    // end of for loop
  }  // end of loop

