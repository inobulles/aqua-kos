
#ifndef __AQUA__SDL2_SRC_KOS_BCM_BCM_H
	#define __AQUA__SDL2_SRC_KOS_BCM_BCM_H
	
	#if KOS_USES_OPENGLES
		void bcm_ogles_init(kos_t* kos) {
			printf("Setting up OpenGL ES ...\n");
			bcm_ogles_setup(kos);
			
		}
		
		void bcm_ogles_exit(kos_t* kos) {
			printf("Exitting OpenGL ES ...\n");
			bcm_ogles_exit(kos);
			
		}
	#endif
	
#endif
