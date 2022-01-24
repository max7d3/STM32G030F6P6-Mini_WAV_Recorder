/*
 * BW_GFX.h
 *
 *  Created on: Mar 11, 2021
 *      Author: Max
 */

#ifndef INC_BW_GFX_H_
#define INC_BW_GFX_H_

#define GFX_DrawPixel(x,y,color) ssd1306_set_pixel(x,y,color)

#define WHITE 1
#define BLACK 0

//**************************SETTINGS**************************

#define USE_LINES 1
#define USE_RECTANGLES 1
#define USE_CIRCLES 1
#define USE_TRIANGLES 1

#define USE_CHARACTERS 1 // Lines & rectangles req.

//************************************************************

#if USE_LINES == 1

void GFX_WriteLine(int x_start, int y_start, int x_end, int y_end, uint8_t color);
void GFX_DrawFastHLine(int x_start, int y_start, int w, uint8_t color);
void GFX_DrawFastVLine(int x_start, int y_start, int h, uint8_t color);
void GFX_DrawLine(int x_start, int y_start, int x_end, int y_end, uint8_t color);

#endif

#if USE_RECTANGLES == 1

void GFX_DrawRectangle(int x_pos, int y_pos, int w, int h, uint8_t color);
void GFX_DrawFiledRectangle(int x_pos, int y_pos, int w, int h, uint8_t color);

#endif

#if USE_CIRCLES == 1

void GFX_DrawCircle(int x_pos, int y_pos, int r, uint8_t color);
void GFX_DrawFilledCircle(int x_pos, int y_pos, int r, uint8_t color);

#endif

#if USE_TRIANGLES == 1

void GFX_DrawTriangle(int x_pos0, int y_pos0, int x_pos1, int y_pos1, int x_pos2, int y_pos2, uint8_t color);
void GFX_DrawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);

#endif

#if USE_CHARACTERS == 1

extern const uint8_t font[];

int8_t cursor_x;
int8_t cursor_y;

void GFX_DrawChar(int x_pos, int y_pos, char c, uint8_t color, uint8_t background, uint8_t size);
void GFX_Print(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size);
void GFX_PrintNOROLLING(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size);
void GFX_PrintROLLING(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size);
void GFX_PrintCentered(int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size);
void GFX_PrintCenteredAddLenght(int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size, uint8_t add_lenght);
void GFX_PrintInt(int x_pos, int y_pos, int data, uint8_t color, uint8_t background, uint8_t size);
void GFX_PrintIntHEX(int x_pos, int y_pos, int data, uint8_t color, uint8_t background, uint8_t size);

#endif

#endif /* INC_BW_GFX_H_ */
