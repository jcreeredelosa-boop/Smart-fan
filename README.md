# Lab 3: Smart Fan Control System (ESP32 + Home Assistant)

An IoT-based smart fan control system built using a standard ESP32 and the ESP-IDF framework. It supports three operating modes (OFF, LOW, and HIGH), controlled through physical push buttons, a Home Assistant dashboard, and voice commands. The system uses a dual-channel relay module, LED indicators, Wi-Fi connectivity, and MQTT communication for remote control and automation.

Integrates Home Assistant OS running on VirtualBox with the Mosquitto MQTT broker and Home Assistant Assist to demonstrate wireless device control and voice-assisted home automation.

![framework](https://img.shields.io/badge/framework-ESP--IDF-red) ![platform](https://img.shields.io/badge/platform-ESP32-blue) ![automation](https://img.shields.io/badge/automation-Home%20Assistant-41BDF5) ![protocol](https://img.shields.io/badge/protocol-MQTT-660066) ![control](https://img.shields.io/badge/control-Voice%20%26%20Physical-orange) ![hardware](https://img.shields.io/badge/hardware-Dual--Channel%20Relay-green)

## 1. Project Overview

The **ESP32 Smart Fan Control System** is an Internet of Things (IoT) prototype designed to demonstrate intelligent fan control through physical buttons, wireless communication, and voice commands.

The system uses an ESP32 microcontroller as its primary control unit, with Home Assistant serving as the home automation platform and Mosquitto MQTT providing communication between the microcontroller and the automation system.

Users can select between three operating states: OFF, LOW, and HIGH. These states can be controlled through physical push buttons or remotely through the Home Assistant interface. Voice interaction is also supported through Home Assistant Assist and a browser-based Voice Satellite.

The project combines embedded systems programming, wireless networking, MQTT communication, relay-based switching, and voice-assisted automation into a single prototype.

## 2. Project Objectives

The project aims to:

- Develop an ESP32-based smart control system.
- Implement multiple fan operating states: OFF, LOW, and HIGH.
- Enable physical control through push buttons.
- Integrate MQTT for wireless communication.
- Connect the ESP32 to Home Assistant for centralized control.
- Implement voice-assisted control using Home Assistant Assist.
- Provide visual operating-state indicators using LEDs.
- Demonstrate communication between embedded hardware and a home automation platform.

## 3. System Architecture

The system consists of three primary layers:

**1. Input Layer**
- Physical push buttons.
- Home Assistant dashboard controls.
- Voice commands through the Voice Satellite.

**2. Processing and Communication Layer**
- ESP32 microcontroller.
- ESP-IDF firmware.
- Wi-Fi connectivity.
- Mosquitto MQTT broker.

**3. Output Layer**
- Dual-channel relay module.
- Green and blue indicator LEDs.
- Fan operating-state feedback through Home Assistant.

### System Communication Flow

```text
       PHYSICAL BUTTONS
              |
              v
        +-----------+
        |   ESP32   |
        | Controller|
        +-----------+
              |
       +------+------+
       |             |
       v             v
    RELAYS       LED INDICATORS
       |
       v
   FAN CONTROL

       HOME ASSISTANT
       /           \
      v             v
 DASHBOARD      VOICE SATELLITE
      \             /
       \           /
        v         v
       MOSQUITTO MQTT
              |
              v
            ESP32
```

The ESP32 processes physical button inputs and MQTT commands. It then updates the relay outputs and indicator LEDs according to the selected operating state.

## 4. Hardware Components

| Component | Purpose |
|---|---|
| ESP32 Development Board | Main microcontroller |
| Dual-Channel Relay Module | Switching control outputs |
| Push Buttons (3) | Manual operating-state selection |
| Green LED | LOW-state indicator |
| Blue LED | HIGH-state indicator |
| 220Ω Resistors | LED current limiting |
| Breadboard | Circuit prototyping |
| Jumper Wires | Electrical connections |
| USB Cable | Programming and power for ESP32 |

**Note:** The current prototype focuses on demonstrating the control system and relay operation. A physical fan is not required for the present control-system testing configuration.

## 5. GPIO Pin Configuration

The following table shows the GPIO assignments used in the ESP32 firmware.

| Component | ESP32 GPIO | Function |
|---|---:|---|
| OFF Button | GPIO 19 | Select OFF |
| LOW Button | GPIO 18 | Select LOW |
| HIGH Button | GPIO 5 | Select HIGH |
| LOW Relay | GPIO 32 | LOW control output |
| HIGH Relay | GPIO 33 | HIGH control output |
| Green LED | GPIO 25 | LOW indicator |
| Blue LED | GPIO 26 | HIGH indicator |

### Button Configuration

The three push buttons use the ESP32's internal pull-up resistors.

- Buttons are connected between their assigned GPIO pins and GND.
- Button inputs use negative-edge interrupts.
- A debounce interval is implemented to prevent repeated triggering from a single press.

### Relay Configuration

The dual-channel relay module uses active-low logic:

| GPIO Output | Relay State |
|---|---|
| HIGH (1) | OFF |
| LOW (0) | ON |

The firmware implements a break-before-make switching sequence. Both relays are deactivated before the selected relay is activated, with a 250 ms delay between switching operations.

This helps prevent both relay channels from being activated simultaneously.

### LED Configuration

- GPIO 25 controls the green LED, indicating LOW mode.
- GPIO 26 controls the blue LED, indicating HIGH mode.
- Both LEDs turn OFF when the system is in OFF mode.

Each LED uses a 220Ω current-limiting resistor.

## 6. Operating Modes

The system supports three operating states.

| Mode | Relay 1 (GPIO 32) | Relay 2 (GPIO 33) | Indicator |
|---|---|---|---|
| OFF | OFF | OFF | All LEDs OFF |
| LOW | ON | OFF | Green LED |
| HIGH | OFF | ON | Blue LED |

The firmware uses the following state values:

| State | Value |
|---|---:|
| OFF | 0 |
| LOW | 33 |
| HIGH | 100 |

These values are also used for communicating the selected operating state through the firmware's callback mechanism.

## 7. Software and Technologies

The project uses the following software and technologies:

| Technology | Application |
|---|---|
| Visual Studio Code | Development environment |
| PlatformIO | Project building and firmware uploading |
| ESP-IDF 4.4.1 | ESP32 development framework |
| C Programming Language | Embedded firmware |
| Home Assistant OS 18.4 | Home automation platform |
| VirtualBox | Home Assistant virtualization |
| Mosquitto MQTT | Message broker |
| MQTT | Wireless device communication |
| Home Assistant Assist | Voice interaction |
| Speech-to-Phrase | Speech recognition |
| Piper | Text-to-speech |
| openWakeWord | Wake-word detection |

## 8. MQTT Communication

MQTT is used as the communication protocol between Home Assistant and the ESP32.

The Mosquitto MQTT broker handles messages sent by Home Assistant and received by the ESP32.

### MQTT Topics

| Topic | Purpose |
|---|---|
| `smartfan/set` | Sends the desired operating state to the ESP32 |
| `smartfan/state` | Publishes the current operating state |

### Command Payloads

The system uses the following MQTT payloads:

| Payload | Description |
|---|---|
| `OFF` | Deactivate both relay channels |
| `LOW` | Activate the LOW relay |
| `HIGH` | Activate the HIGH relay |

### Example MQTT Communication

To activate LOW mode, Home Assistant publishes:

```text
Topic: smartfan/set
Payload: LOW
```

To activate HIGH mode:

```text
Topic: smartfan/set
Payload: HIGH
```

To deactivate the system:

```text
Topic: smartfan/set
Payload: OFF
```

The ESP32 receives the message, interprets the requested state, and updates its outputs.

### MQTT Broker Configuration

The MQTT broker address is configured in `app_mqtt.c`.

Example:

```c
#define MQTT_BROKER_URI "mqtt://YOUR_BROKER_IP:1883"
#define MQTT_USERNAME "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"

#define MQTT_TOPIC_SET "smartfan/set"
#define MQTT_TOPIC_STATE "smartfan/state"
```

Replace the example credentials and broker address with your own configuration. Never publish actual passwords or private credentials in a public repository.

## 9. Home Assistant Integration

Home Assistant serves as the central automation platform.

It runs as a virtual machine through Oracle VirtualBox using Home Assistant OS.

### Main Configuration

The project uses an `input_select` helper:

```yaml
input_select:
  fan_control:
    name: Fan Control
    options:
      - OFF
      - LOW
      - HIGH
```

This helper allows users to select the desired operating state through the Home Assistant dashboard.

An automation publishes the selected value to the MQTT command topic:

```yaml
action:
  - service: mqtt.publish
    data:
      topic: smartfan/set
      payload: "{{ states('input_select.fan_control') }}"
```

The ESP32 subscribes to the command topic and processes the received state.

## 10. Voice Control Integration

The project incorporates Home Assistant Assist to enable voice-based operation.

The browser-based Voice Satellite is used as the voice interaction interface.

### Voice Components

| Component | Function |
|---|---|
| Voice Satellite | Browser-based voice interface |
| Home Assistant Assist | Processes voice commands |
| Speech-to-Phrase | Converts speech into text |
| openWakeWord | Wake-word detection |
| Piper | Provides spoken responses |

### Supported Voice Commands

Examples of intended voice commands include:

- "Set fan to low."
- "Set fan to high."
- "Turn fan off."

The recognized command is processed through Home Assistant automations, which send the appropriate MQTT payload to the ESP32.

## 11. Project Directory Structure

The firmware project follows a modular structure.

```text
smart_fan/
│
├── include/
│   ├── app_hw.h
│   ├── app_wifi.h
│   └── app_mqtt.h
│
├── src/
│   ├── main.c
│   ├── app_hw.c
│   ├── app_wifi.c
│   └── app_mqtt.c
│
├── platformio.ini
│
└── README.md
```

*The structure above represents the main modules discussed during development. Adjust the directory names if your actual PlatformIO project uses a different layout.*

### Main Firmware Modules

| File | Responsibility |
|---|---|
| `main.c` | Application entry point and initialization |
| `app_hw.c` | Button handling, relay switching, and LED control |
| `app_hw.h` | Hardware definitions and function declarations |
| `app_wifi.c` | Wi-Fi connection management |
| `app_wifi.h` | Wi-Fi configuration declarations |
| `app_mqtt.c` | MQTT connection, subscriptions, and message handling |
| `app_mqtt.h` | MQTT declarations and interfaces |
| `platformio.ini` | PlatformIO build and upload configuration |

## 12. Installation and Setup

### Requirements

Before setting up the project, prepare the following:

- ESP32 development board.
- Dual-channel relay module.
- Three push buttons.
- Two LEDs and 220Ω resistors.
- Visual Studio Code.
- PlatformIO extension.
- Home Assistant OS running in VirtualBox.
- Mosquitto MQTT broker.
- A Wi-Fi network accessible to both the ESP32 and Home Assistant.

The ESP32 requires a 2.4 GHz Wi-Fi connection.

### Step 1: Clone the Repository

```bash
git clone YOUR_REPOSITORY_URL
```

Navigate to the project directory:

```bash
cd smart_fan
```

### Step 2: Configure Wi-Fi

Open `app_wifi.c` or the corresponding Wi-Fi configuration header.

Set your network credentials:

```c
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"
```

Use a 2.4 GHz Wi-Fi network supported by the ESP32.

### Step 3: Configure MQTT

Open `app_mqtt.c` and configure:

- MQTT broker IP address.
- MQTT port.
- MQTT username.
- MQTT password.
- MQTT topics.

Ensure that the broker is reachable from the ESP32.

### Step 4: Build the Firmware

Open the project in Visual Studio Code with PlatformIO installed.

Build the project using:

```bash
pio run
```

### Step 5: Upload the Firmware

Connect the ESP32 to your computer through USB.

Upload the firmware:

```bash
pio run --target upload
```

### Step 6: Monitor Serial Output

Open the PlatformIO Serial Monitor:

```bash
pio device monitor
```

Use the serial output to inspect:

- ESP32 initialization.
- Wi-Fi connection.
- MQTT connection.
- Incoming MQTT messages.
- Hardware state changes.

### Step 7: Configure Home Assistant

Install and configure Mosquitto MQTT.

Create the `input_select.fan_control` helper with OFF, LOW, and HIGH options.

Configure the MQTT automation to publish the selected state to:

```text
smartfan/set
```

The ESP32 should subscribe to the same topic.

## 13. Safety Considerations

- Disconnect the ESP32 before modifying wiring.
- Verify GPIO assignments before powering the circuit.
- Use current-limiting resistors for LEDs.
- Ensure relay logic matches the relay module's active-low behavior.
- Never connect mains voltage directly to ESP32 GPIO pins.
- Use properly rated and insulated components when controlling actual electrical appliances.
- Keep low-voltage control circuits isolated from hazardous mains wiring.
- Do not expose MQTT credentials or Wi-Fi passwords in public repositories.

## 14. Current Development Status

This project is being developed and tested as an IoT smart control prototype.

The following features have been implemented or configured during development:


- ESP32 firmware using PlatformIO and ESP-IDF.
- Three physical button inputs.
- Dual-relay output configuration.
- LOW and HIGH LED indicators.
- Wi-Fi connectivity.
- Mosquitto MQTT integration.
- Home Assistant dashboard control.
- Voice Satellite integration.
- Speech-to-Phrase and Piper configuration.

Some integration and reliability testing remains ongoing, particularly network connectivity, relay operation following firmware changes, and browser microphone access for the Voice Satellite.

## 15. Future Improvements

Potential improvements include:

- Complete end-to-end testing of physical and remote controls.
- Improve MQTT connection recovery and error handling.
- Improve synchronization between Home Assistant and ESP32 states.
- Complete local HTTPS configuration for browser-based voice control.
- Add connection-status indicators.
- Integrate an actual fan for full operational testing.
- Improve system reliability and fault handling.

## 16. Author and Project Information

**Project:** ESP32 Smart Fan Control System  
**Category:** Internet of Things (IoT) / Embedded Systems / Home Automation  
**Controller:** ESP32  
**Framework:** ESP-IDF  
**Development Environment:** PlatformIO  
**Automation Platform:** Home Assistant  
**Communication Protocol:** MQTT  
**MQTT Broker:** Mosquitto  
**Authors:**
Kent Jesser Neri
Alwyn Reuyan
JC Redelosa
Section: BSCA-B186 | Date: 11/15/2026

---

*Developed as an IoT prototype demonstrating the integration of embedded hardware, wireless communication, relay-based control, and voice-assisted home automation.*
