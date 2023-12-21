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
bool stateSwitch1 = false;
bool stateSwitch2 = false;

void isSwitch1TurnedOn();
void isSwitch2TurnedOn();
void isSwitch1TurnedOff();
void isSwitch2TurnedOff();

void setState();
void applyStateForAll();

void toggleSwitch1();
void toggleSwitch2();

void turnOnAll();
void turnOnSwitch1();
void turnOnSwitch2();

void turnOffAll();
void turnOffSwitch1();
void turnOffSwitch2();

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

    Serial.println("\n\n\nDevice: " + deviceName);

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

    Homey.addCapability("button.one", toggleSwitch1);
    Homey.addCapability("button.two", toggleSwitch2);

    Homey.addAction("turn_on_all", turnOnAll);
    Homey.addAction("turn_on_switch_one", turnOnSwitch1);
    Homey.addAction("turn_on_switch_two", turnOnSwitch2);
    Homey.addAction("turn_off_all", turnOffAll);
    Homey.addAction("turn_off_switch_one", turnOffSwitch1);
    Homey.addAction("turn_off_switch_two", turnOffSwitch2);

    Homey.addAction("toggle_switch_one", toggleSwitch1);
    Homey.addAction("toggle_switch_two", toggleSwitch2);

#ifndef LED_SHOWS_OUTPUT_STATE
    Homey.addAction("led", setLed);
#endif

    Homey.addCondition("is_switch1_turned_on", isSwitch1TurnedOn);
    Homey.addCondition("is_switch2_turned_on", isSwitch2TurnedOn);
    Homey.addCondition("is_switch1_turned_off", isSwitch1TurnedOff);
    Homey.addCondition("is_switch2_turned_off", isSwitch2TurnedOff);

    digitalWrite(PIN_LED, HIGH); //Turn led off
    applyStateForAll();
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

void applyStateForAll() {
    stateSwitch1 = state;
    stateSwitch2 = state;

    digitalWrite(PIN_RELAY_1, state);
    digitalWrite(PIN_RELAY_2, state);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyStateForAll(): new state is " + String(state));
    Homey.setCapabilityValue("onoff", state);

    if (state) {
        Homey.trigger("turned_on_switch1", stateSwitch1);
        Homey.trigger("turned_on_switch2", stateSwitch2);
    } else {
        Homey.trigger("turned_off_switch1", stateSwitch1);
        Homey.trigger("turned_off_switch2", stateSwitch2);
    }
}

void applyState1() {
    state = ((stateSwitch1 || stateSwitch2));
    digitalWrite(PIN_RELAY_1, stateSwitch1);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState1(): new stateSwitch1 is " + String(stateSwitch1));
    Homey.setCapabilityValue("onoff", state);
    if (stateSwitch1) {
        Homey.trigger("turned_on_switch1", stateSwitch1);
    } else {
        Homey.trigger("turned_off_switch1", stateSwitch1);
    }
}

void applyState2() {
    state = ((stateSwitch1 || stateSwitch2));
    digitalWrite(PIN_RELAY_2, stateSwitch2);
#ifdef LED_SHOWS_OUTPUT_STATE
    digitalWrite(PIN_LED, !state);
#endif
    Serial.println("applyState2(): new stateSwitch2 is " + String(stateSwitch2));
    Homey.setCapabilityValue("onoff", state);
    if (stateSwitch2) {
        Homey.trigger("turned_on_switch2", stateSwitch1);
    } else {
        Homey.trigger("turned_off_switch2", stateSwitch1);
    }
}

void setState() {
    state = Homey.value.toInt();
    applyStateForAll();
}

void toggleSwitch1() {
    stateSwitch1 = !stateSwitch1;
    applyState1();
}

void toggleSwitch2() {
    stateSwitch2 = !stateSwitch2;
    applyState2();
}

void turnOnAll() {
    state = true;
    applyStateForAll();
}

void turnOffAll() {
    state = false;
    applyStateForAll();
}

void turnOnSwitch1() {
    stateSwitch1 = true;
    applyState1();
}

void turnOnSwitch2() {
    stateSwitch2 = true;
    applyState2();
}

void turnOffSwitch1() {
    stateSwitch1 = false;
    applyState1();
}

void turnOffSwitch2() {
    stateSwitch2 = false;
    applyState2();
}

void isSwitch1TurnedOn() {
    Serial.println("isSwitch1TurnedOn(): " + String(stateSwitch1));
    return Homey.returnResult(stateSwitch1);
}

void isSwitch2TurnedOn() {
    Serial.println("isSwitch2TurnedOn(): " + String(stateSwitch2));
    return Homey.returnResult(stateSwitch2);
}

void isSwitch1TurnedOff() {
    Serial.println("isSwitch1TurnedOff(): " + String(!stateSwitch1));
    return Homey.returnResult(!stateSwitch1);
}

void isSwitch2TurnedOff() {
    Serial.println("isSwitch2TurnedOff(): " + String(!stateSwitch2));
    return Homey.returnResult(!stateSwitch2);
}


void buttonChanged(Button *btn, bool released) {
    Serial.print("button #");
    Serial.print(btn->id);
    Serial.print(" ");
    Serial.println((released) ? "RELEASED" : "PRESSED");

    if (!released) {
        switch (btn->id) {
            case 1:
                toggleSwitch1();
                break;
            case 2:
                toggleSwitch2();
                break;
        }
    }
}