#include "cpu/cpu.h"
//some functions for add 
void set_CF_add(uint32_t result, uint32_t src, size_t data_size)
{
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)),data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)),data_size);
    cpu.eflags.CF = result < src;
}
void set_ZF(uint32_t result, size_t data_size)
{
    result = result & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.ZF = (result == 0);
}
void set_SF(uint32_t result, size_t data_size)
{
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)),data_size);
    cpu.eflags.SF = sign(result);
}
void set_PF(uint32_t result)
{
   result = result & 0xFF;
   int num = 0;
   while(result > 0)
   {
       if(result & 1) ++num;
       result >>= 1;
   }
   if((num % 2) == 0) cpu.eflags.PF = 1;
   else cpu.eflags.PF = 0;
}
void set_OF_add(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
    switch(data_size)
    {
        case 8:
               result = sign_ext(result & 0xFF, 8);
               src = sign_ext(src & 0xFF, 8);
               dest = sign_ext(dest & 0xFF, 8);
               break;
        case 16:
               result = sign_ext(result & 0xFFFF, 16);
               src = sign_ext(src & 0xFFFF, 16);
               dest = sign_ext(dest & 0xFFFF, 16);
               break;
        default: break;
    }
    if(sign(src)==sign(dest))
    {
        if(sign(src) != sign(result))
            cpu.eflags.OF = 1;
        else 
            cpu.eflags.OF = 0;
    }
    else
        cpu.eflags.OF = 0;
}

void set_CF_adc(uint32_t result, uint32_t src, size_t data_size)
{
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)),data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)),data_size);
    if(cpu.eflags.CF == 0)
        cpu.eflags.CF = (result < src);
    else
        cpu.eflags.CF = (result <= src);
}
//functuons for add have been finished

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
uint32_t res = 0;
res = dest + src;

set_CF_add(res,src,data_size);
set_PF(res);
//set_AF();
set_ZF(res,data_size);
set_SF(res,data_size);
set_OF_add(res, src, dest, data_size);

return res & (0xFFFFFFFF>>(32 - data_size));//高位清零并返回
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
uint32_t res = 0;
res = dest + src + cpu.eflags.CF;

set_CF_adc(res,src,data_size);
set_PF(res);
//set_AF();
set_ZF(res,data_size);
set_SF(res,data_size);
set_OF_add(res, src, dest, data_size);

return res & (0xFFFFFFFF >> (32 - data_size));
}

//add have been finished

//functuons for sub
void set_CF_sub(uint32_t src, uint32_t dest, size_t data_size)
{
    src = sign_ext(src & (0xFFFFFFFF >>(32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >>(32 - data_size)), data_size);
    if(sign(dest)==0 && sign(src)==1)
    {
        cpu.eflags.CF = 1;
    }
    else if(sign(dest)==1 && sign(src)==0)
    {
        cpu.eflags.CF = 0;
    }
    else
    {
        if(dest < src) cpu.eflags.CF = 1;
        else cpu.eflags.CF = 0;
    }
}

void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest, size_t data_size)
{
    result = sign_ext(result & (0xFFFFFFFF>>(32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >>(32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >>(32 - data_size)), data_size);
    if(sign(dest)==0 && sign(src)==1)
    {
        if(sign(result) == 1)
        cpu.eflags.OF = 1;
        else cpu.eflags.OF = 0;
    }
    else if(sign(dest)==1 && sign(src)==0)
    {
        if(sign(result) == 0)
        cpu.eflags.OF = 1;
        else cpu.eflags.OF = 0;
    }
    else 
        cpu.eflags.OF = 0;
}
uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
    uint32_t res = 0;
    res = dest - src;
    
    set_CF_sub(src,dest,data_size);
    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
}
void set_CF_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
    src = sign_ext(src & (0xFFFFFFFF >>(32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >>(32 - data_size)), data_size);
    if(cpu.eflags.CF == 0)
    {
        if(sign(dest)==0 && sign(src)==1)
            cpu.eflags.CF = 1;
        else if(sign(dest)==1 && sign(src)==0)
            cpu.eflags.CF = 0;
        else
        {
            if(dest < src)
                cpu.eflags.CF = 1;
            else 
                cpu.eflags.CF = 0;
        }
    }
    else 
    {
        if(sign(dest)==0 && sign(src)==1)
            cpu.eflags.CF = 1;
        else if(sign(dest)==1 && sign(src)==0)
            cpu.eflags.CF = 0;
        else if(dest <= src)
            cpu.eflags.CF = 1;
        else
            cpu.eflags.CF = 0;
    }
}
uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
    uint32_t res = 0;
    res = dest - src - cpu.eflags.CF;
    
    set_CF_sbb(src,dest,data_size);
    set_PF(res);
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
}
//functions for mul
void set_CF_OF_mul(uint64_t result, size_t data_size)
{
    result = result >> data_size;
    if(result == 0) 
    {
        cpu.eflags.CF = 0;
        cpu.eflags.OF = 0;
    }
    else 
    {
        cpu.eflags.CF = 1;
        cpu.eflags.OF = 1;
    }
}
void set_CF_OF_imul(int64_t result, size_t data_size)
{
    int64_t tem = result;
    tem = tem >> data_size;
    result = result >> (data_size -1);
    if((tem == 0 && ((result & 0x1) == 0)) || (~tem == 0 && ((result & 0x1) == 1)))
    { 
        cpu.eflags.OF = 0;
        cpu.eflags.CF = 0;
    }
    else
    {
        cpu.eflags.OF = 1;
        cpu.eflags.CF = 1;
    }
}
uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    src = src & (0xFFFFFFFF >> (32 - data_size));
    uint64_t res = (uint64_t)dest * (uint64_t)src;
    res = res & (0xFFFFFFFFFFFFFFFF >> (64 - 2*data_size));
    set_CF_OF_mul(res, data_size);
    return res;
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    
    int64_t res = (int64_t)dest * (int64_t)src;
    //res = res & (0xFFFFFFFFFFFFFFFF >> (64 - 2*data_size));
    set_CF_OF_imul(res ,data_size);
    return res;
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
    assert(src);
    dest = dest & (0xFFFFFFFFFFFFFFFF >> (64 - data_size));
    src = src & (0xFFFFFFFFFFFFFFFF >> (64 - data_size));
    uint32_t res = 0;
    res = dest / src;
    return res & (0xFFFFFFFF >> (32 - data_size));
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
    assert(src);
    dest = sign_ext_64(dest & (0xFFFFFFFFFFFFFFFF >> (64 - data_size)), data_size);
    src = sign_ext_64(src & (0xFFFFFFFFFFFFFFFF >> (64 - data_size)), data_size);
    int32_t res = 0;
    res = dest / src;
    return res;
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
    uint32_t res = 0;
    res = dest % src;
    return res;
}

int32_t alu_imod(int64_t src, int64_t dest)
{
    int32_t res = 0;
    res = dest % src;
    return res;
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    src = src & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest & src;
    cpu.eflags.CF = 0;
    cpu.eflags.OF = 0;
    set_SF(res, data_size);
    set_ZF(res, data_size);
    set_PF(res);
    return res;
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    src = src & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest ^ src;
    cpu.eflags.CF = 0;
    cpu.eflags.OF = 0;
    set_SF(res, data_size);
    set_ZF(res, data_size);
    set_PF(res);
    return res;
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = dest & (0xFFFFFFFF >> (32 - data_size));
    src = src & (0xFFFFFFFF >> (32 - data_size));
    uint32_t res = 0;
    res = dest | src;
    cpu.eflags.CF = 0;
    cpu.eflags.OF = 0;
    set_SF(res, data_size);
    set_ZF(res, data_size);
    set_PF(res);
    return res;
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)),data_size);
    uint32_t res = 0;
    if(src == 1)
    {
        if(sign(dest)==1)
            cpu.eflags.CF = 1;
        else 
            cpu.eflags.CF = 0;
        res = dest << src;
        if(sign(sign_ext(res, data_size)) != sign(dest))
            cpu.eflags.OF = 1;
        else
            cpu.eflags.OF = 0;
    }
    else if(src == 0)
    {
        return dest;
    }
    else
    {
        res = dest << (src - 1);
        if(sign(sign_ext(res, data_size)) == 1)
            cpu.eflags.CF = 1;
        else
            cpu.eflags.CF = 0;
        res = res << 1;
    }
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = dest & 0xFFFFFFFF >> (32 - data_size);
    uint32_t res = 0;
    if(src == 1)
    {
        if((dest & 0x00000001) == 1)
            cpu.eflags.CF = 1;
        else 
            cpu.eflags.CF = 0;
        res = dest >> src;
        if((res & 0x00000001) != (dest & 0x00000001))
            cpu.eflags.OF = 1;
        else 
            cpu.eflags.OF = 0;
    }
    else if(src == 0)
    {
        return dest;
    }
    else
    {
        res = dest >> (src -1);
        if((res & 0x00000001) == 1)
            cpu.eflags.CF = 1;
        else
            cpu.eflags.CF = 0;
        res = res >> 1;
    }
    set_SF(res, data_size);
    set_ZF(res, data_size);
    set_PF(res);
    return res;
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = sign_ext(dest & 0xFFFFFFFF >> (32 - data_size),data_size);
    uint32_t res = 0;
    if(src == 1)
    {
        if((dest & 0x00000001) == 1)
            cpu.eflags.CF = 1;
        else 
            cpu.eflags.CF = 0;
        res = dest >> src;
        if((res & 0x00000001) != (dest & 0x00000001))
            cpu.eflags.OF = 1;
        else 
            cpu.eflags.OF = 0;
    }
    else if(src == 0)
    {
        return dest;
    }
    else
    {
        res = dest >> (src -1);
        if((res & 0x00000001) == 1)
            cpu.eflags.CF = 1;
        else
            cpu.eflags.CF = 0;
        res = res >> 1;
    }
    if(sign(dest) == 1)
        res = res | 0xFFFFFFFF << (data_size - src);
    set_SF(res, data_size);
    set_ZF(res, data_size);
    set_PF(res);
    return res & 0xFFFFFFFF >> (32 - data_size);
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)),data_size);
    uint32_t res = 0;
    if(src == 1)
    {
        if(sign(dest)==1)
            cpu.eflags.CF = 1;
        else 
            cpu.eflags.CF = 0;
        res = dest << src;
        if(sign(sign_ext(res, data_size)) != sign(dest))
            cpu.eflags.OF = 1;
        else
            cpu.eflags.OF = 0;
    }
    else if(src == 0)
    {
        return dest;
    }
    else
    {
        res = dest << (src - 1);
        if(sign(sign_ext(res, data_size)) == 1)
            cpu.eflags.CF = 1;
        else
            cpu.eflags.CF = 0;
        res = res << 1;
    }
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    return res & (0xFFFFFFFF >> (32 - data_size));
}
