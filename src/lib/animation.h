
#define FLOAT_ONE 1000000

typedef struct {
	float original_time;
	float time; // in seconds
	
	float elapsed;
	
	float original_base;
	float base;
	
	unsigned char backwards;
	float         offset;
	
} animation_t;

void new_animation(animation_t* self, float base, float time) {
	self->backwards = 0;
	
	self->elapsed   = 0.0f;
	self->offset    = 0.0f;
	
	self->original_time = time;
	self->time          = time;
	
	self->original_base = base;
	self->base          = base;
	
}

void reset_animation(animation_t* self) {
	self->backwards = 0;
	self->elapsed   = 0.0f;
	
	self->time = self->original_time;
	self->base = self->original_base;
	
}

void reverse_animation(animation_t* self) {
	self->backwards = !self->backwards;
	reset_animation(self);
	
}

#include <math.h>

float animate(animation_t* self, unsigned long long fps) {
	if (fps == 0) {
		return self->base;
		
	}
	
	if (self->backwards) self->elapsed -= 1.0f / fps;
	else                 self->elapsed += 1.0f / fps;
	
	#define ELAPSED (self->elapsed + self->offset)
	
	if (ELAPSED > 0.0f) {
		float radius = self->time;
		float x      = ELAPSED;
		
		float a =      radius  *      radius;
		float b = (x - radius) * (x - radius);
		
		self->base = (float) sqrt(a - b) / self->time;
		
		if (x > radius) {
			//~ self->base = 0;
			
		}
		
	} else {
		self->base = 0.0f;
		return 0;
		
	}
	
	if (self->backwards) return 1.0f - self->base;
	else                 return        self->base;
	
}
