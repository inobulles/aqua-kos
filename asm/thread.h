
#ifndef __AQUA__CW_THREAD_H
	#define __AQUA__CW_THREAD_H
	
	#include "defs.h"
	#include <string.h>
	
	// macros
	
	#define REGISTER_COUNT (REGISTER_LAST + 1)
	
	// type definitions
	
	typedef unsigned_t reg_t;
	typedef unsigned_t zed_stack_t;
	
	// structuress
	
	typedef struct {
		bool valid;
		bool own_stack;
		
		unsigned_t stack_size;
		
		reg_t registers[REGISTER_COUNT];
		zed_stack_t* stack;
		
		// environment variables
		
		signed_t   nest;
		unsigned_t base_rip;
		
		signed_t condition_left;
		signed_t condition_right;
		
		unsigned_t next_skip;
		unsigned_t error_code;
		
	} thread_t;
	
	// functions
	
	static inline void thread_stack_push(thread_t* _this, unsigned_t value) {
		*((zed_stack_t*) (_this->registers[REGISTER_rsp] -= sizeof(zed_stack_t))) = (zed_stack_t) value;
		
	}
	
	void thread_create(thread_t* _this, void* register_pointer, void* stack_pointer, unsigned long long first_stack_value, unsigned long long stack_size) {
		//~ if (_this->valid) {
			//~ kill("You are trying to override a live thread (%p)\n", _this);
			
		//~ }
		
		_this->valid = true;
		_this->stack_size = stack_size;
		
		if (register_pointer != (void*) 0) {
			verbose("\t\tNo need to create new registers\n");
			memcpy(_this->registers, register_pointer, REGISTER_COUNT * sizeof(reg_t));
			
		} else {
			verbose("\t\tCreating registers ...\n");
			
			int i;
			for (i = 0; i < REGISTER_COUNT; i++) {
				_this->registers[i] = 0;
				
			}
			
		}
		
		if (stack_pointer != (void*) 0) {
			verbose("\t\tNo need to create a new stack\n")
			
			_this->own_stack = false;
			_this->stack = (zed_stack_t*) stack_pointer;
			
		} else {
			verbose("\t\tCreating a new stack and setting the rsp register to the end of it\n");
			_this->own_stack = true;
			
			_this->stack = (zed_stack_t*) malloc((size_t) (_this->stack_size * sizeof(zed_stack_t)));
			_this->registers[REGISTER_rsp] =      (reg_t)  _this->stack      + sizeof(zed_stack_t) * (_this->stack_size - 1);
			thread_stack_push(_this, first_stack_value);
			
		}
		
		// set environment variables
		verbose("\t\tSetting environment variables ...\n");
		
		_this->next_skip  = 0;
		_this->nest       = 0;
		_this->error_code = 0;
		
	}
	
	void thread_destroy(thread_t* _this) {
		if (!_this->valid) {
			kill("You are trying to kill an dead thread (%p)\n", _this);
			
		}
		
		_this->valid = false;

		if (_this->own_stack) {
			mfree(_this->stack, _this->stack_size * sizeof(zed_stack_t));
			
		}
		
	}
	
#endif