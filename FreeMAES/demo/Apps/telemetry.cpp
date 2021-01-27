/*Ejemplo de demostración MAES número 2: Competencia de Papel Piedra o Tijera*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "maes-rtos.h"

/* Demo includes. */
#include "supporting_functions.h"


using namespace MAES;
#define AGENT_STACK_DEPTH 1024

/*Agentes*/
Agent logger_current("Current Agent", 3, AGENT_STACK_DEPTH);
Agent logger_voltage("Voltage Agent", 2, AGENT_STACK_DEPTH);
Agent logger_temperature("Temperature Agent", 1, AGENT_STACK_DEPTH);
Agent measurement("Gen Agent", 3, AGENT_STACK_DEPTH);

/*plataforma*/
Agent_Platform AP_TELEMETRY("windows");

/*Enums y structs*/
typedef enum meas_type
{
	CURRENT,
	VOLTAGE,
	TEMPERATURE
}meas_type;

typedef struct logger_info
{
	meas_type type;
	UBaseType_t rate;
}logger_info;

logger_info log_current, log_voltage, log_temperature;

/*definición de comportamientos y funciones de tarea*/
class loggerBehaviour : public CyclicBehaviour
{
public:
	logger_info* info;
	void setup()
	{
		info = (logger_info*)taskParameters;
	}
	void action()
	{
		msg.set_msg_content((char*)info->type);
		msg.send(measurement.AID(), portMAX_DELAY);
		msg.receive(portMAX_DELAY);

		/*Logging*/
		printf("%s\n", msg.get_msg_content());
		AP_TELEMETRY.agent_wait(pdMS_TO_TICKS(info->rate));
	}
};

void logger(void* taskParameter)
{
	loggerBehaviour b;
	b.taskParameters = taskParameter;
	b.execute();
}

class genBehaivour : public CyclicBehaviour
{
public:
	portFLOAT min, max, value;
	char response[50];

	void action()
	{
		msg.receive(portMAX_DELAY);
		switch ((int)msg.get_msg_content())
		{
		case CURRENT:
			min = 0.1; //mA
			max = 1000; //mA
			value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
			snprintf(response, 50, "\r\nCurrent mesasurment: %f\r\n", value);
			break;

		case VOLTAGE:
			min = 0.5; //V
			max = 3.3; //V
			value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
			snprintf(response, 50, "\r\nVoltage mesasurment: %f\r\n", value);
			break;

		case TEMPERATURE:
			min = 30; //C
			max = 100; //C
			value = min + rand() / (RAND_MAX / (max - min + 1) + 1);
			snprintf(response, 50, "\r\nTemperature mesasurment: %f\r\n", value);
			break;

		default:
			snprintf(response, 50, "\r\nNot understood");
			break;
		}

		msg.set_msg_content(response);
		msg.send(msg.get_sender(), portMAX_DELAY);
	}
};

void gen_meas(void* taskParameter)
{
	genBehaivour b;
	b.execute();
};

/*Declaración de funciones de tarea*/
void logger(void* taskParameter);
void gen_meas(void* taskParameter);

/*----------------------------------MAIN----------------------------------*/
int telemetry() {
	printf("MAES DEMO \n");
	log_current.rate = 500;
	log_voltage.rate = 1000;
	log_temperature.rate = 2000;
	log_current.type = CURRENT;
	log_voltage.type = VOLTAGE;
	log_temperature.type = TEMPERATURE;

	AP_TELEMETRY.agent_init(&logger_current, logger, (void*)&log_current);
	AP_TELEMETRY.agent_init(&logger_voltage, logger, (void*)&log_voltage);
	AP_TELEMETRY.agent_init(&logger_temperature, logger, (void*)&log_temperature);
	AP_TELEMETRY.agent_init(&measurement,gen_meas);
	AP_TELEMETRY.boot();
	printf("Boot exitoso \n");
	/* Start the scheduler so the created tasks start executing. */
	vTaskStartScheduler();

	for (;;);

	return 0;
}
/*------------------------------------------------------------------------*/