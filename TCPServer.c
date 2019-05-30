//Dont know what
#include "header_files.h"

void rotate_turn (turn *next_turn, user user_id1, user user_id2);

int main()
{
	struct sockaddr_in servAdd, clntAdd;

	char ip_addr[12+3];
	printf ("Enter the IP address of the server: ");
	scanf ("%s", ip_addr);

	servAdd.sin_addr.s_addr = inet_addr (ip_addr);
	servAdd.sin_port = htons (5910);
	servAdd.sin_family = AF_INET;
	char buf [257];
	int ls = socket (AF_INET, SOCK_STREAM, 0);
	bind (ls, (struct sockaddr *) &servAdd, sizeof (servAdd));
	listen (ls, 16);
	int clntAddLen = sizeof (clntAdd);
	while(1)
	{
		int sd1 = accept (ls, (struct sockaddr *) &clntAdd, &clntAddLen);
		printf ("Received first %d", sd1);
	int sd2 = accept (ls, (struct sockaddr *) &clntAdd, &clntAddLen);
	printf ("Game can start %d %d", sd1, sd2);
	close (sd1);
	close (sd2);
	continue;
	user user_id1, user_id2;
	turn next_turn;
	next_turn.user_id = user_id1;
	send (sd1, &next_turn, sizeof(next_turn), 0);
	int n = recv (sd1, &user_id1, sizeof (user_id1), 0);
	printf ("%s\n", user_id1.name);
	next_turn.user_id = user_id1;
	send (sd1, &next_turn, sizeof(next_turn), 0);

	//	if (buf == "QUIT")
	//		break;
	//}
	/*n = recv (sd2, buf, 256, 0);
	buf [n] = '\0';
	printf ("%s \n", buf);*/
	int ships = 5, ship_no;
	for (ship_no = 0; ship_no < ships; ship_no)
	{
		place ship;
		char *to_send = malloc (30);
		n = recv (sd1, &ship, sizeof(ship), 0);
		printf ("%d %d", ship.coord.x, ship.coord.y);
		strcpy (to_send, "OK");
		send (sd1, to_send, strlen(to_send), 0);
	}
	send (sd1, &next_turn, sizeof (next_turn), 0);
	close (sd1);
	close (sd2);
	}
}

void rotate_turn (turn *next_turn, user user_id1, user user_id2)
{
	if (strcmp(next_turn->user_id.name, user_id1.name) == 0)
		next_turn->user_id = user_id2;
	else
		next_turn->user_id = user_id1;
}