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
 * @file fp_lib_types.h
 * @brief Definition of fixed point types
 * 
 * This file is part of FP-Lib Fixed-point math library for dsPIC33 family of Microchip dsPIC® digital signal controllers
*/

#ifndef FP_LIB_TYPES_H
#define	FP_LIB_TYPES_H

#include <stdint.h>

/// Type definition for Q0.16 unsigned fractional number
typedef uint16_t _Q16;

/// Type definition for Q0.15 signed fractional  number
typedef int16_t _Q15;

/// Type definition for Q0.32 unsigned fractional number
typedef uint32_t _Q32;

/// Type definition for Q16.16 unsigned fractional number
typedef uint32_t _Q1616;

/// Type definition for Q15.16 signed fractional number
typedef int32_t _Q1516;

////////////////////////////////////////////////////////////////////////////
// Union typedefs for upper/lower word access within a long
// allowing for easy access of integer and fractional parts of a Q16.16 or Q15.16 fractional number
////////////////////////////////////////////////////////////////////////////

// uint32_t / Q16.16
typedef union
{
    // Q16.16 representation
    uint32_t value;
    
    struct
    {
        // Q0.16 lower word
        uint16_t low;

        // Q16.0 upper word
        uint16_t high;
    };
} ULong;

// int32_t / Q15.16
typedef union
{
    // Q15.16 representation
   int32_t value;
   
    struct
    {
        // Q0.16 lower word
        uint16_t low;

        // Q15.0 upper word
        int16_t high;
    };
} Long;

/*
 /// Union type definition for easy access of integer and fractional parts of a Q16.16 number
typedef union
{
    /// Q16.16 representation
    uint32_t value;
    
    struct
    {
        // Fractional part of Q16.16
        uint16_t fracPart;

        // Integer part of Q16.16
        uint16_t intPart;
    };
} _Q1616;

//  Union typedefs for easy access of integer and fractional parts of a Q15.16 number
typedef union
{
    // Q15.16 representation
   int32_t value;
   
    struct
    {
        // Fractional part of Q15.16
        uint16_t fracPart;

        // Integer part of Q15.16
        int16_t intPart;
    };
} Long;
 */

#endif
