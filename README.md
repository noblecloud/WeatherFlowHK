# WeatherFlowHK Bridge
Very basic HomeKit bridge for WeatherFlow Tempest Weather Station.

## About
Uses an ESP32 based microcontroller to listen to the UDP packets sent from a Tempest bridge and relays them out as HomeKit HAP Notifications. No internet connection required and everything stays local.

## Directions
### 1). Install with Arduino IDE
1. Open the Arduino sketch __```WeatherFlowHK.ino```__ in the Arduino IDE
2. Install both the [HomeSpan](https://github.com/HomeSpan/HomeSpan) and [ArduinoJson](https://github.com/bblanchon/ArduinoJson) libraries
    
    __*Sketch > Include Libraries > Manage Libraries*__
3. Follow the directions [here](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md) to install the various ESP32 boards or install whatever is relevant to your specific board and select the board you are using from
    
    __*Tools > Board > ESP32 Arduino > Your Board*__
4. Upload the sketch to the board with __*Sketch > Upload*__ or click the little arrow pointing right in the top left corner of the window

### 2). Configuring the device for HomeKit
1. Open the Serial Monitor by clicking the magnifying glass in the top right of the window or __*Tools > Serial Monitor*__
2. Change the line ending to __*Newline*__ and the baud to __*115200*__ *(bottom right of the Serial Monitor)*
3. Type "W" into the serial input and press *Send* or press the enter key on your keyboard then follow the prompts

### 3). Connect to HomeKit
![Default HomeKit Code](https://github.com/HomeSpan/HomeSpan/raw/master/docs/images/defaultSetupCode.png)

Add the device like you would any other accessory by using your camera to scan the code above or typing ```466-37-726``` in manually

\*\**Optional: Set your own code within the Serial Monitor by typing* "S"\*\*

## Functionality
There are currently no services in the HomeKit specification that display anything other than temperature, humidity, and light levels. Hopefully that will eventually change, but until then rain, wind, and lightning are simulated buttons to trigger actions.  The thresholds are currently hardcoded but will eventually be adjustable via a small webpage served from the device.

### Wind
| Action | Trigger |
| ------ | ------- |
| Single Press | Wind over 10m/s (~22mph) |
| Double Press | Nothing |
| Long Press | Nothing |

### Lightning
| Action | Trigger |
| ------ | ------- |
| Single Press | Every event |
| Double Press | Only if last SENT event was more than 5 seconds ago |
| Long Press | Only if last SENT event was more than 10 seconds ago |

__*Note: this is only the last by the last event announced to HomeKit. This will be eventually changed to last received event.*__

### Rain
| Action | Trigger |
| ------ | ------- |
| Single Press | Every event |
| Double Press | Nothing |
| Long Press | Nothing |

## Future
- [ ] Add save most resent values to NVS for better numbers upon reboot
- [ ] Add webserver for adjusting event thresholds
- [ ] Display more info via sensor names *e.g. "Wind: 1.4m/s"*
- [ ] Support for Air and Sky
- [ ] Add optional BLE subscription to hub
## Dependencies
Uses the wonderful [HomeSpan](https://github.com/HomeSpan/HomeSpan) library for HomeKit functionality and [ArduinoJSON](https://github.com/bblanchon/ArduinoJson) for parsing the UDP datagrams.
