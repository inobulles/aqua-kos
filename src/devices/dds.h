
#ifndef __AQUA__KOS_DEVICES_DDS_H
	#define __AQUA__KOS_DEVICES_DDS_H
	
	#define FOURCC_DXT1 0x31545844 // "DXT1" in ascii
	#define FOURCC_DXT3 0x33545844 // "DXT3" in ascii
	#define FOURCC_DXT5 0x35545844 // "DXT5" in ascii
	
	static void dds_device_handle(unsigned long long** result, const char* __data) {
		char* data = (char*) __data;
		
		if (strncmp(data, "DDS ", 4) != 0) {
			printf("WARNING Data is not DDS\n");
			return;
			
		}
		
		data += 4;
		unsigned char header[124];
		memcpy(header, data, sizeof(header));
		data += sizeof(header);
		
		uint32_t height       = *((uint32_t*) &(header[ 8]));
		uint32_t width        = *((uint32_t*) &(header[12]));
		uint32_t linear_size  = *((uint32_t*) &(header[16]));
		uint32_t mipmap_count = *((uint32_t*) &(header[24]));
		uint32_t four_cc      = *((uint32_t*) &(header[80]));
		
		unsigned long long buffer_bytes = mipmap_count > 1 ? linear_size << 1 : linear_size;
		unsigned long long components = (four_cc == FOURCC_DXT1) ? 3 : 4;
		unsigned long long format;
		
		switch (four_cc) {
			case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
			case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
			case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
			
			default: {
				printf("WARNING Unknown four_cc (%d)\n", four_cc);
				return;
				
			}
			
		}
		
		GLuint texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		
		unsigned long long block_size = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
		unsigned long long offset = 0;
		
		for (unsigned long long level = 0; level < mipmap_count && (width || height); ++level) {
			unsigned long long size = ((width + 3) / 4) * ((height + 3) / 4) * block_size;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 0, size, data + offset);
			
			offset += size;
			
			width >>= 1;
			width >>= 1;
			
		}
		
		gl_texture_parameters();
		
		kos_bda_implementation.temp_value = (unsigned long long) texture_id;
		*result = (unsigned long long*) &kos_bda_implementation.temp_value;
		
	}
	
#endif
