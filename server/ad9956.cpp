#include "ad9956.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <QThread>


#define MAP_SIZE 0X10000

#define SPI_CS  0
#define SPI_CLK 0X1
#define SPI_MOSI  0X2
#define SPI_REG_EN 0X3

#define FIRST_JUMP 0X4
#define SECOND_JUMP 0X5
#define ENABLE_JUMP 0X6


ad9956::ad9956()
{
    volatile unsigned int addr;
    fdAD9956 = open("/dev/mem", O_RDWR | O_SYNC);
    if(fdAD9956 < 0 ) {
        printf("ad9956 error open mem\n");
        return;
    }

    pData  = (volatile  unsigned int*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fdAD9956, 0x43c30000);
    if(pData < 0) {
        printf("ad9956 error mmap\n");
        close(fdAD9956);
        return;
    }
    qDebug("vir addr=0x%x\n", pData);

    spi_cs = pData + SPI_CS;
    qDebug("vir spi_cs=0x%x\n", spi_cs);
    spi_clk = pData + SPI_CLK;
    qDebug("vir spi_clk=0x%x\n", spi_clk);
    spi_mosi = pData + SPI_MOSI;
    qDebug("vir spi_mosi=0x%x\n", spi_mosi);
    spi_fix = pData + SPI_REG_EN;
    qDebug("vir spi_fix=0x%x\n", spi_fix);
    first_jump = pData + FIRST_JUMP;
    qDebug("vir first_jump=0x%x\n", first_jump);
    second_jump = pData + SECOND_JUMP;
    qDebug("vir second_jump=0x%x\n", second_jump);
    enable_jump = pData + ENABLE_JUMP;
    qDebug("vir enable_jump=0x%x\n", enable_jump);
    *spi_cs = 0xffffffff;
    *spi_clk = 0x00000000;
    *spi_mosi = 0x00000000;
    *spi_fix = 0x00000000;
    *first_jump = 0;
    *second_jump = 0;
    *enable_jump = 0;



}

void ad9956::refush_update()
{
    *spi_cs = 0xffffffff;
    QThread::msleep(10);
    *spi_fix = 0x00000000;
    QThread::msleep(10);
    *spi_fix = 0xffffffff;
    QThread::msleep(10);
    *spi_fix = 0x00000000;
}


void ad9956::write_command(unsigned char data)
{

    unsigned char temp = 0x80;
    *spi_cs = 0x00000000;
    *spi_mosi = 0x00000000;
    for(int i=0; i<8; i++)
    {
        *spi_clk = 0x00000000;
        QThread::msleep(1);
        if((data & temp) == 0x00)
        {
            *spi_mosi = 0x00000000;
        }
        else
        {
            *spi_mosi = 0xffffffff;
        }
        QThread::msleep(1);
        *spi_clk=0xffffffff;
        QThread::msleep(1);

        temp = temp >> 1;
    }
}

void ad9956::wirte_buffer(unsigned char *buf, int length)
{
    for(int i=0; i<length; i++)
    {
        write_command(buf[i]);
        QThread::msleep(10);
    }

}


void ad9956::set_hop0(unsigned int first)
{
    *first_jump = first;
    qDebug("0x%x", *first_jump);

}

void ad9956::set_hop1(unsigned int second)
{
    *second_jump = second;
    qDebug("0x%x", *second_jump);

}

void ad9956::enable_hop()
{
    *enable_jump = 0xffffffff;
    qDebug("0x%x", *enable_jump);
}


void ad9956::disable_hop()
{
    qDebug("disable_hop");
    *enable_jump = 0;
    *first_jump = 0;
    *second_jump = 0;
}
