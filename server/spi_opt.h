#ifndef SPI_OPT_H
#define SPI_OPT_H

#include <QObject>

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

/*
*	SPI Comm Helpers
*/
#define AD_READ		(0 << 15)
#define AD_WRITE		(1 << 15)
#define AD_CNT(x)	((((x) - 1) & 0x7) << 12)
#define AD_ADDR(x)	((x) & 0x3FF)
#define MAX_MBYTE_SPI 8
#define SPIDEV_DEV0 "/dev/spidev32765.0"
#define SPIDEV_DEV1 "/dev/spidev32766.0"


class spi_opt
{
public:
    spi_opt();

public:
    int spidev_fd0;
    int spidev_fd1;
    int spi_init(int device_id, char  clk_pha, char  clk_pol);
    int spi_write_then_read(int spi_fd,	const unsigned char *txbuf, unsigned n_tx,	unsigned char *rxbuf, unsigned n_rx);
    unsigned int spi0_readm(unsigned int  reg,	unsigned char *rbuf, unsigned int num);
    unsigned int spi0_write(unsigned int reg, unsigned int val);
    unsigned int spi1_readm(unsigned int  reg, unsigned char *rbuf, unsigned int num);
    unsigned int spi1_write(unsigned int reg, unsigned int val);
};

#endif // SPI_OPT_H
