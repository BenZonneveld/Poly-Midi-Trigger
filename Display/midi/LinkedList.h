struct node {
    unsigned char Note;
    unsigned char Velo;
    bool active;
    struct node* next;
};

//struct node* head = NULL;
//struct node* current = NULL;

void ListInsert(unsigned char note, unsigned char velo);
struct node* ListDeleteFirst();
struct node* ListDeleteLast();
bool isListEmpty();
int ListLength();
struct node* ListFindNote(unsigned char note);
void ListDeleteNote(unsigned char Note);
void ListSort();
void ListSwap(struct node* a, struct node* b);
void ListSendNoteOn();
void ListSendNoteOff();
void ListNoteOn(uint8_t randNote);