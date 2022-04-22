extern uint8_t tx_channel, rx_channel, trigger_channel;
extern bool state;

void MidiHandler(uint8_t DataType, uint8_t Note, uint8_t Velo, uint8_t Channel);
void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity);
void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity);