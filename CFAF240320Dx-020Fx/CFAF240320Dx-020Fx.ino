//==============================================================================
//
//  CRYSTALFONTZ CFAF240320Dx-020Fx
//
//  Code written for Seeeduino set to 3.3v (important!)
//
//  This code uses the 4-wire SPI mode of the display.
//
//  The controller is a Sitronix ST7789V:
//    https://www.crystalfontz.com/controllers/Sitronix/ST7789V
//
//  Seeeduino, an open-source 3.3v capable Arduino clone.
//    https://www.seeedstudio.com/Seeeduino-V4.2-p-2517.html
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//==============================================================================
// LCD SPI pins and control lines on Seeeduino:
// ARD       | Port  | Display pin |  Function - SPI                          |
//-----------+-------+-------------+------------------------------------------+
// 3.3V      |       |             |  POWER 3.3V                              |
// GND       |       |             |  GROUND                                  |
//-----------+-------+-------------+------------------------------------------+
// D8        | PORTB |  10         | Register select (data/command)  (RS)     |
// D9        | PORTB |  07         | Reset                           (RST)    |
// D10       | PORTB |  08         | Chip select                     (CS)     |
// D11       | PORTB |  16         | Serial input (display)          (SDA)    |
// D13       | PORTB |  09         | Serial clock                    (SCK)    |
//-----------+-------+-------------+------------------------------------------+
// SD card adapter and control lines on Seeeduino:
// ARD       | Port  | Adapter pin |  Function - SPI                          |
//-----------+-------+-------------+------------------------------------------+
// 3.3V      |       |             |  POWER 3.3V                              |
// GND       |       |             |  GROUND                                  |
//-----------+-------+-------------+------------------------------------------+
// D7        | PORTB |  05         |  Chip select 				     (CS)     |
// D11       | PORTD |  06         |  Serial input (adapter)         (SDA)    |
// D12       | PORTD |  07         |  Serial output (adapter)        (SDO)    |
// D13       | PORTD |  03         |  Serial clock                   (SCK)    |
//-----------+-------+-------------+------------------------------------------+
// Interface Selection
// IM2 | IM1 | IM0 | Interface mode  |
//-----+-----+-----+-----------------+
// 1   | 1   | 0   | 4-wire SPI      |
// 1   | 0   | 1   | 3-wire SPI      |
// 0   | 0   | 1   | 16-bit parallel |
// 0   | 0   | 0   | 8-bit parallel  |
//-----+-----+-----+-----------------+
//==============================================================================
// demo selection
#define WAIT_TIME		2000
#define DEMO_FONT		1
#define DEMO_LINES		1
#define DEMO_CIRCLES	1
#define DEMO_EXPANDING	1
#define DEMO_CHECKER	1
#define DEMO_BMPIMAGES	1 // enabling also enables use of SD card

// BMP images must be save in the root directory of the SD card.
// they must be exactly 240x320 pixels in size, and in 24-bit colour (3 bytes per pixel)

// DISPLAY SPI FREQUENCY
// try lowering this if you have display init or corruption issues
// default is 20Mhz
#define SPI_FREQ 20000000
//==============================================================================
// define includes
#include <Arduino.h>
#include <SPI.h>
#include "font-ascii-12x16.h"

#if DEMO_BMPIMAGES
#include <SD.h>
// this uses the Adafruit SD Arduino library
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.cpp
// C:\Program Files (x86)\Arduino\libraries\SD\src\SD.h
#endif
//==============================================================================
// define pin connections
#define RS (8)
#define RST (9)
#define CS (10)
#define MOSI (11)
#define CLK (13) 

// define set and clear functions for each pin
#define RS_MASK (0x01) 
#define CLR_RS (PORTB &= ~(RS_MASK))
#define SET_RS (PORTB |= (RS_MASK))
#define RESET_MASK (0x02) 
#define CLR_RESET (PORTB &= ~(RESET_MASK))
#define SET_RESET (PORTB |= (RESET_MASK))
#define CS_MASK (0x04) 
#define CLR_CS (PORTB &= ~(CS_MASK))
#define SET_CS (PORTB |= (CS_MASK))
#define MOSI_MASK (0x08) 
#define CLR_MOSI (PORTB &= ~(MOSI_MASK))
#define SET_MOSI (PORTB |= (MOSI_MASK))
#define MISO_MASK (0x10) 
#define CLR_MISO (PORTB &= ~(MISO_MASK))
#define SET_MISO (PORTB |= (MISO_MASK))
#define CLK_MASK (0x20) 
#define CLR_CLK (PORTB &= ~(CLK_MASK))
#define SET_CLK (PORTB |= (CLK_MASK))
//==============================================================================
// ST7789 defines
#define ST7789_11_SLPOUT (0x11) 
#define ST7789_21_INVON (0x21) 
#define ST7789_29_DISPON (0x29) 
#define ST7789_2A_CASET (0x2A) 
#define ST7789_2B_RASET (0x2B) 
#define ST7789_2C_RAMWR (0x2C) 
#define ST7789_36_MADCTL (0x36) 
#define ST7789_3A_COLMOD (0x3A) 
#define ST7789_B2_PORCTRL (0xB2) 
#define ST7789_B7_GCTRL (0xB7) 
#define ST7789_BB_VCOMS (0xBB) 
#define ST7789_C0_LCMCTRL (0xC0) 
#define ST7789_C2_VDVVRHEN (0xC2) 
#define ST7789_C3_VRHS (0xC3) 
#define ST7789_C4_VDVSET (0xC4) 
#define ST7789_C6_FRCTR2 (0xC6) 
#define ST7789_D0_PWCTRL1 (0xD0)
#define ST7789_E0_PVGAMCTRL (0xE0) 
#define ST7789_E1_NVGAMCTRL (0xE1) 
//==============================================================================
// SPI commands
void SPI_sendCommand(uint8_t cmd)
{
	CLR_CS; // select the LCD controller
	CLR_RS; // select the command register
	SPI.transfer(cmd); // send the command via SPI:
	SET_CS; // de-select the LCD controller
}

void SPI_sendData(uint8_t data)
{
	CLR_CS; // select the LCD controller
	SET_RS; // select the data register
	SPI.transfer(data); //send the data via SPI:
	SET_CS; // de-select the LCD controller
}
//==============================================================================
// display initialization sequence
void initialize_LCD(void)
{
	// perform a hardware reset on the display
	CLR_RESET;
	delay(1); 
	SET_RESET;
	delay(150); 

	SPI_sendCommand(ST7789_11_SLPOUT); // turn off sleep mode
	delay(120); // it is required to wait this much time before issuing another command
	
	SPI_sendCommand(ST7789_36_MADCTL); // memory data access control
	SPI_sendData(0x08 | 0x80); // set the RGB vs BGR order to match a windows 24-bit BMP

	SPI_sendCommand(ST7789_3A_COLMOD); // interface pixel format
	SPI_sendData(0x06); // 18-bit/pixel

	SPI_sendCommand(ST7789_B2_PORCTRL); // porch control
	SPI_sendData(0x0C); 
	SPI_sendData(0x0C); 
	SPI_sendData(0x00); 
	SPI_sendData(0x33); 					
	SPI_sendData(0x33); 
						
	SPI_sendCommand(ST7789_B7_GCTRL); // gate control
	SPI_sendData(0x35);

	SPI_sendCommand(ST7789_BB_VCOMS); // VCOM settings
	SPI_sendData(0x2B);

	SPI_sendCommand(ST7789_C0_LCMCTRL); // LCM control
	SPI_sendData(0x2C);

	SPI_sendCommand(ST7789_C2_VDVVRHEN); // VDV and VRH command enable
	SPI_sendData(0x01); 						
	SPI_sendData(0xFF);
	
	SPI_sendCommand(ST7789_C3_VRHS); // VRH set
	SPI_sendData(0x20);

	SPI_sendCommand(ST7789_C4_VDVSET); // VDV set
	SPI_sendData(0x20);

	
	SPI_sendCommand(ST7789_C6_FRCTR2); // frame rate control in normal mode
	SPI_sendData(0x0F); // 60 HZ

	SPI_sendCommand(ST7789_D0_PWCTRL1); // power control 1
	SPI_sendData(0xA4); 
	SPI_sendData(0xA1); 

	SPI_sendCommand(ST7789_E0_PVGAMCTRL); // positive voltage gamma control
	SPI_sendData(0xD0);
	SPI_sendData(0xCA);
	SPI_sendData(0x0E);
	SPI_sendData(0x08);
	SPI_sendData(0x09);
	SPI_sendData(0x07);
	SPI_sendData(0x2D);
	SPI_sendData(0x3B);
	SPI_sendData(0x3D);
	SPI_sendData(0x34);
	SPI_sendData(0x0A);
	SPI_sendData(0x0A);
	SPI_sendData(0x1B);
	SPI_sendData(0x28);

	SPI_sendCommand(ST7789_E1_NVGAMCTRL); // negative voltage gamma control
	SPI_sendData(0xD0);
	SPI_sendData(0xCA);
	SPI_sendData(0x0F);
	SPI_sendData(0x08);
	SPI_sendData(0x08);
	SPI_sendData(0x07);
	SPI_sendData(0x2E);
	SPI_sendData(0x5C);
	SPI_sendData(0x40);
	SPI_sendData(0x34);
	SPI_sendData(0x09);
	SPI_sendData(0x0B);
	SPI_sendData(0x1B);
	SPI_sendData(0x28);

	SPI_sendCommand(ST7789_21_INVON); // display inversion on

	SPI_sendCommand(ST7789_2A_CASET); // column address set
	SPI_sendData(0x00); // start MSB start = 0
	SPI_sendData(0x00); // start LSB
	SPI_sendData(0x00); // end MSB End = 249
	SPI_sendData(0xEF); // end LSB

	SPI_sendCommand(ST7789_2B_RASET); // row address set
	SPI_sendData(0x00); // start MSB start = 0
	SPI_sendData(0x00); // start LSB
	SPI_sendData(0x01); // end MSB end = 319
	SPI_sendData(0x3F); // end LSB

	SPI_sendCommand(ST7789_29_DISPON); // display on
	delay(1);
}
//==============================================================================
// set position to write on display
void Set_LCD_for_write_at_X_Y(uint16_t x, uint16_t y)
{
	SPI_sendCommand(ST7789_2A_CASET); // column address set function
	SPI_sendData(x >> 8);	
	SPI_sendData(x & 0x00FF); // the x axis will increment according to the variable passed through
	SPI_sendData(0);		  
	SPI_sendData(240);		  
	 
	SPI_sendCommand(ST7789_2B_RASET); // row address set function
	// use 1st quadrant coordinates: 0,0 is lower left, 239,319 is upper right
	y = 319 - y; // this will invert the display direction
	SPI_sendData(y >> 8);	 
	SPI_sendData(y & 0x00FF); // the y axis will increment according to the variable passed through
	SPI_sendData(0x01);		  
	SPI_sendData(0x3F);		  
	
	SPI_sendCommand(ST7789_2C_RAMWR); // write the current position to RAM 
}
//==============================================================================
// fill LCD functions
#if (0) // a simple fill LCD function
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
{
	uint32_t
		i;
	Set_LCD_for_write_at_X_Y(0, 319); // set the write position

	// fill display with a given RGB value
	for (i = 0; i < (320UL * 240UL); i++)
	{
		
		SPI_sendData(B); // blue
		SPI_sendData(G); // green
		SPI_sendData(R); // red
	}
}
#else // faster, bigger (6 bytes)
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
{
	uint32_t i;
	Set_LCD_for_write_at_X_Y(0, 319);

	// select the LCD controller
	CLR_CS;
	// select the LCD's data register
	SET_RS;

	// fill display with a given RGB value
	for (i = 0; i < (320UL * 240UL); i++)
	{
		// account for the order of pixels
		SPI.transfer(B); // blue
		SPI.transfer(G); // green
		SPI.transfer(R); // red
	}
	// deselect the LCD controller
	SET_CS;
}
#endif
//==============================================================================
// place pixel functions  
#if (0) // simple
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
{
	Set_LCD_for_write_at_X_Y(x, y); // set the position to pass a pixel
	// write the single pixel's worth of data
	SPI_sendData(B); // blue
	SPI_sendData(G); // green
	SPI_sendData(R); // red
}
#else //faster, bigger (78 bytes)
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
{
	// select the LCD controller
	CLR_CS;
	// implement the Set_LCD_for_write_at_X_Y function here
	SPI_sendCommand(ST7789_2A_CASET); // column address set function
	SPI_sendData(x >> 8);	 
	SPI_sendData(x & 0x00FF); // the x axis will increment according to the variable passed through
	SPI_sendData(0);		 
	SPI_sendData(240);		 
		
	SPI_sendCommand(ST7789_2B_RASET); // row address set function
    // use 1st quadrant coordinates: 0,0 is lower left, 239,319 is upper right
	y = 319 - y;
	SPI_sendData(y >> 8);	 
	SPI_sendData(y & 0x00FF); // the x axis will increment according to the variable passed through
	SPI_sendData(0x01);		 
	SPI_sendData(0x3F);		 
	
	SPI_sendCommand(ST7789_2C_RAMWR); // write the current position to RAM 

	// write the single pixel's worth of data
	SPI_sendData(B); // blue
	SPI_sendData(G); // green
	SPI_sendData(R); // red
	// deselect the LCD controller
	SET_CS;
}
#endif
//==============================================================================
// circle demo from: http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void LCD_Circle(uint16_t x0, uint16_t y0, uint16_t radius,
				uint16_t R, uint16_t G, uint16_t B)
{
	uint16_t x = radius;
	uint16_t y = 0;
	int16_t radiusError = 1 - (int16_t)x;

	while (x >= y)
	{
		// 11 o'clock
		Put_Pixel(x0 - y, y0 + x, R, G, B);
		// 1 o'clock
		Put_Pixel(x0 + y, y0 + x, R, G, B);
		// 10 o'clock
		Put_Pixel(x0 - x, y0 + y, R, G, B);
		// 2 o'clock
		Put_Pixel(x0 + x, y0 + y, R, G, B);
		// 8 o'clock
		Put_Pixel(x0 - x, y0 - y, R, G, B);
		// 4 o'clock
		Put_Pixel(x0 + x, y0 - y, R, G, B);
		// 7 o'clock
		Put_Pixel(x0 - y, y0 - x, R, G, B);
		// 5 o'clock
		Put_Pixel(x0 + y, y0 - x, R, G, B);

		y++;
		if (radiusError < 0)
			radiusError += (int16_t)(2 * y + 1);
		else
		{
			x--;
			radiusError += 2 * (((int16_t)y - (int16_t)x) + 1);
		}
	}
}
//==============================================================================
#define mSwap(a, b, t) \
	{                  \
		t = a;         \
		a = b;         \
		b = t;         \
	}
//==============================================================================
// fast horizontal line demo
void Fast_Horizontal_Line(uint16_t x0, uint16_t y, uint16_t x1,
						  uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t temp;
	if (x1 < x0)
		mSwap(x0, x1, temp);
	Set_LCD_for_write_at_X_Y(x0, y);
	while (x0 <= x1)
	{
		// write the single pixel's worth of data
		SPI_sendData(b); // blue
		SPI_sendData(g); // green
		SPI_sendData(r); // red
		x0++;
	}
}
//==============================================================================
// line demo from: http://rosettacode.org/wiki/Bitmap/Bresenham's_line_algorithm#C
void LCD_Line(uint16_t x0, uint16_t y0,
			  uint16_t x1, uint16_t y1,
			  uint8_t r, uint8_t g, uint8_t b)
{
	int16_t dx;
	int16_t sx;
	int16_t dy;
	int16_t sy;
	int16_t err;
	int16_t e2;

	// general case
	if (y0 != y1)
	{
		dx = abs((int16_t)x1 - (int16_t)x0);
		sx = x0 < x1 ? 1 : -1;
		dy = abs((int16_t)y1 - (int16_t)y0);
		sy = y0 < y1 ? 1 : -1;
		err = (dx > dy ? dx : -dy) / 2;

		for (;;)
		{
			Put_Pixel(x0, y0, r, g, b);
			if ((x0 == x1) && (y0 == y1))
				break;
			e2 = err;
			if (e2 > -dx)
			{
				err -= dy;
				x0 = (uint16_t)((int16_t)x0 + sx);
			}
			if (e2 < dy)
			{
				err += dx;
				y0 = (uint16_t)((int16_t)y0 + sy);
			}
		}
	}
	else
	{
		// pptimized for LCD
		Fast_Horizontal_Line(x0, y0, x1, r, g, b);
	}
}
//==============================================================================
// this function transfers data, in one stream. slightly
// optimized to do index operations during SPI transfers.
void SPI_send_pixels(uint8_t byte_count, uint8_t *data_ptr)
{
	uint8_t subpixel;

	// select the LCD's data register
	SET_RS;
	// select the LCD controller
	CLR_CS;

	// load the first byte
	subpixel = *data_ptr;

	while (byte_count)
	{
		// send the byte out.
		SPDR = subpixel;
		// do something that can happen while transmitting
		data_ptr++; // point to next byte
		// load the next byte
		subpixel = *data_ptr;
		// count this byte
		byte_count--;
		// now that we have done all we can do, wait for the transfer to finish.
		while (!(SPSR & _BV(SPIF)));
	}
	// deselect the LCD controller
	SET_CS;
}
//==============================================================================
// bmp demo for images on sd card
#if DEMO_BMPIMAGES
#define BMP_FLIP	1 // enabling this draws BMP images the right way up
void show_BMPs_in_root(void)
{
	File root_dir;
	root_dir = SD.open("/");
	if (0 == root_dir)
	{
		Serial.println("show_BMPs_in_root: Can't open \"root\"");
		return;
	}
	File bmp_file;

	while (1)
	{

		bmp_file = root_dir.openNextFile();
		if (0 == bmp_file)
		{
			// no more files, break out of while()
			// root_dir will be closed below.
			break;
		}
		// skip directories (what about volume name?)
		if (0 == bmp_file.isDirectory())
		{
			// the file name must include ".BMP"
			if (0 != strstr(bmp_file.name(), ".BMP"))
			{
				// the BMP must be exactly 230454 long
				// (this is correct for 240x320, 24-bit)
				if (230454 == bmp_file.size())
				{
					// jump over BMP header. BMP must be 240x320 24-bit
					bmp_file.seek(54);

					// since we are limited in memory, break the line up from
					// 240*3 = 720 bytes into three chunks of 80 pixels
					// each 80*3 = 240 bytes.
					// making this static speeds it up slightly (10ms)
					// reduces flash size by 114 bytes, and uses 240 bytes.
					static uint8_t third_of_a_line[80 * 3];
					for (uint16_t line = 0; line < 320; line++)
					{
						// set the LCD to the left of this line. BMPs store data
						// lowest line first -- bottom up.
#if BMP_FLIP						
						Set_LCD_for_write_at_X_Y(0, 319-line);
#else
						Set_LCD_for_write_at_X_Y(0, line);
#endif						
						for (uint8_t line_section = 0; line_section < 3; line_section++)
						{
							// get a third of the line
							bmp_file.read(third_of_a_line, 80 * 3);
							// now write this third to the TFT, doing the BGR -> RGB
							// color fixup interlaced with the SPI transfers.
							SPI_send_pixels(80 * 3, third_of_a_line);
						}
					}
			}
			}
		}
		//Release the BMP file handle
		bmp_file.close();

		delay(1000);
	}
	//Release the root directory file handle
	root_dir.close();
}
#endif
//==============================================================================
// character demo
void F12x16_DrawChar(uint16_t x, uint16_t y, char c)
{
	uint8_t state;
	uint8_t xc, yc;

	// draw character to frame buffer
	if (c > FONT_ASCII_12X16_BASE - 1)
	{
		c -= FONT_ASCII_12X16_BASE;
		if (c > FONT_ASCII_12X16_CHARNUM)
			c = ' ';

		for (yc = 0; yc < FONT_ASCII_12X16_HEIGHT; yc++)
		{
			// 12x16 is two bytes wide
			uint16_t w;
			uint8_t b;
			b = pgm_read_byte(&Font_ASCII_12X16[((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2]);
			w = b << 8;
			b = pgm_read_byte(&Font_ASCII_12X16[(((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2) + 1]);
			w |= b;

//			w = (Font_ASCII_12X16[((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2] << 8) |
//				Font_ASCII_12X16[(((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2) + 1];

			for (xc = 0; xc < FONT_ASCII_12X16_WIDTH; xc++)
			{
				state = (w >> (15 - xc)) & 1 ? 0xFF : 0x00;
				Put_Pixel(x+xc, y+yc, state, state, state);
			}
		}
	}
}
//==============================================================================
#define F12CENTREX(a) ((240-((a)*13))/2) // center text
void F12x16_DrawString(uint16_t x, uint16_t y, const char *text)
{
	uint8_t c;

	c = 0;
	while (text[c] != 0)
	{
		x += FONT_ASCII_12X16_WIDTH;
		F12x16_DrawChar(x, y, text[c]);
		c++;
	}
}
//==============================================================================
// setup function
void setup()
{
	// debug console
	Serial.begin(115200);
	Serial.println("setup()");

	// set up port directions
	DDRB = 0x3F;
	DDRC = 0x0F;
	PORTB = 0x00;
	PORTC = 0x00;

	// drive the ports to a reasonable starting state.
	CLR_RESET;
	CLR_RS;
	SET_CS;
	CLR_MOSI;
	CLR_CLK;

#if DEMO_BMPIMAGES
	// initialize the SD card (if used)
	if (!SD.begin(7)) // use pin 7 for SS/CS.
		Serial.println("SD failed to initialize");
#endif

	// initialize SPI
	SPI.begin();
	SPI.beginTransaction(SPISettings(SPI_FREQ, MSBFIRST, SPI_MODE0));

	// initialize the LCD controller
	initialize_LCD();
	
}
//==============================================================================
// main loop
void loop()
{
	uint8_t i;
	uint16_t x;
	uint16_t sub_x;
	uint16_t y;
	uint16_t sub_y;
	uint8_t r;
	uint8_t g;
	uint8_t b;

#if DEMO_FONT
	// text example
	Serial.println("fill LCD");
	Fill_LCD(0x00, 0x00, 0x00);
	Serial.println("font example");
	
	F12x16_DrawString(F12CENTREX(12),  5+(18*0),	"CRYSTALFONTZ");
	F12x16_DrawString(F12CENTREX(18), 10+(18*1),	"CFAF240320Dx-020Fx");
	F12x16_DrawString(F12CENTREX(19), 10+(18*3),	"240x320 TFT Display");
	F12x16_DrawString(F12CENTREX(16), 10+(18*4),	"ST7789V Controller");
	F12x16_DrawString(F12CENTREX(14), 10+(18*8),	"Seeeduino 3.3V");
	F12x16_DrawString(F12CENTREX(19), 10+(18*9),	"Simple Display Demo");
	delay(WAIT_TIME);
#endif

#if DEMO_LINES
	// cheesy lines
	Serial.println("fill LCD");
	Fill_LCD(0x20, 0x20, 0x20);
	Serial.println("cheesy lines");
	r = 0xff; g = 0x00; b = 0x80;
	for (x = 0; x < 240; x++)
		LCD_Line(120, 160, x, 0, r++, g--, b += 2);
	for (y = 0; y < 320; y++)
		LCD_Line(120, 160, 239, y, r++, g += 4, b += 2);
	for (x = 239; 0 != x; x--)
		LCD_Line(120, 160, x, 319, r -= 3, g -= 2, b -= 1);
	for (y = 319; 0 != y; y--)
		LCD_Line(120, 160, 0, y, r + -3, g--, b++);
	delay(WAIT_TIME);
#endif

#if DEMO_CIRCLES
	// fill display with a given RGB value
	Serial.println("fill LCD");
	Fill_LCD(0x00, 0x00, 0xFF);
	Serial.println("circles");
	// draw a cyan circle
	LCD_Circle(120, 120 + 40, 119, 0x00, 0xFF, 0xFF);
	// draw a white circle
	LCD_Circle(120, 120 + 40, 40, 0xFF, 0xFF, 0xFF);
	// draw a green circle
	LCD_Circle(40, 120 + 40, 37, 0x00, 0xFF, 0x00);
	// draw a red circle
	LCD_Circle(200, 120 + 40, 37, 0xFF, 0x00, 0x00);
	// draw a purple circle
	LCD_Circle(120, 200 + 40, 32, 0xFF, 0x00, 0xFF);
	// draw a orange circle
	LCD_Circle(120, 40 + 40, 28, 0xFF, 0xA5, 0x00);
	delay(WAIT_TIME);
#endif

#if DEMO_EXPANDING
	// expanding circles
	Serial.println("fill LCD");
	Fill_LCD(0x00, 0x00, 0x00);

	Serial.println("expanding circles");
	for (i = 2; i < 120; i += 2)
		LCD_Circle(i + 2, 160, i, i << 2, 0xff - (i << 2), 0xFF);
	delay(WAIT_TIME);
#endif

#if DEMO_CHECKER
	// write a 16x16 checkerboard
	Serial.println("Checkerboard");
	for (x = 0; x < (240 / 16); x++)
		for (y = 0; y < (320 / 16); y++)
			for (sub_x = 0; sub_x <= 15; sub_x++)
				for (sub_y = 0; sub_y <= 15; sub_y++)
					if (((x & 0x01) && !(y & 0x01)) || (!(x & 0x01) && (y & 0x01)))
						Put_Pixel((x << 4) + sub_x, (y << 4) + sub_y, 0x00, 0x00, 0x00);
					else
						Put_Pixel((x << 4) + sub_x, (y << 4) + sub_y, 0xFF, 0xFF - (x << 4), 0xFF - (y << 4));
	delay(WAIT_TIME);
#endif

#if DEMO_BMPIMAGES
	// slideshow of bitmap files on uSD card.
	Serial.println("BMPs");
	show_BMPs_in_root();
#endif
}
//==============================================================================
