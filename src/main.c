#include <tm1638.h>

void main()
{
	unsigned char i;
	unsigned char key ;
	tm1638_display_on(3);
	tm1638_clear_all();
	tm1638_show_hex(7, 8, 0xdeadbeef);
	
	for (i = 0; i < 8; i++) {
		tm1638_set_led(i, 0);
	}
	
	
	while(1) {
		key = tm1638_read_keys();
		for (i = 0; i < 8; i++) {
			if (key == (1 << i)) {
				tm1638_set_led(i, 1);
			} else {
				tm1638_set_led(i, 0);
			}
		}
	}
}
