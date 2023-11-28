.equ GPIO3_BASE, 0X6000D200
.equ GPIO3_CNF,  0X6000D204
.equ GPIO3_OE,   0X6000D214
.equ GPIO3_OUT,  0X6000D224
.text
//set pin 12 (GPIO3_PJ.07 GPIO Mode)
    ldr X6,=GPIO3_CNF   //save addr in X6
    ldr X1,=0X8080       //save value 1000 0000 1000 0000
    ldr X2,[X6]         //read value in X6 addr
    ldr X3,=0X4         //CNF OFFSET

    orr X0,X2,X1        //give new value to X6 addr and save in X0
    str X0,[X6]         //write X0 to addr in X6

//set pin 12 (GPIO3_PJ.07 drive the signal out)

    ldr X6,=GPIO3_OE
    ldr X1,=0X80        //1000 0000
    ldr X2,[X6]
    orr X0,X2,X1
    str X0,[X6]

//OUTPUT HIGH Set BIT 7 to 1
    ldr X6,=GPIO3_OUT
    ldr X1,=0X80
    ldr X2,[X6]
    orr X0,X2,X1
    str X0,[X6]
    .end


