# Homeyduino firmware for Sonoff TX

This is a work in progress

## WiFi Config

To compile this project, create a file named `wifi_config.h` in the `src` folder with this content:

```c
#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"

#endif
```

