#ifndef MEMDEV_H
#define MEMDEV_H




#define DOWN_IF_ATT 0X0
#define SWITCH_FIR 0X1
#define UP_RF_ATT 0X2
#define UP_IF_ATT0 0X3
#define UP_IF_ATT1 0x4
#define RDO 0X5
#define SWITCH_3_4 0X6
#define DOWN_RF_ATT 0X7

class memdev
{
public:
    memdev();
    ~memdev();
public:
    int init_memmap(void);
    int write_back_reg(int n, unsigned int val);

    int release(void);
private:
    int fd_mem;
    volatile unsigned int *p_regs;


};

#endif // MEMDEV_H
