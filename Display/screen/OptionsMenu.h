//void Options();
//void OptionName(uint8_t item);
//void OptionValue(uint8_t item, uint8_t value);
#include "MovingAvg.h"
#include "defines.h"

#ifdef __cplusplus
class cOptions {
public:
	cOptions() :
		menu_item(0),
		triglength(MINTRIG),
		oldnotecount(0),
		oldparam(0),
		parequal(0),
		rand_length(0),
		new_rand(255),
		myTrigLength(DATAPOINTS),
		paramVal(DATAPOINTS),
		buffer()
	{}
	void Options();
	uint8_t getTrigLength() { return triglength; }
	uint8_t getRandLength() { return rand_length; }
private:
	void ShowLength();
	void ShowNotes();
	void ShowDisplay();
	void ShowRand();
	void OptionName(uint8_t item);
	void OptionValue(uint8_t item, uint8_t value);
	void DoText(uint8_t Xpos, uint8_t Ypos, bool FontSize, char* text);
	void CenterText(uint8_t Xcenter, uint8_t Ypos, bool FontSize, char* text);
	void SetDisplayPart(uint8_t x, uint8_t cols, uint8_t row, uint8_t rows, bool color);
private:
	uint8_t menu_item;
	uint8_t UpdateDisp;
	int triglength;
	uint8_t oldnotecount;
	uint8_t oldparam;
	bool needsUpdate;
	bool parequal;
	uint8_t rand_length;
	uint8_t new_rand;
	bool menu_mode;
	movingAvg myTrigLength;
	movingAvg paramVal;
	char	buffer[TEXTBUFFER];
};
extern cOptions Opt;

#endif


#ifdef __cplusplus
extern "C"
{
#endif
void OptionsLoop(void* param);
void LoadOptions();
void SaveOptions();
uint8_t getTrigLength();
uint8_t getRandLength();
#ifdef __cplusplus
}
#endif

extern const char* const txt_options[];
extern const char* const nameArp[];
extern const char* const namearp[];
extern const char* const nameOpt[];