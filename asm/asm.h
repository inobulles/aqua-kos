
#ifndef __ZED__VCPU_ASM_H
#define __ZED__VCPU_ASM_H

// inclusions

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// compatibilty checks

#ifndef _UI64_MAX
	#define _UI64_MAX  0xFFFFFFFFFFFFFFFFull
	#define _SI64_MAX (0xFFFFFFFFFFFFFFFFll / 2)

	#define _UI64_MAX_MARGIN (_UI64_MAX >> 8)
	#define _SI64_MAX_MARGIN (_SI64_MAX >> 8)
#endif

#ifndef __cplusplus
	typedef uint8_t bool;
	
	#define true  1
	#define false 0
#endif

#ifdef ALOGI
	#ifndef printf
		#define printf ALOGI
	#endif
	#define ANDROID 1
#else
	#define ANDROID 0
#endif

#ifndef DEBUGGING_MODE
	#define DEBUGGING_MODE true
#endif

#ifndef VERBOSE_MODE
	#define VERBOSE_MODE true
#endif

void mfree(void* pointer, unsigned long long bytes);

// types

#define SIGNED_UNSIGNED 0

#if !SIGNED_UNSIGNED
	typedef unsigned long long unsigned_t;
#else
	typedef signed long long unsigned_t;
#endif

typedef unsigned long long sure_unsigned_t;
typedef   signed long long        signed_t;

#if !SIGNED_UNSIGNED
	typedef uint32_t unsigned_32bit_t;
	typedef uint16_t unsigned_16bit_t;
	typedef uint8_t  unsigned_8bit_t;
#else
	typedef int32_t unsigned_32bit_t;
	typedef int16_t unsigned_16bit_t;
	typedef int8_t  unsigned_8bit_t;
#endif

#define uint64_t sure_unsigned_t
#define  int64_t        signed_t

// main macros

#define MODE_DONT_CARE 0
#define MODE_FORCE_YES 1
#define MODE_FORCE_NO  2

// environment macros

#define DEFAULT_STACK_SIZE (1ll << 16)
#define CURRENT_VERSION 2ll

// global variables
#define BDA_SIGNATURE 0xBDA5

typedef struct {
	unsigned long long signature;

#ifdef KOS_BDA_EXTENSION
kos_bda_extension_t extension;
#endif

} zed_bda_t;

typedef struct {
	void* pointer;
	
	signed long long warning;
	bool debugging_mode; bool verbose_mode;
	
	uint64_t length; uint64_t version;
	void* meta; void* data_section;
	unsigned long long reserved_count; void** reserved;
	
	int main_thread;
	
} __pointer__program_t;

static void*                 __pointer_current_program;
static __pointer__program_t* __pointer___this;

// error macros

#ifndef ALOGE
#define ALOGE printf
#endif

#ifndef ALOGW
#define ALOGW printf
#endif

#ifndef ALOGI
#define ALOGI printf
#endif

#ifndef ALOGD
#define ALOGD printf
#endif

#ifndef ALOGV
#define ALOGV printf
#endif

#define kill(...)    {                              ALOGE("FATAL ERROR " __VA_ARGS__); ALOGE("Aborting ...\n"); ALOGE("TODO Abort\n"); while (1); }
#define warn(...)    { __pointer___this->warning++; ALOGW("WARNING     " __VA_ARGS__); }
#define info(...)    {                              ALOGI("INFO        " __VA_ARGS__); }
#define verbose(...) { if (__pointer___this->verbose_mode) {       ALOGV(__VA_ARGS__); } }
#define debug(...)   {                              ALOGD("DEBUG       " __VA_ARGS__); }

// structures

#include "thread.h"

typedef struct {
	uint64_t length;              uint64_t version;               uint64_t invalidated;
	uint64_t base_reserved_count; uint64_t label_position_offset; uint64_t main_reserved_position;
	uint64_t debugging_mode;      uint64_t verbose_mode;
	uint64_t data_section_element_count;
	
} metadata_t;

typedef struct {
	uint8_t* data;
	
	unsigned long long  element_count; unsigned long long element_count_bytes;
	unsigned long long* element_element_count; bool* qtypes; void** start_position;
	uint8_t* contiguous_data;
	
} data_section_t;

typedef struct {
	void* pointer;
	
	signed long long warning;
	bool debugging_mode; bool verbose_mode;
	
	uint64_t length; uint64_t version;
	metadata_t* meta; data_section_t data_section;
	
	unsigned long long reserved_count; void** reserved;
	
	thread_t main_thread;
	
	uint64_t reserved_positions_count; uint64_t* reserved_positions;
	uint16_t* base_pointer; uint64_t* base_pointer_64; int error_code;

	zed_bda_t* bda;
	
} program_t;

#define current_program ((thread_t*)  __pointer_current_program)
#define ___this         ((program_t*) __pointer___this)

static inline unsigned_t msb(unsigned_t value) { return value & (1ull << (sizeof(unsigned_t) * 8 - 1)); } // calculate most significant bit (of an `unsigned_t`)
static inline unsigned_t lsb(unsigned_t value) { return value & 1; } // calculate least significant bit

static inline unsigned_t zero_extend(unsigned_t value) { return value & 0xFFFFFFFF; } // extend zeroes into the 32 higher bits
static inline unsigned_t sign_extend(unsigned_t value) { /// TODO
	return (value & 0xFFFFFFFF) | ((value & (1llu << 32)) << 32);
	printf("%d %lld\n", (int32_t) value, (value & (1llu << 32)));
	
}

static inline uint8_t* access_byte_address(program_t* __this, unsigned_t address) {
    if (__this->meta->version < 2) {
        warn("The compiler with which the ROM has been compiled does not support byte addressing, yet it is still using them. This could potentially be dangerous.\n");
        
    }
    
    return   (uint8_t*) address;
    
}

static inline unsigned_t* access_address(program_t* __this, unsigned_t address) {
	return   (unsigned_t*) address;

	/*
#include "heap_size.h"
	extern unsigned char heap_space[MAX_HEAP_SPACE];

	unsigned_t stack_address = (unsigned_t) __this->main_thread.stack;
	unsigned_t heap_address  = (unsigned_t) heap_space;

	unsigned_t stack_size = __this->main_thread.stack_size * sizeof(zed_stack_t);
	unsigned_t heap_size  = MAX_HEAP_SPACE;

	if (address < stack_address && address > stack_address + stack_size && \
		address < heap_address  && address > heap_address  + heap_size) {
		ALOGA("INVALID ADDRESS %llu\n", address);

	} else {
		return (unsigned_t*) address;

	}*/

}

/// TODO prettify the get_value / set value functions

static inline unsigned_t get_value(program_t* __this, unsigned_t type, unsigned_t data) { // get value from an argument's type and data values
	switch (type) {
		case TOKEN_REGISTER: {
			if (data <= REGISTER_LAST_GENERAL_PURPOSE) { // check if general purpose register
				unsigned_t temp = __this->main_thread.registers[data & 0b0011]; // get family register value

				switch (data & 0b1100) { // mask according to the size of the register
					case REGISTER_TYPE_64: return temp; // don't do anything if 64 bit

					case REGISTER_TYPE_32: return temp & 0xFFFFFFFF;
					case REGISTER_TYPE_16: return temp & 0xFFFF;
					case REGISTER_TYPE_8:  return temp & 0xFF;

					default: return temp;

				}

			} else {
				return __this->main_thread.registers[data];

			}

		}

		case TOKEN_ADDRESS:     return              *access_address     (__this, __this->main_thread.registers[data]); // return the first `unsigned_t` value at the address of the register
		case TOKEN_BYTE_ADDR:   return (unsigned_t) *access_byte_address(__this, __this->main_thread.registers[data]);
		case TOKEN_NUMBER:      return data;

		case TOKEN_RESERVED:    return __this->reserved_positions[data - __this->meta->label_position_offset];
		case TOKEN_QTYPE:
		case TOKEN_PRERESERVED: return (unsigned_t) __this->reserved[data];
		
		default:                return data;

	}

}

static inline void set_value(program_t* __this, unsigned_t type, unsigned_t data, unsigned_t set) { // set an argument to value from its type and data values
	switch (type) {
		case TOKEN_REGISTER: {
			if (data <= REGISTER_LAST_GENERAL_PURPOSE) { // check if general purpose register
				unsigned_t family = data & 0b0011;

				switch (data & 0b1100) { // set the register family and shift by (times it can fit in an unsigned_t) - 1, to keep one of that data type on the lower end
					case REGISTER_TYPE_64: __this->main_thread.registers[family] = set; break; // don't do anything if 64 bit

					case REGISTER_TYPE_32: ((unsigned_32bit_t*) __this->main_thread.registers)[family * 2] = (unsigned_32bit_t) set; break;
					case REGISTER_TYPE_16: ((unsigned_16bit_t*) __this->main_thread.registers)[family * 4] = (unsigned_16bit_t) set; break;
					case REGISTER_TYPE_8:   ((unsigned_8bit_t*) __this->main_thread.registers)[family * 8] = (unsigned_8bit_t)  set; break;

					default: __this->main_thread.registers[family] = set; break;

				}

			} else {
				__this->main_thread.registers[data] = set;

			}

			break;

		}

		case TOKEN_ADDRESS:   *access_address     (__this, __this->main_thread.registers[data]) =           set; break; // set the first `unsigned_t` value at the address of the register
		case TOKEN_BYTE_ADDR: *access_byte_address(__this, __this->main_thread.registers[data]) = (uint8_t) set; break;
		
		case TOKEN_RESERVED: {
			warn("`type` argument of `%s` is `TOKEN_RESERVED`\n", __func__);
			__this->reserved_positions[data]  = set;
			break; // can't really remember what __this one's for :3
			
		}

		default: break;

	}

}

#include "kos_wrapper.h"
#include "instructions.h"

// static functions

static inline void __program_set_mode(bool* value, int meta_value, bool default_value, const char* string) {
	switch (meta_value) {
		case MODE_FORCE_YES: *value = true;          break;
		case MODE_FORCE_NO:  *value = false;         break;
		default:             *value = default_value; break;
		
	}
	
	verbose("\t%s mode is %s\n", string, *value ? "enabled" : "disabled");
	
}

// phase functions

void program_free(program_t* ____this) {
	verbose("Freeing program ...\n");
	
	// free environment
	verbose("\tKilling main thread ...\n");
	
	// free data section
	verbose("\tFreeing data section (____this->data_section.element_element_count and ____this->data_section.qtypes) ...\n");
	
	mfree(____this->data_section.element_element_count, ____this->data_section.element_count_bytes * sizeof(unsigned long long));
	mfree(____this->data_section.qtypes,                ____this->data_section.element_count_bytes * sizeof(bool));
	
	// free reserved
	verbose("\tFreeing ____this->reserved ...\n");
	mfree(____this->reserved, ____this->reserved_count * sizeof(void*));
	
	// free reserved positions section
	verbose("\tFreeing ____this->reserved_positions ...\n");
	mfree(____this->reserved_positions, ____this->reserved_positions_count * sizeof(uint64_t));

	// free bda
	verbose("\tFreeing ____this->bda ...\n");
	heap_mfree((unsigned long long) ____this->bda, sizeof(zed_bda_t));
	
}

void program_run_global_init_phase(void) {
	init_heap(); // init heap

}

void program_run_setup_phase(program_t* ____this) {
	// notes
	
	info("NOTE When refering to a \"unit\", I am talking about a type with a width of %lld bytes\n", (unsigned_t) sizeof(unsigned long long));
	
	// setting up
	
	__pointer___this          = (__pointer__program_t*) ____this;
	__pointer_current_program = (void*)                &____this->main_thread;
	
	___this->warning    = 0;
	___this->error_code = 0;
	
	___this->verbose_mode   = true;
	___this->debugging_mode = false;
	
	void* base_pointer = ___this->pointer;
	
	// parse metadata
	verbose("Parsing metadata section ...\n");
	
	___this->meta = (metadata_t*) ___this->pointer;
	uint8_t* temp_uint8_t_buffer = (uint8_t*) ___this->pointer;
	temp_uint8_t_buffer += sizeof(metadata_t);
	___this->pointer = (void*) temp_uint8_t_buffer;
	
	verbose("\tMetadata\n");
	verbose("\t\tInvalid: %s\n",   ___this->meta->invalidated ? "yes" : "no");
	verbose("\t\tLength:  %lld\n", ___this->meta->length);
	verbose("\t\tVersion: %lld\n", ___this->meta->version);
	
	verbose("\t___this->meta->base_reserved_count    = %lld\n", ___this->meta->base_reserved_count);
	verbose("\t___this->meta->label_position_offset  = %lld\n", ___this->meta->label_position_offset);
	verbose("\t___this->meta->main_reserved_position = %lld\n", ___this->meta->main_reserved_position);
	
	___this->version = CURRENT_VERSION;
	verbose("\t___this->version = %lld (same as CURRENT_VERSION)\n", ___this->version);
	
	if (___this->meta->invalidated)                kill("The program ROM has been invalidated (meta->invalidated = %lld). This program can not be executed ...\n", ___this->meta->invalidated);
	//~ if (___this->meta->length  > ___this->length)  kill("The program's extracted code length (%lld) is greater than the current code length (%lld)\n", ___this->meta->length, ___this->length);
	if (___this->meta->version > ___this->version) warn("The program's version (%lld) is greater than the current version (%lld)\n", ___this->meta->version, ___this->version);

	// allocate bda
	verbose("Allocating bda ...\n")
	___this->bda = (zed_bda_t*) heap_malloc(sizeof(zed_bda_t));
	___this->bda->signature = BDA_SIGNATURE;

	// set modes
	verbose("Setting modes ...\n");
	
	__program_set_mode(&___this->debugging_mode, (bool) ___this->meta->debugging_mode, DEBUGGING_MODE, "debugging");
	__program_set_mode(&___this->verbose_mode,   (bool) ___this->meta->verbose_mode,     VERBOSE_MODE, "verbose");
	
	// set up environment
	verbose("Setting up environment\n")
	
	verbose("\tCreating main thread ...\n");
	thread_create(&___this->main_thread, (void*) 0, (void*) 0, ___this->meta->length, DEFAULT_STACK_SIZE);
	
	// parse data section
	verbose("Parsing data section ...\n");
	
	___this->data_section.element_count       = ___this->meta->data_section_element_count;
	___this->data_section.element_count_bytes = ___this->meta->data_section_element_count * sizeof(unsigned long long);
	
	verbose("\tAllocating ___this->data_section.element_count (%lld) units for ___this->data_section.element_element_count ...\n", ___this->data_section.element_count);
	___this->data_section.element_element_count = (unsigned long long*) malloc(___this->data_section.element_count_bytes);
	
	verbose("\tAllocating ___this->data_section.element_count (%lld) units for ___this->data_section.qtypes ...\n", ___this->data_section.element_count);
	___this->data_section.qtypes                = (bool*)               malloc(___this->data_section.element_count_bytes);
	
	verbose("\tAllocating ___this->data_section.element_count (%lld) units for ___this->data_section.start_position ...\n", ___this->data_section.element_count);
	___this->data_section.start_position        = (void**)              malloc(___this->data_section.element_count_bytes);
	
	uint64_t* new_pointer = (uint64_t*) ___this->pointer;
	verbose("\tParsing data section to get element bytes and qtypes ...\n")
	
	unsigned long long i;
	for (i = 0; i < ___this->data_section.element_count; i++) {
		___this->data_section.element_element_count[i] =        *new_pointer++;
		___this->data_section.qtypes[i]                = (bool) *new_pointer++;
		
	}
	
	___this->data_section.contiguous_data = (uint8_t*) new_pointer;
	uint8_t* contiguous_data_copy = ___this->data_section.contiguous_data;
	verbose("\tParsing data section to get element starting position pointers ...\n");
	
	for (i = 0; i < ___this->data_section.element_count; i++) {
		___this->data_section.start_position[i] = contiguous_data_copy;
		contiguous_data_copy += ___this->data_section.element_element_count[i] + 1;
		
	}
	
	// create reserved list (from base_reserved)
	verbose("\tCreating reserved list (from base_reserved) ...\n");
	
	___this->reserved_count = ___this->meta->base_reserved_count + ___this->data_section.element_count;
	verbose("\t\t___this->reserved_count = %lld\n", ___this->reserved_count);
	___this->reserved       = (void**) malloc(___this->reserved_count * sizeof(void*));
	
	for (i = 0; i < ___this->meta->base_reserved_count; i++) {
		___this->reserved[i] = base_reserved[i];
		
	}
	
	uint8_t* data = (uint8_t*) ___this->pointer;
	unsigned_t k = 0;
	
	unsigned_t j;
	for (i = 0; i < ___this->data_section.element_count; i++) {
		k += 2 * sizeof(uint64_t); // x2 because one unit for qtype and the other for length
		___this->reserved[i + ___this->meta->base_reserved_count] = (void*) ___this->data_section.start_position[i];
		k += ___this->data_section.element_element_count[i];
		
	}
	
	verbose("\tFinding QTYPES in ___this->data_section.qtypes ...\n");
	
	for (i = 0; i < ___this->data_section.element_count; i++) {
		if (___this->data_section.qtypes[i]) {
			verbose("\t\t%lld is QTYPE. Calculating addresses ...\n", i);
			
			unsigned_t m;
			for (m = 0; m < ___this->data_section.element_element_count[i] / sizeof(unsigned_t); m++) {
				unsigned_t label  = ((unsigned_t*) ___this->data_section.start_position[i])[m] >> 32;
				unsigned_t offset = ((unsigned_t*) ___this->data_section.start_position[i])[m] & 0xFFFFFFFF;
				
				((unsigned_t*) ___this->data_section.start_position[i])[m] = (unsigned_t) ___this->reserved[label + ___this->meta->base_reserved_count] + offset;
				
			}
			
		}
		
	}
	
	// parse the reserved positions section
	verbose("Parsing reserved positions section ...\n");
	
	new_pointer = (uint64_t*) (contiguous_data_copy + 7);
	___this->reserved_positions_count = *new_pointer++;
	verbose("\t___this->reserved_positions_count = %lld\n", ___this->reserved_positions_count);
	
	verbose("\tAllocating ___this->reserved_positions_count (%lld) units for ___this->reserved_positions ...\n", ___this->reserved_positions_count);
	___this->reserved_positions = (uint64_t*) malloc(___this->reserved_positions_count * sizeof(uint64_t));
	
	for (i = 0; i < ___this->reserved_positions_count; i++) {
		___this->reserved_positions[i] = *new_pointer++;
		//~ printf("\t\t___this->reserved_positions[%lld] %lld\n", i, ___this->reserved_positions[i] / sizeof(uint16_t));
		
	}
	
	// set up for parsing the text section
	verbose("Setting up for text section parsing ...\n");
	
	verbose("\tSetting ___this->main_thread.registers[REGISTER_rip] to ___this->meta->main_reserved_position / sizeof(uint16_t) (%lld) ...\n", ___this->meta->main_reserved_position / sizeof(uint16_t));
	___this->main_thread.registers[REGISTER_rip] = ___this->meta->main_reserved_position / sizeof(uint16_t);
	
	verbose("\tPushing the `main` label (___this->main_thread.registers[REGISTER_rip]) to the main thread stack ...\n");
	thread_stack_push(&___this->main_thread, ___this->main_thread.registers[REGISTER_rip]);
	
	___this->base_pointer    = (uint16_t*) base_pointer;
	___this->base_pointer_64 = (uint64_t*) base_pointer;
	
}

static unsigned_t origin_skip;

static unsigned_t param1;
static unsigned_t param2;

static unsigned_t param3;
static unsigned_t param4;

static unsigned_t param5;
static unsigned_t param6;

//~ int usleep(int usec);

int program_run_loop_phase(program_t* __this) {
	uint64_t token;
	
	unsigned long long type;
	unsigned long long data;
	
	#define SIZE (sizeof(uint64_t) / sizeof(uint16_t))
	#define TOKEN_BYTE 13ll
	
	__this->debugging_mode = 0; /// REMME
	
	#define NEXT_TOKEN { \
		token = (uint64_t) *(__this->base_pointer + __this->main_thread.registers[REGISTER_rip]++); \
		type  = token & 0x00FF; \
		if (type==TOKEN_NUMBER||type==TOKEN_QTYPE||type==TOKEN_RESERVED||type==TOKEN_PRERESERVED) { \
			if (__this->main_thread.registers[REGISTER_rip] % SIZE) { \
				data = *(__this->base_pointer_64 + (__this->main_thread.registers[REGISTER_rip]) / SIZE + 1); \
				__this->main_thread.registers[REGISTER_rip] = (__this->main_thread.registers[REGISTER_rip] / SIZE + 2) * SIZE; \
			} else { \
				data = *(__this->base_pointer_64 + (__this->main_thread.registers[REGISTER_rip]) / SIZE); \
				__this->main_thread.registers[REGISTER_rip] = (__this->main_thread.registers[REGISTER_rip] / SIZE + 1) * SIZE; \
			} \
		} else { \
			data = (token & 0xFF00) >> 8; \
		} \
		type = type == TOKEN_BYTE ? TOKEN_NUMBER : type; \
		if (__this->debugging_mode) { \
			printf("%lld:\t0x%llx\t0x%llx\n", __this->main_thread.registers[REGISTER_rip], type, data); \
		} \
	}
	
	//~ usleep(200000); // lock CPU to 5Hz (200000)
	
	if (__this->debugging_mode) {
		printf("\n");
		
	}
	
	NEXT_TOKEN
	
	#define INSTR if (!origin_skip) // this is so that we can check if we are supposed to execute the next instruction
	
	#define PARAMS2 { NEXT_TOKEN param1 = type; param2 = data; } // we need to create these macros, else the parameters will get mixed up
	#define PARAMS4 { NEXT_TOKEN param1 = type; param2 = data; NEXT_TOKEN param3 = type; param4 = data; }
	#define PARAMS6 { NEXT_TOKEN param1 = type; param2 = data; NEXT_TOKEN param3 = type; param4 = data; NEXT_TOKEN param5 = type; param6 = data; }
	
	switch (type) {
		case TOKEN_KEYWORD: { // instruction
			origin_skip = __this->main_thread.next_skip;
			__this->main_thread.next_skip = 0;
			
			switch (data) {
				case TOKEN_cla:   PARAMS6; INSTR cla_instruction  (__this, param1, param2, param3, param4, param5, param6);          break;
				case TOKEN_mov:   PARAMS4; INSTR mov_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_jmp:   PARAMS2; INSTR jmp_instruction  (__this, param1, param2);                                          break;
				case TOKEN_cnd:   PARAMS2; INSTR cnd_instruction  (__this, param1, param2);                                          break;
				case TOKEN_cmp:   PARAMS4; INSTR cmp_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_sar:   PARAMS4; INSTR sar_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_call:  PARAMS2; INSTR call_instruction (__this, param1, param2);                                          break;

				case TOKEN_add:   PARAMS4; INSTR add_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_sub:   PARAMS4; INSTR sub_instruction  (__this, param1, param2, param3, param4);                          break;

				case TOKEN_lea:   PARAMS4; INSTR lea_instruction  (__this, param1, param2, param3, param4);                          break;

				case TOKEN_push:  PARAMS2; INSTR push_instruction (__this, param1, param2);                                          break;
				case TOKEN_pop:   PARAMS2; INSTR pop_instruction  (__this, param1, param2);                                          break;

				case TOKEN_ret:            INSTR ret_instruction  (__this);                                                          break;

				case TOKEN_and:   PARAMS4; INSTR and_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_or:    PARAMS4; INSTR or_instruction   (__this, param1, param2, param3, param4);                          break;
				case TOKEN_xor:   PARAMS4; INSTR xor_instruction  (__this, param1, param2, param3, param4);                          break;

				case TOKEN_mul:   PARAMS4; INSTR mul_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_div:   PARAMS4; INSTR div_instruction  (__this, param1, param2, param3, param4);                          break;

				case TOKEN_mul3:  PARAMS6; INSTR mul3_instruction (__this, param1, param2, param3, param4, param5, param6);          break;
				case TOKEN_div3:  PARAMS6; INSTR div3_instruction (__this, param1, param2, param3, param4, param5, param6);          break;

				case TOKEN_mul1:  PARAMS2; INSTR mul1_instruction (__this, param1, param2);                                          break;
				case TOKEN_div1:  PARAMS2; INSTR div1_instruction (__this, param1, param2);                                          break;

				case TOKEN_shr:   PARAMS4; INSTR rsh_instruction  (__this, param1, param2, param3, param4);                          break;
				case TOKEN_shl:   PARAMS4; INSTR lsh_instruction  (__this, param1, param2, param3, param4);                          break;

				case TOKEN_test:  PARAMS4; INSTR test_instruction (__this, param1, param2, param3, param4);                          break;

				case TOKEN_neg:   PARAMS2; INSTR neg_instruction  (__this, param1, param2);                                          break;
				case TOKEN_not:   PARAMS2; INSTR not_instruction  (__this, param1, param2);                                          break;

				case TOKEN_cdq:            INSTR cdq_instruction  (__this);                                                          break;
				case TOKEN_cqo:            INSTR cqo_instruction  (__this);                                                          break;

				case TOKEN_rep:   PARAMS2; INSTR rep_instruction  (__this, param1, param2);                                          break;

				case TOKEN_movzx: PARAMS4; INSTR movzx_instruction(__this, param1, param2, param3, param4);                          break;
				case TOKEN_movsx: PARAMS4; INSTR movsx_instruction(__this, param1, param2, param3, param4);                          break;
				case TOKEN_set:   PARAMS2;       mov_instruction  (__this, param1, param2, TOKEN_NUMBER, (unsigned_t) !origin_skip); break;

				case TOKEN_inc:   PARAMS2; INSTR inc_instruction  (__this, param1, param2);                                          break;
				case TOKEN_dec:   PARAMS2; INSTR dec_instruction  (__this, param1, param2);                                          break;
				
				case TOKEN_mod:   PARAMS4; INSTR mod_instruction  (__this, param1, param2, param3, param4);                          break;
				
				default: warn("Unknown instruction 0x%llx\n", type); break;
				
			}
			
			break;
			
		} case 0: break; // padding
		default: warn("Unknown token type 0x%llx (was expecting instruction, or 0x%x)\n", type, TOKEN_KEYWORD); break;
		
	}
	
	if (__this->main_thread.registers[REGISTER_rip] > __this->meta->length * (sizeof(uint64_t) / sizeof(uint16_t))) {
		__this->error_code = (int) __this->main_thread.registers[REGISTER_FAMILY_a];
		
		uint64_t signature_hex = *(__this->base_pointer_64 + (__this->main_thread.registers[REGISTER_rip] += sizeof(uint64_t) / sizeof(uint16_t)) / (sizeof(uint64_t) / sizeof(uint16_t)));
		char signature[9]; // signature string is 8 bytes long
		signature[8] = 0;
		
		int i;
		for (i = 0; i < 8; i++) {
			signature[i] = (char) ((signature_hex >> (i * 8)) & 0xFF);
			
		}
		
		verbose("Signature is %s\n", signature);
		#define DEFAULT_SIGNATURE "AQUA-ZED"
		
		 if (strcmp(signature, DEFAULT_SIGNATURE) == 0) {info("Signature matches default AQUA signature\n");
		}else                                            warn("Signature does not match default AQUA signature (%s)\n", DEFAULT_SIGNATURE);
		
		verbose("You have reached the end of the program (with %lld warnings, error code %d, 0x%x)\n", __this->warning, __this->error_code, __this->error_code);
		verbose("\t__this->meta->length * (sizeof(uint64_t) / sizeof(uint16_t)) = %lld\n", __this->meta->length * (sizeof(uint64_t) / sizeof(uint16_t)));
		verbose("\t__this->main_thread.registers[REGISTER_rip] = %lld\n", __this->main_thread.registers[REGISTER_rip]);
		
		return 1;
		
	} else {
		return 0;
		
	}
	
}

#endif
