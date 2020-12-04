#define APP1 1
#define APP2 2
#define APP3 3
#define SELECTED_DEMO APP2

extern int sender_receiver();
extern int rock_paper_scissors();

using namespace std;

int main()
{
#if SELECTED_DEMO == APP1
	sender_receiver();
#elif SELECTED_DEMO == APP2
	rock_paper_scissors();
#elif SELECTED_DEMO == APP3
	sender_receiver();
#else
	printf("NO DEMO APP SELECTED");
#endif // SELECTED_APP == APP1

	return 0;
}