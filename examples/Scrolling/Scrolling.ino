#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219.h>

const byte chips = 1;

// 1 chip, bit banged SPI on pins 6, 7, 8
MAX7219 display (chips, 6, 7, 8);  // Chips / LOAD / DIN / CLK

const char message [] = "Hello there - testing 123456789 ";

void setup ()
  {
  display.begin ();
  }  // end of setup

unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 500;  // mS
unsigned int  messageOffset;

void updateDisplay ()
  {
  const unsigned int messageLength = strlen (message);
  
  // get each byte required
  for (byte i = 0; i < (8 * chips); i++)
    {
    // find the offset in the message array
    unsigned int charOffset = messageOffset + i;
    // if we have passed the end, go back to the start
    if (charOffset >= messageLength)
       charOffset -=  messageLength;
    // send that character
    display.sendChar (i, message [charOffset]);
    }
  // next time show one character on
  if (messageOffset++ >= messageLength)
    messageOffset = 0;
  }  // end of updateDisplay

void loop () 
  { 
    
  // update display if time is up
  if (millis () - lastMoved >= MOVE_INTERVAL)
    {
    updateDisplay ();
    lastMoved = millis ();
    }

  // do other stuff here    
     
  }  // end of loop

