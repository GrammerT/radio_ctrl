#ifndef AD9956_H
#define AD9956_H


class ad9956
{
public:
    ad9956();
private:
    int fdAD9956;
    volatile unsigned int *pData;
    volatile unsigned int *spi_cs;
    volatile unsigned int *spi_clk;
    volatile unsigned int *spi_mosi;
    volatile unsigned int *spi_fix;

    volatile unsigned int *first_jump;
    volatile unsigned int *second_jump;
    volatile unsigned int *enable_jump;
public:
    void wirte_buffer(unsigned char *buf ,int length);
    void set_hop0(unsigned int first);
    void set_hop1(unsigned int second);
    void enable_hop();
    void disable_hop();

    void refush_update();
    void write_command(unsigned char data);

};

#endif // AD9956_H
