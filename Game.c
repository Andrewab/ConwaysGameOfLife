#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdatomic.h>
//#include <sys/sysinfo.h>
//structs
struct thread_args {
	int size;
	int life_condition_low_end;
	int life_condition_high_end;
};
//prototypes
void start_game(int speed, int size, int life_condition_low_end, int life_condition_high_end);
void update_game(int size,int life_condition_low_end,int life_condition_high_end, int current_iteration);
void *thread_check(void* args);
void print_game(int size, int current_iteration);
int check_life(int width, int height, int size);
//Globals
int **Game;
int **PlaceHolder;
atomic_int counter = 0;
int MAX_NUM_THREADS = 6;//Causese bus errors on my computer if too many threads
/*
within the main, the game initializes itself and
collects user input to store for the creation of the game.
4 vars are collected: 
speed: speed of game(in seconds)
size: dimension of game
life_condiition_low_end: the lower threshold of a point being alive
life_condition_high_end: the upper threshold of a point being alive
*/
int main(int argc, char *argv[])
{	
	int speed;
	int size;
	int life_condition_low_end;
	int life_condition_high_end;
	printf("Welcome to Conways Game of Life\n");
	printf("Enter update speed of game(Seconds): ");
	scanf("%d", &speed);
	if (speed < 1)
	{
		printf("Error: Invalid speed\n");
		exit(0);
	}
	printf("Enter size of simulation: ");
	scanf("%d", &size);
	if (size == 0)
	{
		printf("Error: Invalid size\n");
		exit(0);
	}
	printf("Enter valid life condition (1 - 8) low end: ");
	scanf("%d", &life_condition_low_end);
	printf("Enter valid life condition(1 - 8) high end(Must be greater than low end): ");
	scanf("%d",&life_condition_high_end);
	if (life_condition_low_end > 0 && life_condition_low_end < 9)
	{
		if(life_condition_high_end > 0 && life_condition_high_end < 9 && life_condition_high_end > life_condition_low_end)
		{
		start_game(speed,size,life_condition_low_end,life_condition_high_end);
		}
	}
	else{
		printf("Error: invalid life_condition\n");
		exit(0);
	}
	return 0;
}
/*
start_game() creates a placeholder game
that new values get placed into before
setting Game equal to that tmp_game
each point is randomly assigned an either
on or off value.
After initial Game is created, runs
update_game() and sleeps between each 
update for the speed num seconds
*/
void start_game(int speed, int size, int life_condition_low_end, int life_condition_high_end){
	int **tmp_game;
	tmp_game = malloc(size * sizeof(int*));
	for (int i = 0; i < size; ++i)
	{
		tmp_game[i] = malloc(size * sizeof(int*));
	}
	PlaceHolder = tmp_game;
	time_t t;
	srand((unsigned) time(&t));
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			tmp_game[i][j] = (rand()%2);
		}
	}
	Game = tmp_game;
	int current_iteration = 0;
	while(1){
	update_game(size,life_condition_low_end,life_condition_high_end, current_iteration);
	current_iteration++;
	sleep(speed);
	}
	//print_game(speed,size,life_condition);
	//printf("SPEED: %d\nSIZE: %d\nLIFE_CONDITION: %d",speed,size,life_condition);
}
/*
update_game() creates num threads up to
MAX_NUM_THREADS, based upon the size of 
game.
Will run once and update the game board once
*/
void update_game(int size,int life_condition_low_end,int life_condition_high_end, int current_iteration) {
	print_game(size,current_iteration);
	int num_threads;
	if(size < MAX_NUM_THREADS){
		num_threads = size;
	}
	else{
		num_threads = MAX_NUM_THREADS;
	}
	pthread_t thid[num_threads];
	struct thread_args *args = malloc(sizeof(struct thread_args));
	args->size = size;
	args->life_condition_low_end = life_condition_low_end;
	args->life_condition_high_end = life_condition_high_end;
	//printf("HERE\n");
	for (int i = 0; i < num_threads; ++i)
	{
		if(pthread_create(&thid[i],NULL,thread_check,(void*)args) != 0){
			printf("PTHREAD_create error\n");
		}
	}
	for (int i = 0; i < num_threads; ++i)
	{
		pthread_join(thid[i],NULL);
	}
	free(args);
	counter = 0;
	Game = PlaceHolder;
}
/*
thread_check() checks a specific point and sees 
if it should be considered an alive or dead thread 
in the next iteration of the game. After descision
is made, will place new value into global 
PlaceHolder 2d array
*/
void *thread_check(void*_args){
	//printf("Thread created Counter: %d\n", counter);
	struct thread_args *args = (struct thread_args*)_args;
	int size = args->size;
	int life_condition_low_end = args->life_condition_low_end;
	int life_condition_high_end = args->life_condition_high_end;
	while(counter < size*size){
		int point = counter;
		counter++;
		int width = point % size;
		int height = point / size;
		//printf("NUM ALIVE: %d\n",check_life(width,height,size));
		//printf("COORDINATES: %d, %d\n",width,height );
		if (check_life(width,height,size) >= life_condition_low_end && check_life(width,height,size) <= life_condition_high_end)
		{
			//printf("HERE\n");
			PlaceHolder[width][height] = 1;
		}
		else
		{
			PlaceHolder[width][height] = 0;
		}
		//free(args);
	}
	return NULL;
}
/*
check_life() is run by thread_check to see how
many on squares are on adjacent to a specific
square.
It will return the number of alive squares.
*/
int check_life(int width, int height, int size){
	int num_alive = 0;
	int is_left_col_available;
	int is_right_col_available;
	int is_top_row_available;
	int is_bot_row_available;
	if(width - 1 >= 0){
		is_left_col_available = 1;
	}
	else{
		is_left_col_available = 0;
	}
	if(width + 1 < size){
		is_right_col_available = 1;
	}
	else{
		is_right_col_available = 0;
	}
	if (height - 1 >= 0)
	{
		is_top_row_available = 1;
	}
	else{
		is_top_row_available = 0;
	}
	if(height + 1 < size){
		is_bot_row_available = 1;
	}
	else{
		is_bot_row_available = 0;
	}
	//printf("CORDINATES %d, %d\n",width,height );
	//printf("LFT COL: %d\nRHT COL: %d\nTOP ROW: %d\nBOT ROW: %d\n", is_left_col_available,is_right_col_available,is_top_row_available,is_bot_row_available);
	if(is_left_col_available == 1){
		if(Game[height][width - 1] == 1){
			num_alive++;
		}
		if(is_top_row_available == 1){
			if(Game[height - 1][width - 1] == 1){
				num_alive++;
			}
		}
		if(is_bot_row_available == 1){
			if(Game[height + 1][width - 1] == 1){
				num_alive++;
			}
		}
	}
	if (is_right_col_available == 1)
	{
		if(Game[height][width+1] == 1)
		{
			num_alive++;
		}
		if(is_top_row_available == 1){
			if(Game[height - 1][width + 1] == 1){
				num_alive++;
			}
		}
		if(is_bot_row_available == 1){
			if(Game[height + 1][width + 1] == 1){
				num_alive++;
			}
		}
	}
	if(is_top_row_available == 1){
		if(Game[height - 1][width] == 1){
			num_alive++;
		}
	}
	if(is_bot_row_available == 1){
		if(Game[height + 1][width] == 1){
			num_alive++;
		}
	}
	return num_alive;
}
/*
print_game() prints the current gamestate
which is stored in the global var Game
It will print X if the square is on
and it will print 0 if the square is off.
It will also print a seperation line to help
distiguish between different iterations of the
game
*/
void print_game(int size, int current_iteration) {
	printf("CURRENT ITERATION: %d\n", current_iteration);
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			if (Game[i][j] == 1)
			{
				printf("X");
			}
			else
			{
				printf("0");
			}
		}
		printf("\n");
	}
		for (int i = 0; i < size; ++i)
	{
		printf("-");
	}
	printf("\n");
}
