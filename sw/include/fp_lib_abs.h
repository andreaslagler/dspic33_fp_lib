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
 * @file fp_lib_abs.h
 * @brief Type conversion routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
*/

#ifndef FP_LIB_ABS_H
#define	FP_LIB_ABS_H

#include "fp_lib_types.h"

/**
 * @brief Modified calculation of absolute of a number in Q0.15 format
 * 
 * abs(0x8000) returns 0x8000, whereas abs_Q15(0x8000) returns 0x7FFF
 * @note This function executes in 6 CPU clock cycles (using compiler option -o2)
 * @param arg Argument in Q0.15 format
 * @return Modified absolute of arg in Q0.15 format
 */
inline static _Q15 abs_Q15(const _Q15 arg)
{
    // Result
    _Q15 res = arg;
    
    // TODO mit bit check lässt sich evtl noch ein cycle herausholen
    _Q15 mask = 0x8000;
    __asm__ volatile(
            "\
        cpsne   %[val], %[mask]             ;Compare val and bit mask, skip if not equal \n \
        mov     #0x7FFF, %[val]             ;Correct val to get the desired result \n \
        asr     %[val], #15, %[mask]        ;Use bitmask to calculate two's complement of val \n \
        add     %[mask], %[val], %[val]     ;Subtract one (step 2 of two's complement calculation) \n \
        xor     %[mask], %[val], %[val]     ;Invert bit-wise for negative numbers (step 2 of two's complement calculation) \n \
        ;5 cycles total"
            : [val] "+r"(res), [mask] "+r"(mask) /*out*/
            : /*in*/
            : /*clobbered*/
            );

    return res;
}

#endif
