# UDP_RC_TX

Project using the ESP8266 as transmitter UDP RC. Done with arduino for esp8266 ide. It uses PPMSUM imput to transmitter. Transmitter PPM in pin is D4. Easily reconfigurable. 

You go to the transmitter webpage (IP is displayed over serial port). Set your receiver IP and port there. This tells the transmitter to which receiver it is connected, its the "binding".
Be aware the setting is not permanent, hard-code it in firmware.

This does 6 channels at variable rate. Set 100ms for car and 50ms or less for airborne.

![DSC02359.jpg](DSC02359.jpg "Wiring")
