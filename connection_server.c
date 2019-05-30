#include "header_files.h"

char *my_itoa(int num, char *str)
{
	if(str == NULL)
	{
		return NULL;
	}
	sprintf(str, "%d", num);
	return str;
}
int main()
{
	struct sockaddr_in servAdd, clntAdd;

	char ip_addr[12+3];
	printf ("Enter the IP address of the server: ");
	scanf ("%s", ip_addr);

	pid_t pid;
	servAdd.sin_addr.s_addr = inet_addr (ip_addr);
	servAdd.sin_port = htons (5910);
	servAdd.sin_family = AF_INET;
	char buf [257];
	char name1[20];
	char name2[20];
	char message[]="You have been paired to";
	char message1[]="You have been paired to";
	int ls = socket (AF_INET, SOCK_STREAM, 0);
	if	(bind (ls, (struct sockaddr *) &servAdd, sizeof (servAdd)) < 0 )
		perror ("Bind Failed");
	listen (ls, 16);
	int clntAddLen = sizeof (clntAdd);
	while(1)

	{
		int sd1 = accept (ls, (struct sockaddr *) &clntAdd, &clntAddLen);
		int sd2 = accept (ls, (struct sockaddr *) &clntAdd, &clntAddLen);
		turn this_turn;
		user user_id1, user_id2;
		recv (sd1, &user_id1, sizeof (user_id1), 0);
		printf ("Player 1: %s\n", user_id1.name);
		recv (sd2, &user_id2, sizeof (user_id2), 0);
		printf ("Player 2: %s\n", user_id2.name);
		send (sd1, &user_id2, sizeof (user_id2), 0);
        printf ("One send done");
        fflush(stdout);
		send (sd2, &user_id1, sizeof (user_id1), 0);
        printf("Attempting fork");
        fflush(stdout);
        sleep(1);
		if ((pid = fork()) < 0) 
		{
			printf("forking child process failed\n");
            fflush(stdout);
			exit(1);
		}
		else if (pid == 0)
		{
            printf("Fork succesful. In child %d",getpid());
            fflush(stdout);
            close(ls);
			const char *progname = "./server_battle"; 
            char args[3];
            args[0]=sd1;
            args[1]=sd2;
            args[2]='\0';
            printf("Socket %d Socket %d",sd1,sd2);
            fflush(stdout);
			strcpy (name1, user_id1.name);
			strcpy (name2, user_id2.name);
            printf("%s %s %s %s",progname,args,name1,name2);
            fflush(stdout);
			if (execl(progname, args, name1, name2, (char *)NULL) < 0)
			{
				perror("Exec failed");
				exit(1);
			}
            else
            {
                printf("This is else");
                fflush(stdout);
            }
			/*printf ("Calling battleship_server\n");
			battleship_server (sd1, sd2, user_id1, user_id2);*/
		}
		close(sd1);
		close(sd2);
	}
}
