/*
Copyright (C) 2022 Andreas Lagler

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file fp_lib_div.h
 * @brief Division routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
 */

#ifndef FP_LIB_DIV_H
#define	FP_LIB_DIV_H

#include "fp_lib_types.h"

/**
 * @brief Division of two numbers in Q0.16 format with Q16.16 result
 * @note This function executes in 42 CPU clock cycles (using compiler option -o2)
 * @param num Numerator in Q0.16 format
 * @param den Denominator in Q0.16 format
 * @return Quotient in Q16.16 format
 */
inline static _Q1616 div_Q16_Q16(const _Q16 num, const _Q16 den)
{
    _Q1616 res;
    _Q16 denDummy; // For read/write access to const parameter "den"

    __asm__ volatile(
            "\
        repeat  #0x11                   ;Call div.u 18 times \n \
        div.u   %[num], %[den]          ;Call div.u 18 times \n \
        mov     w0, %d[res]             ;Store MSB of result \n \
        lsr     w1, #0x1, w1            ;Unsigned to signed conversion of division remainder \n \
        lsr     %[den], #0x1, %[den]    ;Unsigned to signed conversion of denominator \n \
        repeat  #0x11                   ;Call divf 18 times \n \
        divf    w1, %[den]              ;Call divf 18 times \n \
        sl      w0, #0x1, %[res]        ;Signed to unsigned conversion of result LSB \n \
        ;42 cycles total"
            : [res] "=&e"(res), [den] "=e"(denDummy) /*out*/
            : [num]"r"(num), "[den]"(den) /*in*/
            : "w0", "w1" /*clobbered*/
            );

    return res;
}

#endif
