// Demo of MAX7219 library
// Author: Nick Gammon
// Date: 17 March 2015

#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219.h>

// bit banged SPI on pins 6, 7, 8
MAX7219 display (6, 7, 8, true);  // LOAD, DIN, CLK

void setup ()
  {
  display.begin ();
  display.sendString ("HI THERE");
  delay (2000);
  }  // end of setup

// scrolling display of all available characters
void loop ()
  {
  char buf [9];
  for (char i = '0'; i < 'z'; i++)
    {
    for (byte j = 0; j < 8; j++)
      buf [j] = i + j;
    buf [8] = 0;
    display.sendString (buf);
    delay (1000);
    }    // end of for loop
  }  // end of loop
  
