/*
 * Homeyduino firmware for Sonoff TX
 *
 * WARNING:
 * Never try to program a Sonoff which is connected to the mains! Failure to
 * disconnect mains before connecting the usb to serial converter will cause
 * damage to you and your equipment.
 *
 * Always power the Sonoff using an external 3.3v source while programming.
 *
 */

// -- CONFIGURATION --------------------------------------------------------------

/* Comment out (//) lines to disable the feature */

#define BUTTON_SWITCHES_OUTPUT //Have the button1 toggle the relay directly
//(if disabled the button1 will only send a trigger)

#define LED_SHOWS_OUTPUT_STATE //Have the led show the state of the relay
//(if disabled led can be controlled using an action)

//--------------------------------------------------------------------------------

#include <Arduino.h>
#include <Homey.h>
#include "wifi_config.h"
#include "Button.h"

//GPIO map
#define PIN_BUTTON_1   0
#define PIN_BUTTON_2   9
#define PIN_LED       13
#define PIN_RELAY_1   12
#define PIN_RELAY_2    5

bool state = false;
bool state1 = false;
bool state2 = false;
bool previousButtonState1 = false;
bool previousButtonState2 = false;
unsigned long previousMillis = 0;
const unsigned long interval = 100; //Interval in milliseconds

void getState();
void getState1();
void getState2();

void setState();
void toggleSocket1();
void toggleSocket2();

void buttonChanged(Button *btn, bool released);

Button button1 = Button(PIN_BUTTON_1, 1, &buttonChanged);
Button button2 = Button(PIN_BUTTON_2, 2, &buttonChanged);


void setLed();

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_RELAY_1, OUTPUT);
    pinMode(PIN_RELAY_2, OUTPUT);

    digitalWrite(PIN_LED, LOW); //Turn led on
    digitalWrite(PIN_RELAY_1, LOW); //Turn output off
    digitalWrite(PIN_RELAY_2, LOW); //Turn output off

    String deviceName = "sonoff-" + String(EspClass::getChipId()); // Generate device name based on ID

    Serial.println("\nDevice: " + deviceName);

    button1.begin();
    button2.begin();

    //Connect to network
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    //Print IP address
    Serial.print("Connected. IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Connected!");

    Homey.begin(deviceName, "sonoff"); //Start Homeyduino
    Homey.setClass("socket");

    Homey.addCapability("onoff", setState);

    Homey.addCapability("button.one", toggleSocket1);
    Homey.addCapability("button.two", toggleSocket2);
    Homey.addAction("output", setState);
#ifndef LED_SHOWS_OUTPUT_STATE
    Homey.addAction("led", setLed);
#endif
    Homey.addCondition("state", getState);
    Homey.addCondition("socket1", getState1);
    Homey.addCondition("socket2", getState2);

    digitalWrite(PIN_LED, HIGH); //Turn led off

}

void loop() {
    Homey.loop();
    button1.loop();
    button2.loop();
}

void setLed() {
    bool led = Homey.value.toInt();
    digitalWrite(PIN_LED, !led); //Write to led, output is inverted
}

void applyState() {
    state1 = state;
    state2 = state;

    digitalWrite(PIN_RELAY_1, state);
    digitalWrite(PIN_RELAY_2, state);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState(): new state is " + String(state));
    Homey.setCapabilityValue("onoff", state);
    Homey.trigger("state", state);
    Homey.trigger("socket1", state1);
    Homey.trigger("socket2", state2);
}

void applyState1() {
    state = ((state1 || state2));
    digitalWrite(PIN_RELAY_1, state1);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState(): new state1 is " + String(state1));
    Homey.setCapabilityValue("onoff", state);
    Homey.trigger("state", state);
    Homey.trigger("socket1", state1);
}

void applyState2() {
    state = ((state1 || state2));
    digitalWrite(PIN_RELAY_2, state2);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState(): new state2 is " + String(state2));
    Homey.setCapabilityValue("onoff", state);
    Homey.trigger("state", state);
    Homey.trigger("socket1", state2);
}

void setState() {
    state = Homey.value.toInt();
    applyState();
}

void toggleSocket1() {
    state1 = !state1;
    applyState1();
}

void toggleSocket2() {
    state2 = !state2;
    applyState2();
}

void getState() {
    Serial.println("getState(): state is " + String(state));
    return Homey.returnResult(state);
}

void getState1() {
    Serial.println("getState1(): state1 is " + String(state1));
    return Homey.returnResult(state1);
}

void getState2() {
    Serial.println("getState2(): state2 is " + String(state2));
    return Homey.returnResult(state2);
}


void buttonChanged(Button *btn, bool released) {
    Serial.print("button #");
    Serial.print(btn->id);
    Serial.print(" ");
    Serial.println((released) ? "RELEASED" : "PRESSED");

    if (!released) {
        switch (btn->id) {
            case 1:
                toggleSocket1();
                break;
            case 2:
                toggleSocket2();
                break;
        }
    }
}