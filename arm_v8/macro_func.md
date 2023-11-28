.macro myAdd,x,y //宏函数，类似myAdd(x,y)
add \x,\x,\y //反斜杠用来传参数
.endm
myAdd,x0,x2



.macro switch_el,xreg,el1_label1,el2_label
    mrs \xreg,CurrentEl
    cmp \xreg,#0x04
    b.eq \el1_label1
    cmp \xreg,0x08
    b.eq \el1_label2
.endm

.globl _start
_start:
mov x0,#0x4
switch_el x0,1f,2f
reset_end:
    b reset_end

1:
    mov x1,#1
    b reset_end

2:
    mov x2,#1 
    b reset_end