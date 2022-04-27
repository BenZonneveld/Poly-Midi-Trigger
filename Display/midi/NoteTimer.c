#include <avr/interrupt.h>

void ReadTimer()
{
  cli();
  TCCR3A = 0;
  TCCR3B = 0;
  
  TCNT3 = 0;
  OCR3A = 10000;
  TCCR3B = (1<<WGM32)| (1<<CS32);  // WGM32 => CTC(Clear Timer on Compare Match), CS32 => prescaler 1/256
  TIMSK3 = (1<<OCIE3A);             // OCIE3A => Timer3 compare match A interrupt
  sei();
}

void setTimer3(float _time) {
  long cnt = 16000000 / 256 * _time;  // cnt = clk / prescaler * time(s)
  if(cnt > 65535) {
    cnt = 65535;        // "timer3 16bit counter over."
  }
  OCR3A = cnt;          // Output Compare Register Timer3A
  TIMSK3 = bit(OCIE3A);
}

ISR(TIMER3_COMPA_vect)
{
  UpdateDisp = HIGH;
}

void TimerStart()
{
  TCCR1A = 0;
  TCCR1B = 0;
  
  uint16_t trig = (MINTRIG + triglength);
  float rl = rand_length/100.0f;
  trig = trig + random(-(rl * trig),rl *trig);
  
  TCNT1 = 0;            // preload timer 65536-16MHz/256/2Hz
  TCCR1B = (1<<WGM12) |(1 << CS12) | (1 << CS10);    // 1024 prescaler 
  OCR1A = 16*trig;
  TIMSK1 = (1 << OCIE1A);   // enable timer overflow interrupt
}

void TimerStop(void)
{
  bitWrite(TIMSK1, OCIE1A, 0);
  TCCR1A = TCCR1B = OCR1A = 0;
}

ISR(TIMER1_COMPA_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
  ListSendNoteOff();
}
