#include <stdarg.h>

static char __printf_buffer[1ll << 16];

int __print(unsigned long long format, ...) { /// TODO fix this (potential security exploit)
	va_list args;
	va_start(args, (const char*) format);

	vsprintf(__printf_buffer, (const char*) format, args);
	verbose("%s", __printf_buffer);
	fflush(stdout);

	va_end(args);
	return 0;

}

void ____sprintf(unsigned long long buffer, unsigned long long format, ...) { /// TODO fix this (potential security exploit)
	va_list args;
	va_start(args, (const char*) format);
	
	vsprintf((char*) buffer, (const char*) format, args);
	va_end(args);
	
}

static void __debug(void) {
	verbose("\n=== FLAGS ===\n");

	verbose("ZF = 0x%llx\n", current_program->registers[REGISTER_zf]);
	verbose("OF = 0x%llx\n", current_program->registers[REGISTER_of]);
	verbose("CF = 0x%llx\n", current_program->registers[REGISTER_cf]);

	verbose("\n=== IMPORTANT REGISTERS ===\n");

	verbose("EAX = 0x%llx\tRAX = 0x%llx\n", current_program->registers[REGISTER_eax], current_program->registers[REGISTER_rax]);
	verbose("RDI = 0x%llx\tRSI = 0x%llx\tRDX = 0x%llx\tRCX = 0x%llx\n", current_program->registers[REGISTER_rdi], current_program->registers[REGISTER_rsi], current_program->registers[REGISTER_rdx], current_program->registers[REGISTER_rcx]);
	verbose("RIP = 0x%llx\tRSP = 0x%llx\tRBP = 0x%llx\tADDR = 0x%llx\n", current_program->registers[REGISTER_rip], current_program->registers[REGISTER_rsp], current_program->registers[REGISTER_rbp], current_program->registers[REGISTER_addr]);

	verbose("\n");

}

static void __exit(unsigned_t status) {
	info("EXIT CALLED (with status %lld)\n", status);
	current_program->registers[REGISTER_rip] = ___this->meta->length * (sizeof(uint64_t) / sizeof(uint16_t)) + 1;
	
	current_program->error_code = (int) status;
	current_program->registers[REGISTER_FAMILY_a] = status;

}

static void ____stack_chk_fail() {
	warn("Stack overflow (STACK_SIZE = %lld (units))\n", current_program->stack_size);
	__debug();
	kill("Stack overflow\n");

}

unsigned_t map(unsigned_t x, unsigned_t y) {
	return (unsigned_t) ((double) _UI64_MAX_MARGIN / ((double) y / (double) x));

}

void break_point(unsigned long long message) {
	debug("Setting breakpoint (%s)\n", (const char*) message);
	___this->debugging_mode = true;

}

unsigned long long index_rom(unsigned long long index) {
	warn("This function (%s) is unsupported on this CW\n", __func__);
	return 0;

}

signed_t noop(unsigned long long none, ...) {
	warn("This is a no-operation function. If you reach this, either your current CW/KOS is incomplete, either you have a severe problem in your code, or the function you are trying to execute is deprecated (the former is most likely)\n");
	return 1;

}

signed_t __THREAD_END  (signed_t address); // thread function prototypes
signed_t __THREAD_START(signed_t address, signed_t freq, void* argument);
signed_t __THREAD_FREQ (signed_t address, signed_t freq);
signed_t __THREAD_INST (signed_t address);

#include "heap.h"

/// TODO FOR ALL MEM* FUNCTIONS check if __src + __n and __dest + __n are within heap bounds

void* __memcpy(unsigned_t __dest, unsigned_t __src, unsigned_t __n) { return memcpy((void*) __dest, (const void*) __src, (size_t) __n); }
void* __memset(unsigned_t __s,    unsigned_t __c,   unsigned_t __n) { return memset((void*) __s,    (char)        __c,   (size_t) __n); }

signed long long load_rom(unsigned long long path);

//#define ____sprintf sprintf
#define NOOP (void*) noop,

static void* base_reserved[] = { // predefined functions from the KOS
	(void*) __print,
	(void*) __debug,
	(void*) __exit,

	(void*) ____stack_chk_fail,

	(void*) heap_malloc,
	(void*) heap_mfree,

	(void*) platform,
	(void*) is_device_supported,

	(void*) get_device,
	(void*) send_device,

	(void*) video_fps,

	(void*) video_clear,
	(void*) video_clear_colour,

	NOOP //(void*) video_draw,
	(void*) video_flip,

	(void*) get_events,

	(void*) bmp_load,
	(void*) bmp_free,

	(void*) texture_create,
	(void*) texture_remove,

	(void*) surface_new,
	(void*) surface_set_texture,
	(void*) surface_draw,
	(void*) surface_free,

	NOOP //(void*) new_rectangle,
	NOOP //(void*) draw_rectangle,

	(void*) video_width,
	(void*) video_height,

	(void*) surface_set_alpha,
	(void*) surface_set_colour,
	(void*) surface_set_x,
	(void*) surface_set_y,

	NOOP //(void*) free_events,
	(void*) surface_set_layer,
	NOOP //video_zoom,

	(void*) surface_set_width,
	(void*) surface_set_height,

	#ifdef      SOCKET_FUNCTIONS
		(void*) socket_close,
		(void*) socket_server,
	#else
		NOOP NOOP
	#endif
	
	(void*) socket_support,
	
	#if         SOCKET_FUNCTIONS
		(void*) socket_receive,
		(void*) socket_send,
	#else
		NOOP NOOP
	#endif

	(void*) get_platform,
	(void*) platform_system,

	(void*) load_rom,
	(void*) ____sprintf,

	(void*) fs_support,
	(void*) fs_read,
	(void*) fs_free,

	(void*) get_predefined_texture,
	(void*) update_predefined_texture,

	NOOP // (void*) __THREAD_START,
	NOOP // (void*) __THREAD_FREQ,
	NOOP // (void*) __THREAD_END,
	NOOP // (void*) __THREAD_INST,

	(void*) map,
	(void*) surface_scroll,

	(void*) get_font_width,
	(void*) get_font_height,
	(void*) new_font,
	(void*) create_texture_from_font,
	(void*) font_remove,

	(void*) break_point,
	(void*) index_rom,
	
	(void*) __memcpy,
	(void*) fs_write,
	(void*) __memset,
	
	#ifdef      GET_PREDEFINED_TEXTURE_SIZES
		(void*) get_predefined_texture_width,
		(void*) get_predefined_texture_height,
	#else
		NOOP NOOP
	#endif
	
	#ifdef      SURFACE_GRADIENT_FUNCTIONS
		(void*) surface_gradient_left,
		(void*) surface_gradient_right,
		(void*) surface_gradient_top,
		(void*) surface_gradient_bottom,
	#else
		NOOP NOOP NOOP NOOP
	#endif
	
	#ifdef      CREATE_TEXTURE_FROM_SCREENSHOT_FUNCTION
		(void*) create_texture_from_screenshot,
	#else
		NOOP
	#endif
	
	#ifdef FS_LIST_FUNCTIONS
		(void*) fs_list_count,
		(void*) fs_list,
		(void*) fs_list_free,
	#else
		NOOP NOOP NOOP
	#endif
	
	#ifdef      SOCKET_FUNCTIONS
		(void*) socket_client,
	#else
		NOOP
	#endif
	
};
