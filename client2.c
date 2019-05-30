#include "header_files.h"

void get_user_id (user * user_id);
void get_loc (fire *shot);
void place_ships (int my_board[][10], int);
void mark_placement_on_board (int [][10], place);
void display_board (int a_board[10][10]);
void display_stupid_board (int a_board[10][10]);
void update_board (int board[][10], fire_status);
void update_opponent_board (int board[][10], fire_status);

int sd;

int main()
{
	struct sockaddr_in servAdd;
	int i,j;
	char ip_addr[12+3];
	printf ("Enter the IP address of the server: ");
	scanf ("%s", ip_addr);

	servAdd.sin_addr.s_addr = inet_addr (ip_addr);
	servAdd.sin_port = htons (5910);
	servAdd.sin_family = AF_INET;
	sd = socket (AF_INET, SOCK_STREAM, 0);
	if ((connect (sd, (struct sockaddr *) &servAdd, sizeof (servAdd)))<0)
	{
		perror("Connection failed");
	}
	user my_id, opponent;
	turn user_turn;
	get_user_id (&my_id);
	if ((send (sd, &my_id, sizeof (my_id), 0))<0)
	{
		perror("Send failed");
	}

	int recv_len = recv (sd, &opponent, sizeof (opponent), 0);
	if (recv_len<=0)
	{
		perror("opponent Receive failed");
	}
	printf ("The opponent is %s\n", opponent.name);

	char received[30];

	//Placements
	printf ("Waiting for turn to place ships.\n");
	recv_len = recv (sd, &user_turn, sizeof (user_turn), 0);
	if ( strcmp (user_turn.user_id.name, my_id.name) != 0)
		printf ("Not my name %s \n", user_turn.user_id.name);
	if (recv_len<=0)
	{
		perror("Turn failed");
	}
	int ships = 5, ship_no;
	for (i=0;i<10;i++)
		for (j=0;j<10;j++)
		{
			my_id.board[i][j]=EMPTY;
			opponent.board[i][j]=EMPTY;
		}

	if (strcmp (user_turn.user_id.name, my_id.name) == 0)
	{
		place_ships (my_id.board, ships);
		//printf ("Waiting for other player to put their ships.\n"); 
	}
	printf ("In main:\n");
	//display_stupid_board (my_id.board);
	printf ("Waiting for turn to fire \n");

	//Firings
	do
	{
		//memset (&user_turn, 0, sizeof(user_turn));
		recv_len = recv (sd, &user_turn, sizeof(user_turn), 0);
		printf ("The turn is %s's \n", user_turn.user_id.name);
		fire_status f_status;
		if (strcmp ("shivani", my_id.name) == 0)
		{
			fire shot;
			fire_status f_status;
			get_loc (&shot);
			send (sd, &shot, sizeof (shot), 0);
			recv_len = recv (sd, &f_status, sizeof (f_status), 0);
			printf ("%s\n", f_status.result);
			update_opponent_board (opponent.board, f_status);
			printf ("Opponent's board now\n");
			display_board (opponent.board);
		}
		else
		{
			recv_len = recv (sd, &f_status, sizeof(f_status), 0);
			printf ("The opponent fired at %d,%d and it was a %s \n", f_status.coord.x+1, f_status.coord.y+1, f_status.result);
			update_board (my_id.board, f_status);
			printf ("My board now \n");
			display_board (my_id.board);
		}
	}while (1);
	//printf ("Sent \n");
	close (sd);
}

void get_loc (fire *shot)
{
	printf ("Enter the row and column of postion to fire at: ");
	scanf ("%d %d", &(shot->coord).x, &(shot->coord).y);
	(shot->coord).x --;
	(shot->coord).y --;
}	

void get_user_id (user * user_id)
{
	printf("Enter Player Name: ");
	scanf ("%s", user_id->name);
}

void place_ships (int my_board[][10], int ships)
{
	int ship_no;
	for (ship_no = 0; ship_no < ships; ship_no++)
	{
		place ship;
		ship.size = ship_no+1;
		printf ("Enter the row and column of ship size %d: ", ship.size);
		scanf ("%d", &ship.coord.x);
		scanf ("%d", &ship.coord.y);
		int temp_orient;
		printf ("1. Horizontal \n2. Vertical \nEnter orientation of ship %d: ", ship_no+1);
		scanf ("%d", &temp_orient);
		ship.orientation = temp_orient;
		ship.coord.x --;
		ship.coord.y --;
		printf ("%d %d %d \n", ship.coord.x, ship.coord.y, ship.orientation);

		send (sd, &ship, sizeof (ship), 0);
		char received[30];
		int recv_len = recv (sd, received, sizeof(received), 0);
		if (strcmp(received, "OK") != 0)
		{
			printf ("Invalid Placement: %s", received);
			ship_no--;
		}
		else 
			mark_placement_on_board (my_board, ship);
	}
	printf ("In place_ships:\n");
	//display_stupid_board (my_board);
}

void mark_placement_on_board (int my_board[10][10], place ship)
{
	int x = ship.coord.x, y = ship.coord.y;
	if (ship.orientation == HORIZONTAL)
	{
		for (; y<ship.size+ship.coord.y; y++)
			my_board[x][y] = OCCUPIED[(ship.size)-1];
	}
	else
	{
		for (; x<ship.size+ship.coord.x; x++)
			my_board[x][y] = OCCUPIED[(ship.size)-1];
	}
}

void display_stupid_board (int to_display[10][10])
{
	int i, j;
	printf ("Stupid Board: \n");
	for (i=0; i<10; i++)
	{
		for (j=0; j<10; j++)
			printf ("%d ", to_display[i][j]);
		printf ("\n");
	}
}

void display_board (int to_display[10][10])
{   
	int i, j;
	//display_stupid_board (to_display);
	int ship_displayed [6] = {0};
	printf ("\t");
	for (i=0; i<10; i++)
		printf (" %d", i+1);
	printf ("\n");
	for (i=0; i<14; i++)
		printf ("__");
	printf ("_\n");

	for (i=0; i<10; i++)
	{
		printf ("%d\t|", i+1);
		for (j=0; j<10; j++)
		{
			char symbol = to_display[i][j];
			if (symbol == EMPTY)
				symbol = ' ';
			else if (symbol == SHOT_MISS)
				symbol = 'X';
			else if (symbol == SHOT || symbol == SUNK)
				symbol = '@';
			else
			{
				ship_displayed[symbol] ++;
				if (symbol == ship_displayed[symbol] || ship_displayed[symbol] == 1)
					symbol = '0';
				else symbol = 'o';
			}
			printf ("%c|", symbol);
		}
		printf ("\n");
	}
	for (i=0; i<14; i++)
		printf ("__");
	printf ("_\n");
}

void update_board (int board [][10], fire_status f_status)
{
	int x = f_status.coord.x, y = f_status.coord.y;
	if (strcmp(f_status.result, "MISS") == 0)
		board[x][y] = SHOT_MISS;
	else if (strcmp(f_status.result, "HIT") == 0)
		board[x][y] = SHOT;
	else if (strcmp(f_status.result, "WIN") == 0 || strcmp(f_status.result, "LOSE") == 0)
	{
		if (strcmp (f_status.result, "WIN") == 0)
			printf ("Congratulations on winning. \nThe complete opponent board is \n");
		else printf ("The opponent has won. Better luck next time. \nHere is the complete opponent board: \n");
		int oppo_board[10][10];
		recv (sd, oppo_board, sizeof (oppo_board), 0);
		display_board (oppo_board);
		printf ("Game over. Thank you for playing");
		exit (0);
	}
	else if (strcmp (f_status.result, "SINK") == 0)
	{
		printf ("Your ship of size %d has been sunk\n", board [x][y]);
		board[x][y] = SHOT;
	}
	else
		printf ("It was an invalid shot. Trying again \n");
}

void update_opponent_board (int board[][10], fire_status f_status)
{
	int x = f_status.coord.x, y = f_status.coord.y;
	if (strcmp (f_status.result, "INVALID") == 0)
		printf ("It was an invalid shot. Fire again \n");
	else if (strcmp (f_status.result, "MISS") == 0)
		board [x][y] = SHOT_MISS;
	else if (strcmp (f_status.result, "HIT") == 0)
		board [x][y] = SHOT;
	else if (strcmp (f_status.result, "SUNK") == 0)
	{
		printf ("You sunk ship of size %d \n", board [x][y]);
		board[x][y] = SHOT;
	}
	else if (strcmp(f_status.result, "WIN") == 0 || strcmp(f_status.result, "LOSE") == 0)
	{
		if (strcmp (f_status.result, "WIN") == 0)
			printf ("Congratulations on winning. \nThe complete opponent board is \n");
		else printf ("The opponent has won. Better luck next time. \nHere is the complete opponent board: \n");
		int oppo_board[10][10];
		recv (sd, oppo_board, sizeof (oppo_board), 0);
		display_board (oppo_board);
		printf ("Game over. Thank you for playing");
		exit(0);
	}
}
