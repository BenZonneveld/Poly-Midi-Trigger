
void ReadTimer();
void setTimer3(float _time);
ISR(TIMER3_COMPA_vect);

void TimerStart();
void TimerStop(void);

ISR(TIMER1_COMPA_vect);