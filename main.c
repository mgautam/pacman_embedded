#include "config.h"
#include "inputEncoder.h"
#include "GameRules.h"
#include "mover.h"
#include "display.h"

//#include "testinputEncoder.h"
//#include "testGameRules.h"
//#include "testplayer.h"
//#include "testdisplay.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main (int argc, char **argv)
{
  displayMap ();
  updateDisplay ();

  int read_file = open (SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  fcntl (read_file, F_SETFL, 0);
  
  if (read_file == -1)
    printf ("could not open port\n");
  else
    {
      while (1)
	{
	  //serialIn
	  
	  char input;//= getchar ();// interrupt driven serial input in 8051
	  char encodedByte;
	  
	  read (read_file, &input, 1);
	  if (input == 27)
	    {
	      read (read_file, &input, 1);
	      if (input == 91)
		{
		  read (read_file, &input, 1);
		  encodedByte = serial2nibble (input);
		}
	    }
	  else	  
	    encodedByte = serial2nibble (input);

	  GameEngine (encodedByte);
	  updateDisplay ();
	  
	}
      
      close (read_file);
    }

  return 0;
}
