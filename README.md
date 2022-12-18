# esp8266-park-monitoring
A simple park monitoring with ESP8266.

![Fix](https://user-images.githubusercontent.com/113373725/208310635-f85c2a29-fbcf-45e8-8a8c-e554b0b23ada.png)

- 3 IR Proximity sensors
- 1 LDR sensor
- 1 Push button
- 5 220 ohm sensors
- 5 LED (as slot indicator, gate, and lighting)

How it's work
1. Push the press button to reserve a parking slot.
2. Wait until one of LED indicator slot goes on, system will reserve one of available slot.
3. If there's still one or more available parking slot, the gate LED will be on for 1s, then a LED indicator will on.
4. Park your vehicle in the alocated spot (according to which LED indicator is on). Wait until the LED indicator goes off. The system records the time when the vehicle enter the slot.
5. The system will check if your vehicle is still on the slot or not every 5s, so if after 5s your vehicle is not detected to be park on the slot, the system will count your vehichle is already checked-out.
6. The lighting LED will be on by default, but if you point a flaslight towards the LDR sensor, the lighting LED will on. 
