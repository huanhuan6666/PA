#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/
make_instr_func(lea_rm2r_v)
{
    int len = 1;
    OPERAND r,rm;
    r.data_size = rm.data_size = data_size;
    len += modrm_r_rm(eip + 1, &r, &rm);
    print_asm_2("lea",r.data_size == 8?"b":(r.data_size == 16?"w":"l"),len,&rm,&r);
    r.val = rm.addr;
    operand_write(&r);
    
    return len;
}