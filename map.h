#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 28
#define MAP_HEIGHT 30

// Horizontal Wall, Vertical Wall, Void, Food, Power
/*
#define HW 0
#define VW 1
#define VO 2
#define FD 3
#define PW 4
*/

#define HWAL 0
#define VOID 1
#define FOOD 2
#define POWR 3
#define VWAL 4

extern char map[MAP_WIDTH*MAP_HEIGHT];

#define TOTAL_FOOD 240

#endif
