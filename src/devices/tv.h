#define TV_HDMI 0
#define TV_SDTV 1

#define TV_PROGRESSIVE 0
#define TV_INTERLACED  1

#define TV_ASPECT_4_3  0
#define TV_ASPECT_14_9 1
#define TV_ASPECT_16_9 2

static SDTV_MODE_T tv_sdtv_mode = SDTV_MODE_PAL; /// TODO give applications a way to change this
static int tv_type = TV_HDMI;

static void tv_get_supported_modes(void) {
	uint32_t max_cea_mode_count = 64;
	uint32_t max_dmt_mode_count = 64;
	
	TV_SUPPORTED_MODE_NEW_T* cea_modes = (TV_SUPPORTED_MODE_NEW_T*) malloc(max_cea_mode_count * sizeof TV_SUPPORTED_MODE_NEW_T);
	TV_SUPPORTED_MODE_NEW_T* dmt_modes = (TV_SUPPORTED_MODE_NEW_T*) malloc(max_dmt_mode_count * sizeof TV_SUPPORTED_MODE_NEW_T);
	
	HDMI_RES_GROUP_T preferred_group = HDMI_RES_GROUP_CEA;
	uint32_t preferred_mode = 0;
	
	vc_tv_hdmi_get_supported_modes_new(HDMI_RES_GROUP_CEA, cea_modes, &cea_mode_count, &preferred_group, &preferred_mode);
	vc_tv_hdmi_get_supported_modes_new(HDMI_RES_GROUP_DMT, dmt_modes, &dmt_mode_count, &preferred_group, &preferred_mode);
}

static void tv_sdtv(unsigned long long aspect) {
	tv_type = TV_SDTV;
	SDTV_OPTIONS_T options;
	
	if      (aspect == TV_ASPECT_4_3 ) options.aspect = SDTV_ASPECT_4_3;
	else if (aspect == TV_ASPECT_14_9) options.aspect = SDTV_ASPECT_14_9;
	else if (aspect == TV_ASPECT_16_9) options.aspect = SDTV_ASPECT_16_9;
	
	vc_tv_sdtv_power_on(tv_sdtv_mode, &options);
}

static void tv_preferred(void) {
	if (tv_type == TV_HDMI) vc_tv_hdmi_power_on_preferred();
	else printf("WARNING In SDTV mode, you cannot set the preferred resolution\n");
}

static unsigned long long tv_has_audio(unsigned long long format, unsigned long long channel_count, unsigned long long sample_rate, unsigned long long bitrate) { /// TODO
	return tv_type == TV_HDMI ? vc_tv_hdmi_audio_supported(format, channel_count, sample_rate, bitrate) : 0;
}

static void tv_device_handle(unsigned long long** result, const char* data) {
	unsigned long long* command = (unsigned long long*) data;
	unsigned char result_string = 0;
	
	if (command[0] == 'n') {
		result_string = 1;
		strncpy(kos_bda_implementation.temp_string, TVSERVICE_CLIENT_NAME, sizeof kos_bda_implementation.temp_string);
	}
	
	else if (command[0] == 'h') tv_type = TV_HDMI;
	else if (command[0] == 's') tv_sdtv(command[1]);
	
	else if (command[0] == 'r') tv_resolution();
	else if (command[0] == 'p') tv_preferred();
	
	else if (command[0] == 'a') kos_bda_implementation.temp_value = tv_has_audio();
	
	else if (command[0] == 'f') vc_tv_poweroff();
	
	*result = result_string ? (unsigned long long*) kos_bda_implementation.temp_string : &kos_bda_implementation.temp_value;
}
