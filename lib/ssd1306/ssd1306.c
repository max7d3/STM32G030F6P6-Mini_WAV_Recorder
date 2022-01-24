/* SSD1306 OLED Library. It was inspired by many sources until I get it "working".
*  Sometimes after initialization display is bugged, I don't know why and it 
*  seems random. It likly to happend if delay between init and first data is long
*  In 128x32 for example 2 down pages are on top and 2 top pages are on down
*  side of display.
*/ 

#include "stm32g0xx.h"
#include "string.h"

#include "i2c.h"


#include "ssd1306.h"

#ifdef USE_FULLBUFFER
uint8_t ssd1306_buf[(SSD1306_WIDTH*SSD1306_HEIGHT)/8] = {}; // display bufor
#endif

#ifdef USE_PAGEBUFFER
uint8_t ssd1306_buf[128] = {}; // display bufor
uint8_t page = 0;
#endif


//send command
void ssd1306_cmd(uint8_t cmd)
{
	uint8_t temp[] = {cmd};
	I2C2_Write(SSD1306_ADR, 0x00, temp, sizeof(temp));
}

void ssd1306_cmd_2arg(uint8_t cmd, uint8_t arg_a, uint8_t arg_b)
{
	uint8_t temp[] = {cmd,arg_a,arg_b};
	I2C2_Write(SSD1306_ADR, 0x00, temp, sizeof(temp));
}

//send data
void ssd1306_data(uint8_t data)
{
	uint8_t temp[] = {data};
	I2C2_Write(SSD1306_ADR, 0x40, temp, sizeof(temp));
}

//initialization command set
void ssd1306_init(uint8_t vcc, uint8_t refresh)
{

	ssd1306_cmd(SSD1306_DISPLAYOFF);
	ssd1306_cmd(SSD1306_SETDISPLAYCLOCKDIV);
	ssd1306_cmd(refresh);

	ssd1306_cmd(SSD1306_SETDISPLAYOFFSET);
	ssd1306_cmd(0x00);
	ssd1306_cmd(SSD1306_SETSTARTLINE | 0x0);
	ssd1306_cmd(SSD1306_CHARGEPUMP);

	if (vcc == SSD1306_EXTERNALVCC) ssd1306_cmd(0x10);
	else ssd1306_cmd(0x14);

	ssd1306_cmd(SSD1306_MEMORYMODE);
	ssd1306_cmd(0x00);
	ssd1306_cmd(SSD1306_SEGREMAP | 0x1);
	ssd1306_cmd(SSD1306_COMSCANDEC);

	ssd1306_cmd(SSD1306_SETCONTRAST);

	if (vcc == SSD1306_EXTERNALVCC) ssd1306_cmd(0x9F);
	else ssd1306_cmd(0xCF);

	ssd1306_cmd(SSD1306_SETPRECHARGE);


	ssd1306_cmd(SSD1306_SETMULTIPLEX);
	ssd1306_cmd(SSD1306_HEIGHT - 1); //change if resolution is other

	ssd1306_cmd(SSD1306_SETCOMPINS);
	ssd1306_cmd(0x02); //change if resolution is other


	ssd1306_cmd_2arg(SSD1306_PAGEADDR,4,7);


	ssd1306_cmd(SSD1306_DISPLAYALLON_RESUME);
	ssd1306_cmd(SSD1306_NORMALDISPLAY);
	ssd1306_cmd(SSD1306_DISPLAYON);

}


//transfer bufor to dispaly
#ifdef USE_FULLBUFFER
void ssd1306_display(void)
{
	ssd1306_cmd(SSD1306_SETLOWCOLUMN | 0x00);
	ssd1306_cmd(SSD1306_SETHIGHCOLUMN | 0x00);
	ssd1306_cmd(SSD1306_SETSTARTLINE | 0x00);

	//I2C1_DMA_Write(SSD1306_ADR, 0x40, ssd1306_buf, (SSD1306_WIDTH*SSD1306_HEIGHT)/8);
	//I2C2_Write(SSD1306_ADR, 0x40, ssd1306_buf, (SSD1306_WIDTH*SSD1306_HEIGHT)/8);
	I2C2_Write_IRQ(SSD1306_ADR, 0x40, ssd1306_buf, (SSD1306_WIDTH*SSD1306_HEIGHT)/8);
	
}
#endif

#ifdef USE_PAGEBUFFER
void ssd1306_display(void)
{
	ssd1306_cmd(SSD1306_SETLOWCOLUMN | 0x00);
	ssd1306_cmd(SSD1306_SETHIGHCOLUMN | 0x00);
	ssd1306_cmd(SSD1306_SETSTARTLINE | 0x00);

	I2C_Write(SSD1306_ADR, 0x40, ssd1306_buf, 128);

	ssd1306_cls_page();
}
#endif

#ifdef USE_FULLBUFFER
void ssd1306_cls(void)
{
	memset(ssd1306_buf, 0x00, (SSD1306_WIDTH*SSD1306_HEIGHT)/8);
}
#endif

#ifdef USE_PAGEBUFFER
void ssd1306_cls_page(void)
{
	//while(!I2C_End); IF intterupt i2c
	memset(ssd1306_buf, 0x00, 128);
	
}

void ssd1306_cls(void)
{
	memset(ssd1306_buf, 0x00, 128);
	for (uint8_t i = 0; i < 8; i++)
	{
		ssd1306_display();
	}
	
}
#endif

#ifdef USE_FULLBUFFER
void ssd1306_set_pixel(int x, int y, uint8_t color)
{

	if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 0) || (y >= SSD1306_HEIGHT)) return;

	if(color) ssd1306_buf[x+ (y/8)*SSD1306_WIDTH] |= (1<<(y%8));
	else 	  ssd1306_buf[x+ (y/8)*SSD1306_WIDTH] &= ~(1<<(y%8));

}
#endif

#ifdef USE_PAGEBUFFER
void ssd1306_set_pixel(int x, int y, uint8_t color)
{
	switch (page)
	{
	case 0:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 0) || (y >= 8)) return;


		break;

	case 1:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 8) || (y >= 16)) return;


		break;

	case 2:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 16) || (y >= 24)) return;


		break;

	case 3:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 24) || (y >= 32)) return;


		break;

	case 4:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 32) || (y >= 40)) return;


		break;

	case 5:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 40) || (y >= 48)) return;


		break;

	case 6:
		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 48) || (y >= 56)) return;


		break;

	case 7:

		if ((x < 0) || (x >= SSD1306_WIDTH) || (y < 56) || (y >= 64)) return;

		break;
	
	default:
		break;
	}

	if(color) ssd1306_buf[x] |= (1<<(y%8));
	else 	  ssd1306_buf[x] &= ~(1<<(y%8));

} 
#endif

void ssd1306_set_contrast(uint8_t contrast)
{
	ssd1306_cmd(SSD1306_SETCONTRAST);
	ssd1306_cmd(contrast);
}


void ssd1306_bitmap(int x, int y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color)
{

	int i, j, byteWidth = (w + 7) / 8;

	for(j=0; j<h; j++)
	{
		for(i=0; i<w; i++)
		{
			if(bitmap[j * byteWidth + i / 8] & (128 >> (i & 7)))
			{
				ssd1306_set_pixel(x+i, y+j, color);
			}

		}
	}

}


 