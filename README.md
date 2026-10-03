# Harry Potter ESP32 API Client

An ESP32 Arduino sketch that receives JSON commands over Bluetooth Serial, configures Wi-Fi, and returns Harry Potter character data from the project API over Bluetooth.

## Requirements

- An ESP32 board with Bluetooth Classic SPP support (for example, ESP32-WROOM)
- Arduino IDE or PlatformIO with the ESP32 Arduino core
- The `ArduinoJson` library
- A Bluetooth Serial client on a phone or computer

The sketch uses `BluetoothSerial`, `WiFi`, and `HTTPClient` from the ESP32 Arduino core. Bluetooth Classic SPP is not supported by every ESP32 variant.

## Build and upload

1. Install the ESP32 board support package and the ArduinoJson library.
2. Open `main.cpp` in an ESP32 Arduino project (or copy it into the project's `.ino` file).
3. Select your ESP32 board and serial port, then build and upload.
4. Pair with the Bluetooth device named `ESP32_HARRY_POTTER`.

The sketch uses the API base URL `http://proiectia.bogdanflorea.ro/api/harry-potter`. Wi-Fi credentials are sent at runtime over Bluetooth and are not stored in the source file.

## Bluetooth command protocol

Send one JSON object per line. Responses are JSON lines; network scans and character lists can produce multiple lines.

| Action | Command | Response |
| --- | --- | --- |
| Scan nearby Wi-Fi networks | `{"action":"getNetworks"}` | One object per network, with `ssid`, `strength`, `encryption`, and `teamId` |
| Configure and connect to Wi-Fi | `{"action":"connect","ssid":"YOUR_SSID","password":"YOUR_PASSWORD"}` | Connection result with `connected`, `ssid`, and `teamId` |
| Get characters | `{"action":"getData"}` | One object per character with `id`, `name`, `image`, and `teamId` |
| Get a character's details | `{"action":"getDetails","id":"CHARACTER_ID"}` | Character fields and a formatted `description` |

If Wi-Fi is disconnected when character data is requested, the sketch responds with an error of `NoWiFi`. Invalid JSON and unknown actions are reported over the ESP32 serial console or as an error response.

## Configuration

`TEAM_ID`, `BTC_NAME`, serial speed, Bluetooth timeout, Wi-Fi timeout, and the API endpoint are defined near the top of `main.cpp`. The current team ID is `A08`.

## Notes

- Wi-Fi credentials are sent over Bluetooth in the `connect` command. Use this only with a trusted, paired client.
- API requests currently use plain HTTP.
- This repository contains source code only; board configuration and dependency installation depend on your Arduino environment.
