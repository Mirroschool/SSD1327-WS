#ifndef SSD1327_WS_H
#define SSD1327_WS_H

#include <Arduino.h>

#define OLED_CMD_MODE 0x00	// DC bit is logic 0 -> subsequent bytes are interpreted as commands
#define OLED_DATA_MODE 0x40	// DC bit is logic 1 -> subsequent bytes are written to GDDRAM


// I2C Waveshare 1.5 inch OLED display on SDD1327
class SSD1327_WS_OLED{
	public:
		SSD1327_WS_OLED();
		SSD1327_WS_OLED(byte i2cAddr);
		SSD1327_WS_OLED(byte i2cAddr, byte width, byte height);

		void begin();
		void clear();

		void normalMode();
		void inverseMode();
		void sleepMode();
		void turnOn();

		void setContrast(byte contrastValue);

		int print(char strToPrint[], int length, bool invert = false);
		bool printChar(char character, bool invert = false);

		bool isPageEnd() const;

		void home();
		void setCursor(byte x, byte y);
		void setWorkingArea(byte startX, byte startY, byte endX, byte endY);

	private:
		byte m_i2cAddr, m_width, m_height;
		byte m_cursorX, m_cursorY;

		void _initDisplay();

		void _printChar(char character, byte x, byte y, bool invert = false);
		void _printCharBufferChunk(byte row[4], byte x, byte y);

		void _i2cWrite(byte *pData, int length);
		void _i2cWriteDataBlock(byte *pData, int length);
		void _i2cWriteByteCommand(byte cmd);
		void _i2cWriteDoubleByteCommand(byte cmd, byte arg1);
		void _i2cWriteTripleByteCommand(byte cmd, byte arg1, byte arg2);
};
#endif