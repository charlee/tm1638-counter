/*******************************************************************************
 * TM1638 module driver
 * 
 * A driver for the popular TM1638 LED/7Seg/Key module.
 ******************************************************************************/


#ifndef __TM1638_H_
#define __TM1638_H_

void tm1638_write_byte(unsigned char address, unsigned char b);
void tm1638_write_bytes(unsigned char address, const unsigned char bytes[], unsigned char n);
void tm1638_clear_all();
void tm1638_display_on(unsigned char duty_cycle);
void tm1638_display_off();
void tm1638_show_digit(unsigned char pos, unsigned char digit, unsigned char dot);
void tm1638_set_digit_off(unsigned char pos);
void tm1638_set_led(unsigned char pos, unsigned char on);
void tm1638_show_number(unsigned char pos, unsigned char num_digits, unsigned long num);
void tm1638_show_hex(unsigned char pos, unsigned char num_digits, unsigned long num);
void tm1638_show_dec(unsigned char pos, unsigned char num_digits, unsigned long num);
void tm1638_show_char(unsigned char pos, unsigned char ch, unsigned char dot);
void tm1638_show_text(unsigned char pos, const char* text);

unsigned char tm1638_read_keys();
unsigned char tm1638_wait_for_keypress();

#endif