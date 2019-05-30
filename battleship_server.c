#include "header_files.h"

int is_valid_placement (int a_board[][10], place ship);
void display_board (int[][10] );
void mark_placement_on_board (int (*this_board)[10], place ship);
int is_valid_shot (int this_board[][10],fire shot);
int is_position_occupied (int this_board[][10], fire shot);
int is_ship_sunk (int this_board[][10], fire shot);
void set_ship_sunk (int this_board [][10], fire shot);

int main(int argc, char* argv[])
{
	printf("Running battleship server\n");

	int i, j, k;
	int loserflag=-1;
	char buffer[30];

	/*  Retrieving  command line arguments */

	if (argc<3)
	{
		printf("Not enough command line arguments");
		fflush(stdout);
		exit(0);
	}
	user user_id[2];
	user_id[0].socket_id=argv[0][0];
	user_id[1].socket_id=argv[0][1];
	strcpy(user_id[0].name,argv[1]);
	strcpy(user_id[1].name,argv[2]);
	printf("socket %d socket %d username: %s username: %s\n",user_id[0].socket_id,user_id[1].socket_id,user_id[0].name,user_id[1].name);
	fflush(stdout);

	/* Initialising player boards */

	for (i=0; i<10; i++)
		for (j=0; j<10; j++)
			for (k=0;k<2;k++)
				user_id[k].board[i][j]=EMPTY;
    user_id[0].ships_sunk=0;
    user_id[1].ships_sunk=1;

	turn this_turn;

	/* Ship Placement */

	for (i=0;i<2;i++)
	{
		this_turn.user_id=user_id[i];
		strcpy (this_turn.message, placement_wait);
		send (user_id[i].socket_id,(turn *) &this_turn, sizeof(turn), 0);
		int ship_no, ships=5;
		place placement[5];

		for (ship_no = 0; ship_no < ships; ship_no++)
		{
			place ship;
			recv (user_id[i].socket_id, &ship, sizeof(ship), 0);
			printf ("%d %d %d \n", ship.coord.x, ship.coord.y, ship.orientation);
			if (!is_valid_placement(user_id[i].board,ship))
			{
				strcpy(buffer, invalid_placement);
				send(user_id[i].socket_id, buffer, strlen (buffer)+1, 0);
				ship_no--;
			}
			else
			{
				placement [ship_no] = ship;
				strcpy (buffer, status_ok);
				send (user_id[i].socket_id, buffer, strlen(status_ok)+1, 0);
				mark_placement_on_board(user_id[i].board, ship);
			}
		}
		printf ("Player %d placed ships \n",i+1);
		display_board (user_id[i].board);
	}

	/* Firing */

	fire shot;
	fire_status f_status;
	int turn_flag=0;

	while (1)
	{
		strcpy (this_turn.user_id.name, user_id[turn_flag].name);
		strcpy(this_turn.message,fire_wait);
		printf ("%s's turn\n", this_turn.user_id.name);
		send(user_id[turn_flag].socket_id, &this_turn, sizeof(turn), 0);
		send(user_id[!turn_flag].socket_id, &this_turn, sizeof(turn), 0);
		recv(user_id[turn_flag].socket_id, &shot,sizeof(shot), 0);
		if (!is_valid_shot (user_id[!turn_flag].board, shot))
		{
			strcpy (f_status.result, status_str[INVALID]);
			f_status.coord=shot.coord;
			send(user_id[turn_flag].socket_id,&f_status,sizeof(fire_status),0);
			continue;
		}
		else
		{
			if (is_position_occupied(user_id[!turn_flag].board,shot))
			{
				if (is_ship_sunk(user_id[!turn_flag].board,shot))
				{
					set_ship_sunk(user_id[!turn_flag].board, shot);
					printf ("Set ship as sunk successfully \n");
					user_id[turn_flag].ships_sunk++;
					if (user_id[turn_flag].ships_sunk>=5)
					{
						loserflag=(!turn_flag);
						break;
					}
					strcpy (f_status.result, status_str[SINK]);
				}
				else
				{
					user_id[!turn_flag].board[shot.coord.x][shot.coord.y]=SHOT;
					strcpy (f_status.result, status_str[HIT]);
				}
				f_status.coord=shot.coord;
				send(user_id[turn_flag].socket_id, &f_status,sizeof(fire_status),0);
				send(user_id[!turn_flag].socket_id, &f_status,sizeof(fire_status),0);
			}
			else if (user_id[!turn_flag].board[shot.coord.x][shot.coord.y]==EMPTY)
			{
				user_id[!turn_flag].board[shot.coord.x][shot.coord.y]=SHOT_MISS;
				strcpy (f_status.result, status_str[MISS]);
				f_status.coord=shot.coord;
				send(user_id[turn_flag].socket_id,&f_status,sizeof(fire_status),0);
				send(user_id[!turn_flag].socket_id,&f_status,sizeof(fire_status),0);
				turn_flag=!turn_flag;
			}
            printf("%s's board:\n",user_id[turn_flag].name);
			display_board (user_id[turn_flag].board);
            printf("%s's board:\n",user_id[!turn_flag].name);
            display_board(user_id[!turn_flag].board);
		}
	}
	if (loserflag>=0)
	{
		strcpy(f_status.result,status_str[LOSE]);
		send(user_id[loserflag].socket_id,&f_status,sizeof(fire_status),0);
		send(user_id[loserflag].socket_id,&(user_id[!loserflag].board),sizeof(user_id[!loserflag].board),0);
		strcpy(f_status.result,status_str[WIN]);
		send(user_id[!loserflag].socket_id,&f_status,sizeof(fire_status),0);
		send(user_id[!loserflag].socket_id,&(user_id[loserflag].board),sizeof(user_id[loserflag].board),0);
		printf("\n%s won the game",user_id[!loserflag].name);
	}

	printf("Game over. Battleship server terminating\n");
    return 0;
}

int is_clash_on_board (place ship,int a_board[10][10])
{
	int x,y;
	x=ship.coord.x;
	y=ship.coord.y;
	if (ship.orientation==HORIZONTAL)
	{
		for (;y<ship.size+(ship.coord.y);y++)
			if (a_board[x][y]!=EMPTY)return 1;
	}
	else
	{
		for (;x<ship.size+(ship.coord.x);x++)
			if (a_board[x][y]!=EMPTY)return 1;
	}
	return 0;
}

int is_valid_placement (int a_board[10][10],place ship)
{
	if (ship.coord.x>=10 || ship.coord.y>=10)return 0;
	if (ship.coord.x<0 || ship.coord.y<0)return 0;
    if (ship.orientation!=VERTICAL && ship.orientation!=HORIZONTAL)return 0;
	if (ship.orientation==VERTICAL && ship.coord.x+ship.size-1>=10)return 0;
	if (ship.orientation==HORIZONTAL && ship.coord.y+ship.size-1>=10)return 0;
	if (is_clash_on_board(ship,a_board))return 0;
	return 1;
}

void mark_placement_on_board (int (*a_board)[10], place ship)
{
	int x=ship.coord.x,y=ship.coord.y;
	if (ship.orientation==HORIZONTAL)
	{
		for (;y<ship.size+ship.coord.y;y++)
			a_board[x][y]=OCCUPIED[(ship.size)-1];
	}
	else
	{
		for (;x<ship.size+ship.coord.x;x++)
			a_board[x][y]=OCCUPIED[(ship.size)-1];
	}
}

int is_valid_shot(int this_board[10][10],fire shot)
{
	if (shot.coord.x>=10 || shot.coord.y>=10)
		return 0;
	if (shot.coord.x<0 || shot.coord.y<0)
		return 0;
	if (this_board[shot.coord.x][shot.coord.y] == SHOT || this_board[shot.coord.x][shot.coord.y]== SUNK || this_board[shot.coord.x][shot.coord.y] == SHOT_MISS)
		return 0;
	return 1;
}

void display_board (int to_display[10][10])
{
	int i, j;
	printf ("Board: \n");
	for (i=0; i<10; i++)
	{
		for (j=0; j<10; j++)
			printf ("%d ", to_display[i][j]);
		printf ("\n");
	}
}

int is_position_occupied (int this_board[10][10], fire shot)
{
	int i;
	for (i=0;i<5;i++)
	{
		if (this_board[shot.coord.x][shot.coord.y]==OCCUPIED[i])
			return 1;
	}
	return 0;
}

int is_ship_sunk (int this_board[][10], fire shot)
{
	int i;
	for (i=0;i<10;i++)
	{
		if (i!=shot.coord.x)
		{
			if (this_board[i][shot.coord.y]==this_board[shot.coord.x][shot.coord.y])
				return 0;
		}
		if (i!=shot.coord.y)
		{
			if (this_board[shot.coord.x][i]==this_board[shot.coord.x][shot.coord.y])
				return 0;
		}
	}
	return 1;
}

void set_ship_sunk(int this_board [][10], fire shot)
{
	int i;
	for (i=0;i<10;i++)
	{
		if (i!=shot.coord.x)
		{
			if (this_board[i][shot.coord.y]==this_board[shot.coord.x][shot.coord.y])
				this_board[i][shot.coord.y]=SUNK;

		}
		if (i!=shot.coord.y)
		{
			if (this_board[shot.coord.x][i]==this_board[shot.coord.x][shot.coord.y])
				this_board[shot.coord.x][i]=SUNK;
		}
	}
    this_board[shot.coord.x][shot.coord.y]=SUNK;
}
