
#ifndef __AQUA__SDL2_SRC_FUNCTIONS_DECODERS_BMP_H
	#define __AQUA__SDL2_SRC_FUNCTIONS_DECODERS_BMP_H
	
	#include "../../macros_and_inclusions.h"
#include "../../lib/structs.h"

#pragma pack(push, 1)
		typedef struct {
			uint16_t magic;
			uint32_t file_size;
		
			uint16_t reserved_1;
			uint16_t reserved_2;
		
			uint32_t offset;
		
		} bitmap_header_t;
		
		typedef struct {
			uint32_t size;
		
			int32_t width;
			int32_t height;
		
			uint16_t planes;
			uint16_t bpp;
		
			uint32_t compression_type;
			uint32_t image_bytes;
		
			int64_t pixels_per_meter_x;
			int64_t pixels_per_meter_y;
		
			uint32_t colour_count;
			uint32_t important_colours;
		
		} bitmap_info_header_t;
	#pragma pack(pop)
	
	unsigned long long bmp_support(void) {
		return 1;
	
	}
	
	#define BMP_MAGIC 0x4D42
	static const char* bmp_load_default_path = "test.bmp";
	
	void bmp_load(unsigned long long ____this, unsigned long long _path) {
		bitmap_image_t* __this = (bitmap_image_t*) ____this;
		__this->data = (unsigned long long*) 0;
		
		if (!(*((char*) _path))) {
			_path = (unsigned long long) bmp_load_default_path;
			printf("WARNING Empty path in %s (setting path to %s)\n", __func__, (char*) _path);
			
		}
		
		GET_PATH((char*) _path);
		int error = 0;
		
		#if KOS_USES_JNI
			unsigned long long bytes;
			char* buffer;
			
			error = load_asset_bytes(path, &buffer, &bytes);
			char* original = buffer;
		#else
			FILE* file = fopen(path, "rb");
			error = !file;
		#endif
		
		if (error) {
			printf("WARNING Image file could not be opened (probably wrong path `%s`)\n", path);
			return;
			
		}
		
		bitmap_header_t           header;
		bitmap_info_header_t info_header;
		
		#if KOS_USES_JNI
			header  = *((bitmap_header_t*) buffer);
			buffer += sizeof(bitmap_header_t);
		#else
			fread((char*) &header, sizeof(bitmap_header_t), 1, file);
		#endif
		
		if (header.magic != BMP_MAGIC) {
			printf("WARNING File is not a bitmap image\n");
			
			#if KOS_USES_JNI
				free(original);
			#else
				fclose(file);
			#endif
			
			return;
			
		}
		
		#if KOS_USES_JNI
			info_header = *((bitmap_info_header_t*) buffer);
		#else
			fread((char*) &info_header, sizeof(bitmap_info_header_t), 1, file);
		#endif
		
		__this->image_size = info_header.image_bytes / sizeof(unsigned long long);
		__this->bpp        = (unsigned long long) info_header.bpp;
		
		__this->width  = (unsigned long long) info_header.width;
		__this->height = (unsigned long long) info_header.height;
		
		#if KOS_USES_JNI
			buffer  = original;
			buffer += header.offset;
		
			unsigned char* char_data = (unsigned char*) buffer;
		#else
			unsigned char* char_data = (unsigned char*) malloc(info_header.image_bytes);
		
			fseek(file, header.offset, SEEK_SET);
			fread(char_data, info_header.image_bytes, 1, file);
		#endif
		
		unsigned long long components = __this->bpp >> 3;
		
		__this->data             = (unsigned long long*) malloc(info_header.image_bytes);
		unsigned long long pitch = __this->width * components;
		
		unsigned long long i;
		for (i = 0; i < info_header.image_bytes; i += components) {
			unsigned long long flipped_i = (__this->height - (i / (__this->width * components)) - 1) * pitch + ((i / components) % __this->width) * components;
			
			if (__this->bpp == 32) {
				((char*) __this->data)[flipped_i]     = char_data[i + 3];
				((char*) __this->data)[flipped_i + 1] = char_data[i + 2];
				((char*) __this->data)[flipped_i + 2] = char_data[i + 1];
				((char*) __this->data)[flipped_i + 3] = char_data[i];
				
			} else {
				((char*) __this->data)[flipped_i]     = char_data[i + 2];
				((char*) __this->data)[flipped_i + 1] = char_data[i + 1];
				((char*) __this->data)[flipped_i + 2] = char_data[i];
				
			}
			
		}
		
		#if KOS_USES_JNI
			free(original);
		#else
			free(char_data);
			fclose(file);
		#endif
		
	}
	
	void bmp_free(unsigned long long ____this) {
		bitmap_image_t* __this = (bitmap_image_t*) ____this;
		free(__this->data);
	
	}
	
#endif
