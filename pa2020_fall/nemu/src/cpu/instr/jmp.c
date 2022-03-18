#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}
make_instr_func(jmp_short_)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = 8;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, rel.data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 2, &rel);

        cpu.eip += offset;

        return 2;
}
make_instr_func(jmp_indirct)
{
    int len = 1;
    OPERAND address;
    address.data_size = data_size;
    len += modrm_rm(eip + 1, &address);
    operand_read(&address);
    if(data_size == 16)
        cpu.eip = address.val & 0xFFFF;
    else
        cpu.eip = address.val;
    return 0;
}
make_instr_func(ljmp)//complete in PA 3-2
{
    OPERAND address;
    address.type = OPR_IMM;
    address.sreg = SREG_CS;
    address.data_size = 32;//get the lower 32 bits
    address.addr = eip + 1;
    operand_read(&address);
    if(data_size == 16)
    {
        cpu.eip = address.val & 0xFFFF;
    }
    else if(data_size == 32)
    {
        cpu.eip = address.val;
    }
    
    OPERAND rel;//the upper 16 bits are cs val
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 16;
    rel.addr = eip + 5;
    operand_read(&rel);
    cpu.cs.val = rel.val;
    //cpu.cs.val = instr_fetch(eip + 5, 2);
    
    load_sreg(1);//load the SREG_CS
    return 0;
}
