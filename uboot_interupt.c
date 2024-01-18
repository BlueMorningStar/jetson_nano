#include <asm/gic.h>
#include <asm/io.h>

/*
屏蔽原来的do_irq
void do_irq(struct pt_regs *pt_regs, unsigned int esr)
{
    efi_restore_gd();
    printf("\"Irq\" handler, esr 0x%08x\n", esr);
    show_regs(pt_regs);
    show_efi_loaded_images(pt_regs);
    panic("Resetting CPU ...\n");
}
*/

#define TIMER_PERIOD 1000
void do_irq(struct pt_regs *pt_regs, unsigned int esr)
{
    int irq;
    printk("o irq go");
    irq = readl(GICC_BASE+GICC_IAR);
    if((irq&0x3FF)==30)
    {
        set_physical_timer(TIMER_PERIOD);//定时器中断触发后，重新设置定时器
        printf("%s.%d\n",__FUNCTION__,__LINE__);
    }
    writel(irq,GICC_BASE+GICC_EOIR);
}