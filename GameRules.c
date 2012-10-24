#include "inputEncoder.h"
#include "map.h"
#include "mover.h"
#include "GameRules.h"

#include <stdio.h>
#include <stdlib.h>

static int state = 1;
static int randomDirectionFinder (void)
{
  state = (state*2*state+1) % 1115;
  return state % 4;
}



#define false 0
#define true 1

static int remaining_lives = 3;
static int remaining_food = TOTAL_FOOD;

#define NUM_MONSTERS 5
#define PLAYER_BIRTH_POSITION 17*MAP_WIDTH+MAP_WIDTH/2
#define MONSTER_BIRTH_CENTER 15*MAP_WIDTH+MAP_WIDTH/2
#define POINT_PER_MONSTER_KILLED 200


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
int monster_killer_time = false;
extern int score;
int score = 0;

static void runGame ( char inputCommand )
{
  int player_present_position = player_next_position;// previous next position has present position
  player_next_position = findNextPosition (player_present_position, inputCommand);
  printf ("Player Present:%d Next:%d\n", player_present_position, player_next_position);

  
  int monster_player_touch = false;
  
  for (int monster_index = 0; monster_index < NUM_MONSTERS; monster_index++)
	{
	  int monster_present_position = monster_next_position[monster_index]; // previous next position has present position
	  monster_next_position[monster_index] = findNextPosition ( monster_present_position, randomDirectionFinder () );
	  // Random Direction Finder can be called only when it hits a corner or wall
	  // Monsters run over each other but then the separate. Measures can be taken so they don't overlap at any instant.

	  printf ("Monster%d Present:%d Next:%d\n", monster_index, monster_present_position, monster_next_position[monster_index]);

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



static int game_paused = false;

void GameEngine (char inputCommand)
{
  printf ("REmaining lives: %d\n", remaining_lives);
  if ( game_paused )
    {
      if (inputCommand == PAUSE)
	// Here PAUSE means RESUME
	{
	  game_paused = false;
	  printf ("GAME RESUMED\n");
	}
      else
	{
	  return;
	}
    }
  else if (inputCommand == PAUSE)
    {
      game_paused = true;
      printf ("GAME PAUSED\n");
      return;
    }
  else if ( remaining_food == 0 )
    {
      printf ("GAME COMPLETED\n");
      exit (0);
    }
  else if ( inputCommand == QUIT || remaining_lives <= 0 )
    {
      printf ("GAME QUIT\n");
      exit (0);
    }
  else
    {
      runGame (inputCommand);
    }
}

