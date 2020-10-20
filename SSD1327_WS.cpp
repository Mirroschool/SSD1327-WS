#include "SSD1327_WS.h"
#include "OLED_Font.h"
#include <Wire.h>

SSD1327_WS_OLED::SSD1327_WS_OLED(): m_i2cAddr(0x3D), m_width(128), m_height(128) {}

SSD1327_WS_OLED::SSD1327_WS_OLED(byte i2cAddr): m_i2cAddr(i2cAddr), m_width(128), m_height(128) {}

SSD1327_WS_OLED::SSD1327_WS_OLED(byte i2cAddr, byte width, byte height)
						   : m_i2cAddr(i2cAddr), m_width(width), m_height(height) {}

void SSD1327_WS_OLED::begin()
{
    Wire.setClock(1000000);
    Wire.begin();

    _initDisplay();
    delay(200);
    turnOn();
    clear();
}

void SSD1327_WS_OLED::clear()
{
    byte tempBlackBuf[16];
    memset(tempBlackBuf, 0x00, 16); // Fill buffer with black pixels

    setWorkingArea(0, 0, 63, 127);
    for (byte y = 0; y < 128; y++) {
        for (byte x = 0; x < 4; x++) {
            _i2cWriteDataBlock(tempBlackBuf, 16);
        }
    }
}

int SSD1327_WS_OLED::print(char strToPrint[], int length, bool invert)
{
    int charsPrinted = 0;

    for (int i = 0; i < length; i++) {
        if (printChar(strToPrint[i], invert)) {
            charsPrinted++;
        }
        else {
            return charsPrinted;
        }
    }

    // If printed whole string then just return it's length
    return charsPrinted;
}


bool SSD1327_WS_OLED::printChar(char character, bool invert)
{
    // Handle \n char by adding padding space to end of line
    // So previous page contents get cleared
	if ((character == 0x0A) && (m_cursorY+12 <= 108)){
		while (m_cursorX <= 60){
			_printChar(' ', m_cursorX, m_cursorY, invert);
			m_cursorX += 4;
		}
		m_cursorY += 12;
		m_cursorX = 0;

		return true;
	}

    // If current char is possible to print in current row then print it and increment col position
    if ((m_cursorX <= 60) && (m_cursorY <= 108)) {
        _printChar(character, m_cursorX, m_cursorY, invert);
        m_cursorX += 4;

        return true;
    }
    // Can't print char in current row.
    else {
        // Shift cursor to new row if it's possible
        if (m_cursorY + 12 <= 108) {
            m_cursorX = 0;
            m_cursorY += 12;
            
            // Print char and increment current col position
            _printChar(character, m_cursorX, m_cursorY, invert);
            m_cursorX += 4;

            return true;
        }
        // There is no more space on display
        else {
            return false;
        }
    }
    return false;
}

bool SSD1327_WS_OLED::isPageEnd() const{
    return ((m_cursorY > 108) || (m_cursorX > 60 && m_cursorY == 108));
}

void SSD1327_WS_OLED::home()
{
    setCursor(0, 0);
	setWorkingArea(0, 0, 63, 127);
}

void SSD1327_WS_OLED::setCursor(byte x, byte y)
{
    m_cursorX = x;
    m_cursorY = y;
}

void SSD1327_WS_OLED::setWorkingArea(byte startX, byte startY, byte endX, byte endY)
{
    _i2cWriteTripleByteCommand(0x15, startX, endX); // Set column adress
    _i2cWriteTripleByteCommand(0x75, startY, endY); // Set row adress
}

void SSD1327_WS_OLED::setContrast(byte contastValue)
{
    _i2cWriteDoubleByteCommand(0x81, contastValue);
}

void SSD1327_WS_OLED::normalMode()
{
    _i2cWriteByteCommand(0xA4);
}

void SSD1327_WS_OLED::inverseMode()
{
    _i2cWriteByteCommand(0xA7);
}

void SSD1327_WS_OLED::sleepMode()
{
    _i2cWriteByteCommand(0xAE);
}

void SSD1327_WS_OLED::turnOn()
{
    _i2cWriteByteCommand(0xAF);
}

// Private methods
void SSD1327_WS_OLED::_initDisplay()
{
    _i2cWriteDoubleByteCommand(0xB1, 0xF1); //  Set Phase Length
    _i2cWriteDoubleByteCommand(0xB3, 0x00); //  Set Front Clock Divider / Oscillator Frequency
    _i2cWriteDoubleByteCommand(0xAB, 0x01); //  Enable internal Vdd regulator
    _i2cWriteDoubleByteCommand(0xD5, 0x62); //  Enable second pre-charge + internal VSL
    _i2cWriteDoubleByteCommand(0xB6, 0x0F); //  Second Pre-charge period
    _i2cWriteDoubleByteCommand(0xBE, 0x0F); //  Set COM deselect voltage level
    _i2cWriteDoubleByteCommand(0xBC, 0x08); //  Set pre-charge voltage level
    _i2cWriteDoubleByteCommand(0xA0, 0x51); //  Remap GDDRAM left-to-right and top-to-bottom mapping
    setContrast(80);
}

void SSD1327_WS_OLED::_i2cWrite(byte* pData, int length)
{
    Wire.beginTransmission(m_i2cAddr);
    Wire.write(pData, length);
    Wire.endTransmission();
}

void SSD1327_WS_OLED::_i2cWriteDataBlock(byte* pData, int length)
{
    Wire.beginTransmission(m_i2cAddr);
    Wire.write(0x40);
    Wire.write(pData, length);
    Wire.endTransmission();
}

void SSD1327_WS_OLED::_i2cWriteByteCommand(byte cmd)
{
    byte cmdBuf[2] = {
        OLED_CMD_MODE,
        cmd
    };

    _i2cWrite(cmdBuf, 2);
}

void SSD1327_WS_OLED::_i2cWriteDoubleByteCommand(byte cmd, byte arg1)
{
    byte cmdBuf[3] = {
        OLED_CMD_MODE,
        cmd,
        arg1
    };

    _i2cWrite(cmdBuf, 3);
}

void SSD1327_WS_OLED::_i2cWriteTripleByteCommand(byte cmd, byte arg1, byte arg2)
{
    byte cmdBuf[4] = {
        OLED_CMD_MODE,
        cmd,
        arg1,
        arg2
    };

    _i2cWrite(cmdBuf, 4);
}

void SSD1327_WS_OLED::_printChar(char character, byte x, byte y, bool invert)
{
    byte charBuf[48], tempCharChunkBuf[24];
    fillBufByCharBitmap(charBuf, character, invert);

    for (int i = 0; i < 2; i++) {
        memcpy(tempCharChunkBuf, charBuf + 24 * i, 24);
        _printCharBufferChunk(tempCharChunkBuf, x, y + 6 * i);
    }
}

void SSD1327_WS_OLED::_printCharBufferChunk(byte row[24], byte x, byte y)
{
    setWorkingArea(x, y, x + 3, y + 6);
    _i2cWriteDataBlock(row, 24);
}