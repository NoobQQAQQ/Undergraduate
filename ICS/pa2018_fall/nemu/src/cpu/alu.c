#include "cpu/cpu.h"

// for all instructions
void set_ZF(uint32_t result, size_t data_size) 
{
	result = result & (0xFFFFFFFF >> (32 - data_size));
	cpu.eflags.ZF = (result == 0);
}

void set_SF(uint32_t result, size_t data_size) 
{
	result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
	cpu.eflags.SF = sign(result);
}

void set_PF(uint32_t result) 
{
	result ^= (result >> 4);
	result ^= (result >> 2);
	result ^= (result >> 1);
	cpu.eflags.PF = ~(result & 1);
}
// -------end globel functions here-------

// add set flags 
void set_OF_add(uint32_t res, uint32_t src, uint32_t dest, size_t data_size) 
{
	// do sign extend here
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	// end sign extend here

	if (sign(src) == sign(dest)) 
	{
		if (sign(src) != sign(res))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} 	
	else
		cpu.eflags.OF = 0;
}

void set_CF_add(uint32_t res, uint32_t src, size_t data_size)
{
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	cpu.eflags.CF = res < src;
}
// end add set flags here

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	uint32_t res = src + dest;

	set_CF_add(res, src, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_add(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

// adc set flags here
void set_OF_adc(uint32_t res, uint32_t src, uint32_t dest, size_t data_size) 
{
	// do sign extend here
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	// end sign extend here

	if (sign(src) == sign(dest)) 
	{
		if (sign(src) != sign(res))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} 	
	else
		cpu.eflags.OF = 0;
}

void set_CF_adc(uint32_t res, uint32_t src, uint32_t dest, size_t data_size)
{
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);

	if (src == 0)
		cpu.eflags.CF = res < dest;
	else if (dest == 0)
		cpu.eflags.CF = res < src;
	else
		cpu.eflags.CF = res <= src;
}
// end adc set flags here

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	uint32_t res = src + dest + cpu.eflags.CF;

	set_OF_adc(res, src, dest, data_size);
	set_CF_adc(res, src, dest, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}

// sub set flags here
void set_OF_sub(uint32_t res, uint32_t src, uint32_t dest, size_t data_size) 
{
	// do sign extend here
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	// end sign extend here

	if (sign(src) != sign(dest)) 
	{
		if (sign(src) == sign(res))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} 	
	else
		cpu.eflags.OF = 0;
}

void set_CF_sub(uint32_t src, uint32_t dest, size_t data_size)
{
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	cpu.eflags.CF = dest < src;
}
// end sub set flags here

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//eturn 0;
	uint32_t res = dest + (~src + 1);

	set_CF_sub(src, dest, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sub(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}

// sbb set flags here
void set_OF_sbb(uint32_t res, uint32_t src, uint32_t dest, size_t data_size) 
{
	// do sign extend here
	res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	// end sign extend here

	if (sign(src) != sign(dest)) 
	{
		if (sign(src) == sign(res))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	} 	
	else
		cpu.eflags.OF = 0;
}

void set_CF_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
	if (src == dest && cpu.eflags.CF == 1)
		cpu.eflags.CF = 1;
	else
		cpu.eflags.CF = dest < src;
}
// end sbb set flags here

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	uint32_t res = dest + ~(src + cpu.eflags.CF) + 1;

	set_CF_sbb(src, dest, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_sbb(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}


uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	uint64_t res = (uint64_t)dest * (uint64_t)src;
	cpu.eflags.CF = cpu.eflags.OF = !((res >> data_size) == 0);

	return res;	
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	src = sign_ext(src, data_size);
	dest = sign_ext(dest, data_size);
	int64_t res = (int64_t)dest * (int64_t)src;
	

	return res;	
#endif
}

uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFFFFFFFFFF >> (64 - data_size));
	assert(src);
	dest &= (0xFFFFFFFFFFFFFFFF >> (64 - data_size));
	return dest / src;
#endif
}

int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	assert(src);
	return dest / src;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest) {
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	return dest % src;	
#endif
}

int32_t alu_imod(int64_t src, int64_t dest) {
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	return dest % src;
#endif
}


uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	uint32_t res = dest & src;
	
	cpu.eflags.CF = cpu.eflags.OF = 0;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));

#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	uint32_t res = dest ^ src;
	
	cpu.eflags.CF = cpu.eflags.OF = 0;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));


#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	uint32_t res = dest | src;
	
	cpu.eflags.CF = cpu.eflags.OF = 0;
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));

#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));

	if (src == 1)
	{
		cpu.eflags.OF = (dest >> (data_size - 1))^(dest >> (data_size - 2));
		cpu.eflags.CF = (dest >> (data_size - 1));
	}
	else
		cpu.eflags.CF = ((dest << ((src % 32) - 1)) >> (data_size - 1)) & 1;

	uint32_t res = dest << (src % 32);
	
	
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));

#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));

	if (src == 1)
	{
		cpu.eflags.OF = (dest >> (data_size - 1));
		cpu.eflags.CF = dest & 1;
	}
	else
		cpu.eflags.CF = (dest >> ((src % 32) - 1)) & 1;

	uint32_t res = dest >> (src % 32);
	
	
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));

#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);	
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	src &= (0xFFFFFFFF >> (32 - data_size));
	dest &= (0xFFFFFFFF >> (32 - data_size));
	dest = sign_ext(dest, data_size);

	if (src == 1)
	{
		cpu.eflags.OF = 0;
		cpu.eflags.CF = dest & 1;
	}
	else
		cpu.eflags.CF = (dest >> ((src % 32) - 1)) & 1;

	uint32_t res = ((int)dest) >> (src % 32);
	
	
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));

#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size) {
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	//return 0;
	return alu_shl(src, dest, data_size);

#endif
}
