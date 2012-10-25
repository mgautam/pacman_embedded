#include "inputEncoder.h"

#define ESC 'q'
#define SPACE 32
#define LEFT_KEY 'D'
#define RIGHT_KEY 'C'
#define UP_KEY 'A'
#define DOWN_KEY 'B'

char serial2nibble (char serialIn)
{
  if ( serialIn == ESC )
    {
      return QUIT;
    }
  else if ( serialIn == SPACE )
    {
      return PAUSE;
    }
  else if ( serialIn == LEFT_KEY )
    {
      return LEFT;
    }
  else if ( serialIn == RIGHT_KEY )
    {
      return RIGHT;
    }
  else if ( serialIn == UP_KEY )
    {
      return UP;
    }
  else if ( serialIn == DOWN_KEY )
    {
      return DOWN;
    }
  else
    {
      return -1;
    }

}
