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
#ifndef PROJECT_CODER_H
#define PROJECT_CODER_H

#include "quantum.h"

/* This a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 */

/*
 *   LEFT HAND: LINES 88-95
 *  RIGHT HAND: LINES 97-104
 */
#define LAYOUT_coder( \
    A80, A70, A60, A50, A40, A30, A20,  \
    A81, A71, A61, A51, A41, A31, A21,  \
    A03, A82, A72, A62, A52, A42, A32,       \
    A73, A23, A63, A53, A43, A33, A13, A34, \
    A24, A14, A04, A83, A64, A54, A44,           \
                                            \
         B20, B30, B40, B50, B60, B70, B80, \
         B21, B31, B41, B51, B61, B71, B81, \
         B32, B42, B52, B62, B72, B82, B03, \
    B34, B13, B33, B43, B53, B63, B23, B73, \
         B44, B54, B64, B83, B04, B14, B24 \
) { \
    { KC_NO, KC_NO, KC_NO, A03,   A04 }, \
    { KC_NO, KC_NO, KC_NO, A13,   A14 }, \
    { A20,   A21,   KC_NO, A23,   A24 }, \
    { A30,   A31,   A32,   A33,   A34 }, \
    { A40,   A41,   A42,   A43,   A44 }, \
    { A50,   A51,   A52,   A53,   A54 }, \
    { A60,   A61,   A62,   A63,   A64 }, \
    { A70,   A71,   A72,   A73,   KC_NO }, \
    { A80,   A81,   A82,   A83,   KC_NO }, \
    { KC_NO, KC_NO, KC_NO, B03,   B04 }, \
    { KC_NO, KC_NO, KC_NO, B13,   B14 }, \
    { B20,   B21,   KC_NO, B23,   B24 }, \
    { B30,   B31,   B32,   B33,   B34 }, \
    { B40,   B41,   B42,   B43,   B44 }, \
    { B50,   B51,   B52,   B53,   B54 }, \
    { B60,   B61,   B62,   B63,   B64 }, \
    { B70,   B71,   B72,   B73,   KC_NO }, \
    { B80,   B81,   B82,   B83,   KC_NO }  \
}

#endif
