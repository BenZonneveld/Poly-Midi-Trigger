#ifndef DISP+SSD1306
#define DISP_SSD1306
void ssd1306DoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text);
void ssd1306CenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text);
void ssd1306SetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color);
void ssd1306ShowLength();
void ssd1306ShowNotes();
void ssd1306ShowDisplay();
void ssd1306ShowRand();
#endif