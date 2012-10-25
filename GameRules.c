#include "config.h"
#include "inputEncoder.h"
#include "map.h"
#include "mover.h"
#include "GameRules.h"

#include <stdlib.h>

static int state = 1;
static int uniformRandomNumberGenerator (void)
{
  state = (state*2*state+1) % 7919;// 1000th Prime Number
  return (state * 100) / 7919;// Size of int might give problems on 8051
}



extern int remaining_lives;
int remaining_lives = 3;
static int remaining_food = TOTAL_FOOD;

#define NUM_MONSTERS 5
#define PLAYER_BIRTH_POSITION (MAP_HEIGHT-1)*MAP_WIDTH-2
//17*MAP_WIDTH+MAP_WIDTH/2
#define MONSTER_BIRTH_CENTER 14*MAP_WIDTH+MAP_WIDTH/2
#define POINT_PER_MONSTER_KILLED 20


extern int player_next_position;
int player_next_position = PLAYER_BIRTH_POSITION;
extern int monster_next_position[NUM_MONSTERS];
int monster_birth_position[NUM_MONSTERS] = { MONSTER_BIRTH_CENTER - 2,
					       MONSTER_BIRTH_CENTER - 1, 
					       MONSTER_BIRTH_CENTER,
					       MONSTER_BIRTH_CENTER + 1,
					       MONSTER_BIRTH_CENTER + 2
};
int monster_next_position[NUM_MONSTERS] = { MONSTER_BIRTH_CENTER - 2,
					       MONSTER_BIRTH_CENTER - 1, 
					       MONSTER_BIRTH_CENTER,
					       MONSTER_BIRTH_CENTER + 1,
					       MONSTER_BIRTH_CENTER + 2
};

#define KILLING_TIME_LIMIT 21
extern int monster_killer_time;
int monster_killer_time = false;
extern int score;
int score = 0;
char monster_move[NUM_MONSTERS] = {0};

static int randomDirectionFinder (int monster_index)
{

  int uniformRandomNumber = uniformRandomNumberGenerator ();

  if (uniformRandomNumber % 2 == 0) // 50% probability of complete randomness
    return (uniformRandomNumber/2) % 4;
  else // 50% probability of partial randomness based on player position
    {
      int player_row = player_next_position / MAP_WIDTH;
      int player_col = player_next_position % MAP_WIDTH;
      
      int monster_row =  monster_next_position[monster_index] / MAP_WIDTH;
      int monster_col =  monster_next_position[monster_index] %  MAP_WIDTH;
      
      int priority_distribution[4];
      
      if (monster_row < player_row && monster_col < player_col)
	{
	  priority_distribution[DOWN] = 40;
	  priority_distribution[UP] = 10;
	  priority_distribution[RIGHT] = 40;
	  priority_distribution[LEFT] = 10;
	}
      else if (monster_row < player_row && monster_col >= player_col)
	{
	  priority_distribution[DOWN] = 40;
	  priority_distribution[UP] = 10;
	  priority_distribution[RIGHT] = 10;
	  priority_distribution[LEFT] = 40;
	}
      else if (monster_row >= player_row && monster_col < player_col)
	{
	  priority_distribution[DOWN] = 10;
	  priority_distribution[UP] = 40;
	  priority_distribution[RIGHT] = 40;
	  priority_distribution[LEFT] = 10;
	}
      else // if (monster_row >= player_row && monster_col >= player_col)
	{
	  priority_distribution[DOWN] = 10;
	  priority_distribution[UP] = 40;
	  priority_distribution[RIGHT] = 10;
	  priority_distribution[LEFT] = 40;
	}
      
      
      
      int cumulative_probability = 100;
      int intelligent_direction;
      for (int direction = 3; direction >= 0; direction--)
	{
	  cumulative_probability -= priority_distribution[direction];
	  if (uniformRandomNumber >= cumulative_probability)
	    {
	      intelligent_direction = direction;
	      break;
	    }
	}  
      
      return intelligent_direction;// uniformRandomNumber;//
    }
}


static void runGame ( char inputCommand )
{
  if (monster_killer_time)
    monster_killer_time--;

  int player_present_position = player_next_position;// previous next position has present position
  player_next_position = findNextPosition (player_present_position, inputCommand);


  
  int monster_player_touch = false;
  
  for (int monster_index = 0; monster_index < NUM_MONSTERS; monster_index++)
	{
	  int monster_present_position = monster_next_position[monster_index]; // previous next position has present position

	  monster_next_position[monster_index] = findNextPosition ( monster_present_position, monster_move[monster_index] );
	  if (monster_next_position[monster_index] == monster_present_position
	      || !(map[monster_present_position-1] == VWAL && map[monster_present_position+1] == VWAL
		   || map[monster_present_position-1] == HWAL && map[monster_present_position+1] == HWAL
		   || map[monster_present_position-1] == VWAL && map[monster_present_position+1] == HWAL
		   || map[monster_present_position-1] == HWAL && map[monster_present_position+1] == VWAL
		   || map[monster_present_position-MAP_WIDTH] == HWAL && map[monster_present_position+MAP_WIDTH] == HWAL 
		   || map[monster_present_position-MAP_WIDTH] == HWAL && map[monster_present_position+MAP_WIDTH] == VWAL 
		   || map[monster_present_position-MAP_WIDTH] == VWAL && map[monster_present_position+MAP_WIDTH] == HWAL 
		   || map[monster_present_position-MAP_WIDTH] == VWAL && map[monster_present_position+MAP_WIDTH] == VWAL 
		   )	      
	      )
	  // Random Direction Finder is called only when it hits a corner or wall
	    {
	      monster_move[monster_index] = randomDirectionFinder (monster_index);
	      monster_next_position[monster_index] = findNextPosition ( monster_present_position, monster_move[monster_index] );

	      if (monster_next_position[monster_index] == monster_present_position)
		// if the random direction also hits the wall, choose the next direction
		{
		  monster_move[monster_index] = (monster_move[monster_index] + 1) % 4;// Hope that the directions are within 4
		  monster_next_position[monster_index] = findNextPosition ( monster_present_position, monster_move[monster_index] );
		}
	    }

	  // Monsters run over each other but then the separate. Measures can be taken so they don't overlap at any instant.



	  if (monster_next_position[monster_index] == player_next_position)
	    {
	      monster_player_touch = true;
	      
	      if (monster_killer_time)
		// Player ate Power tablet, we are in monster killer time.
		{
		  score += POINT_PER_MONSTER_KILLED;
		  monster_next_position[monster_index] = monster_birth_position[monster_index];
		}
	      else
		{
		  remaining_lives--;
		  player_next_position = PLAYER_BIRTH_POSITION;
		}	      
	    }
	}
  
  
  if (!monster_player_touch || monster_killer_time)
    {
      if (map[player_next_position] == FOOD)
	{
	  map[player_next_position] = VOID;
	  score++;
	  remaining_food--;
	}
      else if (map[player_next_position] == POWR)
	{	  
	  map[player_next_position] = VOID;
	  remaining_food--;
	  // Player ate Power tablet, now it's monster killer time.
	  monster_killer_time = KILLING_TIME_LIMIT;
	}
    }
}



extern int game_paused;
int game_paused = false;

void GameEngine (char inputCommand)
{
  if ( game_paused )
    {
      if (inputCommand == PAUSE)
	// Here PAUSE means RESUME
	{
	  game_paused = false;
	}
      else
	{
	  return;
	}
    }
  else if (inputCommand == PAUSE)
    {
      game_paused = true;
      return;
    }
  else if ( remaining_food == 0 )
    {
      exit (0);
    }
  else if ( inputCommand == QUIT || remaining_lives <= 0 )
    {
      exit (0);
    }
  else
    {
      runGame (inputCommand);
    }
}

