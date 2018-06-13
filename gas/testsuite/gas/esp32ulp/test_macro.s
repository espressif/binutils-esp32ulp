	.text
    .set ttt, 4
    .set bit_mask, 0x8000
    .global SPI_Trans_Byte
    .global __start


.macro readRegister reg
//   .label ret1
//   .global  ret1
//     .global ret1\@
//     .global ret2\@
//     .global ret3\@
//   .text
     nop;
     jump   ret1\@;
     jump   ret2\@;
     jump   ret3\@;
     move   r1, ret1\@;      // here we reference the address ret1
     move   r1, ret2\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
     move   r1, ret3\@;      // here we reference the address ret1
ret1\@: nop;
     nop;
     nop;
     nop;
ret2\@: nop;
ret3\@: nop;
        nop;
        nop;
        nop;
.endm


__start:
SPI_Trans_Byte:    nop
//    move   r1, twst // dddddd
    readRegister 1
    readRegister 2
//    move   r1, twst // dddddd
twst:
    nop

