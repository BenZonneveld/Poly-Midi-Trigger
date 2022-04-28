#ifndef DISP_HAGL
#define DISP_HAGL
extern const unsigned char font6x13B_ISO8859_15[];
extern const unsigned char font9x18B_ISO8859_15[];

#define BLACK (hagl_color(0,0,0))
#define WHITE (hagl_color(255,255,255))
void screen_core_hagl(void *param);
void init_display();
#endif