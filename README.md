# CFAF240320Dx-020Fx Demonstration Code

This is Arduino sample code for the CFAF240320Dx-020Fx family of displays. These displays are 2.0" Blanview TFTs which use the [Sitronix ST7789V](https://www.crystalfontz.com/controllers/Sitronix/ST7789V/) LCD controller. This LCD controller is capable of 8/9/16/18 bit parallel, 3 or 4-Wire SPI and DOTCLK RGB. In order to interface with the displays, a 45 pin ZIF to header breakout board will be required. The Blanview technology incorporated in this display makes it an ideal candidate for use when sunlight readability is an important requirement.
 
## Connection Guide
```
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
(Micro SD card connection is optional)
```
## Display information
Here are links to our active displays:\
CFAF240320D0-020FN\
CFAF240320D0-020FR

For more information about other TFT offerings, please see our full list [here](https://www.crystalfontz.com/c/tft-lcd-displays/25).