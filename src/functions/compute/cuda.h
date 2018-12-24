
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_COMPUTE_CUDA_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_COMPUTE_CUDA_H
	
	unsigned long long fs_write(unsigned long long _path, unsigned long long data, unsigned long long bytes);
	static unsigned long long __fs_read(unsigned long long _path, unsigned long long data, unsigned long long bytes, unsigned long long offset);
	void fs_free(unsigned long long data, unsigned long long bytes);
	
	#define COMPUTE_PARAMS unsigned long long** result, const char* extra
	
	static unsigned long long* nvcc_result;
	static unsigned long long  cuda_result;
	
	void cuda_compile_bytecode(COMPUTE_PARAMS) {
		if (strcmp(extra,  "__FREE__") == 0) {
			fs_free((unsigned long long) nvcc_result, *nvcc_result);
			
		} else {
			unsigned long long bytes;
			
			((char*) extra)[strlen(extra) - 1] = 0;
			fs_write((unsigned long long) "__temp_cuda.cu", (unsigned long long) extra + 1, strlen(extra));
			
			system("nvcc root/__temp_cuda.cu -o root/__temp_cuda_executable.o");
			
			__fs_read( \
				      (unsigned long long) "__temp_cuda_executable.o", \
				      (unsigned long long) &nvcc_result, \
				      (unsigned long long) &bytes, \
				sizeof(unsigned long long));
			
			*nvcc_result = bytes;
			*result      = nvcc_result;
			
			system("rm root/__temp_cuda.cu");
			system("rm root/__temp_cuda_executable.o");
			
		}
		
	}
	
	void cuda_execute_bytecode(COMPUTE_PARAMS) {
		unsigned long long* bytecode = (unsigned long long*) extra;
		
		fs_write((unsigned long long) "__temp_cuda_bytecode.o", (unsigned long long) (bytecode + 1), (unsigned long long) (*bytecode - 1));
		system("chmod 777 root/__temp_cuda_bytecode.o");
		
		cuda_result = system("root/__temp_cuda_bytecode.o");
		*result = &cuda_result;
		
		system("rm root/__temp_cuda_bytecode.o");
		
	}
	
#endif
