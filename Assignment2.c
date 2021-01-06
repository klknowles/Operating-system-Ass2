#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdbool.h>
#include <string.h>
#define MAXCHARACTER 1000
#define MAXROW 100
FILE *fp;
FILE *fp2;
int matrixArr[MAXROW][4];
int fdN[2];
int northbuses = 0;
int southbuses = 0;
int westbuses = 0;
int eastbuses = 0;
int fdS[2];
int fdW[2];
int fdE[2];
sem_t junctionsem;
sem_t matrixsem;
sem_t northsem;
pid_t prevnorth = 0;
pid_t currentnorth = 0;
sem_t westsem;
pid_t prevwest = 0;
pid_t currentwest = 0;
sem_t eastsem;
pid_t preveast = 0;
pid_t currenteast = 0;
sem_t southsem;
pid_t prevsouth = 0;
pid_t currentsouth = 0;
char arr[MAXCHARACTER];
char instring[8];
size_t len;
int pid;
void busFunc(int busnumber, char dir, int position)
{
	if(dir == 'N')
    {
        printf("North Bus process %d has been created \n", getpid()); //Prints the East bus process number
        printf("%d \n", busnumber); // Prints the number of the current bus
        bool J = true; //Creates a boolean value J that is initialized as true
        while(J)
        {
            //The Matrices are then initialized to 0
            matrixArr[position][0] = 0;
            matrixArr[position][1] = 0;

            if(sem_wait(&matrixsem) == 0)
            {
                fp2 = fopen("matrix.txt","w+");
                for(int i = 0; i < len; i++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                        fscanf(fp2, "%1d", &matrixArr[j][i]);
                    }
                }
                matrixArr[position][0] = 1;
                fp2 = fopen("matrix.txt","w+");
                for(int i = 0; i < len; i++)
                {
                    for(int j = 0; j < 4; j++)
                    {
                       fprintf(fp2,"%d", matrixArr[i][j]);
                    }
                    fprintf(fp2, "\n");
                }
                fclose(fp2);
                sem_post(&matrixsem);
            }
            printf("North semaphore is being requested by process %d", getpid());
            printf("\n");
            if(sem_wait(&northsem) == 0) //If we are successful in decrementing northsem
            {
                if(sem_wait(&matrixsem) == 0)
                {
                   fp2 = fopen("matrix.txt","w+");
                   for(int i = 0; i < len; i++)
                   {
                       for(int j = 0; j < 4; j++)
                       {
                           fscanf(fp2, "%1d", &matrixArr[j][i]);
                       }
                   }
                   matrixArr[position][0] = 2;
                   fp2 = fopen("matrix.txt","w+");
                   for(int i = 0; i < len; i++)
                   {
                       for(int j = 0; j < 4; j++)
                       {
                          fprintf(fp2,"%d", matrixArr[i][j]);
                       }
                       fprintf(fp2, "\n");
                   }
                   fclose(fp2);
                   sem_post(&matrixsem);
                }

                printf("North semaphore has been acquired by process %d \n", getpid());
                printf("West semaphore is being requested by process %d \n", getpid());
                if(sem_wait(&matrixsem) == 0)
                {
                    fp2 = fopen("matrix.txt","w+");
                    for(int i = 0; i < len; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            fscanf(fp2, "%1d", &matrixArr[j][i]);
                        }
                    }
                    matrixArr[position][1] = 1;
                    for(int i = 0; i < len; i++)
                    {
                        for(int j = 0; j < 4; j++)
                        {
                            fprintf(fp2,"%d", matrixArr[i][j]);
                        }
                        fprintf(fp2, "\n");
                    }
                    fclose(fp2);
                    sem_post(&matrixsem);

                }
                if(sem_wait(&westsem) ==0)
                {
                    if(sem_wait(&matrixsem) == 0)
                    {
                       fp2 = fopen("matrix.txt","w+");
                       for(int i = 0; i < len; i++)
                       {
                            for(int j = 0; j < 4; j++)
                            {
                                fscanf(fp2, "%1d", &matrixArr[j][i]);
                            }
                       }
                       matrixArr[position][1] = 2;
                       fp2 = fopen("matrix.txt","w+");
                       for(int i = 0; i < len; i++)
                       {
                          for(int j = 0; j < 4; j++)
                          {
                                fprintf(fp2,"%d",matrixArr[i][j]);
                          }
                          fprintf(fp2, "\n");
                        }
                        fclose(fp2);
                        sem_post(&matrixsem);

                    }

                    printf("West semaphore has been acquired by process %d \n", getpid());
                    printf("Junction semaphore is being requested by process %d \n", getpid());
                    if((sem_wait(&junctionsem)) == 0)
                    {
                        sleep(2); //The junction is then told to wait for two seconds
                        sem_post(&westsem);//westsem is incremented
                        sem_post(&southsem);//southsem is incremented
                        sem_post(&junctionsem);//junctionsem is incremented
                        printf("North, West and Junction semaphores have been released by process %d \n", getpid());
                        J = false; //Changes the value of the boolean variable to false to end the loop
                        if(busnumber > 1)
                        {
                            busnumber -= 1;
                            position += 1;
                            if(fork() == 0)
                            {
                               busFunc(busnumber, dir, position);
                            }
                            else
                            {
                                wait(NULL);
                            }
                        }
                        exit(0);

                    }
                }
                else //If we are not successful in decrementing westsem
                {
                    printf("West semaphore couldn't be acquired by process %d so West semaphore is being released.\n", getpid());
                    sem_post(&northsem); //northsem is incremented
                }
            }
            else //If we are not successful in decrementing northsem
            {
                printf("North semaphore couldnt be acquired by process %d \n", getpid());
            }
        }



	}
	else if(dir == 'W')
    {
        printf("West Bus process %d has been created \n", getpid()); //Prints the West bus process number
        printf("%d \n", busnumber); // Prints the number of the current bus
        bool J = true; //Creates a boolean value J that is initialized as true
        while(J)
        {
            //The Matrices are then initialized to 0
            matrixArr[1][1] = 0;
            matrixArr[1][2] = 0;
            printf("West semaphore is being requested by process %d", getpid());
            printf("\n");
            matrixArr[1][1] = 1; //Begins a process of requesting the westsem (West semaphore)
            if(sem_wait(&westsem) == 0) //If we are successful in obtaining the westsem
            {
					printf("West semaphore has been acquired by process %d \n", getpid());
					matrixArr[1][1] = 2; // The westsem has acquired
					printf("South semaphore is being requested by process %d", getpid());
                    printf("\n");
					matrixArr[1][2] = 1; //Begins a process of requesting the southsem (South semaphore)
                    if(sem_wait(&southsem) ==0) //If we are successful in obtaining the southsem
                    {
						printf("South semaphore has been acquired by process %d \n", getpid());
						matrixArr[1][2] = 2; //The northsem has acquired
						printf("Junction semaphore is being requested by process %d", getpid());
                        printf("\n");
                        if((sem_wait(&junctionsem)) == 0) //If we are successful in obtaining the junctionsem (Junction Semaphore)
                        {
                            sleep(2); //The process is then told to wait for two seconds
                            sem_post(&westsem); //westsem is incremented
                            sem_post(&southsem); //southsem is incremented
                            sem_post(&junctionsem); //junctionsem is incremented
							printf("West, South and Junction semaphores have been released by process %d \n", getpid());
                            J = false; //Changes the value of the boolean variable to false to end the loop
							if(busnumber > 1)
							{
								busnumber -= 1;
								position -= 1;
                                if(fork() == 0) //Creates a child process
                                {
                                    busFunc(busnumber, dir, position); //The next bus process is created and called all over again
                                }
                                else //terminates the child processes
                                {
                                    wait(NULL);
                                }
                            }
							exit(0); //terminates the junctionsem

                        }
                    }
                    else //If we are not successful in decrementing the southsem
                    {
                        printf("South semaphore couldn't be acquired by process %d so West semaphore is being released.\n", getpid());
                        sem_post(&westsem); //westsem is incremented
                    }
            }
            else
            {
                printf("West semaphore couldnt be acquired by process %d \n", getpid());
            }

        }
	}
	else if(dir == 'S')
    {
			printf("South Bus process %d has been created \n", getpid()); //Prints the South bus process number
			printf("%d \n", busnumber);// Prints the number of the current bus
            bool J = true; //Creates a boolean value J that is initialized as true
            while(J)
            {
                //The Matrices are then initialized to 0
				matrixArr[1][2] = 0;
				matrixArr[1][3] = 0;
				printf("South semaphore is being requested by process %d", getpid());
                printf("\n");
				matrixArr[1][2] = 1; //Begins a process requesting the southsem (South semaphore)
                if(sem_wait(&southsem) == 0) //If we are successful in obtaining southsem
                {
					printf("South semaphore has been acquired by process %d \n", getpid());
					matrixArr[1][2] = 2; //The process of obtaining the eastsem was successful
					printf("East semaphore is being requested by process %d", getpid());
                    printf("\n");
					matrixArr[1][3] = 1; //Begins the process requesting the eastsem (East semaphore)
                    if(sem_wait(&eastsem) ==0)//If we are successful in obtaining the eastsem then
                    {
						printf("East semaphore has been acquired by process %d \n", getpid());
						matrixArr[1][3] = 2;//The process of obtaining the eastsem was successful
						printf("Junction semaphore is being requested by process %d", getpid());
                        printf("\n");
                        if((sem_wait(&junctionsem)) == 0) //If we are successful in requesting the junctionsem (Junction Semaphore)
                        {
							printf("Junction semaphore has been acquired by process %d \n", getpid());
                            sleep(2); //The process is then told to wait for two seconds
                            sem_post(&southsem); //southsem is incremented
                            sem_post(&eastsem); //eastsem is incremented
                            sem_post(&junctionsem); //junctionsem is incremented
							printf("South, East and Junction semaphores have been released by process %d \n", getpid());
                            J = false; //Changes the value of the boolean variable J to false to end the loop
							if(busnumber > 1)
							{
								busnumber -= 1;
								position -= 1;
                                if(fork() == 0) //Creates a child process
                                {
                                    busFunc(busnumber, dir, position);//The next bus process is created and called all over again
                                }
                                else
                                {
                                    wait(NULL); //parent waits on the child process
                                }
                            }

							exit(0);//the process is then terminated
                        }
                    }
                    else
                    {
                        printf("East semaphore couldn't be acquired by process %d so South semaphore is being released.\n", getpid());
						sem_post(&southsem);//southsem is incremented
                    }
              }
              else
              {
                    printf("South semaphore couldn't be acquired by process %d \n", getpid());
              }
            }
	}
	else if(dir == 'E')
    {
			printf("East Bus process %d has been created \n", getpid()); //Prints the East bus process number
			printf("%d \n", busnumber); // Prints the number of the current bus
            bool J = true; //Creates a boolean value J that is initialized as true
            while(J)
            {
				matrixArr[1][3] = 0; //The Matrices are then initialized to 0
				matrixArr[1][0] = 0;
				printf("East semaphore is being requested by process %d", getpid());
				printf("\n");
				matrixArr[1][3] = 1; //Begins a process of requesting the eastsem (East semaphore)
                if(sem_wait(&eastsem) == 0) //If we are successful in obtaining the eastsem
                {
					printf("East semaphore has been acquired by process %d \n", getpid());
					matrixArr[1][3] = 2; // The process of obtaining the eastsem was successful
					printf("North semaphore is being requested by process %d", getpid());
                    printf("\n");
					matrixArr[1][0] = 1; //Begins a process of requesting the northsem (North semaphore)
                    if(sem_wait(&northsem) ==0) //If we are successful in obtaining the northsem
                    {
						printf("North semaphore has been acquired by process %d \n", getpid());
						matrixArr[1][0] = 2; // The process of obtaining the northsem was successful
						printf("Junction semaphore is being requested by process %d", getpid()); //
                        printf("\n");
                        if((sem_wait(&junctionsem)) == 0) //If we are successful in requesting the junctionsem (Junction Semaphore)
                        {
							printf("Junction semaphore has been acquired by process %d \n", getpid());
                            sleep(2); //The process is then told to wait for two seconds
                            sem_post(&eastsem); //eastsem is incremented
                            sem_post(&northsem); //northsem is incremented
                            sem_post(&junctionsem); // junctionsem is incremented
							printf("East, South and Junction semaphores have been released by process %d \n", getpid());
                            J = false; //Changes the value of the boolean variable J to false to end the loop
							if(busnumber > 1)
                            {
								busnumber -= 1;
								position -= 1;
								if(fork() == 0)//Creates a child process
								{
									busFunc(busnumber, dir, position); //The next bus process is created and called all over again
								}
								else
								{
									wait(NULL); //parent waits on the child process
								}
							}
							exit(0); // the process is then terminated
                        }
                    }
                    else
                        {
                            printf("North semaphore couldn't be acquired by process %d so East semaphore is being released.\n", getpid());
                            sem_post(&eastsem); //eastsem is incremented
                        }
                }
                else
                {
                    printf("East semaphore couldn't be acquired by process %d \n", getpid());
                }
            }
	}
}
void managerFunc(float p)
{
	len = strlen(instring);
	int i = 0;
	bool val = true;
	char north = 'N';
	char south = 'S';
	char west = 'W';
	char east = 'E';
	while(val)
    {
		float fr = (float)rand()/RAND_MAX;
		fr = (int)(fr * 10 + .5);
		fr = (float)fr/10;
		if(p > fr)
		{
			val = false;
            if(fork() == 0)
            {
				if(northbuses > 0)
				{
					int position = 0;
					busFunc(northbuses, north, position);
				}
				else
				{
					exit(0);
				}
			}
			else if(fork() == 0)
			{
				if(southbuses > 0)
				{
					int position = northbuses;
					busFunc(southbuses, south, position);
				}
				else
				{
					exit(0);
				}
			}
			else if(fork() == 0) //
			{
				if(eastbuses > 0)
				{
					int position = northbuses + southbuses;
					busFunc(eastbuses, east, position);
				}
				else
				{
					exit(0);
				}

			}
			else if(fork() == 0)
            {
				if(westbuses > 0)
				{
					int position = northbuses + southbuses + eastbuses;
					busFunc(westbuses, west, position);
				}
				else
				{
					exit(0);
				}
			}
        }
	}
}
void main()
{
    pipe(fdN);
    pipe(fdW);
    pipe(fdS);
    pipe(fdE);
    float p;
    fp = fopen("sequence.txt", "r"); //opens and reads text file sequence
    fp2 = fopen("matrix.txt","w+"); //opens for reading and writing the text file matrix

    //Semaphores are initialized so that they can be shared with other processes
    sem_init(&northsem, 1, 1);
    sem_init(&southsem, 1, 1);
    sem_init(&westsem, 1, 1);
    sem_init(&eastsem, 1, 1);
    sem_init(&junctionsem, 1, 1);
    sem_init(&matrixsem, 1, 1);
    printf("Please enter a p probability number between 0.2 to 0.7. ");
    scanf("%f", &p);
    printf("\n");
    fscanf(fp, "%s", instring);
    for(int i = 0; i<strlen(instring); i++)
    {
    	if(instring[i] == 'N')
    	{
		   northbuses++;
	    }
	    else if(instring[i] == 'S')
	    {
		   southbuses++;
	    }
	    else if(instring[i] == 'E')
	    {
		   eastbuses++;
	    }
	    else if(instring[i] == 'W')
	    {
		   westbuses++;
	    }
    }
    for(int i = 0; i<strlen(instring); i++)
    {
    	for(int j = 0; j<4; j++)
    	{
		   matrixArr[i][j] = 0;
	    }
    }
    for(int i = 0; i<strlen(instring); i++)
    {
    	for(int j = 0; j<4; j++)
    	{
		   fprintf(fp2,"%d",matrixArr[i][j]);
	    }
	    fprintf(fp2, "\n");
    }
    fclose(fp);
    fclose(fp2);
    managerFunc(p);
}

// Deadlock Detection

/*
east bus holds E sem and requests for N sem,
north bus holds N sem and request W sem,
west bus holds W sem and requests S sem
south bus holds S sem and requests E sem".

This condition when represented in terms of values will be
east bus--> 2(E), 1(N),
north bus-->2(N), 1(W),
west bus-->2(W), 1(S),
south bus-->2(S), 1(E)
where buses are rows and the sems in parantheses are columns.
To detect deadlock, all you have to do is check if this pattern is available in your matrix or not.
If available, declare a deadlock otherwise not.

*/

