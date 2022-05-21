#undef LOG_COMPONENT

#if !defined(__LOG__)
#define __LOG__

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
	LOG_LVL_FATAL,
	LOG_LVL_ERROR,
	LOG_LVL_WARN,
	LOG_LVL_SUCCESS,
	LOG_LVL_INFO,
	LOG_LVL_VERBOSE,
} log_lvl_t;

#define LOG_CLEAR   "\033[0m"
#define LOG_REGULAR "\033[0;"
#define LOG_BOLD    "\033[1;"

#define LOG_PURPLE  "35m"
#define LOG_RED     "31m"
#define LOG_YELLOW  "33m"
#define LOG_GREEN   "32m"
#define LOG_BLUE    "34m"
#define LOG_GREY    "37m"

void _log(log_lvl_t lvl, const char* component, const char* path, const char* func, uint32_t line, char* msg) {
	char* lvl_env = getenv("LOG_LVL");
	log_lvl_t max_lvl = LOG_LVL_SUCCESS;

	if (
		lvl_env &&
		*lvl_env >= '0' + LOG_LVL_FATAL &&
		*lvl_env <= '0' + LOG_LVL_VERBOSE
	) {
		max_lvl = *lvl_env - '0';
	}

	if (lvl > max_lvl) {
		return;
	}

	char* lvl_str;
	FILE* fp;
	char* colour;

	switch (lvl) {
		#define LOG_LVL_CASE(name, _fp, _colour) \
			case LOG_LVL_##name: { \
				lvl_str = #name; \
				fp = (_fp); \
				colour = (_colour); \
				\
				break; \
			}

		default:

		LOG_LVL_CASE(FATAL,   stderr, LOG_PURPLE)
		LOG_LVL_CASE(ERROR,   stderr, LOG_RED   )
		LOG_LVL_CASE(WARN,    stderr, LOG_YELLOW)
		LOG_LVL_CASE(SUCCESS, stdout, LOG_GREEN )
		LOG_LVL_CASE(INFO,    stdout, LOG_BLUE  )
		LOG_LVL_CASE(VERBOSE, stdout, LOG_GREY  )

		#undef LOG_LVL_CASE
	}

	fprintf(fp, LOG_BOLD "%s[%s %s -> %s -> %s -> %d]" LOG_REGULAR "%s %s" LOG_CLEAR "\n",
		colour, lvl_str, component, path, func, line, colour, msg);
}

void vlog(log_lvl_t lvl, const char* component, const char* path, const char* func, uint32_t line, char* fmt, ...) {
	va_list args;
	va_start(args, fmt);

	char* msg;
	vasprintf(&msg, fmt, args);

	va_end(args);

	_log(lvl, component, path, func, line, msg);
	free(msg);
}

// helper macros
// this should be in a common header file

#define LOG_FATAL(...)   vlog(LOG_LVL_FATAL,   LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...)   vlog(LOG_LVL_ERROR,   LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_WARN(...)    vlog(LOG_LVL_WARN,    LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_SUCCESS(...) vlog(LOG_LVL_SUCCESS, LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...)    vlog(LOG_LVL_INFO,    LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);
#define LOG_VERBOSE(...) vlog(LOG_LVL_VERBOSE, LOG_COMPONENT, __FILE__, __func__, __LINE__, __VA_ARGS__);

#endif