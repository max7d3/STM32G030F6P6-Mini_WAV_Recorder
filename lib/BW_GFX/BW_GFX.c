/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */


/*
	Adafruit GFX converted to C with some modifications and added functions
*/

#include "stm32g0xx.h"
#include "stdlib.h"
#include "string.h"


#include "ssd1306.h"
#include "BW_GFX.h"


#define _swap_int(a, b) { int t = a; a = b; b = t; }


//Line Drawing ******************************************************************

#if USE_LINES == 1

void GFX_WriteLine(int x_start, int y_start, int x_end, int y_end, uint8_t color)
{
	int16_t steep = abs(y_end - y_start) > abs(x_end - x_start);

	    if (steep) {
	        _swap_int(x_start, y_start);
	        _swap_int(x_end, y_end);
	    }

	    if (x_start > x_end) {
	        _swap_int(x_start, x_end);
	        _swap_int(y_start, y_end);
	    }

	    int16_t dx, dy;
	    dx = x_end - x_start;
	    dy = abs(y_end - y_start);

	    int16_t err = dx / 2;
	    int16_t ystep;

	    if (y_start < y_end) {
	        ystep = 1;
	    } else {
	        ystep = -1;
	    }

	    for (; x_start<=x_end; x_start++) {
	        if (steep) {
	        	GFX_DrawPixel(y_start, x_start, color);
	        } else {
	        	GFX_DrawPixel(x_start, y_start, color);
	        }
	        err -= dy;
	        if (err < 0) {
	            y_start += ystep;
	            err += dx;
	        }
	    }
}


void GFX_DrawFastVLine(int x_start, int y_start, int h, uint8_t color)
{
	GFX_WriteLine(x_start, y_start, x_start, y_start+h-1, color);
}


void GFX_DrawFastHLine(int x_start, int y_start, int w, uint8_t color)
{
	GFX_WriteLine(x_start, y_start, x_start+w-1, y_start, color);
}


void GFX_DrawLine(int x_start, int y_start, int x_end, int y_end, uint8_t color)  //Czy na pewno jest potrzebne ?
{
	if(x_start == x_end){
	        if(y_start > y_end) _swap_int(y_start, y_end);
	        GFX_DrawFastVLine(x_start, y_start, y_end - y_start + 1, color);
	    } else if(y_start == y_end){
	        if(x_start > x_end) _swap_int(x_start, x_end);
	        GFX_DrawFastHLine(x_start, y_start, x_end - x_start + 1, color);
	    } else {

	    	GFX_WriteLine(x_start, y_start, x_end, y_end, color);
	    }
}

#endif

//*******************************************************************************



//Rectangle drawing**************************************************************

#if USE_RECTANGLES == 1

void GFX_DrawRectangle(int x_pos, int y_pos, int w, int h, uint8_t color)
{
	GFX_DrawLine(x_pos, y_pos, x_pos, y_pos + h - 1 , color);
	GFX_DrawLine(x_pos, y_pos, x_pos + w - 1 , y_pos, color);
	GFX_DrawLine(x_pos, y_pos + h - 1, x_pos + w - 1, y_pos + h - 1, color);
    GFX_DrawLine(x_pos + w - 1, y_pos, x_pos + w - 1, y_pos + h - 1, color);

}

void GFX_DrawFiledRectangle(int x_pos, int y_pos, int w, int h, uint8_t color)
{
	//GFX_DrawLine(x_pos, y_pos, x_pos, y_pos + h - 1 , color);
	//GFX_DrawLine(x_pos, y_pos, x_pos + w - 1 , y_pos, color);
	//GFX_DrawLine(x_pos, y_pos + h - 1, x_pos + w - 1, y_pos + h - 1, color);
    //GFX_DrawLine(x_pos + w - 1, y_pos, x_pos + w - 1, y_pos + h - 1, color);

	  for (int16_t i = x_pos; i < x_pos + w; i++) {
		  GFX_DrawFastVLine(i, y_pos, h, color);
	  }

}

#endif

//*******************************************************************************



//Circle drawing*****************************************************************

#if USE_CIRCLES == 1

void GFX_DrawCircle(int x_pos, int y_pos, int r, uint8_t color)
{
	  int16_t f = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x = 0;
	  int16_t y = r;

	  GFX_DrawPixel(x_pos, y_pos + r, color);
	  GFX_DrawPixel(x_pos, y_pos - r, color);
	  GFX_DrawPixel(x_pos + r, y_pos, color);
	  GFX_DrawPixel(x_pos - r, y_pos, color);

	  while (x < y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x;

	    GFX_DrawPixel(x_pos + x, y_pos + y, color);
	    GFX_DrawPixel(x_pos - x, y_pos + y, color);
	    GFX_DrawPixel(x_pos + x, y_pos - y, color);
	    GFX_DrawPixel(x_pos - x, y_pos - y, color);
	    GFX_DrawPixel(x_pos + y, y_pos + x, color);
	    GFX_DrawPixel(x_pos - y, y_pos + x, color);
	    GFX_DrawPixel(x_pos + y, y_pos - x, color);
	    GFX_DrawPixel(x_pos - y, y_pos - x, color);

	  	}

}

void GFX_DrawFilledCircleHelper(int x0, int y0, int r, uint8_t corners, int16_t delta, uint8_t color)
{
	  int16_t f = 1 - r;
	  int16_t ddF_x = 1;
	  int16_t ddF_y = -2 * r;
	  int16_t x = 0;
	  int16_t y = r;
	  int16_t px = x;
	  int16_t py = y;

	  delta++; // Avoid some +1's in the loop

	  while (x < y) {
	    if (f >= 0) {
	      y--;
	      ddF_y += 2;
	      f += ddF_y;
	    }
	    x++;
	    ddF_x += 2;
	    f += ddF_x;
	    // These checks avoid double-drawing certain lines, important
	    // for the SSD1306 library which has an INVERT drawing mode.
	    if (x < (y + 1)) {
	      if (corners & 1)
	    	  GFX_DrawFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
	      if (corners & 2)
	    	  GFX_DrawFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
	    }
	    if (y != py) {
	      if (corners & 1)
	    	  GFX_DrawFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
	      if (corners & 2)
	    	  GFX_DrawFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
	      py = y;
	    }
	    px = x;
	  }

}

void GFX_DrawFilledCircle(int x_pos, int y_pos, int r, uint8_t color)
{
	GFX_DrawFastVLine(x_pos, y_pos - r, 2 * r + 1, color);
	GFX_DrawFilledCircleHelper(x_pos, y_pos, r, 3, 0, color);
}

#endif

//*******************************************************************************



//Triangle drawing***************************************************************

#if USE_TRIANGLES == 1

void GFX_DrawTriangle(int x_pos0, int y_pos0, int x_pos1, int y_pos1, int x_pos2, int y_pos2, uint8_t color)
{

	GFX_DrawLine(x_pos0, y_pos0, x_pos1, y_pos1, color);
	GFX_DrawLine(x_pos1, y_pos1, x_pos2, y_pos2, color);
	GFX_DrawLine(x_pos2, y_pos2, x_pos0, y_pos0, color);

}

void GFX_DrawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color)
{
	  int16_t a, b, y, last;

	  // Sort coordinates by Y order (y2 >= y1 >= y0)
	  if (y0 > y1) {
		  _swap_int(y0, y1);
		  _swap_int(x0, x1);
	  }
	  if (y1 > y2) {
		  _swap_int(y2, y1);
		  _swap_int(x2, x1);
	  }
	  if (y0 > y1) {
		  _swap_int(y0, y1);
		  _swap_int(x0, x1);
	  }


	  if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
	    a = b = x0;
	    if (x1 < a)
	      a = x1;
	    else if (x1 > b)
	      b = x1;
	    if (x2 < a)
	      a = x2;
	    else if (x2 > b)
	      b = x2;
	    GFX_DrawFastHLine(a, y0, b - a + 1, color);
	    return;
	  }

	  int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
	          dx12 = x2 - x1, dy12 = y2 - y1;
	  int32_t sa = 0, sb = 0;

	  // For upper part of triangle, find scanline crossings for segments
	  // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	  // is included here (and second loop will be skipped, avoiding a /0
	  // error there), otherwise scanline y1 is skipped here and handled
	  // in the second loop...which also avoids a /0 error here if y0=y1
	  // (flat-topped triangle).
	  if (y1 == y2)
	    last = y1; // Include y1 scanline
	  else
	    last = y1 - 1; // Skip it

	  for (y = y0; y <= last; y++) {
	    a = x0 + sa / dy01;
	    b = x0 + sb / dy02;
	    sa += dx01;
	    sb += dx02;
	    /* longhand:
	    a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if (a > b)
	    	_swap_int(a, b);
	    GFX_DrawFastHLine(a, y, b - a + 1, color);
	  }

	  // For lower part of triangle, find scanline crossings for segments
	  // 0-2 and 1-2.  This loop is skipped if y1=y2.
	  sa = (int32_t)dx12 * (y - y1);
	  sb = (int32_t)dx02 * (y - y0);
	  for (; y <= y2; y++) {
	    a = x1 + sa / dy12;
	    b = x0 + sb / dy02;
	    sa += dx12;
	    sb += dx02;
	    /* longhand:
	    a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
	    b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
	    */
	    if (a > b)
	    	_swap_int(a, b);
	    GFX_DrawFastHLine(a, y, b - a + 1, color);
	  }

}

#endif

#if USE_CHARACTERS == 1

void GFX_DrawChar(int x_pos, int y_pos, char c, uint8_t color, uint8_t background, uint8_t size)
{
	uint8_t line;

	for (int8_t i = 0; i < 6; i++)
	{
		if (i == 5)
		{
			line = 0x00;
		}
		else
		{
			line = font[c * 5 + i];
		}

		for (int8_t j = 0; j < 8; j++)
		{
			if (line & 0x01)
			{
				if (size == 1)
				{
					GFX_DrawPixel(x_pos+i, y_pos+j, color);
				}
				else
				{
					GFX_DrawFiledRectangle(x_pos+(i*size), y_pos+(j*size), size, size, color);
				} 
			}
			else if (background != color)
			{
				if (size == 1)
				{
					GFX_DrawPixel(x_pos+i, y_pos+j, background);
				}
				else
				{
					GFX_DrawFiledRectangle(x_pos+(i*size), y_pos+(j*size), size, size, background);
				} 
			}
			
			line >>= 1;

		}	
	}
}

void GFX_Print(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size)
{
	cursor_x = x_pos;
	cursor_y = y_pos;

	while(*str)
	{
		GFX_DrawChar(cursor_x, cursor_y, *str++, color, background, size);
		cursor_x += size*6;
		if (cursor_x > 127 - (size*6))
		{
			cursor_y += size*8;
			cursor_x = 0;
		}
		
	}

}

void GFX_PrintROLLING(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size)
{
	cursor_x = x_pos;
	cursor_y = y_pos;

	while(*str)
	{
		GFX_DrawChar(cursor_x, cursor_y, *str++, color, background, size);
		cursor_x += size*6;
		if (cursor_x > 127 - (size*6))
		{
			cursor_x = 0;
		}
		
	}

}

void GFX_PrintNOROLLING(int x_pos, int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size)
{
	cursor_x = x_pos;
	cursor_y = y_pos;

	while(*str)
	{
		GFX_DrawChar(cursor_x, cursor_y, *str++, color, background, size);
		cursor_x += size*6;
		
	}

}

void GFX_PrintCentered(int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size)
{
	uint8_t lenght = strlen(str)*6*size; //6 is char width so it will only work for standart font

	cursor_x = (128-lenght)/2;
	cursor_y = y_pos;

	while(*str)
	{
		GFX_DrawChar(cursor_x, cursor_y, *str++, color, background, size);
		cursor_x += size*6;
		if (cursor_x > 127 - (size*6))
		{
			cursor_y += size*8;
			cursor_x = 0;
		}
		
	}

}

void GFX_PrintCenteredAddLenght(int y_pos, char * str, uint8_t color, uint8_t background, uint8_t size, uint8_t add_lenght)
{
	uint8_t lenght = (strlen(str)*6*size)+(add_lenght*6); //6 is char width so it will only work for standart font

	cursor_x = (128-lenght)/2;
	cursor_y = y_pos;

	while(*str)
	{
		GFX_DrawChar(cursor_x, cursor_y, *str++, color, background, size);
		cursor_x += size*6;
		if (cursor_x > 127 - (size*6))
		{
			cursor_y += size*8;
			cursor_x = 0;
		}
		
	}

}

void GFX_PrintInt(int x_pos, int y_pos, int data, uint8_t color, uint8_t background, uint8_t size)
{
	char buf[16];
	GFX_Print(x_pos, y_pos, itoa(data, buf, 10), color, background, size);
}

void GFX_PrintIntHEX(int x_pos, int y_pos, int data, uint8_t color, uint8_t background, uint8_t size)
{
	char buf[16];
	GFX_Print(x_pos, y_pos, itoa(data, buf, 16), color, background, size);
}

#endif