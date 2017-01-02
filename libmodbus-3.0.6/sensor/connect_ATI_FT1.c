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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.


 * Code on 2016-12-25
 * By Warheat
 * Goal API using Modbus to getdata from Ati's F/T sensor (mini 45/58)
 * Using libmodbus APIs.(Just using Holding registers and Custom requests.)
 * file based on unit-test.c
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

#include "connect_ATI_FT.h"

enum {
    TCP,
    TCP_PI,
    RTU
};
int ATI_READ_CALIBRATION(int id)
{
  uint16_t *tab_rp_registers;
  modbus_t *ctx;
  int nb_points;


  START_ADDRESS = ATI_CALI_REG_ADDRESS + (id - 1) * ATI_CALI_STEP;

  //ctx = modbus_new_rtu("/dev/pts/25", 1250000, 'E', 8, 1);
  ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);

  modbus_set_debug(ctx, TRUE);
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
  nb_points = UT_REGISTERS_NB;
  //Set the Num of
  tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
  memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));


  free(tab_rp_registers);
  /* Close the connection */
  modbus_close(ctx);
  modbus_free(ctx);
  return 0;
}

int main(int argc, char *argv[])
{
    uint16_t *tab_rp_registers;
    modbus_t *ctx;
    int i;
    int nb_points;
    int rc;
    float real;
    uint32_t ireal;
    int use_backend;
    use_backend = RTU;
    //ctx = modbus_new_rtu("/dev/pts/25", 1250000, 'E', 8, 1);
    ctx = modbus_new_rtu("/dev/ttyUSB0", 1250000, 'E', 8, 1);

    modbus_set_debug(ctx, TRUE);
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
    nb_points = UT_REGISTERS_NB;
    //Set the Num of
    tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
    memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

    printf("** ATI F/T Sensor TESTING **\n");

    /** HOLDING REGISTERS **/

    /* Single register */
    //Write
    // rc = modbus_write_register(ctx, UT_REGISTERS_ADDRESS, 0x1234);
    // printf("1/2 modbus_write_register: ");
    // if (rc == 1) {
    //     printf("\e[32m%s\e[0m\n","[--OK--]");
    // } else {
    //     printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n\n");
    //     goto close;
    // }
/*


int fd;
usleep(10);
fd = open("/dev/ttyUSB0",O_RDWR|O_NOCTTY);
if(fd == 1)
{
  printf("\n  Error! in Opening ttyUSB0\n");
}
else
{
  printf("\n  ttyUSB0 Opened Successfully\n");
}
struct termios SerialPortSettings;
tcgetattr(fd, &SerialPortSettings);
cfsetispeed(&SerialPortSettings,B2500000);
cfsetospeed(&SerialPortSettings,B2500000);

ioctl(fd, TCGETS2, &SerialPortSettings);
SerialPortSettings.c_cflag &= ~CBAUD;
SerialPortSettings.c_cflag |= BOTHER;
SerialPortSettings.c_ispeed = 1250000;
SerialPortSettings.c_ospeed = 1250000;

if (ioctl(fd, TCSETS2, &SerialPortSettings) != 0) {
  perror("ioctl failed to set baudrate");
  return;
}

SerialPortSettings.c_cflag |= PARENB;
SerialPortSettings.c_cflag &= PARODD;

SerialPortSettings.c_cflag &= ~CSTOPB; //Stop bits = 1
SerialPortSettings.c_cflag &= ~CSIZE;
SerialPortSettings.c_cflag |=  CS8;
SerialPortSettings.c_cflag |= CREAD | CLOCAL;

SerialPortSettings.c_cc[VMIN]  = 13; 
SerialPortSettings.c_cc[VTIME] = 0;
tcflush(fd,TCIOFLUSH);//Flush
tcsetattr(fd,TCSANOW,&SerialPortSettings);
uint8_t read_buffer[13];
int  bytes_read = 0;
int SuccessRead = 0;
int Counts = 0;
while (1) {
  bytes_read = read(fd,&read_buffer,13);
  printf("*****************READ %d********************\n",bytes_read );
  uint8_t CheckSum = 0;
  for(i = 0;i<12;i++)
  {
    CheckSum = CheckSum + read_buffer[i];
    printf("%2x\n",read_buffer[i] );
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
    SuccessRead = SuccessRead +1;
  }
  usleep(1);
  Counts = Counts + 1;
  printf("Succes %d times in %d \n",SuccessRead, Counts );
}






close(fd);





*/
    /* End of single register */

    /* Many registers */
    // rc = modbus_write_registers(ctx, UT_REGISTERS_ADDRESS,
    //                             UT_REGISTERS_NB, UT_REGISTERS_TAB);
    // printf("1/5 modbus_write_registers: ");
    // if (rc == UT_REGISTERS_NB) {
    //     printf("\e[32m%s\e[0m\n","[--OK--]");
    // } else {
    //     printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n\n");
    //     goto close;
    // }

    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               UT_REGISTERS_NB, tab_rp_registers);
    printf("2/5 modbus_read_registers: ");
    if (rc != UT_REGISTERS_NB) {
        printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d)\n", rc);
        goto close;
    }

    printf("\e[32m%s\e[0m\n","[--OK--]");
    for(int i = 0;i<UT_REGISTERS_NB;i++)
    {
      printf("Holding Register [%d] = %c%c\n",i,
              (tab_rp_registers[i]&0xFF00)>>8, tab_rp_registers[i]&0x00FF);
    }

    ATI_READ_CALIBRATION(1);
    goto close;







    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               3, tab_rp_registers);
    printf("3/5 modbus_read_registers (0): ");
    if (rc != -1 && errno == EMBMDATA) {
        printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d)\n", rc);
        goto close;
    }
    printf("\e[32m%s\e[0m\n","[--OK--]");

    nb_points = UT_REGISTERS_NB;
    memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));

    /* Write registers to zero from tab_rp_registers and store read registers
       into tab_rp_registers. So the read registers must set to 0, except the
       first one because there is an offset of 1 register on write. */
    rc = modbus_write_and_read_registers(ctx,
                                         UT_REGISTERS_ADDRESS + 1, UT_REGISTERS_NB - 1,
                                         tab_rp_registers,
                                         UT_REGISTERS_ADDRESS,
                                         UT_REGISTERS_NB,
                                         tab_rp_registers);
    printf("4/5 modbus_write_and_read_registers: ");
    if (rc != UT_REGISTERS_NB) {
        printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (nb points %d != %d)\n", rc, UT_REGISTERS_NB);
        goto close;
    }



    for (i=1; i < UT_REGISTERS_NB; i++) {
        if (tab_rp_registers[i] != 0) {
            printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%0X != %0X)\n",
                   tab_rp_registers[i], 0);
            goto close;
        }
    }
    printf("\e[32m%s\e[0m\n","[--OK--]");

    /* End of many registers */


    printf("\nTEST FLOATS\n");
    /** FLOAT **/
    printf("1/2 Set float: ");
    modbus_set_float(UT_REAL, tab_rp_registers);
    if (tab_rp_registers[1] == (UT_IREAL >> 16) &&
        tab_rp_registers[0] == (UT_IREAL & 0xFFFF)) {
        printf("\e[32m%s\e[0m\n","[--OK--]");
    } else {
        /* Avoid *((uint32_t *)tab_rp_registers)
         * https://github.com/stephane/libmodbus/pull/104 */
        ireal = (uint32_t) tab_rp_registers[0] & 0xFFFF;
        ireal |= (uint32_t) tab_rp_registers[1] << 16;
        printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%x != %x)\n", ireal, UT_IREAL);
        goto close;
    }

    printf("2/2 Get float: ");
    real = modbus_get_float(tab_rp_registers);
    if (real == UT_REAL) {
        printf("\e[32m%s\e[0m\n","[--OK--]");
    } else {
        printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%f != %f)\n", real, UT_REAL);
        goto close;
    }

    printf("\nAt this point, error messages doesn't mean the test has failed\n");
    goto close;
    /** RAW REQUEST */
    printf("\nTEST RAW REQUEST:\n");
    {
        const int RAW_REQ_LENGTH = 6;
        uint8_t raw_req[] = { (use_backend == RTU) ? SERVER_ID : 0xFF,
                              0x03, 0x00, 0x01, 0x0, 0x05 };
        int req_length;
        uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];

        req_length = modbus_send_raw_request(ctx, raw_req,
                                             RAW_REQ_LENGTH * sizeof(uint8_t));

        printf("* modbus_send_raw_request: ");
        if ((use_backend == RTU && req_length == (RAW_REQ_LENGTH + 2)) ||
            ((use_backend == TCP || use_backend == TCP_PI) &&
             req_length == (RAW_REQ_LENGTH + 6))) {
            printf("\e[32m%s\e[0m\n","[--OK--]");
        } else {
            printf("\e[31m%s\e[0m\n", "[--FAILD!--]");printf("\n (%d)\n", req_length);
            goto close;
        }

        printf("* modbus_receive_confirmation: ");
        rc  = modbus_receive_confirmation(ctx, rsp);
        if ((use_backend == RTU && rc == 15) ||
            ((use_backend == TCP || use_backend == TCP_PI) &&
             rc == 19)) {
            printf("\e[32m%s\e[0m\n","[--OK--]");
        } else {
            printf("\e[31m%s\e[0m\n", "[--FAILD!--]");
            printf("\n (%d)\n", rc);
            goto close;
        }
    }

    printf("\nALL TESTS PASS WITH SUCCESS.\n");

close:
    /* Free the memory */
    free(tab_rp_registers);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
