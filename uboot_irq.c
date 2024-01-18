//在cmd/boot.c的末尾添加下面程序
#include <common.h>
#include <command.h>
#include <asm/gic.h>
#include <asm/io.h>


void set_physical_timer(int timeout_ms)
{
    unsigned long value, freq, cnt,cmp;
    //关闭定时器
    value=0;
        asm volatile("msr CNTP_CTL_EL0, %0": : "r"(value)); //输入参数不用"="变为只读
    //设置定时器的超时的时间
        asm volatile("mrs %0, CNTFRQ_EL0" : "=r" (freq));   //输出参数用"="变为只读
        asm volatile("mrs %0, CNTPCT_EL0 " : "=r"(cnt));
        cmp=cnt+(freq/1000)*timeout_ms;//freq单位是HZ,除以1000对应的就是毫秒
        asm volatile("msr CNTP_CVAL_EL0, %0": : "r"(cmp)); //把比较值存入比较寄存器

        //打开定时器
        value=1;
        asm volatile("msr CNTP_CTL_EL0, %0" : : "r"(value));
}

void timer_gic_init(void)
{
    uint32_t value;
    //打开GIC Distributor总中断搜GICD_CTLR在5.3 ARM通用中断规范
    value = readl(GICD_BASE+GICD_CTLR);//打开GIC Distributor总中断搜GICD_CTLR在5.3 ARM通用中断规范
    //IHIOO69H_gic_architecture_specification.pdf
    value |= 1; //0b1 Group 0 interrupt signaling is enabled.
    writel(value,GICD_BASE+GICD_CTLR);
    //打开Non-secure physical timer中断
    value = readl(GICD_BASE+GICD_ISENABLERn);
    value |=(1<<30);
    writel(value,GICD_BASE+GICD_ISENABLERn);

    //打开GIC CPU interface总中断
    value= readl(GICC_BASE+GICC_CTLR);
    value |=1; //EnableGrp0 bit0=1: 打开CPU interface group0的总中断
               //EnableGrp1 bit1=1:打开CPU interface group1的中断
    writel(value, GICC_BASE+GICC_CTLR);
}

irq_init()
{
    unsigned long value, cur_el;
    //1.打开中断
    asm volatile("msr daifclr, #2");//清除daif的2号位 ->打开中断
    //源头：5.1. ARMV8技术手册 DDI0487G_a_armv8_arm.pdf 搜daifclr
    //见： DAIF 中断屏蔽位（为1时屏蔽） 见 DAIF对应位的清除
    // bit[9:6]: D A I F D(断电观察点)  A:SError I(IRQ) F(FIQ)
    //daifclr专用于DAIF对应位的清除，dairset专用于DAIF对应位的置1
    //asm volatile("msr daifset, #2"); //关闭中断

    //2.配置中断路由：如果当前处于EL2级别，需要设置HCR_EL2寄存器，将IRQ路由凹EL2
    asm volatile("mrs %0, CurrentEL" : "=r" (cur_el));
    if(cur_el == 0x8)
    {
        //IMO, bit [4]
        // physical IRQ Routing
        // 0b1 When xexcuting at any Exeption level, and EL2 is enabled in the current Security state:
        asm volatile("mrs %, HCR_EL2" : "=r"(value));
        value |=(1<<4);
        asm volatile("msr HCR_EL2, %0": : "r" (value));
    }
}

static int do_timer_irq(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    printf("yhai test timer interrupt \n");
    irq_init();
    timer_gic_init();
    set_physical_timer(1000);//1000ms
    return 0;
}

U_BOOT_CMD(
    timer_irq, CONFIG_SYS_MAXARGS,1, do_timer_irq,
    "timer irq 1000ms",
    "timer irq\n"
    "timer irq"
);