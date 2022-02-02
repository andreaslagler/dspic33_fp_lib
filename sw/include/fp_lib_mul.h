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
 * @brief Multiplication routines for fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
*/

#ifndef FP_LIB_MUL_H
#define	FP_LIB_MUL_H

#include "fp_lib_types.h"

/**
 * @brief Multiplication of two scalars in Q0.15 format
 * @note The multiplication result is truncated to Q0.15
 * @note This function executes in 3 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.15 format
 * @param arg2 Scalar factor in Q0.15 format
 * @return Multiplication result in Q0.15 format
 */
inline static _Q15 mul_Q15_Q15(const _Q15 arg1, const _Q15 arg2)
{
    // The result of the direct multiplication is Q1.30
    const Long res = {.value = __builtin_mulss(arg1, arg2) << 1};
    return res.high;
}

/**
 * @brief Multiplication of two scalars in Q0.15 and Q0.16 format
 * @note The multiplication result is truncated to Q0.15
 * @note This function executes in 1 CPU clock cycle (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.15 format
 * @param arg2 Scalar factor in Q0.16 format
 * @return Multiplication result in Q0.15 format
 */
inline static _Q15 mul_Q15_Q16(const _Q15 arg1, const _Q16 arg2)
{
    // The result of the direct multiplication is Q31
    const Long res = {.value = __builtin_mulsu(arg1, arg2)};
    return res.high;
}

/**
 * @brief Multiplication of two scalars in Q0.15 and Q16.16 format
 * @note The multiplication result is truncated to Q0.15 but not clipped
 * @note This function executes in 1 CPU clock cycle (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.15 format
 * @param arg2 Scalar factor in Q16.16 format
 * @return Multiplication result in Q0.15 format
 */
inline static _Q15 mul_Q15_Q1616(const _Q15 arg1, const _Q1616 arg2)
{
    // The result of the direct multiplication is Q1532
    const Long res = {.value = __builtin_mulsu(arg1, ((ULong) arg2).high)};
    return res.low + mul_Q15_Q16(arg1, ((ULong) arg2).low);
}

/**
 * @brief Multiplication of two scalars in Q0.16 format
 * @note The multiplication result is truncated to Q0.16
 * @note This function executes in 1 CPU clock cycle (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.16 format
 * @param arg2 Scalar factor in Q0.16 format
 * @return Multiplication result in Q0.16 format
 */
inline static _Q16 mul_Q16_Q16(const _Q16 arg1, const _Q16 arg2)
{
    // The result of the direct multiplication is Q0.32
    const ULong res = {.value = __builtin_muluu(arg1, arg2)};
    return res.high;
}

/**
 * @brief Multiplication of two scalars in Q0.32 and Q0.16 format
 * @note The multiplication result is truncated to Q0.32
 * @note This function executes in 4 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.32 format
 * @param arg2 Scalar factor in Q0.16 format
 * @return Multiplication result in Q0.32 format
 */
inline static _Q32 mul_Q32_Q16(const _Q32 arg1, const _Q16 arg2)
{
    // Result of Q32 * Q16 multiplication is Q48.
    // In this function, the Result is truncated to Q32
    _Q32 res = __builtin_muluu(((ULong) arg1).high, arg2);
    _Q32 temp = __builtin_muluu(((ULong) arg1).low, arg2);
    res += ((ULong) temp).high;
   return res;
}

/**
 * @brief Multiplication of two scalars in Q0.32 and Q16.0 format
 * @note The multiplication result is truncated to Q0.32
 * @note This function executes in 3 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q0.32 format
 * @param arg2 Scalar factor in Q16.0 format
 * @return Multiplication result in Q0.32 format
 */
static inline _Q32 mul_Q32_UINT(const _Q32 arg1, const uint16_t arg2)
{
    // Result of Q0.32 * Q16.0 multiplication is Q16.32.
    // In this function, the Result is truncated to Q0.32
    // --> Make sure that integer part of result is 0

    // Multiply high-byte of arg1 with arg2
    _Q32 res = __builtin_muluu(arg2,((ULong) arg1).low);
    
    // Multiply and add high-byte of arg1 with arg2
    // (use mulw.uu for truncated 16 bit result) 
    __asm__ volatile("\
        mulw.uu %[arg2], %d[arg1], w0 \n \
        add w0, %d[res], %d[res]"
            : [res]"+r"(res)
            : [arg1]"r"(arg1), [arg2]"r"(arg2)
            : "w0");

    return res;
}

/**
 * @brief Multiplication of two scalars in Q16.16 and Q0.16 format
 * @note The multiplication result is truncated to Q16.16
 * @note This function executes in 4 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q16.16 format
 * @param arg2 Scalar factor in Q0.16 format
 * @return Multiplication result in Q16.16 format
 */
inline static _Q1616 mul_Q1616_Q16(const _Q1616 arg1, const _Q16 arg2)
{
    // Result of Q1616 * Q16 multiplication is Q1632.
    // In this function, the Result is truncated to Q1616
    _Q1616 res = __builtin_muluu(((ULong) arg1).high, arg2);
    _Q32 temp = __builtin_muluu(((ULong) arg1).low, arg2);
    res += ((ULong) temp).high;

    return res;
}

/**
 * @brief Multiplication of two scalars in Q16.16 and Q16.0 (ie. uint16_t) format
 * @note The multiplication result is truncated to Q16.16 (make sure that integer part of result does not exceed 65535)
 * @note This function executes in 3 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q16.16 format
 * @param arg2 Scalar factor in Q16.0 format
 * @return Multiplication result in Q16.16 format
 */
inline static _Q1616 mul_Q1616_UINT(const _Q1616 arg1, const uint16_t arg2)
{
    // Result of Q16.16 * Q16.0 multiplication is Q32.16.
    // In this function, the Result is truncated to Q16.16
    // --> Make sure that integer part of result does not exceed 65535
    _Q1616 res = __builtin_muluu(((ULong) arg1).low, arg2);
    __asm__ volatile("\
        mulw.uu %d[arg1], %[arg2], w0 \n \
        add w0, %d[res], %d[res]"
            : [res]"+r"(res)
            : [arg1]"r"(arg1), [arg2]"r"(arg2)
            : "w0");

    return res;
}

/**
 * @brief Multiplication of two scalars in Q16.16 format
 * @note The multiplication result is truncated to Q16.16 (make sure that integer part of result does not exceed 65535)
 * @note This function executes in 9 CPU clock cycles (using compiler option -o2)
 * @param arg1 Scalar factor in Q16.16 format
 * @param arg2 Scalar factor in Q16.16 format
 * @return Multiplication result in Q16.16 format
 */
inline static _Q1616 mul_Q1616_Q1616(const _Q1616 arg1, const _Q1616 arg2)
{
    // Result of Q1616 * Q1616 multiplication is Q3232.
    // In this funnction, the Result is truncated to Q1616
    // --> Make sure that integer part of result does not exceed 65535
    return mul_Q1616_UINT(arg1, ((ULong) arg2).high) + mul_Q1616_Q16(arg1, ((ULong) arg2).low);
}

/**
 * @brief Multiplication of array in Q0.15 format and scalar Q0.16 format
 * @note The multiplication result is truncated to Q0.15
 * @note This function executes in 3 + 2 * len CPU clock cycles (using compiler option -o2)
 * @param src   Pointer to array in Q0.15 format
 * @param val   Factor in Q0.16 format
 * @param dst   Pointer to multiplication result array in Q0.15 format
 * @param len   Number of array elements
 */
inline static void mul_aQ15_Q16(
                             const _Q15 * src,
                             const _Q16 val,
                             _Q15 * dst,
                             const uint16_t len)
{
    __asm__ volatile(
            "\
        do      %[len], mul_aQ15_Q16_end_%=     ;Init Loop \n \
        mul.us  %[val], [%[src]++], w0          ;Actual multiplication \n \
        mul_aQ15_Q16_end_%=:                    ;\n \
        mov     w1, [%[dst]++]                  ;Store result \n \
        ; 3 + 2 * len cycles total, 1 DO level"
            : [src] "+r"(src), [dst] "+r"(dst) /*out*/
            : [val] "r"(val), [len] "r"(len - 1) /*in*/
            : "w0", "w1" /*clobbered*/
            );
}

#endif
