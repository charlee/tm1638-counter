#include <8052.h>
#include "tm1638.h"

#define DIO P1_0
#define CLK P1_1
#define STB P1_2


__code unsigned char sevenseg_hex[] = {
	0x3F, 0x06, 0x5B,	0x4F,
	0x66, 0x6D, 0x7D, 0x07,
  0x7F, 0x6F, 0x77, 0x7C,
	0x39, 0x5E, 0x79, 0x71
};

// src: https://en.wikichip.org/wiki/seven-segment_display/representing_letters
__code const unsigned char sevenseg_digits_alphabets[75]= {
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x00, 
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
    0x76, 0x30, 0x1E, 0x75, 0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
    0x78, 0x3E, 0x1C, 0x1D, 0x64, 0x6E, 0x5B, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
    0x00, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x3D, 0x76, 0x30, 0x1E, 0x75, 
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
    0x38, 0x55, 0x54, 0x5C, 0x73, 0x67, 0x50, 0x6D, 0x78, 0x3E, 0x1C, 0x1D, 
/*  x     y     z     */
    0x64, 0x6E, 0x5B
};

/**
 * Send a byte to TM1638.
 */
void _tm1638_write(unsigned char b) {
	unsigned char i;
	
	for (i = 0; i < 8; i++) {
		CLK = 0;
		DIO = b & 0x01;
		b >>= 1;
		CLK = 1;
	}
}

/**
 * Read a byte from TM1638.
 */
unsigned _tm1638_read() {
	unsigned char i;
	unsigned char ch = 0;
	
	// set DIO pin as input
	DIO = 1;
	
	for (i = 0; i < 8; i++) {
		ch >>= 1;
		CLK = 0;
		if (DIO) {
			ch |= 0x80;
		}
		CLK = 1;
	}
	
	return ch;
}

void _tm1638_write_cmd(unsigned char cmd) {
	STB = 0;
	_tm1638_write(cmd);
	STB = 1;
}


void tm1638_write_byte(unsigned char address, unsigned char b) {
	// write to fixed addr
	_tm1638_write_cmd(0x44);
	
	STB = 0;
	// write address
	_tm1638_write(0xc0 | address);
	// write data
	_tm1638_write(b);
	STB = 1;
}

void tm1638_write_bytes(unsigned char address, const unsigned char bytes[], unsigned char n) {
	unsigned char i;
	// write, address auto increment
	_tm1638_write_cmd(0x40);
	
	STB = 0;
	_tm1638_write(0xc0 | address);
	for (i = 0; i < n; i++) {
		_tm1638_write(bytes[i]);
	}
	STB = 1;
}

/**
 * Read key value from the keyboard.
 * The TM1638 module only uses K3 to provide 8 inputs, so only the K3 bits (bit 0 and bit4) are returned.
 */
unsigned char tm1638_read_keys() {
	unsigned char i;
	unsigned char ch = 0;
	
	STB = 0;
	_tm1638_write(0x42);
	for (i = 0; i < 4; i++) {
		ch |= _tm1638_read() << i;
	}
	STB = 1;

  return ch;
}

/**
 * Wait for a key to be pressed.
 * 
 * Params:
 *   timeout: how many loops to wait.
 * 
 * Returns: A number [0, 7] representing which key is pressed.
 *          Returns -1 if no key pressed before timeout.
 * 
 */
unsigned char tm1638_wait_for_keypress() {
	unsigned char i;
	unsigned char ch;
	while (1) {
		ch = tm1638_read_keys();
		if (ch > 0) {

 			// wait until the key is released
			while (tm1638_read_keys() == ch);

			// detect which key is pressed
			for (i = 0; i < 8; i++) {
				if (ch == (1 << i)) {
					return i;
				}
			}
		}
	}
}

void tm1638_clear_all() {
	unsigned char i;
	// write, address auto increment
	_tm1638_write_cmd(0x42);
	
	STB = 0;
	_tm1638_write(0xc0);
	for (i = 0; i < 16; i++) {
		_tm1638_write(0x00);
	}
	STB = 1;
}

void tm1638_clear_7seg() {
	unsigned char i;
	for (i = 0; i < 8; i++) {
		tm1638_set_digit_off(i);
	}
}

void tm1638_display_on(unsigned char duty_cycle) {
	_tm1638_write_cmd(0x88 | (duty_cycle & 0x07));
}

void tm1638_display_off() {
	_tm1638_write_cmd(0x80);
}

/**
 * Show `digit` at position `pos`.
 */
void tm1638_show_digit(unsigned char pos, unsigned char digit, unsigned char dot) {
	tm1638_write_byte(pos << 1, sevenseg_hex[digit] | (dot ? 0x80: 0x00));
}

void tm1638_set_digit_off(unsigned char pos) {
	tm1638_write_byte(pos << 1, 0);
}

void tm1638_set_led(unsigned char pos, unsigned char on) {
	tm1638_write_byte((pos << 1) + 1, on ? 0x01 : 0x00);
}

/**
 * Show character `ch` at position `pos`. Position 0 is the left-most.
 */
void tm1638_show_char(unsigned char pos, unsigned char ch, unsigned char dot) {
	tm1638_write_byte(pos << 1, sevenseg_digits_alphabets[ch - 48] | (dot ? 0x80: 0x00));
}

/**
 * Show number `num` starting from position `pos`. Only the `num_digits` digits from LSB are displayed.
 */
void tm1638_show_hex(unsigned char pos, unsigned char num_digits, unsigned long num) {
	unsigned char i;
	
	for (i = 0; i < num_digits; i++) {
		tm1638_show_digit(pos - i, num & 0x0f, 0);
		num >>= 4;
		if(num == 0) {
			break;
		}
	}
}

void tm1638_show_dec(unsigned char pos, unsigned char num_digits, unsigned long num) {
	unsigned char i;
	
	for (i = 0; i < num_digits; i++) {
		tm1638_show_digit(pos - i, num % 10, 0);
		num /= 10;
		if(num == 0) {
			break;
		}
	}
}

void tm1638_show_dec_z(unsigned char pos, unsigned char num_digits, unsigned long num) {
	unsigned char i;
	
	for (i = 0; i < num_digits; i++) {
		tm1638_show_digit(pos - i, num % 10, 0);
		num /= 10;
	}
}

void tm1638_show_dec_zd(unsigned char pos, unsigned char num_digits, unsigned long num) {
	unsigned char i;
	
	for (i = 0; i < num_digits; i++) {
		tm1638_show_digit(pos - i, num % 10, (i == 0));
		num /= 10;
	}
}

void tm1638_show_dec_d(unsigned char pos, unsigned char num_digits, unsigned long num) {
	unsigned char i;
	
	for (i = 0; i < num_digits; i++) {
		tm1638_show_digit(pos - i, num % 10, (i == 0));
		num /= 10;
		if(num == 0) {
			break;
		}
	}
}

void tm1638_show_text(unsigned char pos, const char* text) {
	unsigned char i = 0;
	while (text[i]) {
		if (pos + i > 7) break;
		tm1638_show_char(pos + i, text[i], 0);
		i++;
	}
}
