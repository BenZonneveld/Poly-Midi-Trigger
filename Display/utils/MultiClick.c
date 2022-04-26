//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include "hardware/gpio.h"
#include "MultiClick.h"
#include "vars.h"

// Button timing variables
//unsigned int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
//unsigned int DCgap = 250;            // max ms between clicks for a double click event
//unsigned int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
//unsigned int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

// Button variables
bool buttonVal = true;   // value read from button
bool buttonLast = true;  // buffered value of the button's previous state
bool DCwaiting = false;  // whether we're waiting for a double click (down)
bool DConUp = false;     // whether to register a double click on next release, or whether to wait and click
bool singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
bool ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
bool waitForUp = false;        // when held, whether to wait for the up event
bool holdEventPast = false;    // whether or not the hold event happened already
bool longHoldEventPast = false;// whether or not the long hold event happened already

int checkButton() {
    int event = BTNONE;
    buttonVal = digitalRead(MENUBUTTON);
    unsigned long Millis = millis();
    // Button pressed down
    if (buttonVal == LOW && buttonLast == HIGH && (Millis - upTime) > debounce)
    {
        downTime = Millis;
        ignoreUp = false;
        waitForUp = false;
        singleOK = true;
        holdEventPast = false;
        longHoldEventPast = false;
        if ((Millis - upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
        else  DConUp = false;
        DCwaiting = false;
    }
    // Button released
    else if (buttonVal == HIGH && buttonLast == LOW && (Millis - downTime) > debounce)
    {
        if (not ignoreUp)
        {
            upTime = Millis;
            if (DConUp == false) DCwaiting = true;
            else
            {
                event = BTDBLC;
                DConUp = false;
                DCwaiting = false;
                singleOK = false;
            }
        }
    }
    // Test for normal click event: DCgap expired
    if (buttonVal == HIGH && (Millis - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
    {
        event = BTCLICK;
        DCwaiting = false;
    }
    // Test for hold
    if (buttonVal == LOW && (Millis - downTime) >= holdTime) {
        // Trigger "normal" hold
        if (not holdEventPast)
        {
            event = BTHOLD;
            waitForUp = true;
            ignoreUp = true;
            DConUp = false;
            DCwaiting = false;
            //downTime = millis();
            holdEventPast = true;
        }
        // Trigger "long" hold
        if ((Millis - downTime) >= longHoldTime)
        {
            if (not longHoldEventPast)
            {
                event = BTLONG;
                longHoldEventPast = true;
            }
        }
    }
    buttonLast = buttonVal;
    return event;
}