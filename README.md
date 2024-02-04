# esp-alerts

ESP32 based HTTP alerts system.

The [alerts.ino](alerts.ino) file is the main source code for the project. It includes functionality for connecting to a Wi-Fi network, obtaining local and public IP addresses, setting up a web server, handling incoming requests, playing audio files from an SD card, and more.

This project uses several libraries, including the `Audio.h` library for playing audio.

## Schematics (sort of)

Please check [schematics.png](resources/schematics.png) file

```
Module used: ESP32-Wroom 32, 80MHz/240MHz, 512kb, 4MB

Sketch uses 1191793 bytes (90%) of program storage space. Maximum is 1310720 bytes.
Global variables use 58544 bytes (17%) of dynamic memory, leaving 269136 bytes for local variables. Maximum is 327680 bytes.
```

## Libraries Used

- [Audio.h](https://github.com/earlephilhower/ESP8266Audio)
- Arduino.h
- WiFi.h
- HTTPClient.h
- WebServer.h
- Arduino_JSON.h
- SD.h
- FS.h
- NTPClient.h

## Audio Files

You don't need to use an SD card module to play audio files. You can also stream them from a host.

## Request Payload

You can modify the request payload to suit your needs. Here's an example of a payload:

```json
{
	"back": true,
	"product": "platform",
	"version": "1.4.30"
}
```

## Customization

This project is highly customizable. For instance, you can remove the audio output module and repurpose the project to trigger other actions, such as turning LEDs on/off. You could even set it up to taze the developer who deploys on Fridays (just kidding, don't do this).
