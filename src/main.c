#include <tm1638.h>


#define FUNC_STOPWATCH 5
#define FUNC_TIMER 7
#define FUNC_COUNTER 6


void init();
unsigned char show_menu();
void start_counter();
void start_timer();
void start_stopwatch();
void delay(unsigned int ms);


void main()
{
	unsigned char func;

	init();

	while(1) {
		func = show_menu();

		if (func == FUNC_TIMER) start_timer();
		else if (func == FUNC_COUNTER) start_counter();
		else if (func == FUNC_STOPWATCH) start_stopwatch();
	}
}

void delay(unsigned int ms) {
	unsigned int n;
	while (ms--) {
		n = 100;
		while (n--);
	}
}


void init() {
	tm1638_display_on(3);
	tm1638_clear_all();
}

unsigned char show_menu() {
	unsigned char key;

	tm1638_clear_all();
	tm1638_show_text(2, "SELECT");
	key = tm1638_wait_for_keypress();

	return key;
}

void start_counter() {
	tm1638_clear_all();
	tm1638_show_digit(0, FUNC_COUNTER, 1);
	tm1638_show_text(1, "COUNTER");
	tm1638_set_led(FUNC_COUNTER, 1);

	delay(2000);
}

void start_timer() {
	tm1638_clear_all();
	tm1638_show_digit(0, FUNC_TIMER, 1);
	tm1638_show_text(3, "TIMER");
	tm1638_set_led(FUNC_TIMER, 1);

	delay(2000);
}

void start_stopwatch() {
	tm1638_clear_all();
	tm1638_show_digit(0, FUNC_STOPWATCH, 1);
	tm1638_show_text(4, "STOP");
	tm1638_set_led(FUNC_STOPWATCH, 1);

	delay(2000);
}