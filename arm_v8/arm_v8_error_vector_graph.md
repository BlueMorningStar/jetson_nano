adr x0,vectors  //读取异常向量表的首地址

msr vbar_el1,x0 //设置异常表的基地址

svc #2          //系统调用（触发同步异常陷入，sp_el不同陷入不同异常）

![Alt text](image-1.png)
