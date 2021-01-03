#define APP1 1 //Sender-Receiver
#define APP2 2 //Rock, Paper, Scissors
#define APP3 3 //Telemetry Demo
#define SELECTED_DEMO APP1

extern int sender_receiver();
extern int rock_paper_scissors();
extern int telemetry();
using namespace std;

int main()
{
#if SELECTED_DEMO == APP1
	sender_receiver();
#elif SELECTED_DEMO == APP2
	rock_paper_scissors();
#elif SELECTED_DEMO == APP3
	telemetry();
#else
	printf("NO DEMO APP SELECTED");
#endif // SELECTED_APP == APP1

	return 0;
}