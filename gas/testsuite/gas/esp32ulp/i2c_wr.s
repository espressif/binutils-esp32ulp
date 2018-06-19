	 .data
foodata:	.word 42
	.text
	.global BBB
	.global AAA
	.global CCC
	.global QQQ
	.global __start
	.global ooo1
	.global ooo2
	.global ooo3
	.set KKK, 0x03
	.set ddd, 0x2222

__start:
//        nop
CCC:     nop
        i2c_wr  0x11, 2, 1,1,1
        nop
ooo1:   nop
        ST     r3, r2, CCC
