struct s_midi_data
{
	char unsigned command;
	char unsigned data1;
	char unsigned data2;
	char unsigned cn_cin; // Cable number and Code Index Number
	/*
	* CIN MIDI_x Size Description
	0x0		1, 2 or 3	Miscellaneous function codes. Reserved for future extensions.
	0x1		1, 2 or 3	Cable events. Reserved for future expansion.
	0x2		2			Two-byte System Common messages like MTC, SongSelect, etc.
	0x3		3			Three-byte System Common messages like SPP, etc.
	0x4		3			SysEx starts or continues
	0x5		1			Single-byte System Common Message or SysEx ends with following single byte.
	0x6		2			SysEx ends with following two bytes.
	0x7		3			SysEx ends with following three bytes.
	0x8		3			Note-off
	0x9		3			Note-on
	0xA		3			Poly-KeyPress
	0xB		3			Control Change
	0xC		2 			Program Change
	0xD		2 			Channel Pressure
	0xE		3 			PitchBend Change
	0xF		1 			Single Byte

	*/
};