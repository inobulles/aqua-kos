
#ifndef __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_DEVICES_H
	#define __AQUA__SDL2_SRC_MACROS_AND_INCLUSIONS_DEVICES_H
	
	#define DEVICE_NULL            0
	#define DEVICE_TEXTURE         1
	#define DEVICE_KEYBOARD        2
	#define DEVICE_WM              3
	#define DEVICE_MATH            4
	#define DEVICE_CLOCK           5
	#define DEVICE_ANDROID         6
	#define DEVICE_FBO             7
	#define DEVICE_SHADER          8
	#define DEVICE_REQUESTS        9
	#define DEVICE_DISCORD         10
	#define DEVICE_GL              11
	#define DEVICE_GL_BATCH        12
	#define DEVICE_FS              13
	#define DEVICE_KEYBOARD_DIALOG 14
	
	// compute devices
	
	#define COMPUTE_BASE 0xC0C0
	
	#define __COMPUTE_COMPILER_DEVICE 0
	#define __COMPUTE_EXECUTOR_DEVICE 1
	
	#define COMPUTE_COMPILER_DEVICE(x) (COMPUTE_BASE + __COMPUTE_COMPILER_DEVICE + (x) * 2)
	#define COMPUTE_EXECUTOR_DEVICE(x) (COMPUTE_BASE + __COMPUTE_EXECUTOR_DEVICE + (x) * 2)
	
	#define DEVICE_COMPUTE_CUDA_COMPILER (COMPUTE_COMPILER_DEVICE(0))
	#define DEVICE_COMPUTE_CUDA_EXECUTOR (COMPUTE_EXECUTOR_DEVICE(0))
	
#endif
