#ifndef INC_SSD1306_H_
#define INC_SSD1306_H_

//**************************SETTINGS**************************

//adress
#define SSD1306_ADR 0x3C

//resolution
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

//buffer
#define USE_FULLBUFFER
//#define USE_PAGEBUFFER
/*
    To use PAGEBUFFER create in main functions like this 
    void draw1(void)
    {
        for(page = 0; page < NUM_PAGES; page++)
        {
            GFX_something;
            ssd1306_display();
        }
    }

*/

//************************************************************

#ifdef USE_PAGEBUFFER
extern uint8_t page;
#endif

//refresh rate
#define REFRESH_MIN 0x80
#define REFTESH_MID 0xB0
#define REFRESH_MAX 0xF0

//command list by Adafruit
#define SSD1306_MEMORYMODE          0x20 ///< See datasheet
#define SSD1306_COLUMNADDR          0x21 ///< See datasheet
#define SSD1306_PAGEADDR            0x22 ///< See datasheet
#define SSD1306_SETCONTRAST         0x81 ///< See datasheet
#define SSD1306_CHARGEPUMP          0x8D ///< See datasheet
#define SSD1306_SEGREMAP            0xA0 ///< See datasheet
#define SSD1306_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SSD1306_DISPLAYALLON        0xA5 ///< Not currently used
#define SSD1306_NORMALDISPLAY       0xA6 ///< See datasheet
#define SSD1306_INVERTDISPLAY       0xA7 ///< See datasheet
#define SSD1306_SETMULTIPLEX        0xA8 ///< See datasheet
#define SSD1306_DISPLAYOFF          0xAE ///< See datasheet
#define SSD1306_DISPLAYON           0xAF ///< See datasheet
#define SSD1306_COMSCANINC          0xC0 ///< Not currently used
#define SSD1306_COMSCANDEC          0xC8 ///< See datasheet
#define SSD1306_SETDISPLAYOFFSET    0xD3 ///< See datasheet
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 ///< See datasheet
#define SSD1306_SETPRECHARGE        0xD9 ///< See datasheet
#define SSD1306_SETCOMPINS          0xDA ///< See datasheet
#define SSD1306_SETVCOMDETECT       0xDB ///< See datasheet

#define SSD1306_SETLOWCOLUMN        0x00 ///< Not currently used
#define SSD1306_SETHIGHCOLUMN       0x10 ///< Not currently used
#define SSD1306_SETSTARTLINE        0x40 ///< See datasheet

#define SSD1306_EXTERNALVCC         0x01 ///< External display voltage source
#define SSD1306_SWITCHCAPVCC        0x02 ///< Gen. display voltage from 3.3V

#define SSD1306_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Init rt scroll
#define SSD1306_LEFT_HORIZONTAL_SCROLL               0x27 ///< Init left scroll
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Init diag scroll
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Init diag scroll
#define SSD1306_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1306_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1306_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Set scroll range




void ssd1306_cmd(uint8_t cmd);

void ssd1306_data(uint8_t data);

void ssd1306_init(uint8_t vcc, uint8_t refresh);

void ssd1306_display(void);

void ssd1306_cls(void);

#ifdef USE_PAGEBUFFER
void ssd1306_cls_page(void);
#endif

void ssd1306_set_pixel(int x, int y, uint8_t color);

void ssd1306_bitmap(int x, int y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);

void ssd1306_set_contrast(uint8_t contrast);






#endif /* INC_SSD1306_H_ */
