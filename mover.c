#include "inputEncoder.h"
#include "map.h"
#include "mover.h"

#include <stdio.h>

int findNextPosition (int present_position, char direction)
{
  if ( present_position == 14*MAP_WIDTH && direction == LEFT )
    {// if on the left end in the 15th row
      return 15*MAP_WIDTH - 1; // Enter the right side
    }
  else if ( present_position == 15*MAP_WIDTH-1 && direction == RIGHT )
    {// if on the right end in the 15th row
      return 14*MAP_WIDTH; // Enter the left side
    }
  else if ( direction == LEFT && map[present_position - 1] != VWAL ) 
    {
      return present_position - 1;
    }
  else if ( direction == RIGHT && map[present_position + 1] != VWAL )
    {
      return present_position + 1;
    }
  else if ( direction == UP && map[present_position - MAP_WIDTH] != HWAL
	    && map[present_position - MAP_WIDTH] != VWAL)
    {
      return present_position - MAP_WIDTH ;      
    }
  else if ( direction == DOWN && map[present_position + MAP_WIDTH] != HWAL 
	    && map[present_position + MAP_WIDTH] != VWAL )
    {
      return present_position + MAP_WIDTH ;      
    }
    else
      {
	return present_position; // don't move
      }
  
}

