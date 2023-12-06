#include <linux/module.h>  //模块的头文件
#include <asm/io.h>   //io操作的头文件


/*************************************************************
The following is a programming example for 7 byte writes from master to an external slave:
1. Program the I2C_CLK_DIVISOR register to get the required data rate based on I2C CLK_SOURCE register 
programming in CAR module.
2. Write slave address in I2C_CMD_ADDR0 register based on 7-bit/10-bit addressing mode.
3. Write first 4 bytes data in I2C_CMD_DATA1 register.
4. Write remaining 3 bytes in I2C_CMD_DATA2 register.
5. Program I2C_CNFG[DEBOUNCE_CNT] to the required value. 
6. Set I2C_CNFG[A_MOD] to Seven or Ten_bit addressing based on slave device address width.
7. Program I2C_CNFG [LENGTH]. This bit field works in (n+1) fashion so for 7 bytes transfer, 6 needs to be programmed 
in LENGTH bit field.
8. Set I2C_CNFG [SLV2] = 0 as this is one slave access.
9. Set CMD1 = 0 for write operation.
10. Set NOACK to 0 or 1 based on Slave type.
11. Set MSTR_CONFIG_LOAD bit in I2C_CONFIG_LOAD register.
12. Wait until I2C_CONFIG_LOAD[MSTR_CONFIG_LOAD] is auto-cleared by hardware to 0.
13. Set I2C_CNFG [SEND] to 1 to begin write transaction on the interface.
14. Wait until the transaction is complete. For this, either wait until interrupt is received or I2C_STATUS[BUSY] bit 
becomes zero.
15. Check I2C_STATUS [CMD1_STAT] to see if the transaction is successful or NOACK from the slave for any of the 
bytes transferred.
*********************************************************************/




void i2c_init(void)
{

}