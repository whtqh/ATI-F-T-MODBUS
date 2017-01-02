/* sensor/unit-test.h.  Generated from unit-test.h.in by configure.  */
/*
 * Copyright © 2008-2011 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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

#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

/* Constants defined by configure.ac
 * Ln 27 :AC_CONFIG_HEADERS([config.h sensor/unit-test.h])
 *
 *
 *
 */
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# ifndef _MSC_VER
# include <stdint.h>
# else
# include "stdint.h"
# endif
#endif

#define SERVER_ID         10
#define INVALID_SERVER_ID 18

const uint16_t UT_BITS_ADDRESS = 0x00;
const uint16_t UT_BITS_NB = 0x0A;
const uint8_t UT_BITS_TAB[] = { 0xFF, 0x03};

const uint16_t UT_INPUT_BITS_ADDRESS = 0x00;
const uint16_t UT_INPUT_BITS_NB = 0x0A;
const uint8_t UT_INPUT_BITS_TAB[] = { 0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00};

const uint16_t UT_REGISTERS_ADDRESS = 0x00;
/* Raise a manual exception when this adress is used for the first byte */
const uint16_t UT_REGISTERS_ADDRESS_SPECIAL = 0x6C;
const uint16_t UT_REGISTERS_NB = 0x06;
const uint16_t UT_REGISTERS_TAB[] = { 0x022B, 0x0001, 0x0064,0x022B, 0x0001, 0x0064,0x022B, 0x0001, 0x0064,0x0000 };
/* If the following value is used, a bad response is sent.
   It's better to test with a lower value than
   UT_REGISTERS_NB_POINTS to try to raise a segfault. */
const uint16_t UT_REGISTERS_NB_SPECIAL = 0x2;

const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x00;
const uint16_t UT_INPUT_REGISTERS_NB = 0x0A;
const uint16_t UT_INPUT_REGISTERS_TAB[] = { 0x0000 };

const float UT_REAL = 916.540649;
const uint32_t UT_IREAL = 0x4465229a;

#endif /* _UNIT_TEST_H_ */