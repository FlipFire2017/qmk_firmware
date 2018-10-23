/*
Copyright 2016 Fred Sundvik <fsundvik@gmail.com>
Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal.h"
#include "timer.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "matrix.h"
#include "serial_link/system/serial_link.h"


/*
 * Infinity ErgoDox Pinusage:
 * Column pins are input with internal pull-down. Row pins are output and strobe with high.
 * Key is high or 1 when it turns on.
 *
 *     col: { PTD1, PTD4, PTD5, PTD6, PTD7 }
 *     row: { PTB2, PTB3, PTB18, PTB19, PTC0, PTC9, PTC10, PTC11, PTD0 }
 */
/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[LOCAL_MATRIX_ROWS];
static bool debouncing = false;
static uint16_t debouncing_time = 0;


void matrix_init(void)
{
    debug_enable = true;
    debug_keyboard = true;
    debug_matrix = true;
    /* Column(sense) */
    palSetPadMode(GPIOB, 0,  PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, 1,  PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, 2,  PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, 3,  PAL_MODE_INPUT_PULLDOWN);
    palSetPadMode(GPIOB, 4,  PAL_MODE_INPUT_PULLDOWN);

    /* Row(strobe) */
    palSetPadMode(GPIOA, 0,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 1,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 4, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 5, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 6,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 7,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 8, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 9, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPadMode(GPIOA, 10,  PAL_MODE_OUTPUT_PUSHPULL);

    memset(matrix, 0, MATRIX_ROWS * sizeof(matrix_row_t));
    memset(matrix_debouncing, 0, LOCAL_MATRIX_ROWS * sizeof(matrix_row_t));

    matrix_init_quantum();
}

uint8_t matrix_scan(void)
{
    for (int row = 0; row < LOCAL_MATRIX_ROWS; row++) {
        matrix_row_t data = 0;

        // strobe row
        switch (row) {
            case 0: palSetPad(GPIOA, 0);    break;
            case 1: palSetPad(GPIOA, 1);    break;
            case 2: palSetPad(GPIOA, 4);   break;
            case 3: palSetPad(GPIOA, 5);   break;
            case 4: palSetPad(GPIOA, 6);    break;
            case 5: palSetPad(GPIOA, 7);    break;
            case 6: palSetPad(GPIOA, 8);   break;
            case 7: palSetPad(GPIOA, 9);   break;
            case 8: palSetPad(GPIOA, 10);    break;
        }

        // need wait to settle pin state
        // if you wait too short, or have a too high update rate
        // the keyboard might freeze, or there might not be enough
        // processing power to update the LCD screen properly.
        // 20us, or two ticks at 100000Hz seems to be OK
        wait_us(20);

        // read col data: { PB0,1,2,3,4 }
        data = palReadPort(GPIOB) & 0x1F;
             

        // un-strobe row
        switch (row) {
            case 0: palClearPad(GPIOA, 0);    break;
            case 1: palClearPad(GPIOA, 1);    break;
            case 2: palClearPad(GPIOA, 4);   break;
            case 3: palClearPad(GPIOA, 5);   break;
            case 4: palClearPad(GPIOA, 6);    break;
            case 5: palClearPad(GPIOA, 7);    break;
            case 6: palClearPad(GPIOA, 8);   break;
            case 7: palClearPad(GPIOA, 9);   break;
            case 8: palClearPad(GPIOA, 10);    break;
        }

        if (matrix_debouncing[row] != data) {
            matrix_debouncing[row] = data;
            debouncing = true;
            debouncing_time = timer_read();
        }
    }

    uint8_t offset = 0;
#ifdef MASTER_IS_ON_RIGHT
    if (is_serial_link_master()) {
        offset = MATRIX_ROWS - LOCAL_MATRIX_ROWS;
    }
#endif

    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int row = 0; row < LOCAL_MATRIX_ROWS; row++) {
            matrix[offset + row] = matrix_debouncing[row];
        }
        debouncing = false;
    }
    matrix_scan_quantum();
    return 1;
}

bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & (1<<col));
}

matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    dprintf("\nr/c 01234567\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        dprintf("%X0: ", row);
        matrix_row_t data = matrix_get_row(row);
        for (int col = 0; col < MATRIX_COLS; col++) {
	    dprintf("%d",(data & (1<<col))?1:0);
        }
        dprintf("\n");
    }
}

void matrix_set_remote(matrix_row_t* rows, uint8_t index) {
    uint8_t offset = 0;
#ifdef MASTER_IS_ON_RIGHT
    offset = MATRIX_ROWS - LOCAL_MATRIX_ROWS * (index + 2);
#else
    offset = LOCAL_MATRIX_ROWS * (index + 1);
#endif
    for (int row = 0; row < LOCAL_MATRIX_ROWS; row++) {
        matrix[offset + row] = rows[row];
    }
}
