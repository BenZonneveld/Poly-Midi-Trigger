//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

typedef enum { BTNONE, BTCLICK, BTDBLC, BTHOLD, BTLONG } BTNEVNT;
extern BTNEVNT event;

int checkButton();