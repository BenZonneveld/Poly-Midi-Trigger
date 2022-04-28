#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <pico.h>

#include "defines.h"
#include "midi.h"

//LinkedList LList;
//static struct node* head = NULL;
//static struct node* current = NULL;

//display the list
//void ListPrint() {
//   struct node *ptr = head;
//   Serial.print("\n[ ");
//  
//   //start from the beginning
//   while(ptr != NULL) {
//      Serial.print("( Act: ");
//      Serial.print(ptr->active);
//      Serial.print(", Note: ");
//      Serial.print(ptr->Note);
//      Serial.print(", Velo: ");
//      Serial.print(ptr->Velo);
//      Serial.println(" ) ");
//      ptr = ptr->next;
//   }
//  
//   Serial.println(" ]");
//}

//insert link at the first location
void cMidi::ListInsert(unsigned char note, unsigned char velo) {
    // Check if the note is already in the list
    if (ListFindNote(note) != NULL) return;

    // Drop oldest note
    if (ListLength() >= MAXNOTES)
    {
        // Should probably add a note off before this 
        ListDeleteLast();
    }

    //create a link
    struct node* link = (struct node*)malloc(sizeof(struct node));
    if (link == NULL)
    {
        return;
    }
    //  struct node *link = new node;

    link->Note = note;
    link->Velo = velo;
    link->active = false;
    //point it to old first node
    link->next = head;

    //point first to new first node
    head = link;
    ListSort();
}

//delete first item
struct node* cMidi::ListDeleteFirst() {
    //save reference to first link
    struct node* tempLink = head;

    if (head->active) midi.noteOff(tx_channel, head->Note, 0);

    //mark next to first link as first 
    head = head->next;

    //return the deleted link
    return tempLink;
}

//delete last item
struct node* cMidi::ListDeleteLast() {
    if (head == NULL) return NULL;

    if (head->next == NULL)
    {
        if (head->active) midi.noteOff(tx_channel, head->Note, 0);
        free(head);
        return NULL;
    }

    // Find the second last node
    struct node* second_last = head;
    while (second_last->next->next != NULL)
        second_last = second_last->next;


    // Delete last node
    if (second_last->next->active) midi.noteOff(tx_channel, head->Note, 0);
    free(second_last->next);

    // Change next of second last
    second_last->next = NULL;

    return head;
}

//is list empty
bool cMidi::isListEmpty() {
    return head == NULL;
}

int cMidi::ListLength() {
    int length = 0;
    struct node* current;

    for (current = head; current != NULL; current = current->next) {
        length++;
    }

    return length;
}

//find a link with given key
struct node* cMidi::ListFindNote(unsigned char note) {
    //start from the first link
    struct node* current = head;

    //if list is empty
    if (head == NULL) {
        return NULL;
    }

    //navigate through list
    while (current->Note != note) {

        //if it is last node
        if (current->next == NULL) {
            return NULL;
        }
        else {
            //go to next link
            current = current->next;
        }
    }

    //if data found, return the current Link
    return current;
}

//delete a link with given Note
void cMidi::ListDeleteNote(unsigned char Note)
{
    struct node* temp = head;
    struct node* prev = NULL;

    if (temp != NULL && temp->Note == Note)
    {
        if (temp->active)
        {
            midi.noteOff(tx_channel, temp->Note, 0);
        }

        head = temp->next;
        free(temp);
    }
    else
    {
        while (temp != NULL && temp->Note != Note)
        {
            prev = temp;
            temp = temp->next;
        }

        if (temp == NULL)
        {
            return;
        }

        if (temp->active)
        {
            midi.noteOff(tx_channel, temp->Note, 0);
        }

        prev->next = temp->next;
        free(temp);
    }
}

// Do a bubble sort on the list
void cMidi::ListSort()
{
    //    struct node *start = head;

    int swapped;
    struct node* ptr1;
    struct node* lptr = NULL;

    /* Checking for empty list */
    if (head == NULL)
        return;

    do
    {
        swapped = 0;
        ptr1 = head;

        while (ptr1->next != lptr)
        {
            if (ptr1->Note > ptr1->next->Note)
            {
                ListSwap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}

/* function to swap data of two nodes a and b*/
void cMidi::ListSwap(struct node* a, struct node* b)
{
    int tempNote = a->Note;
    int tempVelo = a->Velo;
    bool tempAct = a->active;

    a->Note = b->Note;
    a->Velo = b->Velo;
    a->active = b->active;
    b->Note = tempNote;
    b->Velo = tempVelo;
    b->active = tempAct;
}

void cMidi::ListSendNoteOn()
{
    struct node* ptr = head;

    //start from the beginning
    while (ptr != NULL) {
        if (ptr->active)
        {
            midi.noteOff(tx_channel, ptr->Note, 0);
        }
        midi.noteOn(tx_channel, ptr->Note, ptr->Velo);
        ptr->active = true;
        ptr = ptr->next;
    }
//TODO
//    TimerStart();
}

void cMidi::ListSendNoteOff()
{
    struct node* ptr = head;

    //start from the beginning
    while (ptr != NULL) {
        midi.noteOff(tx_channel, ptr->Note, 0);
        ptr->active = false;
        ptr = ptr->next;
    }
//TODO
//    TimerStop();
}

void cMidi::ListNoteOn(uint8_t randNote)
{
    struct node* ptr = head;
    uint8_t i = 0;

    //start from the beginning
    while (ptr != NULL) {
        if (i == randNote)
        {
            if (ptr->active)
            {
                midi.noteOff(tx_channel, ptr->Note, 0);
            }
            midi.noteOn(tx_channel, ptr->Note, ptr->Velo);
            ptr->active = true;
//TODO
//            TimerStart();
            break;
        }
        i++;
        ptr = ptr->next;
    }
}
