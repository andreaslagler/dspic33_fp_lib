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
 * @file fp_lib_interp.h
 * @brief Interpolation routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
*/

#ifndef FP_LIB_INTERP_H
#define	FP_LIB_INTERP_H

#include "fp_lib_types.h"

#include <stdint.h>

/**
 * @brief Linear interpolation between two numbers in Q0.15 format
 * 
 * y(x) = y1 * (1 - x) + y2 * x = y1 - y1 * x + y2 * x \n
 * with x being a fractional x coordinate in the range [0 1[
 * 
 * @note This function executes in 4 CPU clock cycles (using compiler option -o2)
 * @param y1    y coordinate of first sampling point
 * @param y2    y coordinate of second sampling point
 * @param x     fractional x coordinate of interpolation result
 * @return      y coordinate of interpolation result
 */
inline static _Q15 interpLinear(
                                  const _Q15 y1,
                                  const _Q15 y2,
                                  const _Q16 x)
{
    _Q15 y;

    // Use accA to calculate the linear interpolation
    // Y = Y1 * (1 - X) + Y2 * X = Y1 - Y1 * X + Y2 * X
    __asm__ volatile(
            "\
        lac     %[y1], #0, A                        ;Load y1 in A \n \
        msc     %[y1] * %[x], A                     ;Subtract y1 * x from A \n \
        mac     %[y2] * %[x], A                     ;Add y2 * x to A \n \
        sac.r   A, #0, %[y]                         ;Store A in y \n \
        ;4 cycles total"
            : [y] "=r"(y) /*out*/
            : [y1] "z"(y1), [y2] "z"(y2), [x] "z"(x >> 1) /*in*/
            : /*clobbered*/
            );

    return y;
}

/**
 * @brief Linear interpolation of a 256-point lookup-table in Q0.15 format
 * 
 * y(x) = y_left(x) * (1 - x_frac(x)) + y_right(x) * x_frac(x) = y_left(x) - y_left(x) * x_frac(x) + y_right(x) * x_frac(x) \n
 * with \n
 * x being a fractional x coordinate in the range [0 1[ \n
 * x_frac(x) being the fractional part of x, in this case (256-point lookup-table) given by the LSB of x \n
 * y_left(x) being the left-hand sampling point , in this case (256-point lookup-table) given by yTable(MSB(x)) \n
 * y_right(x) being the right-hand sampling point , in this case (256-point lookup-table) given by yTable(MSB(x)+1) \n
 * y(x) being the interpolation result such that y(0) = yTable[0] and y(1) = yTable[256]
 * 
 * @note This function executes in 9 CPU clock cycles (using compiler option -o2)
 * @note The length of the lookup-table must be 256+1 = 257
 * @param yTable Pointer to a lookup-table holding 256+1 = 257 sampling points in Q0.15 format
 * @param x     fractional x coordinate of interpolation result in Q0.16 format
 * @return      y coordinate of interpolation result in Q0.15 format
 */
inline static _Q15 interpLUT_256_Q15(
                                              const _Q15 * const yTable,
                                              const _Q16 x)
{
    _Q15 y;

    // Dummy variables for read/write access to const parameters in inline assembly
    _Q15 yTableDummy;
    _Q16 xDummy;

    // Use accA to calculate the linear interpolation
    // y = y_left * (1 - x_frac) + y_right * x_frac = y_left - y_left * x_frac + y_right * x_frac
    // x_frac is the fractional part of x, i.e. 0 <= x_frac <= 255,
    // so we can use msc and mac instructions with DSP unit in signed/signed mode (default)
    __asm__ volatile(
            "\
        lsr     %[x], #0x8, %[y]                    ;Mask LSB of X --> table index 0..255 \n \
        add     %[y], %[y], %[y]                    ;Double the table index for 16 bit table access \n \
        add     %[yTable], %[y], %[yTable]          ;yTable points to y_left now \n \
        and     #0xff, %[x]                         ;Mask fractional part of X \n \
        movsac  A, [%[yTable]]+=2, %[y]             ;Prefetch y_left \n \
        lac     %[y], #7, A                         ;Load prescaled y_left in A \n \
        msc     %[y] * %[x], A, [%[yTable]], %[y]   ;Subtract y_left * x_frac from A, prefetch y_right \n \
        mac     %[y] * %[x], A                      ;Add y_right * x_frac to A \n \
        sac.r   A, #-7, %[y]                        ;Store scaled A in y \n \
        ;9 cycles total"
            : [y] "=&z"(y), [yTable] "=x"(yTableDummy), [x] "=z"(xDummy) /*out*/
            : "[yTable]"(yTable), "[x]"(x) /*in*/
            : /*clobbered*/
            );

    return y;
}
#endif
