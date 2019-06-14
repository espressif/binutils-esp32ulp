	.text
    .set const1, 216
    .set const2, 0x3ff48014

    .set bit_mask, 0x8000


start:
    reg_rd 12, 7, 0
    reg_rd 0x3ff48000, 7, 0

    reg_rd const1, 7, 0
//    reg_rd const2, 7, 0
    nop
    nop
    nop

