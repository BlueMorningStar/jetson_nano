.globl  _start
_start:
    mov x0,#0x5
    adr x0,vectors
    msr vbar_el1,x0   //异常向量表基地址
    svc #0x02   //系统调用
reset_end:
    b   reset_end

do_bad_sync:
  mov x2,#1
  b reset_end

    .align 11 
    //2^11=2048   整个异常向量表  2K对齐  -> 通过对齐，实现向量表空间的预留
    //16个异常，每个异常32条指令，16*32*4=2048
    //16个异常，这里使用前8个

vectors:
/**********************sp0**************************/
    //---同步异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_sync
    //---irq异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_irq
      //---fiq异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_fiq
  //---serror异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_serror
    /**********************sp_elx*************************/
        //---同步异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_sync
    //---irq异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_irq
      //---fiq异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_fiq
  //---serror异常
    .align 7  //2^7 1000 0000=0x80
    mov x0,#1
    b   do_bad_serror