struct osd_info {
	int currcon;
	
	// Physical base address
	unsigned long video_pbase;
	// Relative base address (relative to start of decoder memory)
    	u32 video_rbase;
	// Mapped base address
	char *video_vbase;
	// Buffer size
	u32 video_buffer_size;

	/* video_base rounded down as required by hardware MTRRs */
	unsigned long fb_start_aligned_physaddr;
	/* video_base rounded up as required by hardware MTRRs */
	unsigned long fb_end_aligned_physaddr;

	// Track visible size & position of OSD
	int window_x;
	int window_y;
	int window_width;
	int window_height;

	// Track the OSD pan
	u32 pan;
	u16 pan_x, pan_y;
	
	/* alpha value for osd firmware resets */
	int global_alpha;
	int global_alpha_state;
	int local_alpha_state;

	// Color key info
	int color_key_state;
	uint32_t color_key;

	// Current palette
	u32 palette[256];

	// Store the buffer offset
	int set_osd_coords_x;
	int set_osd_coords_y;
	
	// Current dimensions (NOT VISIBLE SIZE!)
	int display_width;
	int display_height;
	int display_byte_stride;
	
	// Current bits per pixel
	int bits_per_pixel;
	int bytes_per_pixel;
	
	// Frame buffer stuff
	struct fb_info ivtvfb_info;
	struct fb_var_screeninfo ivtvfb_defined;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	struct display disp;
	union {
		u32 cfb32[16];
	} fbcon_cmap;
	struct display_switch ivtvfb_sw;
#else
	struct fb_fix_screeninfo ivtvfb_fix;
#endif /* LINUX26 */


};
