#define OLED_DC     5
#define OLED_CS     4
#define OLED_RESET  6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define FONTSIZE    1
//char buffer[10];
#define MAXNOTES  16
#define TRIG_PIN  7
#define MAXLOOP   512
#define MINTRIG   5
#define DATAPOINTS 4
#define NOTELENGTH  A1
#define PARAM       A0
#define MENUBUTTON  8
#define MAXMENU     3

//uint8_t menu_item = 0;
//volatile uint8_t state = false;
//volatile uint8_t UpdateDisp = false;
//int triglength = MINTRIG;

//uint8_t oldnotecount = 0;
//uint8_t oldparam = 0;
extern uint8_t arp_mode;
extern int8_t arp_note;

//bool needsUpdate = false;
//bool parequal = false;
//volatile uint8_t rand_length = 0;
//uint8_t new_rand = 255;
//bool menu_mode = false;
extern bool arpdir;
//extern enum mode;

enum mode { ARP_OFF, ARP_UP, ARP_DOWN, ARP_UPDWNA, ARP_UPDWNB, ARP_DWNUPA, ARP_DWNUPB, ARP_RANDOM };
enum btn_event { BTNONE, BTCLICK, BTDBLC, BTHOLD, BTLONG };
enum eeprom { EEPROM_ARP, EEPROM_TX, EEPROM_RX, EEPROM_TRIG };
/*
const char txt_ARP0[] = "Off";
const char txt_ARP1[] = "Up";
const char txt_ARP2[] = "Down";
const char txt_ARP3[] = "UpDownA";
const char txt_ARP4[] = "UpDownB";
const char txt_ARP5[] = "DownUpA";
const char txt_ARP6[] = "DownUpB";
const char txt_ARP7[] = "Random";

const char txt_arp0[] = "Off";
const char txt_arp1[] = "Up";
const char txt_arp2[] = "Down";
const char txt_arp3[] = "U>D A";
const char txt_arp4[] = "U>D B";
const char txt_arp5[] = "D>U A";
const char txt_arp6[] = "D>U B";
const char txt_arp7[] = "Rand";

//const char txt_SWTCH1[] PROGMEM = "On";

const char txt_opt[] = "Options";
const char* const txt_options[] = { txt_opt };

const char txt_opt0[] = "Arp Mode";
const char txt_opt1[] = "RX Channel";
const char txt_opt2[] = "TX Channel";
const char txt_opt3[] = "Trig Channel";

const char* const nameArp[] = { txt_ARP0, txt_ARP1, txt_ARP2, txt_ARP3, txt_ARP4, txt_ARP5, txt_ARP6, txt_ARP7 };
const char* const namearp[] = { txt_arp0, txt_arp1, txt_arp2, txt_arp3, txt_arp4, txt_arp5, txt_arp6, txt_arp7 };
const char* const nameOpt[] = { txt_opt0, txt_opt1, txt_opt2, txt_opt3 };
*/
//PROGMEM const char * const nameSwtch[]= {txt_ARP0, txt_SWTCH1};
