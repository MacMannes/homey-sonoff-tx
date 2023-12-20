//
// Created by André Mathlener on 24/03/2021.
//

#include "Button.h"

/*
 * Constructor
 */
Button::Button(uint8_t buttonPin, int id, onActionFunction actionFunc) {
    this->buttonPin = buttonPin;
    this->id = id;
    this->actionFunc = actionFunc;
    currentState = HIGH;
    lastButtonState = HIGH;
}

void Button::begin() {
    pinMode(buttonPin, INPUT);

    currentState = digitalRead(buttonPin);
}

void Button::loop() {
    uint8_t pinState = digitalRead(buttonPin);
    process(pinState);
}

void Button::process(int pinState) {
    if (pinState != lastButtonState) {
        // If the switch changed, due to noise or pressing:
        lastDebounceTime = millis();
    }

    unsigned long time = millis() - lastDebounceTime;

    if (time > debounceDelay) {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state

        if (pinState != currentState) {
            // The button state has been changed:
            currentState = pinState;
            bool released = pinState == HIGH;

            // Call action function if registered
            if (actionFunc) {
                actionFunc(this, released);
            }
        }

    }

    lastButtonState = pinState;
}




