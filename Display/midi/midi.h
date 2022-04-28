#pragma once
#ifndef PICO_MIDI
#define PICO_MIDI

#ifdef __cplusplus
#   define EXTERNC extern "C"
#else
#   define EXTERNC
#endif

//#define IGNORE_MIDI_CC


#define FLAG_VALUE 123

typedef enum { ARP_OFF, ARP_UP, ARP_DOWN, ARP_UPDWNA, ARP_UPDWNB, ARP_DWNUPA, ARP_DWNUPB, ARP_RANDOM }e_mode;

#ifdef __cplusplus
struct node {
	unsigned char Note;
	unsigned char Velo;
	bool active;
	struct node* next;
};

class cMidi {
public:
	cMidi() :	tx_channel(1),
				rx_channel(1),
				trigger_channel(2),
				head(NULL),
				current(NULL),
				arp_mode(ARP_OFF),
				arp_note(0),
				arpdir(false) {}
	void setTX(uint8_t channel);
	void setRX(uint8_t channel);
	void setTRIG(uint8_t channel);
	uint8_t getTX() { return tx_channel; }
	uint8_t getRX() { return rx_channel; }
	uint8_t getTRIG() { return trigger_channel; }
	void MidiHandler(uint8_t DataType, uint8_t Note, uint8_t Velo, uint8_t Channel);
	void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity);
	void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity);
	int ListLength();
	void ListSendNoteOn();
	void Arp();
	e_mode getArpMode() { return arp_mode; }
	void setArpMode(e_mode mode);
	bool getTrigState() { return trigger_state; }
	void setTrigState() { trigger_state = true; }
	void resetTrigState() { trigger_state = false; }
	void resetArp() { arpdir = false; }
protected:
	void ListInsert(unsigned char note, unsigned char velo);
	struct node* ListDeleteFirst();
	struct node* ListDeleteLast();
	bool isListEmpty();
	struct node* ListFindNote(unsigned char note);
	void ListDeleteNote(unsigned char Note);
	void ListSort();
	void ListSwap(struct node* a, struct node* b);
	void ListSendNoteOff();
	void ListNoteOn(uint8_t randNote);
private:
	uint8_t tx_channel;
	uint8_t rx_channel;
	uint8_t trigger_channel;
	struct node* head;
	struct node* current;
	e_mode arp_mode;
	int8_t arp_note;
	bool arpdir;
	bool trigger_state;
};

extern cMidi midi;
#endif
//void midi_core(void);
#ifdef __cplusplus
extern "C" {
#endif
	static size_t __time_critical_func(uart_read) (uart_inst_t* uart, uint8_t* dst, size_t maxlen);
	int init_midi(void);
	void midi_task(void);
	e_mode getArpMode();
	void setArpMode(e_mode mode);
	uint8_t getMidiRX();
	uint8_t getMidiTX();
	uint8_t getMidiTrig();
	void setMidiRX(uint8_t channel);
	void setMidiTX(uint8_t channel);
	void setMidiTrig(uint8_t channel);
#ifdef __cplusplus
}
#endif
#endif

