#ifndef _Arduino_SSD1306_ESP_H_
#define _Arduino_SSD1306_ESP_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Adafruit_GFX.h>

#define OLED_WIDTH 64
#define OLED_HEIGHT 48

#define BLACK 0
#define WHITE 1
#define INVERSE 2

#define SSD1306_I2C_ADDRESS   0x3C

class Arduino_SSD1306_ESP : public Adafruit_GFX {
 public:
    Arduino_SSD1306_ESP(int16_t width = OLED_WIDTH);

    void begin(uint8_t i2caddr = SSD1306_I2C_ADDRESS);
    void ssd1306_command(uint8_t c);

    void clearDisplay(void);
    void cleanDisplayMemory(void);
    void invertDisplay(uint8_t i);
    void display(void);

    void dim(boolean dim);
    void setContrast(uint8_t contrast);

    void drawPixel(int16_t x, int16_t y, uint16_t color);

    void startscrollright(uint8_t start, uint8_t stop);
    void startscrollleft(uint8_t start, uint8_t stop);
    void stopscroll(void);

    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 private:    
    int8_t _i2caddr;
    uint8_t buffer[128*48/8];
    boolean overflow;

    inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
    inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
};

#endif /* _Arduino_SSD1306_ESP_H_ */
