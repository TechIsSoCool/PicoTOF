# PicoTOF
This project uses a Raspberry Pi Pico to interface with the Adafruit VL53L1X Time of flight sensor module. Measurements are displayed on a SSD1306-based OLED Display. The code is written for the Arduino IDE.
Please see [Pi Pico Time-of-Flight Project in Arduino IDE](https://techissocool.com/pi-pico-tof-arduino) for a complete walk-through of the project.

## Hardware
- Raspberry Pi Pico [DigiKey](https://www.digikey.com/en/products/detail/raspberry-pi/SC0915/13624793)
- Adafruit VL53L1X Time of flight sensor module [Adafruit](https://www.adafruit.com/product/3967)
- 0.91 inch, 128x32 pixel OLED monochrome display [Amazon](https://amzn.to/3cfAUvI)

## Circuit
The VL53L1X sensor module and SSD1306 display are connected via I2C. A momentary pushbutton on one GPIO pin cycles through the display modes. See link above and code for more specific description.

## External Software Libraries Used
- arduino-pico - RP2040 board support for Arduino IDE [GitHub](https://github.com/earlephilhower/arduino-pico/)
- Wire.h – to handle the I2C communications (native to Arduino IDE)
- Adafruit VL53L1X – for the time-of-flight sensor [GitHub](https://github.com/adafruit/Adafruit_VL53L1X)
- Adafruit GFX Library – Display core graphics library [GitHub](https://github.com/adafruit/Adafruit-GFX-Library)
- Adafruit SSD1306 – Or other library to support your display driver [GitHub](https://github.com/adafruit/Adafruit_SSD1306)
