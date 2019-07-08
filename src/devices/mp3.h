
// thanks https://svn.xiph.org/experimental/giles/mp3dump.c

#ifndef __AQUA__KOS_DEVICES_MP3_H
	#define __AQUA__KOS_DEVICES_MP3_H
	
	typedef struct {
		int version;
		int layer;
		int errp;
		int bitrate;
		int freq;
		int pad;
		int priv;
		int mode;
		int modex;
		int copyright;
		int original;
		int emphasis;
		
	} mp3_header_t;
	
	static inline void mp3_parse(const unsigned char* pointer, mp3_header_t* header) {
		const int bitrates   [16] = {0,  32000,  40000,  48000,  56000,  64000,  80000,  96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 0};
		const int samplerates[4]  = {44100, 48000, 32000};
		
		header->version =      (pointer[1] & 0x08) >> 3;
		header->layer   = 4 - ((pointer[1] & 0x06) >> 1);
		header->errp    =      (pointer[1] & 0x01);
		
		header->bitrate =    bitrates[(pointer[2] & 0xF0) >> 4];
		header->freq    = samplerates[(pointer[2] & 0x0C) >> 2];
		header->pad     =             (pointer[2] & 0x02) >> 1;
		header->priv    =             (pointer[2] & 0x01);
		
		header->mode      = (pointer[3] & 0xC0) >> 6;
		header->modex     = (pointer[3] & 0x30) >> 4;
		header->copyright = (pointer[3] & 0x08) >> 3;
		header->original  = (pointer[3] & 0x04) >> 2;
		header->emphasis  = (pointer[3] & 0x03);
		
	} static inline int mp3_framesize(mp3_header_t* header) {
		int size = header->bitrate * (header->layer == 1 ? 48 : 144) / header->freq;
		size += !!header->pad;
		return size;
		
	} static inline double mp3_header_duration(mp3_header_t* header) {
		return (double) mp3_framesize(header) * 8 / header->bitrate;
		
	} static inline unsigned char mp3_mpcom(unsigned char command, const unsigned char* a, const unsigned char* b) {
		if     (a > b) return 0;
		if (b - a < 4) return 0;
		
		return a[0] == 0x00 && a[1] == 0x00 && a[2] == 0x01 && a[3] == command;
		
	}
	
	static inline unsigned char mp3_mpack(const unsigned char* a, const unsigned char* b) { return mp3_mpcom(0xBA, a, b); }
	static inline unsigned char mp3_mpsys(const unsigned char* a, const unsigned char* b) { return mp3_mpcom(0xBB, a, b); }
	static inline unsigned char mp3_mpmap(const unsigned char* a, const unsigned char* b) { return mp3_mpcom(0xBC, a, b); }
	
	static inline unsigned char mp3_mp3(const unsigned char* a, const unsigned char* b) {
		if     (a > b) return 0;
		if (b - a < 4) return 0;
		
		if (a[0] == 0xFF && (a[1] & 0xE0) == 0xE0) {
			if (((a[1] & 0x06) >> 1) == 0)  return 0;
			if (((a[2] & 0xF0) >> 4) == 15) return 0;
			if (((a[2] & 0x0C) >> 2) == 3)  return 0;
			
			return 1;
			
		}
		
		return 0;
		
	} static inline unsigned char mp3_id3(const unsigned char* a, const unsigned char* b) {
		if     (a >  b) return 0;
		if (b - a < 10) return 0;
		
		if (a[0] == 'I' && a[1] == 'D' && a[2] == '3') {
			if (a[3] == 0xFF || a[4] == 0xFF) return 0;
			if (a[6] &  0x80 || a[7] &  0x80 || a[8] & 0x80) return 0;
			
			return 1;
			
		}
		
		return 0;
		
	}
	
	static inline unsigned long long mp3_load(unsigned long long __path) {
		GET_PATH(__path)
		
		#if KOS_USES_JNI
			/// TODO
		#else
			sound_t* self = (sound_t*) malloc(sizeof(sound_t));
			memset(self, 0, sizeof(sound_t));
			
			mad_stream_init(&self->stream);
			mad_synth_init (&self->synth);
			mad_frame_init (&self->frame);
			
			self->fp = fopen(path, "r");
			if (!self->fp) {
				printf("WARNING Failed to open mp3 file %s\n", path);
				return 0;
				
			}
			
			self->fd = fileno(self->fp);
			if (fstat(self->fd, &self->meta) < 0) {
				printf("WARNING Failed to stat %s\n", path);
				fclose(self->fp);
				return 0;
				
			}
			
			self->bytes = self->meta.st_size;
			unsigned char* buffer = (unsigned char*) malloc(self->bytes);
			fread(buffer, 1, self->bytes, self->fp);
			
			unsigned char* a = buffer;
			unsigned char* c = buffer;
			unsigned char* b = buffer + self->bytes - 2;
			
			mp3_header_t header;
			self->ecart = 1.0; // one second span between each checkpoint
			
			self->checkpoint_count = 1;
			self->checkpoints = (sound_checkpoint_t*) malloc(self->checkpoint_count * sizeof(sound_checkpoint_t));
			self->checkpoints[0].bytes = 0;
			
			long hbytes = 0;
			while (a < b) {
				if (mp3_id3(a, b)) {
					if (a - c) printf("WARNING ID3 parser lost sync\n");
					long skip = 10 + (a[9] | (a[8] << 7) | (a[7] << 14) | (a[6] << 21));
					
					a      += skip;
					hbytes += skip;
					c = a;
					
				} else if (mp3_mp3(a, b)) {
					if (a - c) printf("WARNING MP3 parser lost sync\n");
					mp3_parse(a, &header);
					
					long skip = mp3_framesize(&header);
					if (skip <= 4) printf("WARNING MP3 parser failed to calculate frame size\n");
					
					unsigned long long old_seconds = (unsigned long long) (self->seconds / self->ecart);
					self->seconds += mp3_header_duration(&header);
					
					if ((unsigned long long) (self->seconds / self->ecart) != old_seconds) { // new second, new checkpoint
						self->checkpoints = (sound_checkpoint_t*) realloc(self->checkpoints, ++self->checkpoint_count * sizeof(sound_checkpoint_t));
						self->checkpoints[self->checkpoint_count - 1].bytes = a - buffer + skip;
						
					}
					
					a      += skip;
					hbytes += 4;
					c = a;
					
				} else {
					if      (mp3_mpack(a, b)) {} // MPEG pack header
					else if (mp3_mpsys(a, b)) {} // MPEG system header
					else if (mp3_mpmap(a, b)) {} // MPEG program map
					
					a++;
					
				}
				
			}
			
			self->frequency = header.freq;
			mfree(buffer, self->bytes);
			
			self->input_stream = mmap(0, self->bytes, PROT_READ, MAP_SHARED, self->fd, 0);
			mad_stream_buffer(&self->stream, (const unsigned char*) self->input_stream, self->bytes);
			
			return (unsigned long long) self;
		#endif
		
		return 0;
		
	}
	
	static void mp3_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 1;
		*result = &kos_bda_implementation.temp_value;
		
		if (command[0] == 's') kos_bda_implementation.temp_value = mp3_load(command[1]);
		else KOS_DEVICE_COMMAND_WARNING("mp3")
		
	}
	
#endif
