#include "memdev.h"

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



#define MAP_SIZE 0X10000
#define PHY_ADDR 0X43C20000



memdev::memdev()
{
    init_memmap();
}

memdev::~memdev()
{
    release();
}

int memdev::init_memmap()
{
    fd_mem = open("/dev/mem", O_RDWR | O_SYNC);
    if(fd_mem < 0 ) {
        printf("error open mem\n");
        return -1;
    }

    p_regs  = (volatile  unsigned int*)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, PHY_ADDR);
    if(p_regs < 0) {
        printf("error mmap\n");
        close(fd_mem);
        return -1;
    }
    return 0;
}

int memdev::write_back_reg(int n, unsigned int val){
    int ret;
    p_regs[n] = val;
    ret = p_regs[n];
    return ret;
}
int memdev::release(void){
    munmap((void*)p_regs, MAP_SIZE);
    close(fd_mem);
    fd_mem = 0;
    p_regs  = NULL;
    return 0;
}


