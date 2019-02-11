/* Copyright 2018 REPLACE_WITH_YOUR_NAME
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "project_coder.h"
#include "serial_link/system/serial_link.h"
#include "color.h"

static HSV hsv = {
	0,1.0,1.0
};

static uint16_t slow_down_ct = 0;
#define PRESCALER 100;

static void colorTick(PWMDriver *pwmp){
	(void)pwmp;
	static RGB rgb;
	slow_down_ct = (slow_down_ct +1 ) % PRESCALER;
	if(slow_down_ct == 0){
		hsv.h = (hsv.h + 1) % 360;
		rgb = hsv_to_rgb(hsv);
		// dprintf("[HSV] (%d,%.2f,%.2f)",hsv.h,hsv.s,hsv.v);
		// dprintf("[RGB] (%.2f,%.2f,%.2f)",rgb.r,rgb.g,rgb.b);
		pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, rgb.r * 10000  ));
		pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, rgb.g * 10000  ));
		pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, rgb.b * 10000  ));
	}
}

static PWMConfig rgbPWMConfig = {
  255000,                                    /* 10kHz PWM clock frequency.     */
  255,                                    /* Initial PWM period 1S.         */
  colorTick,                                     /* Period callback.               */
  {
   {PWM_OUTPUT_ACTIVE_LOW, NULL},          /* CH1 mode and callback.         */
   {PWM_OUTPUT_ACTIVE_LOW, NULL},             /* CH2 mode and callback.         */
   {PWM_OUTPUT_ACTIVE_LOW, NULL},             /* CH3 mode and callback.         */
   {PWM_OUTPUT_DISABLED, NULL}              /* CH4 mode and callback.         */
  },
  0,                                        /* Control Register 2.            */
  0                                         /* DMA/Interrupt Enable Register. */
};

void matrix_init_kb(void) {
	
	// put your keyboard start-up code here
	// runs once when the firmware starts up
  // leds init
	// palSetLineMode(LEDR,  PAL_MODE_OUTPUT_PUSHPULL);
	// palSetLineMode(LEDG,  PAL_MODE_OUTPUT_PUSHPULL);
	// palSetLineMode(LEDB,  PAL_MODE_OUTPUT_PUSHPULL);
	// palClearLine(LEDR);
	// palClearLine(LEDG);
	// palClearLine(LEDB);
	matrix_init_user();
}

void matrix_scan_kb(void) {
	// put your looping keyboard code here
	// runs every cycle (a lot)

	matrix_scan_user();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
	// put your per-action keyboard code here
	// runs for every action, just before processing by the firmware

	return process_record_user(keycode, record);
}

void led_set_kb(uint8_t usb_led) {
	// put your keyboard LED indicator (ex: Caps Lock LED) toggling code here
	led_set_user(usb_led);
}

void init_serial_link_hal(void){
	/**
	Init USART2 PIN
	**/
	palSetPadMode(GPIOA,2,PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA,3,PAL_MODE_ALTERNATE(7));
	/**
	Init USART1 PIN
	**/
	palSetPadMode(GPIOB,6,PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOB,7,PAL_MODE_ALTERNATE(7));
	/**
	 * Test
	 **/
	palSetLineMode(LEDR,PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LEDG,PAL_MODE_OUTPUT_PUSHPULL);
	palSetLineMode(LEDB,PAL_MODE_OUTPUT_PUSHPULL);
	palClearLine(LEDB);
	palSetLine(LEDR);
	palSetLine(LEDG);

}

bool is_keyboard_master(void) {
    return is_serial_link_master();
}

void backlight_init_ports(void){
	palSetLine(LEDB);
	palSetLineMode(LEDR,PAL_MODE_ALTERNATE(6));
	palSetLineMode(LEDG,PAL_MODE_ALTERNATE(6));
	palSetLineMode(LEDB,PAL_MODE_ALTERNATE(6));
	
	pwmStart(&PWMD1,&rgbPWMConfig);
	pwmEnablePeriodicNotification(&PWMD1);
}