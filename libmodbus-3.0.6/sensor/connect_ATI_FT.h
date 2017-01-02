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

#ifndef _CONNECT_ATI_FT_H_
#define _CONNECT_ATI_FT_H_

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
const uint16_t ATI_GAGE_REG_ADDRESS = 0x0000;
const uint16_t ATI_CALI_REG_ADDRESS = 0x00E3;
const uint16_t ATI_CALI_STEP        = 0x00C0;
const uint16_t ATI_CALI_LEN         = 0x00A9;
uint16_t START_ADDRESS              = 0x0000;
const uint16_t ATI_CALI_NUM_1       = 0x0068;
const uint16_t ATI_CALI_NUM_2       = 0x0041;
const uint16_t ATI_GAGE_NUM         = 0x000C;
const uint16_t ATI_GAIN_NUM         = 0x0006;
const uint16_t ATI_BAUD_ADDRESS     = 0x001F;
const uint16_t ATI_MODE_ADDRESS     = 0x001E;
uint8_t UNLOCK_GAGE_REG[]       = {SERVER_ID,0x6A,0xaa};
uint8_t TOLOCK_GAGE_REG[]       = {SERVER_ID,0x6A,0x18};
uint8_t START_STREAMING[]       = {SERVER_ID,0x46,0x55};
uint8_t STOP_STREAMING[]       = {SERVER_ID,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
const int RAW_REQ_LOCK_CMD_LEN          = 3;
const int RAW_STREAM_CMD_LEN            = 3;
const int RAW_STREAM_STP_LEN            = 20;
int ATI_GAGES[6] = {0};
#define BIAS_NUM    700
float ATI_FT_DATA[6] = {0.0};
float ATI_FT_BIAS[6] = {0.0};
float ATI_FT_BIAS_MATRIX[BIAS_NUM][6] = {{0.0}};
struct Calibration{
  uint8_t CalibSerialNumber[8]  ;
  uint8_t CalibPartNumber[32]   ;
  uint8_t CalibFamilyId[4]      ;
  uint8_t CalibTime[20]         ;
  float   BasicMatrix[6][6]     ;
  uint8_t ForceUnits            ;
  uint8_t TorqueUnits           ;
  float   MaxRating[6]          ;
  int     CountsPerForce        ;
  int     CountsPerTorque       ;
  uint16_t GageGain[6]          ;
  uint16_t GageOffset[6]        ;
};
struct Calibration Calibration_ONE;

// uint8_t CalibSerialNumber[8]  = {0};
// uint8_t CalibPartNumber[32]   = {0};
// uint8_t CalibFamilyId[4]      = {0};
// uint8_t CalibTime[20]         = {0};
// float   BasicMatrix[6][6]     = {0};
// uint8_t ForceUnits            =  0 ;
// uint8_t TorqueUnits           =  0 ;
// float   MaxRating[6]          = {0};
// int     CountsPerForce        =  0 ;
// int     CountsPerTorque       =  0 ;
// uint16_t GageGain[6]          = {0};
// uint16_t GageOffset[6]        = {0};


int ATI_READ_CALIBRATION(int id);
int ATI_PRINT_CALIBRATION(int id);
int ATI_READ_GAGE(int id);
int ATI_WRITE_GAGE(int id);
int ATI_START_STREAMING(int id);
void STOP_STREAM(int signal);
#endif /* _CONNECT_ATI_FT_H_ */
