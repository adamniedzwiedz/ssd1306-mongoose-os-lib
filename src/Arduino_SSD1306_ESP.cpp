#include <pgmspace.h>

#include <stdlib.h>

#include <Wire.h>
#include "Adafruit_GFX.h"
#include "Arduino_SSD1306_ESP.h"

/* commands */
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA

#define SSD1306_SETVCOMDETECT 0xDB

#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9

#define SSD1306_SETMULTIPLEX 0xA8

#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10

#define SSD1306_SETSTARTLINE 0x40

#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8

#define SSD1306_SEGREMAP 0xA0

#define SSD1306_CHARGEPUMP 0x8D

#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling #defines
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

#define ssd1306_swap(a, b) { int16_t t = a; a = b; b = t; }

Arduino_SSD1306_ESP::Arduino_SSD1306_ESP(int16_t width) : Adafruit_GFX(width, OLED_HEIGHT) {
    overflow = width > OLED_WIDTH;
}

void Arduino_SSD1306_ESP::begin(uint8_t i2caddr) {
    _i2caddr = i2caddr;
    
    Wire.begin();

    // Init sequence
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80

    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(HEIGHT - 1);

    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    
    ssd1306_command(SSD1306_CHARGEPUMP);   
    ssd1306_command(0x14);

    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4           

    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);

    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x12);

    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0x8F);

    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);

    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);   

    ssd1306_command(SSD1306_DEACTIVATE_SCROLL);

    ssd1306_command(SSD1306_DISPLAYON); //--turn on oled panel
}

// the most basic function, set a single pixel
void Arduino_SSD1306_ESP::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT))
        return;
    
    // check rotation, move pixel around if necessary
    switch (getRotation()) {
        case 1:
            ssd1306_swap(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            ssd1306_swap(x, y);
            y = HEIGHT - y - 1;
        break;
    }

    // x is which column
    switch (color)
    {
        case WHITE:   
            buffer[x+ (y/8)*WIDTH] |=  (1 << (y&7)); 
            break;
        case BLACK:   
            buffer[x+ (y/8)*WIDTH] &= ~(1 << (y&7)); 
            break;
        case INVERSE: 
            buffer[x+ (y/8)*WIDTH] ^=  (1 << (y&7)); 
            break;
    }
}

void Arduino_SSD1306_ESP::invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void Arduino_SSD1306_ESP::startscrollright(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void Arduino_SSD1306_ESP::startscrollleft(uint8_t start, uint8_t stop){
  ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
  ssd1306_command(0X00);
  ssd1306_command(start);
  ssd1306_command(0X00);
  ssd1306_command(stop);
  ssd1306_command(0X00);
  ssd1306_command(0XFF);
  ssd1306_command(SSD1306_ACTIVATE_SCROLL);
} 

void Arduino_SSD1306_ESP::stopscroll(void){
  ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void Arduino_SSD1306_ESP::dim(boolean dim) {
    if (dim) {
        setContrast(0); // Dimmed display
    } else {
        setContrast(0xCF);
    }
    // the range of contrast to too small to be really useful
    // it is useful to dim the display
}

void Arduino_SSD1306_ESP::ssd1306_command(uint8_t c) {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    Wire.beginTransmission(_i2caddr);
    Wire.write(control);
    Wire.write(c);
    Wire.endTransmission();
}

void Arduino_SSD1306_ESP::setContrast(uint8_t contrast) {
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(contrast);
}

// clear everything
void Arduino_SSD1306_ESP::clearDisplay(void) {
    memset(buffer, 0, (WIDTH*HEIGHT/8));
}

void Arduino_SSD1306_ESP::cleanDisplayMemory(void) {
    clearDisplay();
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(127);
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(5);
    for (uint16_t i=0; i<64; i++) {
        Wire.beginTransmission(_i2caddr);
        Wire.write(0x40);
        for (uint8_t x=0; x<16; x++) {
            Wire.write(0);
        }
        Wire.endTransmission();
    }
}

void Arduino_SSD1306_ESP::display(void) {
    uint16_t length = WIDTH*HEIGHT/8;

    ssd1306_command(SSD1306_COLUMNADDR);
    
    if (overflow) {
        ssd1306_command(0);   // Column start address (0 = reset)
        ssd1306_command(WIDTH-1); // Column end address (127 = reset)
    }
    else {
        ssd1306_command(32);   // Column start address (0 = reset)
        ssd1306_command(WIDTH-1+32); // Column end address (127 = reset)
    }

    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0); // Page start address (0 = reset)
    ssd1306_command(5); // Page end address

    for (uint16_t i=0; i<length; i++) {
      // send a bunch of data in one xmission
      Wire.beginTransmission(_i2caddr);
      Wire.write(0x40);
      for (uint8_t x=0; x<16; x++) {
        Wire.write(buffer[i]);
        i++;
      }
      i--;
      Wire.endTransmission();
    }
}

void Arduino_SSD1306_ESP::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    boolean bSwap = false;
    switch(rotation) {
        case 0:
            // 0 degree rotation, do nothing
            break;
        case 1:
            // 90 degree rotation, swap x & y for rotation, then invert x
            bSwap = true;
            ssd1306_swap(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            // 180 degree rotation, invert x and y - then shift y around for height.
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            x -= (w-1);
            break;
        case 3:
            // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
            bSwap = true;
            ssd1306_swap(x, y);
            y = HEIGHT - y - 1;
            y -= (w-1);
            break;
    }

    if(bSwap) {
        drawFastVLineInternal(x, y, w, color);
    } else {
        drawFastHLineInternal(x, y, w, color);
    }
}

void Arduino_SSD1306_ESP::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
    // Do bounds/limit checks
    if(y < 0 || y >= HEIGHT) { return; }

    // make sure we don't try to draw below 0
    if(x < 0) {
        w += x;
        x = 0;
    }

    // make sure we don't go off the edge of the display
    if( (x + w) > WIDTH) {
        w = (WIDTH - x);
    }

    // if our width is now negative, punt
    if(w <= 0) { return; }

    // set up the pointer for  movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y/8) * WIDTH);
    // and offset x columns in
    pBuf += x;

    register uint8_t mask = 1 << (y&7);

    switch (color)
    {
        case WHITE:         
            while(w--) { *pBuf++ |= mask; }; 
            break;
        case BLACK: 
            mask = ~mask;   
            while(w--) { 
                *pBuf++ &= mask; 
            }; 
            break;
        case INVERSE:         
            while(w--) { 
                *pBuf++ ^= mask; 
            }; 
            break;
    }
}

void Arduino_SSD1306_ESP::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    bool bSwap = false;
    switch(rotation) {
        case 0:
            break;
        case 1:
            // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
            bSwap = true;
            ssd1306_swap(x, y);
            x = WIDTH - x - 1;
            x -= (h-1);
            break;
        case 2:
            // 180 degree rotation, invert x and y - then shift y around for height.
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            y -= (h-1);
            break;
        case 3:
            // 270 degree rotation, swap x & y for rotation, then invert y
            bSwap = true;
            ssd1306_swap(x, y);
            y = HEIGHT - y - 1;
            break;
    }

    if(bSwap) {
        drawFastHLineInternal(x, y, h, color);
    } else {
        drawFastVLineInternal(x, y, h, color);
    }
}

void Arduino_SSD1306_ESP::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {
    // do nothing if we're off the left or right side of the screen
    if(x < 0 || x >= WIDTH) { return; }

    // make sure we don't try to draw below 0
    if(__y < 0) {
        // __y is negative, this will subtract enough from __h to account for __y being 0
        __h += __y;
        __y = 0;
    }

    // make sure we don't go past the height of the display
    if( (__y + __h) > HEIGHT) {
        __h = (HEIGHT - __y);
    }

    // if our height is now negative, punt
    if(__h <= 0) {
        return;
    }

    // this display doesn't need ints for coordinates, use local byte registers for faster juggling
    register uint8_t y = __y;
    register uint8_t h = __h;

    // set up the pointer for fast movement through the buffer
    register uint8_t *pBuf = buffer;
    // adjust the buffer pointer for the current row
    pBuf += ((y/8) * WIDTH);
    // and offset x columns in
    pBuf += x;

    // do the first partial byte, if necessary - this requires some masking
    register uint8_t mod = (y&7);
    if(mod) {
        // mask off the high n bits we want to set
        mod = 8-mod;

        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        // register uint8_t mask = ~(0xFF >> (mod));
        static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
        register uint8_t mask = premask[mod];

        // adjust the mask if we're not going to reach the end of this byte
        if( h < mod) {
            mask &= (0XFF >> (mod-h));
        }

        switch (color)
        {
            case WHITE:   
                *pBuf |=  mask;  
                break;
            case BLACK:   
                *pBuf &= ~mask;  
                break;
            case INVERSE: 
                *pBuf ^=  mask;  
                break;
        }

        // fast exit if we're done here!
        if(h<mod) { return; }

        h -= mod;

        pBuf += WIDTH;
    }

    // write solid bytes while we can - effectively doing 8 rows at a time
    if(h >= 8) {
        if (color == INVERSE)  {          // separate copy of the code so we don't impact performance of the black/white write version with an extra comparison per loop
            do  {
                *pBuf=~(*pBuf);

                // adjust the buffer forward 8 rows worth of data
                pBuf += WIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            } while(h >= 8);
        }
        else {
            // store a local value to work with
            register uint8_t val = (color == WHITE) ? 255 : 0;

            do  {
                // write our value in
                *pBuf = val;

                // adjust the buffer forward 8 rows worth of data
                pBuf += WIDTH;

                // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
                h -= 8;
            } while(h >= 8);
        }
    }

    // now do the final partial byte, if necessary
    if(h) {
        mod = h & 7;
        // this time we want to mask the low bits of the byte, vs the high bits we did above
        // register uint8_t mask = (1 << mod) - 1;
        // note - lookup table results in a nearly 10% performance improvement in fill* functions
        static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
        register uint8_t mask = postmask[mod];
        switch (color)
        {
            case WHITE:   
                *pBuf |=  mask;  
                break;
            case BLACK:   
                *pBuf &= ~mask;  
                break;
            case INVERSE: 
                *pBuf ^=  mask;  
                break;
        }
    }
}