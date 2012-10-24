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
      
      movePainterTo (MAP_HEIGHT+1,1);
      write (fd, "Lives: ",7);

      movePainterTo (MAP_HEIGHT+1,10);
      write (fd, "Status: ",8);

      movePainterTo (MAP_HEIGHT+1, 44);
      write (fd, "Score: ",7);
                  
      close (fd);
    }
}



#define WIDTH_OF_EACH_OBJECT 2

static void write_to_display_buffer (int mapPosition, char *graphic)
{
  int row = mapPosition / MAP_WIDTH;
  int col = mapPosition % MAP_WIDTH;

  movePainterTo (row+1,WIDTH_OF_EACH_OBJECT*col+1); // +1 is just a hack, try to know why +1 was required.

  write (fd, graphic, WIDTH_OF_EACH_OBJECT);
}



extern int remaining_lives;
extern int game_paused;
extern int monster_killer_time;
extern int score;

static int previous_remaining_lives = 0;
static int previous_pause = false;
static int previous_killer_type = 0;
static int previous_score = 0;

#define ASCII_SCORE_LENGTH 5

static void updateStatistics (void)
{
  if (previous_remaining_lives != remaining_lives)
    {
      movePainterTo (MAP_HEIGHT+1,8);
      char ascii_lives = remaining_lives + NUM_ASCII_OFFSET;
      write (fd, &ascii_lives,1);
      previous_remaining_lives = remaining_lives;
    }
  
  if (game_paused != previous_pause)
    {
      movePainterTo (MAP_HEIGHT+1,18);
      
      if (game_paused)
	{
	  write (fd, "P",1);
	}
      else
	{
	  write (fd, " ",1);
	}
      
      previous_pause = game_paused ;
    }

  if (monster_killer_time != previous_killer_type)
    {
      movePainterTo (MAP_HEIGHT+1,19);
      
      if (monster_killer_time)
	{
	  write (fd, "K",1);
	}
      else
	{
	  write (fd, " ",1);
	}
    
      previous_killer_type = monster_killer_time;    
    }

  if (previous_score != score)
    {
      movePainterTo (MAP_HEIGHT+1, 51);
      char ascii_score[ ASCII_SCORE_LENGTH ];

      previous_score = score;
      for (int digit = 0; digit < ASCII_SCORE_LENGTH ; digit++)
	{	 
	  ascii_score[ ASCII_SCORE_LENGTH -(digit+1)] = previous_score % 10 + NUM_ASCII_OFFSET;
	  previous_score /= 10;
	}

      write ( fd, ascii_score, ASCII_SCORE_LENGTH );
      previous_score = score;
    }

}




#define NUM_MONSTERS 5

extern int player_next_position;
extern int monster_next_position[NUM_MONSTERS];

static int player_present_position = 12*MAP_WIDTH;
static int monster_present_position[NUM_MONSTERS] = {12*MAP_WIDTH};

void updateDisplay (void)
{
  int fd = open (SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (fd == -1)
    printf ("could not open port\n");
  else
    {

      if ( player_present_position != player_next_position )
	{
	  write_to_display_buffer (player_present_position,"  ");
	  write_to_display_buffer (player_next_position,"C ");
	  player_present_position = player_next_position;
	}

	
      for (int monster_index = 0; monster_index < NUM_MONSTERS; monster_index++)
	{
	  if ( monster_present_position[monster_index] != monster_next_position[monster_index] )
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
	}

      updateStatistics ();
      movePainterTo (1,1);      
      close (fd);
    }
}
