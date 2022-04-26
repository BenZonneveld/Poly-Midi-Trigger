#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>

#include "vars.h"
#include "LinkedList.h"
#include "arp.h"

e_mode arp_mode = ARP_OFF;
//uint8_t arp_mode;
int8_t arp_note = 0;
bool arpdir = false;

void Arp()
{
    switch (arp_mode)
    {
    case ARP_UP:
        arp_note++;
        if (arp_note >= ListLength())
        {
            arp_note = 0;
        }
        break;
    case ARP_DOWN:
        arp_note--;
        if (arp_note < 0)
        {
            arp_note = ListLength() - 1;
        }
        break;
    case ARP_UPDWNA:
        if (arpdir)
        {
            arp_note++;
            if (arp_note >= ListLength())
            {
                arp_note = ListLength() - 2;
                arpdir = !arpdir;
            }
        }
        else {
            arp_note--;
            if (arp_note < 0)
            {
                arp_note = 1;
                arpdir = !arpdir;
            }
        }
        break;
    case ARP_UPDWNB:
        if (arpdir)
        {
            arp_note++;
            if (arp_note >= ListLength())
            {
                arp_note = ListLength() - 1;
                arpdir = !arpdir;
            }
        }
        else {
            arp_note--;
            if (arp_note < 0)
            {
                arp_note = 0;
                arpdir = !arpdir;
            }
        }
        break;
    case ARP_DWNUPA:
        if (!arpdir)
        {
            arp_note++;
            if (arp_note >= ListLength())
            {
                arp_note = ListLength() - 2;
                arpdir = !arpdir;
            }
        }
        else {
            arp_note--;
            if (arp_note < 0)
            {
                arp_note = 1;
                arpdir = !arpdir;
            }
        }
        break;
    case ARP_DWNUPB:
        if (!arpdir)
        {
            arp_note++;
            if (arp_note >= ListLength())
            {
                arp_note = ListLength() - 1;
                arpdir = !arpdir;
            }
        }
        else {
            arp_note--;
            if (arp_note < 0)
            {
                arp_note = 0;
                arpdir = !arpdir;
            }
        }
        break;
    case ARP_RANDOM:
        arp_note = rand() % ListLength();
        break;
    default:
        break;
    }
    ListNoteOn(arp_note);
}