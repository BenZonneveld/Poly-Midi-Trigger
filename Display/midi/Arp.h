//uint8_t arp_mode;
//int8_t arp_note = 0;
//bool arpdir = false;
typedef enum { ARP_OFF, ARP_UP, ARP_DOWN, ARP_UPDWNA, ARP_UPDWNB, ARP_DWNUPA, ARP_DWNUPB, ARP_RANDOM }e_mode;
extern e_mode arp_mode;

void Arp();