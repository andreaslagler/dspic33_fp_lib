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
 * @file fp_lib_trig.h
 * @brief Trigonometric function routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
 */

#ifndef FP_LIB_TRIG_H
#define	FP_LIB_TRIG_H

#include "fp_lib_types.h"

/**
 * @brief Calculation of sine of fractional argument in Q0.15 format
 *
 * sin_Q15(const _Q15 phase) returns sin(pi*phase) 
 * i.e. fractional interval [-1 ... 1[ of phase argument is mapped to [-pi ... pi[
 * Calculation is done by extrapolation of y0/dy pairs given by a lookup-table
 * y = y0[xInt(x)] + dy[xInt(x)] * xFrac(x))
 * 
 * @note This function executes in 6 CPU clock cycles (using compiler option -o2)
 * @param x Argument of sine in Q0.15 format
 * @return Result of sine calculation in Q0.15 format
 */
inline static const _Q15 sin_Q15(const _Q15 x)
{
    // Result
    _Q15 y;

    // Extrapolation lookup-table 
    // Organization of table is as follows
    // dy[0] y0[0] dy[1] y0[1] ... dy[511] y0[511]
    static const _Q15 table[512] = {
        804, 0, 804, 804, 802, 1608, 802, 2410, 799, 3212, 797, 4011, 794, 4808, 791, 5602,
        786, 6393, 783, 7179, 777, 7962, 773, 8739, 766, 9512, 761, 10278, 754, 11039, 746, 11793,
        740, 12539, 731, 13279, 722, 14010, 714, 14732, 705, 15446, 695, 16151, 684, 16846, 674, 17530,
        664, 18204, 651, 18868, 640, 19519, 628, 20159, 616, 20787, 602, 21403, 589, 22005, 576, 22594,
        561, 23170, 548, 23731, 532, 24279, 518, 24811, 503, 25329, 487, 25832, 471, 26319, 455, 26790,
        438, 27245, 422, 27683, 405, 28105, 388, 28510, 370, 28898, 353, 29268, 335, 29621, 317, 29956,
        298, 30273, 281, 30571, 261, 30852, 243, 31113, 224, 31356, 205, 31580, 186, 31785, 166, 31971,
        148, 32137, 127, 32285, 109, 32412, 88, 32521, 69, 32609, 50, 32678, 29, 32728, 10, 32757,
        -10, 32767, -29, 32757, -50, 32728, -69, 32678, -88, 32609, -109, 32521, -127, 32412, -148, 32285,
        -166, 32137, -186, 31971, -205, 31785, -224, 31580, -243, 31356, -261, 31113, -281, 30852, -298, 30571,
        -317, 30273, -335, 29956, -353, 29621, -370, 29268, -388, 28898, -405, 28510, -422, 28105, -438, 27683,
        -455, 27245, -471, 26790, -487, 26319, -503, 25832, -518, 25329, -532, 24811, -548, 24279, -561, 23731,
        -576, 23170, -589, 22594, -602, 22005, -616, 21403, -628, 20787, -640, 20159, -651, 19519, -664, 18868,
        -674, 18204, -684, 17530, -695, 16846, -705, 16151, -714, 15446, -722, 14732, -731, 14010, -740, 13279,
        -746, 12539, -754, 11793, -761, 11039, -766, 10278, -773, 9512, -777, 8739, -783, 7962, -786, 7179,
        -791, 6393, -794, 5602, -797, 4808, -799, 4011, -802, 3212, -802, 2410, -804, 1608, -804, 804,
        -804, 0, -804, -804, -802, -1608, -802, -2410, -799, -3212, -797, -4011, -794, -4808, -791, -5602,
        -786, -6393, -783, -7179, -777, -7962, -773, -8739, -766, -9512, -761, -10278, -754, -11039, -746, -11793,
        -740, -12539, -731, -13279, -722, -14010, -714, -14732, -705, -15446, -695, -16151, -684, -16846, -674, -17530,
        -664, -18204, -651, -18868, -640, -19519, -628, -20159, -616, -20787, -602, -21403, -589, -22005, -576, -22594,
        -561, -23170, -548, -23731, -532, -24279, -518, -24811, -503, -25329, -487, -25832, -471, -26319, -455, -26790,
        -438, -27245, -422, -27683, -405, -28105, -388, -28510, -370, -28898, -353, -29268, -335, -29621, -317, -29956,
        -298, -30273, -281, -30571, -261, -30852, -243, -31113, -224, -31356, -205, -31580, -186, -31785, -166, -31971,
        -148, -32137, -127, -32285, -109, -32412, -88, -32521, -69, -32609, -50, -32678, -29, -32728, -10, -32757,
        10, -32767, 29, -32757, 50, -32728, 69, -32678, 88, -32609, 109, -32521, 127, -32412, 148, -32285,
        166, -32137, 186, -31971, 205, -31785, 224, -31580, 243, -31356, 261, -31113, 281, -30852, 298, -30571,
        317, -30273, 335, -29956, 353, -29621, 370, -29268, 388, -28898, 405, -28510, 422, -28105, 438, -27683,
        455, -27245, 471, -26790, 487, -26319, 503, -25832, 518, -25329, 532, -24811, 548, -24279, 561, -23731,
        576, -23170, 589, -22594, 602, -22005, 616, -21403, 628, -20787, 640, -20159, 651, -19519, 664, -18868,
        674, -18204, 684, -17530, 695, -16846, 705, -16151, 714, -15446, 722, -14732, 731, -14010, 740, -13279,
        746, -12539, 754, -11793, 761, -11039, 766, -10278, 773, -9512, 777, -8739, 783, -7962, 786, -7179,
        791, -6393, 794, -5602, 797, -4808, 799, -4011, 802, -3212, 802, -2410, 804, -1608, 804, -804
    };

    // Cached table pointer
    const _Q15 * yTable = table;

    // Dummy variable for read/write access to const parameter in inline assembly
    _Q15 xDummy;
 
    // Calculate result y = y0[xInt(x)] + dy * xFrac(x))
    // xInt(x) is x quantized to multiples of 256 such that xInt(x) =< x
    // xFrac(x) is the remainder given by x - xInt
    // y0 and dy are given by the table

    // TODO Registerpaar für y verwenden
    __asm__ volatile(
            "\
        lsr     %[x], #0x8, %[y]                    ;xInt = LSB of x = 0..255 \n \
        sl      %[y], #2, %[y]                      ;Quadruple xInt for access of 16 bit dy|y0 pairs \n \
        add     %[yTable], %[y], %[yTable]          ;yTable points to dy[xInt] now \n \
        sl      %[x], #0x8, %[x]                    ;Calculate xFrac = x - xInt in upper byte\n \
        mul.us  %[x], [%[yTable]++], %[y]           ;Calculate dy[xInt] * xFrac, increment table pointer \n \
        add     w3, [%[yTable]], %[y]               ;add y0[xInt] \n \
        ;6 cycles total"
            : [y] "=&c"(y), [yTable] "+r"(yTable), [x] "=r"(xDummy) /*out*/
            : "[x]" (x)/*in*/
            : "w3" /*clobbered*/
            );

    return y;
}

#endif
