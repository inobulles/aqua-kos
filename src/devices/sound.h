// thanks https://lauri.xn--vsandi-pxa.com/2013/12/implementing-mp3-player.en.html

#ifndef __AQUA__KOS_DEVICES_SOUND_H
	#define __AQUA__KOS_DEVICES_SOUND_H
	
	#include <pthread.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	
	#if !KOS_USES_JNI
		#include <mad.h> // for mp3.h
		
		#include <pulse/pulseaudio.h>
		#include <pulse/volume.h>
	#endif
	
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
		#endif
		
	} sound_t;
	
	#if !KOS_USES_JNI
		static sound_t* pulse_current_sound;
		
		static pa_threaded_mainloop* pulse_mainloop;
		static pa_mainloop_api*      pulse_api;
		static pa_context*           pulse_context;
		static pa_stream*            pulse_stream;
		
		static inline int sound_scale(mad_fixed_t sample) {
			sample += (1L << (MAD_F_FRACBITS - 16));
			
			if      (sample >= MAD_F_ONE) sample =  MAD_F_ONE - 1;
			else if (sample < -MAD_F_ONE) sample = -MAD_F_ONE;
			
			return sample >> (MAD_F_FRACBITS + 1 - 16);
			
		}
		
		void pulse_free(void) {
			if (pulse_mainloop) {
				/// TODO
				
			}
			
		}
		
		void pulse_context_state_callback(pa_context* context, void* mainloop) { pa_threaded_mainloop_signal(mainloop, 0); }
		void pulse_state_callback        (pa_stream*  stream,  void* mainloop) { pa_threaded_mainloop_signal(mainloop, 0); }
		
		void pulse_write_callback(pa_stream* stream, size_t requested_bytes, void* data) {
			sound_t* self = pulse_current_sound;
			if (!self) return;
			
			int remaining = requested_bytes;
			while (remaining > 0) {
				uint16_t* buffer = NULL;
				
				if (mad_frame_decode(&self->frame, &self->stream)) {
					if (MAD_RECOVERABLE(self->stream.error) || self->stream.error == MAD_ERROR_BUFLEN) continue;
					else break;
					
				}
				
				mad_synth_frame(&self->synth, &self->frame);
				// &self->frame.header, &self->synth.pcm
				
				size_t chunk_bytes = 1152 * 4;
				if (chunk_bytes > remaining) chunk_bytes = remaining;
				
				pa_stream_begin_write(stream, (void**) &buffer, &chunk_bytes);
				
				mad_fixed_t const* left  = self->synth.pcm.samples[0];
				mad_fixed_t const* right = self->synth.pcm.samples[1];
				
				for (int i = 0; i < self->synth.pcm.length; i++) {
					buffer[(self->synth.pcm.length - i) * 2    ] = self->values[0] = sound_scale(* left++);
					buffer[(self->synth.pcm.length - i) * 2 + 1] = self->values[1] = sound_scale(*right++);
					
				}
				
				pa_stream_write(pulse_stream, buffer, chunk_bytes, NULL, 0LL, PA_SEEK_RELATIVE);
				remaining -= chunk_bytes;
				
			}
			
		}
		
		void pulse_success_callback(pa_stream* stream, int success, void* data) {
			return;
			
		}
	#endif
	
	static inline unsigned long long sound_settings(unsigned long long frequency, unsigned long long channels, unsigned long long format) {
		#if KOS_USES_JNI
			return 1; /// TODO
		#else
			pulse_free();
			if (!(pulse_mainloop = pa_threaded_mainloop_new())) {
				printf("WARNING Failed to create PulseAudio threaded mainloop\n");
				return 1;
				
			} if (!(pulse_context = pa_context_new(pulse_api = pa_threaded_mainloop_get_api(pulse_mainloop), "playback"))) {
				printf("WARNING Failed to create PulseAudio context\n");
				return 1;
				
			}
			
			pa_context_set_state_callback(pulse_context, &pulse_context_state_callback, pulse_mainloop);
			pa_threaded_mainloop_lock(pulse_mainloop);
			
			if (pa_threaded_mainloop_start(pulse_mainloop)) {
				printf("WARNING Failed to start PulseAudio threaded mainloop\n");
				return 1;
				
			} if (pa_context_connect(pulse_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL)) {
				printf("WARNING Failed to connect PulseAudio context\n");
				return 1;
				
			} while (1) {
				pa_context_state_t state = pa_context_get_state(pulse_context);
				if (!PA_CONTEXT_IS_GOOD(state)) {
					printf("WARNING PulseAudio context is not good\n");
					return 1;
					
				} if (state == PA_CONTEXT_READY) {
					break;
					
				}
				
				pa_threaded_mainloop_wait(pulse_mainloop);
				
			}
			
			pa_sample_spec spec = { .format = format, .rate = frequency, .channels = channels };
			pa_channel_map map;
			pa_channel_map_init_stereo(&map);
			
			pulse_stream = pa_stream_new(pulse_context, "AQUA sound connection", &spec, &map);
			
			pa_stream_set_state_callback(pulse_stream, pulse_state_callback, pulse_mainloop);
			pa_stream_set_write_callback(pulse_stream, pulse_write_callback, pulse_mainloop);
			
			pa_buffer_attr attr = { .maxlength = (uint32_t) -1, .tlength = (uint32_t) -1, .prebuf = (uint32_t) -1, .minreq = (uint32_t) -1 };
			pa_stream_flags_t flags = PA_STREAM_START_CORKED | PA_STREAM_INTERPOLATE_TIMING | PA_STREAM_NOT_MONOTONIC | PA_STREAM_AUTO_TIMING_UPDATE | PA_STREAM_ADJUST_LATENCY;
			
			if (pa_stream_connect_playback(pulse_stream, NULL, &attr, flags, NULL, NULL)) {
				printf("WARNING Failed to connect PulseAudio stream to playback\n");
				return 1;
				
			} while (1) {
				pa_stream_state_t state = pa_stream_get_state(pulse_stream);
				if (!PA_STREAM_IS_GOOD(state)) {
					printf("WARNING PulseAudio stream is not good\n");
					return 1;
					
				} if (state == PA_STREAM_READY) {
					break;
					
				}
				
				pa_threaded_mainloop_wait(pulse_mainloop);
				
			}
			
			pa_threaded_mainloop_unlock(pulse_mainloop);
		#endif
		
		return 0;
		
	} static inline unsigned long long sound_action(unsigned long long __self, unsigned long long action) {
		sound_t* self = (sound_t*) __self;
		
		if (action == 0) { // stop
			#if KOS_USES_JNI
				return 1; /// TODO
			#else
				self->playing = 0;
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
			pulse_current_sound = self;
			pa_stream_cork(pulse_stream, 0, pulse_success_callback, pulse_mainloop);
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
