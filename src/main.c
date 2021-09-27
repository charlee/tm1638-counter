#include <tm1638.h>
#include <reg52.h>


#define TIMER_10MS (65536 - 10000)
#define reset_T0() { TH0 = TIMER_10MS / 256; TL0 = TIMER_10MS % 256; }
#define reset_T1() { TH1 = (65536 - 50000) / 256; TL1 = (65536 - 50000) % 256; }

#define FUNC_STOPWATCH 5
#define FUNC_TIMER 7
#define FUNC_COUNTER 6


void init();
unsigned char show_menu();
void func_counter();
void func_timer();
void func_stopwatch();
void show_quit();
void show_reset();
void reset_ticks();
void show_stopwatch(unsigned char is_running);
void show_timer(unsigned char is_running, unsigned long timer_ticks);
void delay(unsigned int ms);


// 10ms ticks
unsigned long ticks10 = 0;

// unsigned int centisecs = 0;
// unsigned char secs = 0;
// unsigned char mins = 0;
// unsigned char hours = 0;
// unsigned char count_down = 0;


void main()
{
	unsigned char func;

	init();

	while(1) {
		func = show_menu();

		if (func == FUNC_TIMER) func_timer();
		else if (func == FUNC_COUNTER) func_counter();
		else if (func == FUNC_STOPWATCH) func_stopwatch();
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

	// set both T0 and T1 to 16bit timer
	TMOD = 0x11;
	
	EA = 1;
	
	// enable timers
	ET0 = 1;
}

unsigned char show_menu() {
	unsigned char key;

	tm1638_clear_all();
	tm1638_show_text(2, "SELECT");

	// show available functions in LEDs
	tm1638_set_led(FUNC_STOPWATCH, 1);
	tm1638_set_led(FUNC_COUNTER, 1);
	tm1638_set_led(FUNC_TIMER, 1);

	key = tm1638_wait_for_keypress();

	return key;
}

void start_timer0() {
	reset_T0();
	TR0 = 1;
}

void stop_timer0() {
	TR0 = 0;
}


void show_reset() {
	tm1638_clear_7seg();
	tm1638_show_text(2, "RESET");
}

/**
 * Reset the global ticks counter.
 */
void reset_ticks() {
	ticks10 = 0;
}


/**
 * Start the counter.
 */
void func_counter() {
	unsigned char key;
	unsigned long counter = 0;
	unsigned char quit = 0;

	tm1638_clear_all();
	tm1638_show_text(3, "COUNT");
	tm1638_set_led(FUNC_COUNTER, 1);

	delay(2000);

	while (!quit) {
		tm1638_clear_7seg();
		tm1638_show_dec(7, 8, counter);
		key = tm1638_wait_for_keypress();
		switch (key) {
			case 7: counter++; break;
			case 6: counter += 10; break;
			case 5: counter += 100; break;
			case 4: if (counter >= 1) { counter--; }break;
			case 3: if (counter >= 10) { counter -= 10; } break;
			case 2: if (counter >= 100) { counter -= 100; } break;
			case 1: {
				show_reset();
				key = tm1638_wait_for_keypress();
				if (key == 1) {
					counter = 0;
				}
				break;
			}
			case 0: {
				show_quit();
				key = tm1638_wait_for_keypress();
				if (key == 0) {
					quit = 1;
					break;
				}
			}
		}
	}
}

void show_timer(unsigned char is_running, unsigned long timer_ticks) {
	unsigned long t = timer_ticks - ticks10;
	unsigned char centisecs, secs, mins, hours;
	centisecs = t % 100;
	t = t / 100;

	secs = t % 60;
	t = t / 60;

	mins = t % 60;
	hours = t / 60;

	tm1638_show_dec_d(1, 2, hours);
	tm1638_show_dec_zd(3, 2, mins);
	tm1638_show_dec_zd(5, 2, secs);
	tm1638_show_dec_z(7, 2, centisecs);

	// flash the LED if running
	tm1638_set_led(FUNC_TIMER, centisecs < 50 || !is_running);
}

void func_timer() {
	unsigned char key;
	unsigned char last_key;
	unsigned char is_running = 0;
	unsigned char quit = 0;
	unsigned long timer_ticks = 0;

	tm1638_clear_all();
	tm1638_show_text(3, "TIMER");
	tm1638_set_led(FUNC_TIMER, 1);

	delay(2000);

	tm1638_clear_7seg();
	reset_ticks();

	show_timer(is_running, timer_ticks);

	while (!quit) {
		show_timer(is_running, timer_ticks);

		key = tm1638_read_keys();

		if (key != last_key) {
			last_key = key;

			switch (key) {
				case 0x80:
					if (is_running) {
						stop_timer0();
						is_running = 0;
						show_timer(is_running, timer_ticks);
					} else if (timer_ticks > 0) {
						is_running = 1;
						start_timer0();
					}
					break;

				case 0x40:
					if (!is_running) {
						reset_ticks();
						show_timer(is_running, timer_ticks);
					}

				case 0x20:
					if (ticks10 == 0) {
						timer_ticks += 100;
					}
					break;

				case 0x10:
					if (ticks10 == 0) {
						timer_ticks += 6000;
					}
					break;

				case 0x08:
					if (ticks10 == 0) {
						timer_ticks += 360000;
					}
					break;

				case 0x01:{
					if (!is_running) {
						while (tm1638_read_keys() != 0x01);
						show_quit();
						delay(200);
						key = tm1638_wait_for_keypress();
						if (key == 0) {
							quit = 1;
							break;
						}
					}
				}
			}

		}

	}
}

void show_quit() {
	tm1638_clear_7seg();
	tm1638_show_text(3, "QUIT");
}

void show_stopwatch(unsigned char is_running) {
	unsigned long t = ticks10;
	unsigned char centisecs, secs, mins, hours;
	centisecs = t % 100;
	t = t / 100;

	secs = t % 60;
	t = t / 60;

	mins = t % 60;
	hours = t / 60;

	tm1638_show_dec_d(1, 2, hours);
	tm1638_show_dec_zd(3, 2, mins);
	tm1638_show_dec_zd(5, 2, secs);
	tm1638_show_dec_z(7, 2, centisecs);

	// flash the LED if running
	tm1638_set_led(FUNC_STOPWATCH, centisecs < 50 || !is_running);
}

/**
 * Start the stopwatch.
 */
void func_stopwatch() {
	unsigned char key;
	unsigned char last_key;
	unsigned char is_running = 0;
	unsigned char quit = 0;

	tm1638_clear_all();
	tm1638_show_text(4, "STOP");
	tm1638_set_led(FUNC_STOPWATCH, 1);

	delay(2000);

	tm1638_clear_7seg();
	reset_ticks();

	while (!quit) {
		show_stopwatch(is_running);

		key = tm1638_read_keys();

		// make sure the key state is changed to prevent from being triggered repeatedly
		if (key != last_key) {
			last_key = key;

			switch (key) {
				case 0x80: {
					if (is_running) {
						stop_timer0();
						is_running = 0;
						show_stopwatch(is_running);

					} else {
						is_running = 1;
						start_timer0();
					}
					break;
				}

				case 0x40: {
					if (!is_running) {
						reset_ticks();
						show_stopwatch(is_running);
					}

					break;
				}

				case 0x01: {
					if (!is_running) {
						while (tm1638_read_keys() != 0x01);
						show_quit();
						delay(200);
						key = tm1638_wait_for_keypress();
						if (key == 0) {
							quit = 1;
							break;
						}
					}
				}

				default: break;
			}
		}

	}
}


/**
 * 10ms timer for the stopwatch / timer
 */
void timer0() interrupt 1 {

	ticks10++;
	reset_T0();
}
