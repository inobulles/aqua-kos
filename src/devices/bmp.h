
#pragma pack(push, 1)
	typedef struct {
		uint16_t magic;
		uint32_t file_size;
		uint16_t reserved_1, reserved_2;
		uint32_t offset;
	} bitmap_header_t;
	
	typedef struct {
		uint32_t size;
		int32_t width, height;
		uint16_t planes, bpp;
		uint32_t compression_type, image_bytes;
		int64_t pixels_per_meter_x, pixels_per_meter_y;
		uint32_t colour_count, important_colours;
	} bitmap_info_header_t;
#pragma pack(pop)

static void bmp_device_handle(unsigned long long** result, const char* __data) {
	unsigned long long* command = (unsigned long long*) __data;
	char* data = (char*) command[0];
	if (!data) {
		*result = (unsigned long long*) 0;
		return;
	}
	
	bitmap_header_t header = *((bitmap_header_t*) data);
	data += sizeof(bitmap_header_t);
	
	if (header.magic != 0x4D42) {
		printf("WARNING Data is not BMP\n");
		*result = (unsigned long long*) 0;
		return;
	}
	
	bitmap_info_header_t info_header = *((bitmap_info_header_t*) data);
	data = (char*) command[0] + header.offset;
	
	uint8_t* data8 = (uint8_t*) data;
	char* final_data = (char*) malloc(info_header.image_bytes);
	
	kos_bda_implementation.temp_value_field[1] = (unsigned long long) info_header.width;
	kos_bda_implementation.temp_value_field[2] = (unsigned long long) info_header.height;
	
	unsigned long long components = info_header.bpp >> 3;
	unsigned long long pitch = info_header.width * components;
	
	for (unsigned long long i = 0; i < info_header.height; i++) {
		memcpy(final_data + i * pitch, data8 + (info_header.height - i - 1) * pitch, pitch);
	}
	
	//~ swizzle_abgr = (info_header.bpp == 24) + 1;
	kos_bda_implementation.temp_value_field[0] = texture_create((unsigned long long) final_data, info_header.bpp, (unsigned long long) info_header.width, (unsigned long long) info_header.height);
	*result = (unsigned long long*) kos_bda_implementation.temp_value_field;
	free(final_data);
	//~ swizzle_abgr = 0;
}
