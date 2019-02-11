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
#include "project_coder.h"







static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debouncing[LOCAL_MATRIX_ROWS];
static bool debouncing = false;
static uint16_t debouncing_time = 0;

static uint8_t is_left_hand = 0;


void matrix_init(void)
{
    debug_enable = true;
    debug_keyboard = true;
    debug_matrix = true;
    /* Column(sense) */
    palSetLineMode(COL1,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(COL2,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(COL3,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(COL4,  PAL_MODE_INPUT_PULLDOWN);
    palSetLineMode(COL5,  PAL_MODE_INPUT_PULLDOWN);

    /* Row(strobe) */
    palSetLineMode(ROW1,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW2,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW3, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW4, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW5,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW6,  PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW7, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW8, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(ROW9,  PAL_MODE_OUTPUT_PUSHPULL);

    palSetLineMode(LR_DETECT_PIN,PAL_MODE_INPUT_PULLDOWN);

	is_left_hand = palReadLine(LR_DETECT_PIN);

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
            case 0: palSetLine(ROW1);    break;
            case 1: palSetLine(ROW2);    break;
            case 2: palSetLine(ROW3);   break;
            case 3: palSetLine(ROW4);   break;
            case 4: palSetLine(ROW5);    break;
            case 5: palSetLine(ROW6);    break;
            case 6: palSetLine(ROW7);   break;
            case 7: palSetLine(ROW8);   break;
            case 8: palSetLine(ROW9);    break;
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
            case 0: palClearLine(ROW1);    break;
            case 1: palClearLine(ROW2);    break;
            case 2: palClearLine(ROW3);   break;
            case 3: palClearLine(ROW4);   break;
            case 4: palClearLine(ROW5);    break;
            case 5: palClearLine(ROW6);    break;
            case 6: palClearLine(ROW7);   break;
            case 7: palClearLine(ROW8);   break;
            case 8: palClearLine(ROW9);    break;
        }

        if (matrix_debouncing[row] != data) {
            matrix_debouncing[row] = data;
            debouncing = true;
            debouncing_time = timer_read();
        }
    }

    uint8_t offset = 0;
    if (!is_left_hand && is_serial_link_master()) {
        offset = MATRIX_ROWS - LOCAL_MATRIX_ROWS;
    }

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
    if (is_left_hand) {
        offset = MATRIX_ROWS - LOCAL_MATRIX_ROWS;
    }
    for (int row = 0; row < LOCAL_MATRIX_ROWS; row++) {
        matrix[offset + row] = rows[row];
    }
}
