


/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include "spi_opt.h"
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


#include <QDebug>


spi_opt::spi_opt()
{

}

int spi_opt::spi_init(int device_id, char clk_pha, char clk_pol)
{
    char mode = SPI_MODE_2;
    char bits = 8;
    int speed = 10000000;
    int ret;
    qDebug("spi_init \n");
    if (device_id) {
        // Unused variable - fix compiler warning
    }
    if (clk_pha) {
        // Unused variable - fix compiler warning
    }
    if (clk_pol) {
        // Unused variable - fix compiler warning
    }

    spidev_fd0 = open(SPIDEV_DEV0, O_RDWR);
    if (spidev_fd0 < 0) {
        qDebug("%s: Can't open device\n\r", __func__);
        return -1;
    }

    ret = ioctl(spidev_fd0, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        qDebug("%s: Can't set spi mode\n\r", __func__);
        return ret;
    }

    ret = ioctl(spidev_fd0, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        qDebug("%s: Can't set bits per word\n\r", __func__);
        return ret;
    }

    ret = ioctl(spidev_fd0, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        qDebug("%s: Can't set max speed hz\n\r", __func__);
        return ret;
    }

    spidev_fd1 = open(SPIDEV_DEV1, O_RDWR);
    if (spidev_fd1 < 0) {
        qDebug("%s: Can't open device\n\r", __func__);
        return -1;
    }

    ret = ioctl(spidev_fd1, SPI_IOC_WR_MODE, &mode);
    if (ret == -1) {
        qDebug("%s: Can't set spi mode\n\r", __func__);
        return ret;
    }

    ret = ioctl(spidev_fd1, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1) {
        qDebug("%s: Can't set bits per word\n\r", __func__);
        return ret;
    }

    ret = ioctl(spidev_fd1, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1) {
        qDebug("%s: Can't set max speed hz\n\r", __func__);
        return ret;
    }



//    unsigned int data[8] = {0x000018, 0x01A6D7,0x01A7A3, 0x01A870, 0x01A93D,\
//                       0x01AA0A,\
//                       0x01AB17,\
//                       0x000501
//                       };

//    char p[3];
//    char *p_temp;

//     for(int i=0; i<8; i++){
//     p_temp = (char*)&data[i];
//     p[0] = p_temp[2];
//     p[1] = p_temp[1];
//     p[2] = p_temp[0];
//     qDebug("0x%x", p[0]);
//     qDebug("0x%x", p[1]);
//     qDebug("0x%x", p[2]);
//     spi_write_then_read(0, (const unsigned char*)p, 3, 0, 0);
//     spi_write_then_read(1, (const unsigned char*)p, 3, 0, 0);
//     }
    return 0;
}

/***************************************************************************//**
 * @brief spi_write_then_read
*******************************************************************************/

int spi_opt::spi_write_then_read(int spi_fd, const unsigned char *txbuf, unsigned n_tx, unsigned char *rxbuf, unsigned n_rx)
{
    int ret = 0;

    if(spi_fd == 0)
        spi_fd = spidev_fd0;
    else
        spi_fd = spidev_fd1;

    struct spi_ioc_transfer tr;
    memset((void*)&tr, 0, sizeof(struct spi_ioc_transfer));
    tr.tx_buf = (unsigned long)txbuf;
    tr.len = n_tx;
    tr.delay_usecs = 0;
    tr.speed_hz = 1000000;
    tr.bits_per_word = 8;
    tr.rx_buf = 0;
    tr.cs_change = 0;
    tr.delay_usecs  = 0;
    ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);

    if (ret == 1) {
        printf("%s: Can't send spi message\n\r", __func__);
        return -EIO;
    }

    return ret;
}

/**
 * SPI multiple bytes register read.
 * @param reg The register address.
 * @param rbuf The data buffer.
 * @param num The number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
unsigned int spi_opt::spi0_readm(unsigned int reg, unsigned char *rbuf, unsigned int num)
{
    unsigned char  buf[2];
    unsigned int ret;
    unsigned short cmd;

    if (num > MAX_MBYTE_SPI)
        return -EINVAL;

    cmd = AD_READ | AD_CNT(num) | AD_ADDR(reg);
    buf[0] = cmd >> 8;
    buf[1] = cmd & 0xFF;

    ret = spi_write_then_read(spidev_fd0, &buf[0], 1, rbuf, num);
    if (ret < 0) {
        qDebug("Read Error\n");
        return ret;
    }
    return ret;
}

/**
 * SPI register write.
 * @param reg The register address.
 * @param val The value of the register.
 * @return 0 in case of success, negative error code otherwise.
 */
unsigned int spi_opt::spi0_write(unsigned int reg, unsigned int val)
{
    uint8_t buf[3];
    int32_t ret;
    uint16_t cmd;

    char *p;
    p = (char*)&reg;

    buf[0] = p[1];
    buf[1] = p[0];
    buf[2] = (char)val;
    ret = spi_write_then_read(0, buf, 3, NULL, 0);
    if (ret < 0) {
        qDebug("Write Error\n");;
        return ret;
    }
    return ret;
}

/**
 * SPI multiple bytes register read.
 * @param reg The register address.
 * @param rbuf The data buffer.
 * @param num The number of bytes to read.
 * @return 0 in case of success, negative error code otherwise.
 */
unsigned int spi_opt::spi1_readm(unsigned int reg, unsigned char *rbuf, unsigned int num)
{
    unsigned char  buf[2];
    unsigned int ret;
    unsigned short cmd;

    if (num > MAX_MBYTE_SPI)
        return -EINVAL;

    cmd = AD_READ | AD_CNT(num) | AD_ADDR(reg);
    buf[0] = cmd >> 8;
    buf[1] = cmd & 0xFF;

    ret = spi_write_then_read(1, &buf[0], 2, rbuf, num);
    if (ret < 0) {
        qDebug("Read Error\n");
        return ret;
    }
    return ret;
}

/**
 * SPI register write.
 * @param reg The register address.
 * @param val The value of the register.
 * @return 0 in case of success, negative error code otherwise.
 */
unsigned int spi_opt::spi1_write(unsigned int reg, unsigned int val)
{
    uint8_t buf[3];
    int32_t ret;
    uint16_t cmd;
    char *p;
    p = (char*)&reg;
    buf[0] = p[1];
    buf[1] = p[0];
    buf[2] = (char)val;
    ret = spi_write_then_read(1, buf, 3, NULL, 0);
    if (ret < 0) {
        qDebug("Write Error\n");;
        return ret;
    }
    return ret;
}


