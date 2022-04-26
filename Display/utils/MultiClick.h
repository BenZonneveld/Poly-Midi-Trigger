//=================================================
//  MULTI-CLICK:  One Button, Multiple Events

// Button timing variables
unsigned int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
unsigned int DCgap = 250;            // max ms between clicks for a double click event
unsigned int holdTime = 1000;        // ms hold period: how long to wait for press+hold event
unsigned int longHoldTime = 3000;    // ms long hold period: how long to wait for press+hold event

int checkButton();