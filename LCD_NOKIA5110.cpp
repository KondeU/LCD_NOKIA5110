#include"LCD_NOKIA5110.h"

C_LCD_NOKIA5110::C_LCD_NOKIA5110
(
	unsigned char byPortRST,
	unsigned char byPortSCE,
	unsigned char byPortDC,
	unsigned char byPortSDIN,
	unsigned char byPortSCLK
	)
{
	this->cl_byPortRST = byPortRST;
	this->cl_byPortSCE = byPortSCE;
	this->cl_byPortDC = byPortDC;
	this->cl_byPortSDIN = byPortSDIN;
	this->cl_byPortSCLK = byPortSCLK;
}

void C_LCD_NOKIA5110::WriteData(unsigned char bySelectDC, unsigned char byData)
{
	digitalWrite(this->cl_byPortDC, bySelectDC);

	digitalWrite(this->cl_byPortSCE, LOW);
	shiftOut(this->cl_byPortSDIN, this->cl_byPortSCLK, MSBFIRST, byData);
	digitalWrite(this->cl_byPortSCE, HIGH);

	return;
}

void C_LCD_NOKIA5110::Init()
{
	//设置所有引脚为输出模式
	pinMode(cl_byPortRST, OUTPUT);
	pinMode(cl_byPortSCE, OUTPUT);
	pinMode(cl_byPortDC, OUTPUT);
	pinMode(cl_byPortSDIN, OUTPUT);
	pinMode(cl_byPortSCLK, OUTPUT);

	//软重置
	this->Reset();

	//设置LCD的相关参数
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x21); //LCD Extended Commands  - 使用扩展命令设置LCD模式
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0xC4); //Set LCD Vop (Contrast) - 设置偏置电压
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x06); //Set Temp coefficent    - 温度校正
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x13); //LCD bias mode 1:48     - 偏置模式
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x20); //LCD Basic Commands     - 使用基本命令
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x0C); //LCD in normal mode     - 正常显示

														   //清空屏幕
	this->Clear();

	//使能归位
	digitalWrite(this->cl_byPortSCE, LOW);

	return;
}

bool C_LCD_NOKIA5110::SetCursor(unsigned char byRow, unsigned char byColumn)
{
	if ((byRow >= this->cl_byRow) || (byColumn >= this->cl_byColumn))
	{
		return false;
	}

	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x40 | byRow);
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, 0x80 | byColumn);

	return true;
}

void C_LCD_NOKIA5110::Clear()
{
	this->SetCursor();

	for (unsigned short wd = 0; wd < (this->cl_byRow * this->cl_byColumn); wd++)
	{
		this->WriteData(LCD_NOKIA5110_SELECTDC_DATA, 0x00);
	}

	this->SetCursor();

	return;
}

bool C_LCD_NOKIA5110::ClearRow(unsigned char byRow)
{
	if (byRow >= this->cl_byRow)
	{
		return false;
	}

	this->SetCursor(byRow);

	for (unsigned char by = 0; by < this->cl_byColumn; by++)
	{
		this->WriteData(LCD_NOKIA5110_SELECTDC_DATA, 0x80 | 0x00);
	}

	this->SetCursor(byRow);

	return true;
}

bool C_LCD_NOKIA5110::Reset()
{
	if (-1 == this->cl_byPortRST)
	{
		return false;
	}

	digitalWrite(this->cl_byPortRST, LOW);
	delayMicroseconds(1);
	digitalWrite(this->cl_byPortRST, HIGH);

	return true;
}

void C_LCD_NOKIA5110::Power(bool bIsOn)
{
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, bIsOn ? 0x20 : 0x24);
	return;
}

void C_LCD_NOKIA5110::Inverse(bool bIsInverse)
{
	this->WriteData(LCD_NOKIA5110_SELECTDC_COMMAND, bIsInverse ? 0x0d : 0x0c);
	return;
}

void C_LCD_NOKIA5110::DrawBitmap(const unsigned char * const pbyData, unsigned char byBgnRow, unsigned char byBgnColumn, unsigned char byCntRow, unsigned char byCntColumn)
{
	for (unsigned char byCurrentRow = byBgnRow; byCurrentRow < byBgnRow + byCntRow; byCurrentRow++)
	{
		this->SetCursor(byCurrentRow, byBgnColumn);
		for (unsigned char byCurrentColumn = byBgnColumn; byCurrentColumn < byBgnColumn + byCntColumn; byCurrentColumn++)
		{
			this->WriteData(LCD_NOKIA5110_SELECTDC_DATA, pbyData[(byCurrentRow - byBgnRow) * byCntColumn + byCurrentColumn]);
		}
	}

	this->SetCursor();

	return;
}

void C_LCD_NOKIA5110::DrawChar(char cData)
{
	for (unsigned char by = 0; by < LCD_NOKIA5110_ASCII_LENGHT; by++)
	{
		this->WriteData(LCD_NOKIA5110_SELECTDC_DATA, arrAscii[cData - 0x20][by]);
	}

	return;
}

void C_LCD_NOKIA5110::DrawString(const char * const szData)
{
	const char * pData = szData;

	while (*pData)
	{
		this->DrawChar(*pData);
		pData++;
	}

	return;
}