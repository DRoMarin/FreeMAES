/*Ejemplo de demostración MAES número 2: Competencia de Papel Piedra o Tijera*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "maes-rtos.h"

/* Demo includes. */
#include "supporting_functions.h"
#include <random>

using namespace MAES;

/*Agentes*/
Agent A("Player A", 1, 1024);
Agent B("Player B", 1, 1024);
Agent Referee("Referee", 2, 1024);

/*Declaración de funciones de tarea*/
void play(void* pvParameters);
void watchover(void* pvParameters);

/*plataforma*/
Agent_Platform AP_RPS("windows");

/*función aleatoria*/
int getRandom() {
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution dist(0, 2);
	return dist(mt);
}

/*función msg to int: papel, piedra o tijeras*/
int choices(char* msg) {
	if (msg == "ROCK")
	{
		return 0;
	}
	else if (msg == "PAPER")
	{
		return 1;
	}
	else
	{
		return 2; //scissors
	}
}

/*Comportamiento de jugadores*/
class playingBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(Referee.AID());
	}
	void action() {
		printf("Rock, Paper, Scissors... \n");
		AP_RPS.agent_wait(pdMS_TO_TICKS(10));
		auto num = getRandom();
		char* bet;
		switch (num)
		{
		case 0:
			bet = "ROCK";
			break;

		case 1:
			bet = "PAPER";
			break;

		case 2:
			bet = "SCISSORS";
			break;

		default:
			break;
		}
		msg.set_msg_content(bet);
		msg.set_msg_type(INFORM);
		msg.send();
	}
};

/*Definición función de tarea de jugador*/
void play(void* pvParameters) {
	playingBehaviour b;
	for (;;)
	{
		b.execute();
	}
}

/*Comportamiento del arbitro*/
class watchoverBehaviour :public OneShotBehaviour {
public:
	void setup() {
		msg.add_receiver(A.AID());
		msg.add_receiver(B.AID());
	}
	void action() {
		char* msgA;
		char* msgB;
		int choiceA;
		int choiceB;
		int winner[3][3] = {
			{0, 2, 1},
			{1, 0, 2},
			{2, 1, 0}
		};
		while (true)
		{
			msg.receive(portMAX_DELAY);
			if (msg.get_msg_type() == INFORM)
			{
				//printf("Playing now: ");
				printf(AP_RPS.get_Agent_description(msg.get_sender()).agent_name);
				printf(": ");
				printf(msg.get_msg_content());
				printf("\n");
				if (msg.get_sender() == A.AID())
				{
					msgA = msg.get_msg_content();
					choiceA = choices(msgA);
				}
				else if (msg.get_sender() == B.AID())
				{
					msgB = msg.get_msg_content();
					choiceB = choices(msgB);
				}
				msg.suspend(msg.get_sender());
			}
			if (AP_RPS.get_state(A.AID()) == SUSPENDED && AP_RPS.get_state(B.AID()) == SUSPENDED) {
				break;
			}
		}

		switch (winner[choiceA][choiceB])
		{
		case 0:
			printf("DRAW!\n");
			break;

		case 1:
			printf("PLAYER A WINS!\n");
			break;

		case 2:
			printf("PLAYER B WINS!\n");
			break;

		default:
			break;
		}
		AP_RPS.agent_wait(pdMS_TO_TICKS(2000));
		msg.resume(A.AID());
		msg.resume(B.AID());
		printf("-------------------PLAYING AGAIN---------------------\n");
	}
};

/*Definición función de arbitro*/
void watchover(void* pvParameters) {
	watchoverBehaviour b;
	for (;;)
	{
		b.execute();
	}
}

/*----------------------------------MAIN----------------------------------*/
int rock_paper_scissors() {
	printf("MAES DEMO \n");
	AP_RPS.agent_init(&A, play);
	AP_RPS.agent_init(&B, play);
	AP_RPS.agent_init(&Referee, watchover);
	AP_RPS.boot();
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}
/*------------------------------------------------------------------------*/