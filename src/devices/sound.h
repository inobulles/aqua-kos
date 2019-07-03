// thanks https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html

#ifndef __AQUA__KOS_DEVICES_SOUND_H
	#define __AQUA__KOS_DEVICES_SOUND_H
	
	#include <pthread.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	
	typedef struct {
		unsigned long long bytes;
		
	} sound_checkpoint_t;
	
	typedef struct {
		double seconds;
		int frequency;
		
		unsigned char paused;
		unsigned char playing;
		
		double  left_volume;
		double right_volume;
		
		signed int values[2];
		
		double ecart;
		unsigned long long checkpoint_count;
		sound_checkpoint_t* checkpoints;
		
		#if !KOS_USES_JNI
			FILE* fp;
			int fd;
			
			struct stat meta;
			char* input_stream;
			
			struct mad_stream stream;
			struct mad_synth  synth;
			struct mad_frame  frame;
			
			pthread_t thread;
		#endif
		
	} sound_t;
	
	#if !KOS_USES_JNI
		static pa_simple* pulse_device = (pa_simple*) 0;
		static int        pulse_error  = 0;
		
		static inline int sound_scale(mad_fixed_t sample) {
			sample += (1L << (MAD_F_FRACBITS - 16));
			
			if      (sample >= MAD_F_ONE) sample =  MAD_F_ONE - 1;
			else if (sample < -MAD_F_ONE) sample = -MAD_F_ONE;
			
			return sample >> (MAD_F_FRACBITS + 1 - 16);
			
		} static void sound_output(sound_t* self, struct mad_header const* header, struct mad_pcm* pcm) {
			register int sample_count = pcm->length;
			
			mad_fixed_t const* left  = pcm->samples[0];
			mad_fixed_t const* right = pcm->samples[1];
			
			#define BYTES (1152 * 4)
			
			static char stream[BYTES];
			if (pcm->channels == 2) {
				while (sample_count--) {
					signed int sample = (self->values[0] = sound_scale(*left++)) * self->left_volume;
					
					stream[(pcm->length - sample_count) * 4]     = ((sample >> 0) & 0xFF);
					stream[(pcm->length - sample_count) * 4 + 1] = ((sample >> 8) & 0xFF);
					
					sample = (self->values[1] = sound_scale(*right++)) * self->right_volume;
					
					stream[(pcm->length - sample_count) * 4 + 2] = ((sample >> 0) & 0xFF);
					stream[(pcm->length - sample_count) * 4 + 3] = ((sample >> 8) & 0xFF);
					
				} if (pa_simple_write(pulse_device, stream, (size_t) BYTES, &pulse_error)) {
					printf("WARNING PulseAudio failed to write (%s)\n", pa_strerror(pulse_error));
					
				}
				
			} else {
				printf("WARNING Mono is not supported\n");
				
			}
			
		}
		
		static void* sound_loop(void* __self) {
			sound_t* self = (sound_t*) __self;
			
			while (self->playing) {
				if (self->paused) {
					usleep(20);
					continue;
					
				} if (mad_frame_decode(&self->frame, &self->stream)) {
					if (MAD_RECOVERABLE(self->stream.error) || self->stream.error == MAD_ERROR_BUFLEN) continue;
					else break;
					
				}
				
				mad_synth_frame(&self->synth, &self->frame);
				sound_output(self, &self->frame.header, &self->synth.pcm);
				
			}
			
			self->playing = 0;
			pthread_exit(NULL);
			
		}
	#endif
	
	static inline unsigned long long sound_settings(unsigned long long frequency, unsigned long long channels, unsigned long long format) {
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			if (pulse_device) {
				pa_simple_free(pulse_device);
				
			}
			
			pa_sample_spec spec = { .format = format, .rate = frequency, .channels = channels };
			if (!(pulse_device = pa_simple_new(NULL, "AQUA sound connection", PA_STREAM_PLAYBACK, NULL, "playback", &spec, NULL, NULL, &pulse_error))) {
				printf("WARNING Failed to create a PulseAudio connection (%s)\n", pa_strerror(pulse_error));
				return 1;
				
			}
		#endif
		
		return 0;
		
	} static inline unsigned long long sound_action(unsigned long long __self, unsigned long long action) {
		sound_t* self = (sound_t*) __self;
		
		if (action == 0) { // stop
			#if KOS_USES_JNI
				return 1; /// TODO
			#else
				self->playing = 0;
				
				if (self->thread) {
					pthread_join(self->thread, NULL);
					self->thread = (pthread_t) NULL;
					
				}
			#endif
			
		} else if (action == 1) { // pause
			#if KOS_USES_JNI
				return 1; /// TODO
			#else
				self->paused = 1;
			#endif
			
		} else if (action == 2) { // resume
			#if KOS_USES_JNI
				return 1; /// TODO
			#else
				self->paused = 0;
			#endif
			
		} else {
			printf("WARNING Unknown sound action %lld\n", action);
			return 1;
			
		}
		
		return 0;
		
	} static inline unsigned long long sound_volume(unsigned long long __self, double volume) {
		sound_t* self = (sound_t*) __self;
		
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			self-> left_volume = volume;
			self->right_volume = volume;
		#endif
		
		return 0;
		
	} static inline unsigned long long sound_play(unsigned long long __self) {
		sound_t* self = (sound_t*) __self;
		
		self->paused = 0;
		self->playing = 1;
		
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			int temp_error = pthread_create(&self->thread, NULL, sound_loop, (void*) self);
			if (temp_error) {
				printf("WARNING Failed to play sound (%d)\n", temp_error);
				return 1;
				
			}
		#endif
		
		return 0;
		
	} static inline unsigned long long sound_position(unsigned long long __self, double fraction) {
		fraction = fraction > 1.0 || fraction < 0.0 ? 0.0 : fraction;
		
		sound_t* self = (sound_t*) __self;
		unsigned long long bytes = self->checkpoints[(unsigned long long) (self->seconds * fraction / self->ecart)].bytes;
		
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			mad_stream_buffer(&self->stream, (const unsigned char*) self->input_stream, self->meta.st_size);
			mad_stream_skip(&self->stream, bytes);
		#endif
		
		return 0;
		
	}
	
	static inline unsigned long long sound_remove(unsigned long long __self) {
		sound_t* self = (sound_t*) __self;
		sound_action(__self, 0);
		
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			mad_stream_finish(&self->stream);
			mad_synth_finish (&self->synth);
			mad_frame_finish (&self->frame);
			
			fclose(self->fp);
		#endif
		
		mfree(self->checkpoints, self->checkpoint_count * sizeof(sound_checkpoint_t));
		mfree(self, sizeof(sound_t));
		
		return 0;
		
	} static inline double sound_length(unsigned long long __self) {
		sound_t* self = (sound_t*) __self;
		return self->seconds;
		
	} static inline double sound_frequency(unsigned long long __self) {
		sound_t* self = (sound_t*) __self;
		return self->frequency;
		
	} static inline double sound_value(unsigned long long __self, unsigned long long channel) {
		sound_t* self = (sound_t*) __self;
		return (double) self->values[channel] / 0x8000;
		
	}
	
	static void sound_device_handle(unsigned long long** result, const char* data) {
		unsigned long long* command = (unsigned long long*) data;
		kos_bda_implementation.temp_value = 1;
		*result = &kos_bda_implementation.temp_value;
		
		if      (command[0] == 's') kos_bda_implementation.temp_value = sound_settings(command[1],          command[2], command[3]);
		else if (command[0] == 'a') kos_bda_implementation.temp_value = sound_action  (command[1],          command[2]);
		else if (command[0] == 'v') kos_bda_implementation.temp_value = sound_volume  (command[1], (double) command[2] / FLOAT_ONE);
		else if (command[0] == 'm') kos_bda_implementation.temp_value = sound_play    (command[1]);
		else if (command[0] == 'p') kos_bda_implementation.temp_value = sound_position(command[1], (double) command[2] / FLOAT_ONE);
		else if (command[0] == 'r') kos_bda_implementation.temp_value = sound_remove  (command[1]);
		
		else if (command[0] == 'i') {
			*result = kos_bda_implementation.temp_value_field;
			
			kos_bda_implementation.temp_value_field[0] = (unsigned long long) (sound_length   (command[1]) * FLOAT_ONE);
			kos_bda_implementation.temp_value_field[1] =                       sound_frequency(command[1]);
			
		} else if (command[0] == 'u') {
			*result = kos_bda_implementation.temp_value_field;
			
			kos_bda_implementation.temp_value_field[0] = (unsigned long long) (sound_value(command[1], 0) * FLOAT_ONE);
			kos_bda_implementation.temp_value_field[1] = (unsigned long long) (sound_value(command[1], 1) * FLOAT_ONE);
			
		} else {
			KOS_DEVICE_COMMAND_WARNING("sound")
			
		}
		
	}
	
#endif
