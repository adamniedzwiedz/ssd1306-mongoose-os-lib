# Arduino Adafruit SSD1306 LCD library


## Overview

This is port of the mongoose os library https://github.com/mongoose-os-libs/arduino-adafruit-ssd1306 adjusted to 
the 0.66" (64x48) OLED LCD version.

<p align="center">
  <img src="https://github.com/adamniedzwiedz/ssd1306-mongoose-os-lib/blob/master/oled_m.png">
</p>

## Example of usage

```c
#include <Arduino.h>
#include <Arduino_SSD1306_ESP.h>  

Arduino_SSD1306_ESP *oled = new Arduino_SSD1306_ESP();
oled->begin(SSD1306_I2C_ADDRESS);
oled->cleanDisplayMemory();

oled->setTextSize(1);
oled->setTextColor(WHITE);

oled->setCursor(0,0);
oled->println("Long text will be wrapped");

oled->display();
```

## Example of the scrolling

```c
#include <Arduino.h>
#include <Arduino_SSD1306_ESP.h>  

Arduino_SSD1306_ESP *oled = new Arduino_SSD1306_ESP();
oled->begin(SSD1306_I2C_ADDRESS);
oled->cleanDisplayMemory();

oled->setTextSize(1);
oled->setTextColor(WHITE);

oled->setCursor(0,0);
oled->println("Long text to scroll");

oled->setCursor(0,20);
oled->println("=The 2nd long text=");

oled->display();
delay(2000);
oled->startscrollleft(0, 5);
```