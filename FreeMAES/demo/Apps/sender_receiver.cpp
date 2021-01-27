/*Ejemplo de demostración MAES número 2: Competencia de Papel Piedra o Tijera*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "maes-rtos.h"

/* Demo includes. */
#include "supporting_functions.h"

using namespace MAES;

Agent sender("Agent Sender", 1, 1024);
Agent receiver("Agent Receiver", 3, 1024);

void write(void* pvParameters);
void read(void* pvParameters);

class writingBehaviour :public CyclicBehaviour {
public:
	void setup() {
		msg.add_receiver(receiver.AID());
	}
	void action() {
		printf("Enviando mensaje... \n");
		msg.send();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
};

void write(void* pvParameters) {
	writingBehaviour b;

	b.execute();

}

class readingBehaviour :public CyclicBehaviour {
public:
	void action() {
		printf("Esperando... \n");
		msg.receive(portMAX_DELAY);
		printf("Mensaje recibido: ¡Hola MAES! \n");
	}
};

void read(void* pvParameters) {
	readingBehaviour b;

	b.execute();
}

Agent_Platform AP("windows");


int sender_receiver() {

	printf("MAES DEMO \n");
	AP.agent_init(&sender, write);
	printf("Mensajero Listo \n");
	AP.agent_init(&receiver, read);
	printf("Receptor Listo \n");
	AP.boot();
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}

/*-----------------------------------------------------------*/

