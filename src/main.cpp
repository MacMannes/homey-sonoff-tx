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

//GPIO map
#define PIN_BUTTON_1   0
#define PIN_BUTTON_2   9
#define PIN_LED       13
#define PIN_RELAY_1   12
#define PIN_RELAY_2    5

bool state = false;
bool previousButtonState1 = false;
bool previousButtonState2 = false;
unsigned long previousMillis = 0;
const unsigned long interval = 100; //Interval in milliseconds

void button1();

void button2();

void getState();

void setState();
void setButton1State();
void setButton2State();


void setLed();

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BUTTON_1, INPUT);
    pinMode(PIN_BUTTON_2, INPUT);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_RELAY_1, OUTPUT);
    pinMode(PIN_RELAY_2, OUTPUT);

    digitalWrite(PIN_LED, LOW); //Turn led on
    digitalWrite(PIN_RELAY_1, LOW); //Turn output off
    digitalWrite(PIN_RELAY_2, LOW); //Turn output off


    String deviceName = "sonoff-" + String(EspClass::getChipId()); // Generate device name based on ID

    Serial.println("Device: " + deviceName);

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

    Homey.addCapability("button.1", setState);
    Homey.addCapability("button.2", setButton2State);
    Homey.addAction("output", setState);
#ifndef LED_SHOWS_OUTPUT_STATE
    Homey.addAction("led", setLed);
#endif
    Homey.addCondition("state", getState);

    digitalWrite(PIN_LED, HIGH); //Turn led off

}

void loop() {
    Homey.loop();
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > interval) {
        previousMillis = currentMillis;
        button1();
        button2();
    }
}

void setLed() {
    bool led = Homey.value.toInt();
    digitalWrite(PIN_LED, !led); //Write to led, output is inverted
}

void applyState() {
    digitalWrite(PIN_RELAY_1, state);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState(): new state is " + String(state));
    Homey.setCapabilityValue("onoff", state);
    Homey.trigger("state", state);
}

void setState() {
    state = Homey.value.toInt();
    applyState();
}

void setButton1State() {

}

void setButton2State() {

}

void getState() {
    Serial.println("getState(): state is " + String(state));
    return Homey.returnResult(state);
}

void button1() {
    bool currentButtonState = !digitalRead(PIN_BUTTON_1);
    if (currentButtonState != previousButtonState1) {
        currentButtonState = previousButtonState1;
        Serial.println("button(): button1 is " + String(currentButtonState));
        Homey.trigger("button1", currentButtonState);
#ifdef BUTTON_SWITCHES_OUTPUT
        Serial.println("button1(): toggle output");
        if (currentButtonState) {
            state = !state;
            applyState();
        }
#endif
    }
}

void button2() {
    bool currentButtonState = !digitalRead(PIN_BUTTON_2);
    if (currentButtonState != previousButtonState2) {
        currentButtonState = previousButtonState2;
        Serial.println("button(): button2 is " + String(currentButtonState));
    }
}