# Project Description

In this project, we aim to establish a system that automates garden watering to conserve the precious resource of water, based on local weather conditions. The system comprises:

- A PIC16F877 microcontroller operating at an oscillator frequency of 4 MHz.
- An analog soil moisture sensor.
- A rain sensor connected to pin RB0.
- A switch connected to pin RB4 (open = 1 / closed = 0) used to manually activate/deactivate the watering system.
- A "Consult" button connected to pin RB5.
- A watering system connected to activate the pump (activated = 1 / deactivated = 0).
- A buzzer.
- A serial LCD screen.
- An EEPROM memory.

## Functionalities

At startup, all actuators of this system are inactive, and the LCD screen displays the message "SMART GARDEN".

Once activated, this sprinkler system provides plants with the necessary amount of water as follows:

- It reads the real-time soil moisture through the soil moisture sensor, ensuring the following functions:
  - If the moisture value is less than or equal to 30% Hr, the watering system activates, and the LCD screen displays the message "watering lawn" until the moisture value reaches 50% Hr. Once watering is finished, the LCD screen displays the message "end watering lawn".
  - In case the watering system fails, an alarm system will be implemented using a buzzer that will operate as follows: If the moisture value is below 29% Hr, the buzzer activates for 5 minutes, and the LCD screen displays the message "system failure". Each time a failure occurs, the number of failures already stored in the EEPROM memory will be incremented. This number is initially set to 0.
  
- As soon as the switch is opened, the watering system should activate immediately, and the LCD screen displays the message "watering lawn". The watering system will remain active until the switch is closed, at which point the LCD screen displays the message "end watering lawn".

- When the rain sensor detects rain, the watering system should stop immediately. The watering system will remain off until there is no more rain.

- To check the number of failures, the user presses the "Consult" button, and this number will be displayed on the screen for 1 minute.
