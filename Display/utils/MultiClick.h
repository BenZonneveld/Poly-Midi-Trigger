//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

typedef enum { BTNONE, BTCLICK, BTDBLC, BTHOLD, BTLONG } BTNEVNT;
extern BTNEVNT event;
#ifdef __cplusplus
extern "C"
{
#endif
	int checkButton();
#ifdef __cplusplus
}
#endif