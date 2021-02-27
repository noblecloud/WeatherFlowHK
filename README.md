# WeatherFlowHK Bridge
Very basic HomeKit bridge for WeatherFlow Tempest Weather Station.

## About
Uses an esp32 based micro controller to listen to the UDP packets sent from the Tempest bridge and relays them out as HomeKit announcements. No internet connection required and everything stays local.

## Functionality
There are currently no services in the HomeKit specification that display anything other than temperature, humidity, and light levels. Hopefully that will eventually change, but until then rain, wind, and lightning are simulated buttons to trigger actions.  The thresholds are currently hardcoded but will eventually be adjustable via a small webpage served from the device.

### Wind
Single Press: Wind over 10m/s (~22mph)
Double Press: Nothing
Long Press: Nothing

### Lightning
Single Press: Every event
Double Press: Only if last SENT event was more than 5 seconds ago
Long Press: Only if last SENT event was more than 10 seconds ago
__*Note: this is only the last by the last event announced to HomeKit. This will be eventually changed to last received event.*__

### Rain
Single Press: Every event
Double Press: Nothing
Long Press: Nothing

## Future
- [ ] Add webserver for adjusting event thresholds
- [ ] Display more info via sensor names *e.g. "Wind: 1.4m/s"*
