# Lab 3: Smart Fan Control System (ESP32 + Home Assistant)

An IoT-based smart fan control system built using an ESP32 and the ESP-IDF framework. It supports OFF, LOW, and HIGH modes through physical push buttons, a Home Assistant dashboard, and voice commands. The system uses dual-channel relays, LED indicators, Wi-Fi, and MQTT for wireless control and automation.

Integrates Home Assistant OS, Mosquitto MQTT, and Home Assistant Assist to demonstrate smart device control and voice-assisted home automation.

![framework](https://img.shields.io/badge/framework-ESP--IDF-red) ![platform](https://img.shields.io/badge/platform-ESP32-blue) ![automation](https://img.shields.io/badge/automation-Home%20Assistant-41BDF5) ![protocol](https://img.shields.io/badge/protocol-MQTT-660066) ![control](https://img.shields.io/badge/control-Voice%20%26%20Physical-orange) ![hardware](https://img.shields.io/badge/hardware-Dual--Channel%20Relay-green)

---

## Features

- Three operating modes: OFF, LOW, and HIGH.
- Physical push-button control.
- Remote control through Home Assistant.
- Voice commands through Home Assistant Assist.
- MQTT-based wireless communication.
- LED indicators for LOW and HIGH modes.
- Break-before-make relay switching.

## Hardware Components

- ESP32 Development Board
- Dual-Channel Relay Module
- Three Push Buttons
- Green and Blue LEDs
- 220Ω Resistors
- Breadboard and Jumper Wires

## GPIO Pin Configuration

| Component | GPIO |
|---|---:|
| OFF Button | 19 |
| LOW Button | 18 |
| HIGH Button | 5 |
| LOW Relay | 32 |
| HIGH Relay | 33 |
| Green LED (LOW) | 25 |
| Blue LED (HIGH) | 26 |

**Relay Logic:** Active-low (0 = ON, 1 = OFF).

## Technologies Used

- ESP32 and ESP-IDF 4.4.1
- C Programming Language
- Visual Studio Code and PlatformIO
- Home Assistant OS 18.4
- Oracle VirtualBox
- Mosquitto MQTT Broker
- Home Assistant Assist
- Speech-to-Phrase, Piper, and openWakeWord

## System Communication

```text
Physical Buttons ──┐
Home Assistant ────┼── MQTT ── ESP32
Voice Commands ────┘             |
                                 |
                         ┌───────┴───────┐
                         |               |
                       Relays        LED Indicators
```

## MQTT Configuration

| Topic | Purpose |
|---|---|
| `smartfan/set` | Receives fan commands |
| `smartfan/state` | Publishes fan state |

Supported MQTT payloads: `OFF`, `LOW`, and `HIGH`.

Configure the broker address and credentials in `app_mqtt.c`. Set the Wi-Fi credentials in the appropriate Wi-Fi configuration file. Keep passwords and private credentials out of public repositories.

## Installation and Setup

Clone the repository:

```bash
git clone YOUR_REPOSITORY_URL
cd smart_fan
```

Open the project in Visual Studio Code with PlatformIO installed.

Configure your Wi-Fi and MQTT broker settings, then connect the ESP32 through USB.

Build and upload the firmware:

```bash
pio run
pio run --target upload
```

Monitor the ESP32:

```bash
pio device monitor
```

Install Mosquitto MQTT in Home Assistant and configure the `input_select.fan_control` helper with OFF, LOW, and HIGH options. Connect the Home Assistant automations to the MQTT command topic.

## Authors

- Kent Jesser Neri
- Alwyn Reuyan
- JC Redelosa

**Section:** BSCA-B186  
**Date:** November 15, 2026

---

*Academic Laboratory Project — IoT and Embedded Systems*
