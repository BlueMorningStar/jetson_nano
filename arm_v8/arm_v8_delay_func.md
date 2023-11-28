b.ne label //不等时跳转到标号
cbz w10,1f //w10值等于0的时候，跳转到1f
ret //子程序返回指令，返回地址默认保存在LR(X30),代替了mov pc,lr

.globl _start
_start:
    mov x2,0
    bl delay
    mov x3,0
reset_end:
    b reset_end

delay:
    ldr x4,=0x03
loop_delay:
    sub x4,x4,#1
    cmp x4,#0
    cbz x4,delay_end
    b.ne  loop_delay

delay_end:
ret
