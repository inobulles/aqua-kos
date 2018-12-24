
// https://c9x.me/x86/ ← really good references
// http://www.felixcloutier.com/x86/

#include "asm.h"
#include "thread.h"

static inline void cla_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t mtype, unsigned_t mdata, unsigned_t rtype, unsigned_t rdata) {
	unsigned_t right = rtype == TOKEN_NUMBER ? rdata : __this->main_thread.registers[rdata];
	
	#define CLA_SKIP_LEFT_NUMBER_CHECK 0
	
	#if CLA_SKIP_LEFT_NUMBER_CHECK
		#define left __this->main_thread.registers[ldata];
	#else
		unsigned_t left  = ltype == TOKEN_NUMBER ? ldata : __this->main_thread.registers[ldata];
	#endif
	
	switch (mdata) {
		case TOKEN_sub: __this->main_thread.registers[REGISTER_addr] = left - right; break;
		case TOKEN_add: __this->main_thread.registers[REGISTER_addr] = left + right; break;

		case TOKEN_mul1:
		case TOKEN_mul3:
		case TOKEN_mul: __this->main_thread.registers[REGISTER_addr] = left * right; break;
		
		default: {
			printf("WARNING Unknown address operation (%lld)\n", mdata);
			break;
			
		}
		
		
	}
	
}

static inline void mov_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	set_value(__this, ltype, ldata, get_value(__this, rtype, rdata));
	
}

static inline void movzx_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	set_value(__this, ltype, ldata, zero_extend(get_value(__this, rtype, rdata)));
	
}

static inline void movsx_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	set_value(__this, ltype, ldata, sign_extend(get_value(__this, rtype, rdata)));
	
}

static inline void jmp_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	//~ data--;
	
	//~ unsigned_t position = type == TOKEN_RESERVED ? __this->main_thread.reserved_positions[data - __this->main_thread.label_position_offset] : data;
	__this->main_thread.registers[REGISTER_rip] = get_value(__this, type, data) / sizeof(uint16_t);
	
}

static inline void push_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	thread_stack_push(&__this->main_thread, get_value(__this, type, data));
	
}

static inline void pop_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	set_value(__this, type, data, *((unsigned_t*) __this->main_thread.registers[REGISTER_rsp]));
	__this->main_thread.registers[REGISTER_rsp] += sizeof(unsigned_t); // pop
	
}

static inline void lea_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	if (rtype == TOKEN_ADDRESS) set_value(__this, ltype, ldata, __this->main_thread.registers[rdata]);
	else                        set_value(__this, ltype, ldata, get_value(__this, rtype, rdata));
	
}

static inline void cmp_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	__this->main_thread.condition_left  = get_value(__this, ltype, ldata);
	__this->main_thread.condition_right = get_value(__this, rtype, rdata);
	
	__this->main_thread.registers[REGISTER_zf] = (unsigned_t) (__this->main_thread.condition_left == __this->main_thread.condition_right);
	
}

static inline void test_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	unsigned_t temp = get_value(__this, ltype, ldata) & get_value(__this, rtype, rdata);
	
	if (temp == 0) {
		__this->main_thread.registers[REGISTER_zf] = 1;
		__this->main_thread.condition_left = __this->main_thread.condition_right;
		
	} else {
		__this->main_thread.registers[REGISTER_zf] = 0;
		__this->main_thread.condition_left = __this->main_thread.condition_right + 1;
		
	}
	
	__this->main_thread.registers[REGISTER_sf] = msb(temp);
	__this->main_thread.registers[REGISTER_pf] = ~(temp & 0b11111111);
	
	unsigned_t i;
	for (__this->main_thread.registers[REGISTER_pf] = 1, i = 0; i < 8; ++i) {
		__this->main_thread.registers[REGISTER_pf] ^= (temp >> i) & 0b1;
		
	}
	
}

// I'm pretty sure these instructions are redundant now that I have the `cnd` instruction

/*static inline void je_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left == __this->main_thread.condition_right) jmp_instruction(__this, type, data); }
static inline void jne_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left != __this->main_thread.condition_right) jmp_instruction(__this, type, data); }
static inline void jlt_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left < __this->main_thread.condition_right) jmp_instruction(__this, type, data); }
static inline void jgt_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left > __this->main_thread.condition_right) jmp_instruction(__this, type, data); }
static inline void jle_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left <= __this->main_thread.condition_right) jmp_instruction(__this, type, data); }
static inline void jge_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.condition_left >= __this->main_thread.condition_right) jmp_instruction(__this, type, data); }

static inline void jns_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.sf == 0) jmp_instruction(__this, type, data); }

static inline void jz_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.zf != 0) jmp_instruction(__this, type, data); }
static inline void jnz_instruction(program_t* __this, unsigned_t type, unsigned_t data) { if (__this->main_thread.zf == 0) jmp_instruction(__this, type, data); }*/

static inline void cnd_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	switch (data) {
		case CONDITION_GT: __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left <= __this->main_thread.condition_right); break;
		case CONDITION_NZ: __this->main_thread.next_skip =                __this->main_thread.registers[REGISTER_zf];                                 break;
		case CONDITION_LT: __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left >= __this->main_thread.condition_right); break;
		case CONDITION_LE: __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left > __this->main_thread.condition_right);  break;
		case CONDITION_S:  __this->main_thread.next_skip = (unsigned_t) (!__this->main_thread.registers[REGISTER_sf]);                                break;
		case CONDITION_Z:  __this->main_thread.next_skip = (unsigned_t) (!__this->main_thread.registers[REGISTER_zf]);                                break;

		case CONDITION_C:  __this->main_thread.next_skip = (unsigned_t) (!__this->main_thread.registers[REGISTER_cf]);                                break;
		case CONDITION_NS: __this->main_thread.next_skip =                __this->main_thread.registers[REGISTER_sf];                                 break;
		case CONDITION_NC: __this->main_thread.next_skip =                __this->main_thread.registers[REGISTER_cf];                                 break;
		
		case CONDITION_E:  __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left != __this->main_thread.condition_right); break;
		case CONDITION_NE: __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left == __this->main_thread.condition_right); break;

		case CONDITION_GE: __this->main_thread.next_skip = (unsigned_t) ( __this->main_thread.condition_left < __this->main_thread.condition_right);  break;
		
		default: {
			printf("WARNING Unknown condition (%lld)\n", data);
			break;
			
		}
		
	}
	
}

static inline void call_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	if (type == TOKEN_PRERESERVED) {
		__this->main_thread.registers[REGISTER_FAMILY_a] = ((unsigned_t (*) (unsigned_t, unsigned_t, unsigned_t, unsigned_t, unsigned_t)) __this->reserved[data])( \
			__this->main_thread.registers[REGISTER_rdi], \
			__this->main_thread.registers[REGISTER_rsi], \
			__this->main_thread.registers[REGISTER_FAMILY_d], \
			__this->main_thread.registers[REGISTER_FAMILY_c], \
			__this->main_thread.registers[REGISTER_r8d]);
		
	} else if (type == TOKEN_REGISTER || type == TOKEN_RESERVED) {
		/*#if VERBOSE
			printf("=== FUNCTION ===\n");
		#endif*/
		
		push_instruction(__this, TOKEN_REGISTER, REGISTER_rip);
		jmp_instruction(__this, type, data);

		__this->main_thread.nest++;
		
	} else {
		printf("FIXME super weird: call_instruction's type parameter = %lld, data = %lld\n", type, data);

	}
	
}

//~ static inline void __thread_ret(program_t* __this);

static inline void ret_instruction(program_t* __this) {
	/*#if VERBOSE
		printf("=== END FUNCTION ===\n");
	#endif*/
	
	__this->main_thread.registers[REGISTER_rip] = *((unsigned_t*) __this->main_thread.registers[REGISTER_rsp]);
	__this->main_thread.registers[REGISTER_rsp] += sizeof(unsigned_t); // pop

	__this->main_thread.nest--;
	
	if (__this->main_thread.nest < 0) {
		//~ if (__this->main_thread.current_thread == 0) {
			__exit(__this->main_thread.registers[REGISTER_FAMILY_a]);
			
		//~ } else {
			//~ __this->main_thread.nest = 0;
			//~ //__THREAD_END(__this->main_thread.threads[__this->main_thread.current_thread_index].address); // TODO (also in program_run_loop_phase)
			
		//~ }
		
	}
	
	/*if (__this->main_thread.inception > 0) {
		__thread_ret(__this);
		
	}*/
	
}

static inline void add_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) + get_value(__this, rtype, rdata)); }
static inline void sub_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) - get_value(__this, rtype, rdata)); }
//~ static inline void idiv_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { operation_instruction(ltype, ldata, rtype, rdata); set_data(ltype, ldata, sign_extend(get_value(__this, ltype, ldata)) / sign_extend(get_value(__this, rtype, rdata))); }
static inline void mod_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) % get_value(__this, rtype, rdata)); }
static inline void and_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) & get_value(__this, rtype, rdata)); }
static inline void or_instruction (program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) | get_value(__this, rtype, rdata)); }
static inline void xor_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) ^ get_value(__this, rtype, rdata)); }

static inline void div_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) / get_value(__this, rtype, rdata)); }
static inline void mul_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) * get_value(__this, rtype, rdata)); }

static inline void neg_instruction(program_t* __this, unsigned_t type, unsigned_t data) { set_value(__this, type, data, (unsigned_t) -((signed_t) get_value(__this, type, data))); }
static inline void not_instruction(program_t* __this, unsigned_t type, unsigned_t data) { set_value(__this, type, data, ~get_value(__this, type, data)); }

static inline void inc_instruction(program_t* __this, unsigned_t type, unsigned_t data) { set_value(__this, type, data, get_value(__this, type, data) + 1); }
static inline void dec_instruction(program_t* __this, unsigned_t type, unsigned_t data) { set_value(__this, type, data, get_value(__this, type, data) - 1); }

static inline unsigned_t __hi(unsigned_t x) { return x >> 32; }
static inline unsigned_t __lo(unsigned_t x) { return ((1ull << 32) - 1) & x; }

static inline void mul1_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	unsigned_t a = __this->main_thread.registers[REGISTER_FAMILY_a];
	unsigned_t b = get_value(__this, type, data);
	
	unsigned_t x = __lo(a) * __lo(b);
	unsigned_t s0 = __lo(x);
	
	x = __hi(a) * __lo(b) + __hi(x);
	
	unsigned_t s1 = __lo(x);
	unsigned_t s2 = __hi(x);

	x = s1 + __lo(a) * __hi(b);
	s1 = __lo(x);
	x = s2 + __hi(a) * __hi(b) + __hi(x);
	
	s2 = __lo(x);
	unsigned_t s3 = __hi(x);
	
	__this->main_thread.registers[REGISTER_FAMILY_a] = s1 << 32 | s0; // result
	__this->main_thread.registers[REGISTER_FAMILY_d] = s3 << 32 | s2; // carry
	
}

static inline void div1_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	signed_t divisor  = get_value(__this, type, data);
	signed_t dividend = __this->main_thread.registers[REGISTER_FAMILY_a];
	
	//~ if (type == TOKEN_REGISTER) dividend = __this->main_thread.registers[REGISTER_FAMILY_a]; // if just a register, __this needs to be just rax
	//~ else dividend = (__this->main_thread.registers[REGISTER_FAMILY_d] & 0xFFFFFFFF) << 32 | (__this->main_thread.registers[REGISTER_FAMILY_a] & 0xFFFFFFFF);

	if (!divisor) {
		warn("Divisor is 0, setting it to 1 ...\n");
		divisor++;
		
	}

	__this->main_thread.registers[REGISTER_FAMILY_a] = dividend / divisor;
	__this->main_thread.registers[REGISTER_FAMILY_d] = dividend % divisor;

}

static inline void div3_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata, unsigned_t ntype, unsigned_t ndata) { set_value(__this, ltype, ldata, get_value(__this, rtype, rdata) / get_value(__this, ntype, ndata)); }
static inline void mul3_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata, unsigned_t ntype, unsigned_t ndata) { set_value(__this, ltype, ldata, get_value(__this, rtype, rdata) * get_value(__this, ntype, ndata)); }

static inline void lsh_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) << get_value(__this, rtype, rdata)); } // same as SAL
static inline void rsh_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) { set_value(__this, ltype, ldata, get_value(__this, ltype, ldata) >> get_value(__this, rtype, rdata)); } // don't need to change

static inline void sar_instruction(program_t* __this, unsigned_t ltype, unsigned_t ldata, unsigned_t rtype, unsigned_t rdata) {
	unsigned_t count = get_value(__this, rtype, rdata) & 0x1F;
	unsigned_t destination = get_value(__this, ltype, ldata);
	
	while (count != 0) {
		__this->main_thread.registers[REGISTER_cf] = lsb(destination);
		destination /= 2;
		
		count--;
		
	}
	
	if (count == 1) __this->main_thread.registers[REGISTER_of] = msb(destination);
	else __this->main_thread.registers[REGISTER_of] = 0;
	
	set_value(__this, ltype, ldata, destination);
	
}

static inline void cdq_instruction(program_t* __this) {
	#if VERBOSE
		printf("THIS SHOULD NOT HAPPEN (CDQ INSTRUCTION)\n");
	#endif
	
}

static inline void cqo_instruction(program_t* __this) {
	__this->main_thread.registers[REGISTER_FAMILY_d] = msb(__this->main_thread.registers[REGISTER_FAMILY_a]) ? 0 : 0b1111111111111111111111111111111111111111111111111111111111111111ull; // sign extend into the rdx register
	
}

static inline void rep_instruction(program_t* __this, unsigned_t type, unsigned_t data) {
	unsigned_t i = 0;
	
	while (__this->main_thread.registers[REGISTER_FAMILY_c]--) {
		printf("TODO REP %lld\n", i++);
		
	}
	
}
