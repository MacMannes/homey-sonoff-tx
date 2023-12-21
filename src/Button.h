#ifndef BUTTON_H
#define BUTTON_H


#include <Arduino.h>

class Button {
public:
    // Function pointer definition
    typedef void (*onActionFunction)(Button *button, bool released);

    int id = 0;                             // ID for identification

    Button(uint8_t buttonPin, int id, onActionFunction actionFunc);

    virtual void begin();

    void loop();

    void process(int pinState);



protected:
    uint8_t buttonPin = 0;
    onActionFunction actionFunc = nullptr; // Function pointer, will be called when there is an action happening

    int currentState;
    int lastButtonState;

    unsigned long lastDebounceTime = 0;     // The last time the output pin was toggled
    unsigned long debounceDelay = 10;      // The debounce time
};



#endif //BUTTON_H
