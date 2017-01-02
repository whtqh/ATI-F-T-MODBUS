/*
 * Copyright © 2008-2010 Stéphane Raimbault <stephane.raimbault@gmail.com>
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
 * Code on 2016-12-25
 * By Warheat
 * Goal API using Modbus to getdata from Ati's F/T sensor (mini 45/58)
 * Using libmodbus APIs.(Just using Holding registers and Custom requests.)
 * file based on unit-test.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <modbus.h>     /* Modbus Interface                   */

#include <fcntl.h>      /*  File Control Definitions          */
#include <stropts.h>    /*  Custom Baud Rate Settings         */
#include <asm/termios.h>
#include <unistd.h>     /*  UNIX Standard Definitions         */
#include <errno.h>      /*  ERROR Number Definitions          */

#include <signal.h>     /*  Catch Ctrl-C Signal               */

#include "connect_ATI_FT.h"

int main(int argc, char *argv[])
{
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = STOP_STREAM;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT,&sigIntHandler,NULL);

    printf("** ATI F/T Sensor TESTING **\n");
    while (1) {
      if(ATI_READ_CALIBRATION(1) == 1)
      {
        if(ATI_PRINT_CALIBRATION(1) == 1)
        {
          if(ATI_READ_GAGE(1) == 1)
          {
            if(ATI_WRITE_GAGE(1) == 1)
            {
              break;
            }
            else{
              printf("Error in ATI_WRITE_GAGE\n");
              printf("GAGE Now is:\n");
              ATI_READ_GAGE(1);
            }
          }
          else{
            printf("Error in ATI_READ_GAGE\n");
          }
        }
        else{
          printf("Error in ATI_PRINT_CALIBRATION\n");
        }
      }
      else{
        printf("Error in ATI_READ_CALIBRATION\n");
      }
    }


    //return 0;
    ATI_START_STREAMING(1);
    //return 0;
    struct termios2 SerialPortSettings;
  	int fd;

  	if ((fd = open("/dev/ttyUSB0",O_RDWR|O_NOCTTY)) == -1)
  	{
  		printf("Error opening PTS\n");
  		return -1;
  	}

  	if (ioctl(fd, TCGETS2, &SerialPortSettings) < 0)
  	{
  		printf("Failed to get\n");
  		return -1;
  	}

  	SerialPortSettings.c_cflag &= ~CBAUD;
  	SerialPortSettings.c_cflag |= BOTHER;
  	SerialPortSettings.c_ispeed = 1250000;
  	SerialPortSettings.c_ospeed = 1250000;
    SerialPortSettings.c_cflag |=   PARENB;
    SerialPortSettings.c_cflag &= ~ PARODD; //EVEN

    SerialPortSettings.c_cflag &= ~CSTOPB; //Stop bits = 1
    SerialPortSettings.c_cflag &= ~CSIZE;
    SerialPortSettings.c_cflag |=  CS8;
    SerialPortSettings.c_cflag |= CREAD | CLOCAL;
    SerialPortSettings.c_cc[VMIN] = 13; //读取字符的最小数量
    SerialPortSettings.c_cc[VTIME] = 0; //读取第一个字符的等待时间

  	if (ioctl(fd, TCSETS2, &SerialPortSettings) < 0)
  	{
  		printf("Failed to set\n");
  		return -1;
  	}
    printf("Successfully baudrate Set\n");
    uint8_t read_buffer[100];
    int  bytes_read = 0;
    int SuccessRead = 0;
    int Counts = 0;
    int i,j;

    while (1) {
      bytes_read = read(fd,&read_buffer,13);
      printf("*****************READ %d********************\n",bytes_read );
      uint8_t CheckSum = 0;
      for(i = 0;i<12;i++)
      {
        CheckSum = CheckSum + read_buffer[i];
        //printf("%2x ",read_buffer[i] );
      }
      if((read_buffer[12] & 0x80) == 0x80)
      {
        printf("STATUS ERROR\n" );
      }
      else if( (CheckSum & 0x7f) != (read_buffer[12]& 0x7f))
      {
        printf("CheckSum = %2x\nCheckSum received = %2x\n",CheckSum,read_buffer[12] );
      }
      else
      {
        printf("CheckSum OK\n");
        ATI_GAGES[0] = (int)(((uint16_t)read_buffer[0]<<8) + (uint16_t)read_buffer[1]) ;
        ATI_GAGES[2] = (int)(((uint16_t)read_buffer[2]<<8) + (uint16_t)read_buffer[3]) ;
        ATI_GAGES[4] = (int)(((uint16_t)read_buffer[4]<<8) + (uint16_t)read_buffer[5]) ;
        ATI_GAGES[1] = (int)(((uint16_t)read_buffer[6]<<8) + (uint16_t)read_buffer[7]) ;
        ATI_GAGES[3] = (int)(((uint16_t)read_buffer[8]<<8) + (uint16_t)read_buffer[9]) ;
        ATI_GAGES[5] = (int)(((uint16_t)read_buffer[10]<<8) + (uint16_t)read_buffer[11]) ;
        for(int i = 0;i<6;i++)
        {
          if(ATI_GAGES[i] > 32767)
          {
            ATI_GAGES[i] = ATI_GAGES[i] - 65536;  //Change it to int16
          }
          printf("%d ", ATI_GAGES[i]);
        }

        printf("\n" );

        for(int i = 0;i<6;i++)
        {
          ATI_FT_DATA[i] = 0.0;
          for( j = 0;j<6;j++)
          {
            ATI_FT_DATA[i] = ATI_FT_DATA[i] + Calibration_ONE.BasicMatrix[i][j] * (float)ATI_GAGES[j];
          }
          if(i < 3)
          {
            ATI_FT_DATA[i] = ATI_FT_DATA[i] / Calibration_ONE.CountsPerForce;
            //printf("%f\t",ATI_FT_DATA[i] / Calibration_ONE.CountsPerForce );
          }
          else
          {
            ATI_FT_DATA[i] = ATI_FT_DATA[i] / Calibration_ONE.CountsPerTorque;
            //printf("%f\t",ATI_FT_DATA[i] / Calibration_ONE.CountsPerTorque );
          }
        }
        printf("\n");

        if(SuccessRead < BIAS_NUM)
        {
          for(int i = 0;i<6;i++)
          {
            ATI_FT_BIAS_MATRIX[SuccessRead][i] = ATI_FT_DATA[i];
          }
        }
        else if(SuccessRead == BIAS_NUM)
        {
          for(int i = 0;i<6;i++)
          {
            int sum = 0;
            for(int j = 0;j<BIAS_NUM;j++)
            {
              sum = sum + ATI_FT_BIAS_MATRIX[j][i];
            }
            ATI_FT_BIAS[i] = sum / BIAS_NUM;
          }
        }
        else{
          for(int i = 0;i<6;i++)
          {
              printf("%f\t",ATI_FT_DATA[i] - ATI_FT_BIAS[i]);
          }
        }
        SuccessRead = SuccessRead +1;
      }
      printf("\n" );
      Counts = Counts + 1;
      printf("Succes %d times in %d \n",SuccessRead, Counts );
    }
    return 1;
}
void STOP_STREAM(int signal) {
  printf("Caught Signal %d\n", signal);
  modbus_t *ctx;
  int req_length;

  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);
  modbus_set_debug(ctx, TRUE);//TRUE WILL print the DEBUG INFO

  modbus_set_error_recovery(ctx,
                            MODBUS_ERROR_RECOVERY_LINK |
                            MODBUS_ERROR_RECOVERY_PROTOCOL);
  //Try to Connect with the slava server.
  modbus_set_slave(ctx, SERVER_ID);
  printf("SERVER_ID: %d\n",SERVER_ID );
  if (modbus_connect(ctx) == -1) {
      fprintf(stderr, "Connection failed: %s\n",
              modbus_strerror(errno));
      modbus_free(ctx);
      return;
  }

  req_length = modbus_send_raw_request(ctx, STOP_STREAMING,
                                       RAW_STREAM_STP_LEN * sizeof(uint8_t));

  printf("* modbus_send_raw_request: ");
  if ( req_length == (RAW_STREAM_STP_LEN + 2)) {
      printf("\e[32m%s\e[0m\n","[--OK--]");
  } else {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%d)\n", req_length);
  }
  exit(1);
}
int ATI_READ_GAGE(int id)
{
  uint16_t *tab_rp_registers;
  modbus_t *ctx;
  int nb_points;
  int rc;
  int i;
  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);
  modbus_set_debug(ctx, TRUE);//TRUE WILL print the DEBUG INFO

  modbus_set_error_recovery(ctx,
                            MODBUS_ERROR_RECOVERY_LINK |
                            MODBUS_ERROR_RECOVERY_PROTOCOL);
  //Try to Connect with the slava server.
  modbus_set_slave(ctx, SERVER_ID);
  printf("SERVER_ID: %d\n",SERVER_ID );
  if (modbus_connect(ctx) == -1) {
      fprintf(stderr, "Connection failed: %s\n",
              modbus_strerror(errno));
      modbus_free(ctx);
      return -1;
  }
  nb_points = ATI_GAGE_NUM;
  //Set the Num as 0x0068 to get first half of the data.
  //malloc and set zero.
  tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
  memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

  rc = modbus_read_registers(ctx, ATI_GAGE_REG_ADDRESS,
                             ATI_GAGE_NUM, tab_rp_registers);
  if (rc != ATI_GAGE_NUM) {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d)\n", rc);
      /* Close the connection */
      modbus_close(ctx);
      modbus_free(ctx);
      return -1;
  }
  for(i = 0;i<ATI_GAGE_NUM;i++)
  {
    printf("%d\n", tab_rp_registers[i]);
  }
  free(tab_rp_registers);

  modbus_close(ctx);
  modbus_free(ctx);
  return 1;
}
int ATI_WRITE_GAGE(int id)
{
  uint16_t *tab_rp_registers;
  modbus_t *ctx;
  int nb_points;
  int rc;
  int i;
  int req_length;
  uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];



  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);
  modbus_set_debug(ctx, TRUE);//TRUE WILL print the DEBUG INFO

  modbus_set_error_recovery(ctx,
                            MODBUS_ERROR_RECOVERY_LINK |
                            MODBUS_ERROR_RECOVERY_PROTOCOL);
  //Try to Connect with the slava server.
  modbus_set_slave(ctx, SERVER_ID);
  printf("SERVER_ID: %d\n",SERVER_ID );
  if (modbus_connect(ctx) == -1) {
      fprintf(stderr, "Connection failed: %s\n",
              modbus_strerror(errno));
      modbus_free(ctx);
      return -1;
  }

  req_length = modbus_send_raw_request(ctx, UNLOCK_GAGE_REG,
                                       RAW_REQ_LOCK_CMD_LEN * sizeof(uint8_t));

  printf("* modbus_send_raw_request: ");
  if ( req_length == (RAW_REQ_LOCK_CMD_LEN + 2)) {
      printf("\e[32m%s\e[0m\n","[--OK--]");
  } else {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%d)\n", req_length);
  }

  printf("* modbus_receive_confirmation: ");
  rc  = modbus_receive_confirmation(ctx, rsp);
  /******************************WRITE GAGE REG*****************************/

  nb_points = ATI_GAGE_NUM;
  //Set the Num as 0x0068 to get first half of the data.
  //malloc and set zero.
  tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
  memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));
  for(i = 0;i<ATI_GAGE_NUM;i++)
  {
    if(i<ATI_GAIN_NUM)
    {
      tab_rp_registers[i] = Calibration_ONE.GageGain[i];
    }
    else
    {
      tab_rp_registers[i] = Calibration_ONE.GageOffset[i-ATI_GAIN_NUM];
    }
  }
  rc = modbus_write_registers(ctx, ATI_GAGE_REG_ADDRESS,
                              ATI_GAGE_NUM, tab_rp_registers);
  if (rc == ATI_GAGE_NUM ) {
      printf("\e[32m%s\e[0m\n","[---GAGE-DATA-WRITE--OK--]");
  } else {
      printf("\e[31m%s\e[0m\n", "[-GAGE-DATA-WRITE-FAILD!--]");printf("\n\n");
      return -1;
  }
  /*********************LOCK GAIN REG *******************************/
  req_length = modbus_send_raw_request(ctx, TOLOCK_GAGE_REG,
                                       RAW_REQ_LOCK_CMD_LEN * sizeof(uint8_t));

  printf("* modbus_send_raw_request: ");
  if ( req_length == (RAW_REQ_LOCK_CMD_LEN + 2)) {
      printf("\e[32m%s\e[0m\n","[--OK--]");
  } else {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%d)\n", req_length);
  }

  printf("* modbus_receive_confirmation: ");
  rc  = modbus_receive_confirmation(ctx, rsp);

  /****************FREE MALLOC & CTX　**************************************/
  free(tab_rp_registers);

  modbus_close(ctx);
  modbus_free(ctx);

  return 1;
}

int ATI_START_STREAMING(int id)
{
  modbus_t *ctx;
  int rc;
  int req_length;
  uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];

  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);
  modbus_set_debug(ctx, TRUE);//TRUE WILL print the DEBUG INFO

  modbus_set_error_recovery(ctx,
                            MODBUS_ERROR_RECOVERY_LINK |
                            MODBUS_ERROR_RECOVERY_PROTOCOL);
  //Try to Connect with the slava server.
  modbus_set_slave(ctx, SERVER_ID);
  printf("SERVER_ID: %d\n",SERVER_ID );
  if (modbus_connect(ctx) == -1) {
      fprintf(stderr, "Connection failed: %s\n",
              modbus_strerror(errno));
      modbus_free(ctx);
      return -1;
  }

  req_length = modbus_send_raw_request(ctx, START_STREAMING,
                                       RAW_STREAM_CMD_LEN * sizeof(uint8_t));

  printf("* modbus_send_raw_request: ");
  if ( req_length == (RAW_STREAM_CMD_LEN + 2)) {
      printf("\e[32m%s\e[0m\n","[--OK--]");
  } else {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%d)\n", req_length);
  }

  printf("* modbus_receive_confirmation: ");
  rc  = modbus_receive_confirmation(ctx, rsp);
  if(rc!=5)
    return -1;
  printf("rc = %d\n", rc);

  modbus_close(ctx);
  modbus_free(ctx);

  return 1;
}
int ATI_READ_CALIBRATION(int id)
{
  uint16_t *tab_rp_registers;
  modbus_t *ctx;
  int nb_points;
  int rc;
  int i,j;
  float real;
  uint32_t temp;

  START_ADDRESS = ATI_CALI_REG_ADDRESS + (id - 1) * ATI_CALI_STEP;

  //ctx = modbus_new_rtu("/dev/pts/25", 1250000, 'E', 8, 1);
  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);

  modbus_set_debug(ctx, TRUE);//TRUE WILL print the DEBUG INFO

  modbus_set_error_recovery(ctx,
                            MODBUS_ERROR_RECOVERY_LINK |
                            MODBUS_ERROR_RECOVERY_PROTOCOL);
  //Try to Connect with the slava server.
  modbus_set_slave(ctx, SERVER_ID);
  printf("SERVER_ID: %d\n",SERVER_ID );
  if (modbus_connect(ctx) == -1) {
      fprintf(stderr, "Connection failed: %s\n",
              modbus_strerror(errno));
      modbus_free(ctx);
      return -1;
  }

  /* Allocate and initialize the memory to store the registers */
  nb_points = ATI_CALI_NUM_1;
  //Set the Num as 0x0068 to get first half of the data.
  //malloc and set zero.
  tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
  memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

  rc = modbus_read_registers(ctx, START_ADDRESS,
                             ATI_CALI_NUM_1, tab_rp_registers);
  if (rc != ATI_CALI_NUM_1) {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d)\n", rc);
      /* Close the connection */
      modbus_close(ctx);
      modbus_free(ctx);
      return -1;
  }

  printf("\e[32m%s\e[0m\n","[--OK--]");
  for(i = 0;i<32;i++)
  {
    // printf("Holding Register [%d] = %c%c\n",i,
    //         (tab_rp_registers[i]&0xFF00)>>8, tab_rp_registers[i]&0x00FF);
    if(i < 4)
    {
      Calibration_ONE.CalibSerialNumber[i*2] = (tab_rp_registers[i]&0xFF00)>>8;
      Calibration_ONE.CalibSerialNumber[i*2+1] = tab_rp_registers[i]&0x00FF;
    }
    else if(i < 20)
    {
      Calibration_ONE.CalibPartNumber[(i-4)*2] = (tab_rp_registers[i]&0xFF00)>>8;
      Calibration_ONE.CalibPartNumber[(i-4)*2+1] = tab_rp_registers[i]&0x00FF;
    }
    else if(i < 22)
    {
      Calibration_ONE.CalibFamilyId[(i-20)*2] = (tab_rp_registers[i]&0xFF00)>>8;
      Calibration_ONE.CalibFamilyId[(i-20)*2+1] = tab_rp_registers[i]&0x00FF;
    }
    else
    {
      Calibration_ONE.CalibTime[(i-22)*2] = (tab_rp_registers[i]&0xFF00)>>8;
      Calibration_ONE.CalibTime[(i-22)*2+1]=tab_rp_registers[i]&0x00FF;
    }
  }
  //Some Problem has been found in NULL end ASCII code from Calibration
  Calibration_ONE.CalibSerialNumber[7] = 0x00;
  Calibration_ONE.CalibPartNumber[31] = 0x00;
  Calibration_ONE.CalibFamilyId[3] = 0x00;
  Calibration_ONE.CalibTime[19] = 0x00;

  for(i = 0;i<6;i++)
  {
    for(j = 0;j < 6;j++)
    {
      //real = modbus_get_float(tab_rp_registers + 31 + (i*6 +j)*2 ); //????? Have a tiny Bug
      temp = (((uint32_t)tab_rp_registers[32 + (i*6 +j)*2]) << 16) + tab_rp_registers[32 + (i*6 +j)*2+1];
      memcpy(&real, &temp, sizeof(float));
      //printf("%f\t", real);
      Calibration_ONE.BasicMatrix[i][j] = real;
    }
  }
  free(tab_rp_registers);

  /**********************************************************************/
  // PART 2
  /**********************************************************************/

  nb_points = ATI_CALI_NUM_2;
  //Set the Num as 0x0068 to get first half of the data.
  //malloc and set zero.
  tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
  memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

  rc = modbus_read_registers(ctx, START_ADDRESS + ATI_CALI_NUM_1,
                             ATI_CALI_NUM_2, tab_rp_registers);
  if (rc != ATI_CALI_NUM_2) {
      printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d)\n", rc);
      /* Close the connection */
      modbus_close(ctx);
      modbus_free(ctx);
      return -1;
  }

  printf("\e[32m%s\e[0m\n","[--OK--]");

  // printf("Force&Torque Units = %d%d\n",
  //         (tab_rp_registers[0]&0xFF00)>>8, tab_rp_registers[0]&0x00FF);
  Calibration_ONE.ForceUnits = (tab_rp_registers[0]&0xFF00)>>8;
  Calibration_ONE.TorqueUnits = tab_rp_registers[0]&0x00FF;

  for(i = 0;i<6;i++)
  {
      //real = modbus_get_float(tab_rp_registers  + i*2);
      temp = (((uint32_t)tab_rp_registers[1 + i*2]) << 16) + tab_rp_registers[1 + i*2 + 1];
      memcpy(&real, &temp, sizeof(float));
      // printf("%f\n", real);
      Calibration_ONE.MaxRating[i] = real;
  }
  // printf("CountsPerForce  = %d\n",(int)((((uint32_t)tab_rp_registers[13])<<16) + ((uint32_t)tab_rp_registers[14])) );
  // printf("CountsPerTorque = %d\n",(int)((((uint32_t)tab_rp_registers[15])<<16) + ((uint32_t)tab_rp_registers[16])) );
  Calibration_ONE.CountsPerForce = (int)((((uint32_t)tab_rp_registers[13])<<16) + ((uint32_t)tab_rp_registers[14]));
  Calibration_ONE.CountsPerTorque = (int)((((uint32_t)tab_rp_registers[15])<<16) + ((uint32_t)tab_rp_registers[16]));

  for(i = 0;i<6;i++)
  {
    //printf("GageGain[%d]    = %d\n",i,tab_rp_registers[17+i] );
    Calibration_ONE.GageGain[i] = tab_rp_registers[17+i];
  }
  for(i = 0;i<6;i++)
  {
    // printf("GageOffset[%d]  = %d\n",i,tab_rp_registers[23+i] );
    Calibration_ONE.GageOffset[i] = tab_rp_registers[23+i];
  }
  free(tab_rp_registers);

  modbus_close(ctx);
  modbus_free(ctx);
  return 1;
}

int ATI_PRINT_CALIBRATION(int id)
{
  int i,j;
  printf("%s\n",Calibration_ONE.CalibSerialNumber );
  printf("%s\n",Calibration_ONE.CalibPartNumber );
  printf("%s\n",Calibration_ONE.CalibFamilyId );
  printf("%s\n",Calibration_ONE.CalibTime );
  for(i = 0;i<6;i++)
  {
    for(j = 0;j<6;j++)
    {
      printf("%f\t",Calibration_ONE.BasicMatrix[i][j] );
    }
    printf("\n");
  }
  switch (Calibration_ONE.ForceUnits) {
    case 1:
    printf("%s\n","Pound" );
    break;
    case 2:
    printf("%s\n","Newton(N)" );
    break;
    case 3:
    printf("%s\n","Kilopound" );
    break;
    case 4:
    printf("%s\n","KiloNewton" );
    break;
    case 5:
    printf("%s\n","Kilogram-equivalent force" );
    break;
    case 6:
    printf("%s\n","Gram-equivalent force" );
    break;
    default:
    printf("%s\n","Unkown Force Unit" );
    break;
  }
  switch (Calibration_ONE.TorqueUnits) {
    case 1:
    printf("%s\n","Pound-inch" );
    break;
    case 2:
    printf("%s\n","Pound-foot" );
    break;
    case 3:
    printf("%s\n","Newton-meter(N×m)" );
    break;
    case 4:
    printf("%s\n","Newton-milimeter" );
    break;
    case 5:
    printf("%s\n","Kilogram(-equivalent)-centimeter" );
    break;
    case 6:
    printf("%s\n","KiloNewton-meter" );
    break;
    default:
    printf("%s\n","Unkown Torque Unit" );
    break;
  }
  for(i = 0;i<6;i++)
  {
    printf("%f\t",Calibration_ONE.MaxRating[i] );
  }
  printf("\n");
  printf("CountsPerForce: %d\n",Calibration_ONE.CountsPerForce );
  printf("CountsPerTorque:%d\n",Calibration_ONE.CountsPerTorque );

  for(i = 0;i<6;i++)
  {
    printf("%d\n",Calibration_ONE.GageGain[i] );
  }
  for(i = 0;i<6;i++)
  {
    printf("%d\n",Calibration_ONE.GageOffset[i] );
  }
  return 1;
}
