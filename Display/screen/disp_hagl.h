#ifndef DISP_HAGL
#define DISP_HAGL
void screen_core_hagl(void *param);
void haglDoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text);
void haglCenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text);
void haglSetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color);
void haglShowLength();
void haglShowNotes();
void haglShowDisplay();
void haglShowRand();
void Options();
void OptionName(uint8_t item);
void OptionValue(uint8_t item, uint8_t value);
#endif