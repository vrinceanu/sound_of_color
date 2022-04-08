# sound of color

An exploration of the physics and the physiology of light and acoustic waves.

This system has the following parts

- [Adafruit Feather M0 Bluefruit LE](https://www.adafruit.com/product/2995) is a board that has a microcontrolled coupled with a bluetooth module
- [Flora Color Sensor with White Illumination LED - TCS34725](https://www.adafruit.com/product/1356) is a 12 pixel sensor with groups of 3 pixels sensitive to red, green, blue and clear light. In addition, it has a bright white LED that illuminates the surface that is probed by the sensor
- [Flora RGB Smart NeoPixel version 2](https://www.adafruit.com/product/1260) is used to display the color code acquired by the sensor
- the microcontroller is powered from USB when connected to a computer for transferring the program, an by battery when the system is in operation

![Diagramatic representation of the system](/assets/diagram.png)


When enough light is received, the sensor reports the RGB components of the received light to the microcontroller. Microcontroller converts the color code to midi sound instructions that are transmitted via Bluetooth to a smartphone or other Bluetooth capable device where the midi instructions are interpreted as a synthetic instrument through Garage Band-like software. In addition a RGB LED can use to reflect the sensed color.

This work is inspired by the [Piano Glove](https://learn.adafruit.com/pianoglove) project.

![Piano_glove](https://learn.adafruit.com/assets/10099)
