#include "Constants.h"

typedef enum st { HIT, MISS, SINK, WIN, INVALID, LOSE } status;
typedef enum ori { HORIZONTAL=1, VERTICAL } orient;

typedef struct
{
	int x, y;
} coordinate;

typedef struct
{
	coordinate coord;
	int size; 
    orient orientation;
} place;

typedef struct
{
	coordinate coord;
} fire;

typedef struct
{
	char result[20];
	coordinate coord;
} fire_status;

typedef struct
{
	char name[30];
    int socket_id;
    int board[10][10];
    int ships_sunk;
} user;

typedef struct
{
	user user_id;
    char message[30];
} turn;
