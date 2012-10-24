#include "config.h"
#include "inputEncoder.h"
#include "map.h"
#include "mover.h"
#include "GameRules.h"

#include <stdlib.h>

static int state = 1;
static int randomDirectionFinder (void)
{
  state = (state*2*state+1) % 7919;// 1000th Prime Number
  return state % 4;
}



extern int remaining_lives;
int remaining_lives = 3;
static int remaining_food = TOTAL_FOOD;

#define NUM_MONSTERS 5
#define PLAYER_BIRTH_POSITION 17*MAP_WIDTH+MAP_WIDTH/2
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
extern int monster_killer_time;
int monster_killer_time = false;
extern int score;
int score = 0;
char monster_move[NUM_MONSTERS] = {0};

static void runGame ( char inputCommand )
{
  int player_present_position = player_next_position;// previous next position has present position
  player_next_position = findNextPosition (player_present_position, inputCommand);


  
  int monster_player_touch = false;
  
  for (int monster_index = 0; monster_index < NUM_MONSTERS; monster_index++)
	{
	  int monster_present_position = monster_next_position[monster_index]; // previous next position has present position

	  monster_next_position[monster_index] = findNextPosition ( monster_present_position, monster_move[monster_index] );
	  if (monster_next_position[monster_index] == monster_present_position)
	  // Random Direction Finder is called only when it hits a corner or wall
	    {
	      monster_move[monster_index] = randomDirectionFinder ();
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
	  // Player ate Power tablet, now it's monster killer time.
	  monster_killer_time = true;
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

