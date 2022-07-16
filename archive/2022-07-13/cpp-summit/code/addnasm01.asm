
global add_f_asm, add_d_asm, add_vf_asm, add_vd_asm
section .text

add_f_asm:
    addss XMM0, XMM1
    ret

add_d_asm:
    addsd XMM0, XMM1
    ret

add_vf_asm:
    addps XMM0, XMM1
    ret

add_vd_asm:
    addpd XMM0, XMM1
    ret
