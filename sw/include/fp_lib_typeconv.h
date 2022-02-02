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
 * @file fp_lib_typeconv.h
 * @brief Type conversion routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
*/

#ifndef FP_LIB_TYPECONV_H
#define	FP_LIB_TYPECONV_H

#include "fp_lib_types.h"
#include <stdint.h>


/**
 * @brief Conversion of Q0.15 scalar to Q0.16 format
 * @note Naive conversion ignoring negative numbers
 * @note This function executes in 3 CPU clock cycles (using compiler option -o2)
 * @param arg Q0.15 scalar to be converted to Q0.16 format
 * @return Conversion result in Q0.16 format
 */
inline static _Q16 convert_Q15_Q16_Naive(const _Q15 arg)
{
    // Naive conversion shifting the decimal point
    // This is only correct for positive arguments, negative values have to be
    // clipped using a saturation logic
    _Q16 res = (arg << 1) + (arg >> 14);
    
    return res;
}

/**
 * @brief Conversion of Q0.15 scalar to Q0.16 format
 * @note Negative values will be clipped to zero.
 * @note This function executes in 6 CPU clock cycles (using compiler option -o2)
 * @param arg Q0.15 scalar to be converted to Q0.16 format
 * @return Conversion result in Q0.16 format
 */
inline static _Q16 convert_Q15_Q16(const _Q15 arg)
{
    // Naive conversion 
    _Q16 res = convert_Q15_Q16_Naive(arg);
    
    // Saturation bit mask, will be 0x0000 for arg < 0 and 0xFFFF for arg >= 0
    const _Q16 sat = ~(arg >> 15); 
    
    // Clip negative values to 0 by bitwise multiplication with saturation bit mask
    res &= sat;
    
    return res;
}

/**
 * @brief Conversion of Q0.16 scalar to Q15.16 format
 * @note This function executes in 1 CPU clock cycle (using compiler option -o2)
 * @param arg Q0.16 scalar to be converted to Q15.16 format
 * @return Conversion result in Q15.16 format
 */
inline static _Q1516 convert_Q16_Q1516(const _Q16 arg)
{
    _Q1516 res;
    
    // Multiplication by 1 with mul.uu implicitly extends the number to Q1516       
    __asm__ volatile("mul.uu %[arg], #1, %[res]" : [res]"=C"(res) : [arg]"r"(arg) : );

    return res;
}

/**
 * @brief Conversion of Q0.16 scalar to Q0.15 format
 * @note This function executes in 1 CPU clock cycle (using compiler option -o2)
 * @param arg Q0.16 scalar to be converted to Q10.15 format
 * @return Conversion result in Q0.15 format
 */
inline static _Q15 convert_Q16_Q15(const _Q16 arg)
{
    // Naive conversion shifting the decimal point
    return arg >> 1;
}

/**
 * @brief Conversion of Q15.16 scalar to Q0.16 format
 * @note Negative values will be clipped to zero, positive values >= 1.0 will be clipped to Q0.16 maximum value (0.9999847412109375)
 * @note This function executes in 4 CPU clock cycle (using compiler option -o2)
 * @param arg Q15.16 scalar to be converted to Q0.16 format
 * @return Conversion result in Q0.16 format
 */
inline static _Q16 convert_Q1516_Q16(const _Q1516 arg)
{
    _Q16 res;

    // Check high word of argument
    if (((Long) arg).high == 0)
    {
        // High word is zero --> value is in the valid range of Q0.16
        res = ((Long) arg).low;
    }
    else
    {
        // High word is not zero --> fill zeros if negative, fill ones if positive
        res = ~(((Long) arg).high >> 15);
    }

    return res;
}

#endif
