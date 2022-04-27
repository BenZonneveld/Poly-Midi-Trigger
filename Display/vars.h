#define OLED_DC     5
#define OLED_CS     4
#define OLED_RESET  6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FONTSIZE    1
#define MAXNOTES  16
#define TRIG_PIN  7
#define MAXLOOP   512
#define MINTRIG   5
#define DATAPOINTS 4
#define NOTELENGTH  1    // Pin to use for notelength adc
#define NOTELENGTH_ADC_IN 0
#define PARAM       2    // Pin to use for Parameter ADC
#define PARAM_ADC_IN 1
#define MENUBUTTON  8
#define MAXMENU     3

extern char	buffer[];
//volatile uint8_t state = false;
extern uint8_t UpdateDisp;
extern int triglength;

extern uint8_t oldnotecount;
extern uint8_t oldparam;
//extern uint8_t arp_mode;
extern int8_t arp_note;

extern bool needsUpdate;
extern bool parequal;
extern uint8_t rand_length;
extern uint8_t new_rand;
extern bool menu_mode;
extern bool arpdir;
//extern enum mode;

//typedef enum { ARP_OFF, ARP_UP, ARP_DOWN, ARP_UPDWNA, ARP_UPDWNB, ARP_DWNUPA, ARP_DWNUPB, ARP_RANDOM }e_mode;
//extern enum btn_event;// { BTNONE, BTCLICK, BTDBLC, BTHOLD, BTLONG };
//extern enum eeprom;// { EEPROM_ARP, EEPROM_TX, EEPROM_RX, EEPROM_TRIG };
//extern e_mode arp_mode;

extern const char* const txt_options[];
extern const char* const nameArp[];
extern const char* const namearp[];
extern const char* const nameOpt[];