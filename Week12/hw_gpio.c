#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wiringPi.h>
#include <pthread.h>
#include <semaphore.h>


#define LED_R 3
#define LED_Y 2
#define LED_G 0
#define SW_R 6
#define SW_Y 5
#define SW_G 4
#define SW_W 27

int correct_answer[5];
int my_answer[5];

sem_t semRED,semYELLOW,semGREEN;
pthread_t thread[3];

int n = 0; // index of player's inputs
int turn = 1; // the round of game

void init(void);
void off(void);
void start(void);

void blink(void); // R-Y-G
void fail(void); // RYG

void createNewAnwer(void);
int check_answer(void);

// log the inputs
void switchRED(void);
void switchYELLOW(void);
void switchGREEN(void);

int main(void)
{
    sem_init(&semRED, 0, 0);
    sem_init(&semYELLOW, 0, 0);
    sem_init(&semGREEN, 0, 0);
   
    init();
    start();

    while(turn <= 5){ // game is 5 stages


		if(digitalRead(SW_R) == 0) // input RED
		{
			sem_post(&semRED); // store input value
			sem_wait(&semRED);
		}
		else if(digitalRead(SW_Y) == 0)
		{
			sem_post(&semYELLOW);
			sem_wait(&semYELLOW);
		}
		else if(digitalRead(SW_G) == 0)
		{
			sem_post(&semGREEN);
			sem_wait(&semGREEN);
		}
		else if(digitalRead(SW_W) == 0)
		{
		    	// check_answer() == 0 : input value is wrong
			// n != turn : number of player inputs are wrong
			if(check_answer()==0 || n != turn)
			{
				fail(); // turn on the RYG
				exit(1); // exit the program
			}

			blink(); // turn on R-Y-G
			turn++; // go to next stage
			if(turn == 6) exit(1); // if we ha
			n = 0;
			
			delay(250);
			createNewAnwer(); // crate new answer
			start(); // start the next game
		}

		off(); // turn off the lights
    }
    sem_destroy(&semRED);
    sem_destroy(&semYELLOW);
    sem_destroy(&semGREEN);

    return 0;
}

// settings before starting game
void init(void)
{
	if(wiringPiSetup() == -1)
	{
		exit(1);
	}

	// set switches INPUT mode
	pinMode(SW_R, INPUT);
	pinMode(SW_Y, INPUT);
	pinMode(SW_G, INPUT);
	pinMode(SW_W, INPUT);
	// set LEDs OUTPUT mode
	pinMode(LED_R, OUTPUT);
	pinMode(LED_Y, OUTPUT);
	pinMode(LED_G, OUTPUT);
	
	// turn off the light
	off();

	// create new answers
	createNewAnwer();

	// create new thread
	pthread_create(&thread[0], NULL, (void*)&switchRED, NULL);
	pthread_create(&thread[1], NULL, (void*)&switchYELLOW, NULL);
	pthread_create(&thread[2], NULL, (void*)&switchGREEN, NULL);
	
	// turn on R-Y-G three times
	blink();

	delay(250);
}

// turn off the lights
void off(void)
{
	digitalWrite(LED_R, 0);
	digitalWrite(LED_Y, 0);
	digitalWrite(LED_G, 0);
}

// start the game
void start(void)
{
	for(int i = 0; i < 5; i++)
	{
	    	// turn on the correct answers
    		if(correct_answer[i] == 1)
		{
	    		digitalWrite(LED_R, 1);
	    		delay(250);
			digitalWrite(LED_R,0);
		}
		else if(correct_answer[i] == 2)
		{
			digitalWrite(LED_Y, 1);
			delay(250);
			digitalWrite(LED_Y, 0);
		}
		else if(correct_answer[i] == 3)
		{
			digitalWrite(LED_G, 1);
			delay(250);
			digitalWrite(LED_G, 0);
		}
		delay(250);
    }
}

// turn on R-Y-G three times
void blink(void)
{
	int i = 0;
	while(i < 3)
	{
		digitalWrite(LED_R, 1);
		delay(250);
		digitalWrite(LED_R, 0);

		digitalWrite(LED_Y, 1);
		delay(250);
		digitalWrite(LED_Y, 0);

		digitalWrite(LED_G, 1);
		delay(250);
		digitalWrite(LED_G, 0);

		i++;
	}
}

// turn on RYG three times
void fail(void)
{
    int i = 0;
	while(i < 3)
	{
    	digitalWrite(LED_R,1);
		digitalWrite(LED_Y,1);
		digitalWrite(LED_G,1);
		delay(250);
		digitalWrite(LED_R,0);
		digitalWrite(LED_Y,0);
		digitalWrite(LED_G,0);
		delay(250);

		i++;
	}
}


// create new answers
void createNewAnwer(void)
{
	for (int i = 0; i < turn; i++)
	{
		correct_answer[i] = (rand() % 3) + 1;
	}
}

// check the answer
int check_answer(void)
{
	for (int i = 0; i < turn; i++)
	{
		if (my_answer[i] != correct_answer[i]) // if the answer is wrong
			return 0;
	}
	return 1;
}

// switch~~ : log the player's inputs 
void switchRED(void)
{
	while (turn <= 5) {
		sem_wait(&semRED);
		my_answer[n] = 1;
		n++;
		digitalWrite(LED_R, 1);
		delay(250);
		digitalWrite(LED_R, 0);
		sem_post(&semRED);
		delay(250);
	}
}

void switchYELLOW(void)
{
	while (turn <= 5) {
		sem_wait(&semYELLOW);
		my_answer[n] = 2;
		n++;
		digitalWrite(LED_Y, 1);
		delay(250);
		digitalWrite(LED_Y, 0);
		sem_post(&semYELLOW);
		delay(250);
	}
}

void switchGREEN(void)
{
	while (turn <= 5) {
		sem_wait(&semGREEN);
		my_answer[n] = 3;
		n++;
		digitalWrite(LED_G, 1);
		delay(250);
		digitalWrite(LED_G, 0);
		sem_post(&semGREEN);
		delay(250);

	}
}
