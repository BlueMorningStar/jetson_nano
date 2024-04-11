#include <linux/module.h>  //模块的头文件
#include <asm/io.h>   //io操作的头文件
//#define UART_A     0X70006000
#define UART_B     0X70006040  //本例程用uart2
// #define UART_C     0X70006200
// #define UART_D     0X70006300
// #define UART_E     0X70006400

#define UART_A_CLK_CTR          0X60006178
#define PINMUX_BASE             0X70000000
#define PINMUX_UART2_TX_0      *(int*)(PINMUX_BASE+0X30F4)       
#define PINMUX_UART2_RX_0      *(int*) (PINMUX_BASE+0X30F8)

#define UARTB_BASE              0X70006040
#define UARTB_MCR               *(int*) (UARTB_BASE+0X10)
#define UARTB_FCR               *(int*)(UARTB_BASE+0X8)
#define UART_THR_DLAB_0_0       *(int*)(UARTB_BASE+0x0)
#define UART_LCR                *(int*)(UARTB_BASE+0XC)      


#define CLK_RST_BASE             0X60006000
#define CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0  *(int*)(CLK_RST_BASE+0X10)
#define CLK_UARTB_SOURCE        *(int*)(CLK_RST_BASE+0X17C)

typedef struct {
    volatile unsigned int32_t UART_A_CLK_CTR;

}

unsigned char * pinmux_uart2_tx;  
unsigned char * pinmux_uart2_rx;
unsigned char * uart2_mcr;
unsigned char * uart2_fcr;
unsigned char * uart2_thr;
unsigned char * uart2_clk_out_enb;
unsigned char * uart2_clk_source;
unsigned char * uart2_lcr;

/****************************************************
 * Use Uart2
 *          uart program guidline
1. Program pin mux settings to select a UART
2. Enable the UART clocks
3. For enabling internal loopback, program MCR[4] to 1
4. Program FCR[0] to 0
5. Enable interrupts in the IER register as needed
6. Write data into the THR register
7. Wait for a THR interrupt, if enabled, or poll for LSR[5]
8. During a receive, wait for an RDR interrupt or poll for LSR[0]/LSR[9]
9. Read the UART.LSR register to clear interrupts
******************************************************/

typedef struct pinset
{   
    int reg;
    int readdata;
    int writedata;
}pinset_t;

pinset_t [7] pinset_array;
pinset_array[0]->reg=UART_LCR;
int uart_init(void)
{
    printk("uart init strt------------3\n");
    //set uart 8 data bits 1 stop bit no parity
    uart2_lcr = ioremap(UART_LCR,8);
    if(uart2_lcr == NULL)
    {
        printk("uart2_lcr ioremap error-------3\n");
        return -1;
    }
    writel(0x03,uart2_lcr); //     0000 0011

    // baudrate
    //1. Program pin mux settings to select a UART
    //TX
    pinmux_uart2_tx = ioremap(PINMUX_UART2_TX_0,8);
    if(pinmux_uart2_tx == NULL)
    {
        printk("pinmux_uart2_tx ioremap error-------3\n");
        return -1;
    }
    writel(0x07,pinmux_uart2_tx);
    //RX
    pinmux_uart2_rx = ioremap(PINMUX_UART2_RX_0,8);
    if(pinmux_uart2_rx == NULL)
    {
        printk("pinmux_uart2_rx ioremap error-------3\n");
        return -1;
    }
    writel(0x07,pinmux_uart2_rx);
    //2. Enable the UART clocks
    uart2_clk_out_enb=ioremap(CLK_RST_CONTROLLER_CLK_OUT_ENB_L_0,8);
    if(uart2_clk_out_enb == NULL)
    {
        printk("uart2_clk_out_enb ioremap error-------3\n"); 
        return -1;
    }
    writel((readl(uart2_clk_out_enb)|0x08),uart2_clk_out_enb);

    // uart2_clk_source=ioremap(CLK_UARTB_SOURCE,8);
    // if(uart2_clk_source == NULL)
    // {
    //     printk("uart2_clk_source ioremap error-------3\n");
    //     return -1;
    // }
    // writel(0xC1000002,uart2_clk_source);

    //3. For enabling internal loopback, program MCR[4] to 1
    uart2_mcr=ioremap(UARTB_MCR,8);
    if(uart2_mcr == NULL)
    {
        printk("uart2_mcr ioremap error-------3\n");
        return -1;
    }
    writel(readl(uart2_mcr)| 1 << 4, uart2_mcr);

    //4. Program FCR[0] to 0
    uart2_fcr=ioremap(UARTB_FCR,8);
    if(uart2_fcr == NULL)
    {
        printk("uart2_fcr ioremap error-------3\n");
        return -1;
    }
    writel(readl(uart2_fcr)| ~(1 << 0), uart2_fcr); //turn off FIFO

    //5. Enable interrupts in the IER register as needed(PASS)
    //6. Write data into the THR register
    uart2_thr=ioremap(UART_THR_DLAB_0_0,8);
    if(uart2_thr == NULL)
    {
        printk("uart2_thr ioremap error-------3\n");
        return -1;
    }
    writel(0X5A, uart2_thr);
    return 0; 
}

 
void uart_exit(void)
{
    iounmap(pinmux_uart2_tx);
    iounmap(pinmux_uart2_rx);
    iounmap(uart2_mcr);
    iounmap(uart2_fcr);
    iounmap(uart2_thr);
    iounmap(uart2_lcr);
    printk("uart exit \n")
}

module_init(uart_init);
module_exit(uart_exit);
MODULE_LICENSE("GPL v2");