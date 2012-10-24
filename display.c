#include "config.h"
#include "map.h"
#include "display.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

char carriage_return = 13;
char line_feed = 10;

int fd = -1;

void displayMap (void)
{
  fd = open (SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (fd == -1)
    printf ("could not open port\n");
  else
    {
      for (int row = 0; row < MAP_HEIGHT; row++)
	{
	  for (int col = 0; col < MAP_WIDTH; col++)
	    {
	      if (map[row*MAP_WIDTH+col] == HWAL)
		{
		  write (fd,"--",2);
		}
	      else if (map[row*MAP_WIDTH+col] == VWAL)
		{
		  write (fd,"|",1);
		  if (col != MAP_WIDTH-1 && map[row*MAP_WIDTH+col+1] == HWAL)
		    write (fd,"-",1);
		  else
		    write (fd, " ",1);
		}
	      else if (map[row*MAP_WIDTH+col] == VOID)
		{
		  write (fd,"  ",2);
		}
	      else if (map[row*MAP_WIDTH+col] == FOOD)
		{
		  write (fd,"o ",2);
		}
	      else if (map[row*MAP_WIDTH+col] == POWR)
		{
		  write (fd,"O ",2);
		}
	      else 
		{
		}
	    }
	  write (fd,&carriage_return,2);
	  //write (fd,&line_feed,2);
	}
              
      close (fd);
    }
}

#define NUM_ASCII_OFFSET 48
void movePainterTo (char row, char col)
{
  char ESC = 27;
  write (fd, &ESC, 1);
  write (fd, "[", 1);


  char loByte = (int) row % 10 + NUM_ASCII_OFFSET;
  row /= 10;
  char midByte = (int) row % 10 + NUM_ASCII_OFFSET;
  row /= 10;
  char hiByte = (int) row % 10 + NUM_ASCII_OFFSET;

  write (fd, &hiByte, 1);
  write (fd, &midByte, 1);
  write (fd, &loByte, 1);


  write (fd, ";", 1);


  loByte = (int) col % 10 + NUM_ASCII_OFFSET;
  col /= 10;
  midByte = (int) col % 10 + NUM_ASCII_OFFSET;
  col /= 10;
  hiByte = (int) col % 10 + NUM_ASCII_OFFSET;

  write (fd, &hiByte, 1);
  write (fd, &midByte, 1);
  write (fd, &loByte, 1);


  write (fd, "H", 1);
}


#define WIDTH_OF_EACH_OBJECT 2

static void write_to_display_buffer (int mapPosition, char *graphic)
{
  int row = mapPosition / MAP_WIDTH;
  int col = mapPosition % MAP_WIDTH;

  movePainterTo (row,WIDTH_OF_EACH_OBJECT*col);

  write (fd, graphic, WIDTH_OF_EACH_OBJECT);
}




#define NUM_MONSTERS 5

extern int player_next_position;
extern int monster_next_position[NUM_MONSTERS];

static int player_present_position = 13*MAP_WIDTH;
static int monster_present_position[NUM_MONSTERS] = {13*MAP_WIDTH};

void updateDisplay (void)
{
  int fd = open (SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (fd == -1)
    printf ("could not open port\n");
  else
    {

      write_to_display_buffer (player_present_position,"  ");
      write_to_display_buffer (player_next_position,"C ");
      player_present_position = player_next_position;

	
      for (int monster_index = 0; monster_index < NUM_MONSTERS; monster_index++)
	{

	  if ( map[monster_present_position[monster_index]] == VOID )
	    {
	      write_to_display_buffer (monster_present_position[monster_index], "  ");
	    }
	  else if ( map[monster_present_position[monster_index]] == FOOD )
	    {
	      write_to_display_buffer (monster_present_position[monster_index], "o ");
	    }
	  else if ( map[monster_present_position[monster_index]] == POWR )
	    {
	      write_to_display_buffer (monster_present_position[monster_index], "O ");
	    }

	  write_to_display_buffer (monster_next_position[monster_index], "M ");

	  monster_present_position[monster_index] = monster_next_position[monster_index];
	}
      
      close (fd);
    }
}
