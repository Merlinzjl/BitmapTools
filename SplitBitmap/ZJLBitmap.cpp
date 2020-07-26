#include "stdafx.h"
#include "ZJLBitmap.h"
#include <fstream>


ZJLBitmap::ZJLBitmap()
{
	m_pPalette = nullptr;
	m_pData = nullptr;
}


ZJLBitmap::~ZJLBitmap()
{
	if (m_pPalette != nullptr)
	{
		delete[] m_pPalette;
		m_pPalette = nullptr;
	}

	if (m_pData != nullptr)
	{
		delete[] m_pData;
		m_pData = nullptr;
	}
}


bool ZJLBitmap::InitBitmap(CString & bitmapFilePath)
{
	std::ifstream istream(bitmapFilePath, std::ios::binary);
	if (!istream.is_open())
	{
		return false;
	}

	int index = bitmapFilePath.ReverseFind('\\');
	if (index < 0)
	{
		index = bitmapFilePath.ReverseFind('/');
	}
	int dotIndex = bitmapFilePath.Find('.');
	m_fileName = bitmapFilePath.Mid(index + 1, dotIndex - index - 1);
	m_directory = bitmapFilePath.Mid(0, index+1);
	m_bitmapFileHeader = { 0 };
	m_bitmapInfoHeader = { 0 };
	int i = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*2;
	istream.read((char *)&m_bitmapFileHeader, sizeof(BITMAPFILEHEADER));
	if (m_bitmapFileHeader.bfType != 0x4d42)//必须为"BM"，小端模式，即0x4d42 才是Windows位图文件
	{
		istream.close();
		return false;
	}

	istream.read((char *)&m_bitmapInfoHeader, sizeof(BITMAPINFOHEADER));
	
	if (m_bitmapInfoHeader.biBitCount <= 8)
	{
		int size = 1 << m_bitmapInfoHeader.biBitCount;
		m_pPalette = new RGBQUAD[1 << m_bitmapInfoHeader.biBitCount];
		for (int i = 0; i < size; ++i)
		{
			istream.read((char*)&m_pPalette[i], sizeof(RGBQUAD));//ARGB,a =0时表示全透明， a=ff表示不透明
		}
	}

	LONG lineBytes = (m_bitmapInfoHeader.biWidth * m_bitmapInfoHeader.biBitCount + 31) / 32 * 4;
	m_pData = new byte[lineBytes * m_bitmapInfoHeader.biHeight];
	istream.read((char*)m_pData, lineBytes * m_bitmapInfoHeader.biHeight);

	istream.close();
	return true;
}

bool ZJLBitmap::SplitBitmapByRowAndCol(int row, int col)
{
	if (row <= 0)
	{
		row = 1;
	}
	if (row > m_bitmapInfoHeader.biHeight)
	{
		row = m_bitmapInfoHeader.biHeight;
	}

	if (col <= 0)
	{
		col = 1;
	}
	if (col > m_bitmapInfoHeader.biWidth)
	{
		col = m_bitmapInfoHeader.biWidth;
	}

	int lineBytes = (m_bitmapInfoHeader.biWidth * m_bitmapInfoHeader.biBitCount + 31) / 32 * 4;//每行字节数

	int heightPerRow = m_bitmapInfoHeader.biHeight / row;//像素数
	int widthPerCol = m_bitmapInfoHeader.biWidth / col;
	int lastRowHeight = heightPerRow + m_bitmapInfoHeader.biHeight % heightPerRow;
	int lastColWidth = widthPerCol + m_bitmapInfoHeader.biWidth % widthPerCol;

	int totlrowLen = 0;
	for (int rowIndex = 0; rowIndex < row; rowIndex++ )
	{
		int rowLen = heightPerRow;
		if (rowIndex == 0)
		{
			rowLen = lastRowHeight;
		}

		int startColpixel = 0;//像素数
		for (int colIndex = 0; colIndex < col; colIndex++)
		{
			int colLen = widthPerCol;
			if (colIndex == 0)
			{
				colLen = lastColWidth;
			}

			BITMAPFILEHEADER bitmapFileHeader = m_bitmapFileHeader;
			BITMAPINFOHEADER bitmapInfoHeader = m_bitmapInfoHeader;
			bitmapInfoHeader.biHeight = rowLen;//每张小图的高度
			bitmapInfoHeader.biWidth = colLen;//每张小图的像素数
			LONG align4BytesPerRow = (bitmapInfoHeader.biBitCount * colLen + 31) / 32 * 4;//每行字节数
			bitmapInfoHeader.biSizeImage = align4BytesPerRow * bitmapInfoHeader.biHeight;
			bitmapFileHeader.bfSize = bitmapFileHeader.bfOffBits + bitmapInfoHeader.biSizeImage;

			CString filePathName;
			filePathName.Format("--%d_%d", rowIndex, colIndex);
			filePathName = m_directory + m_fileName + filePathName + ".bmp";
			std::ofstream outstream(filePathName,std::ios::binary);
			if (!outstream.is_open())
			{
				return false;
			}

			outstream.write((char*)(&bitmapFileHeader), sizeof(BITMAPFILEHEADER));
			outstream.write((char*)(&bitmapInfoHeader), sizeof(BITMAPINFOHEADER));

			if (m_bitmapInfoHeader.biBitCount <= 8)
			{
				int size = 1 << m_bitmapInfoHeader.biBitCount;
				for (int i = 0; i < size; ++i)
				{
					outstream.write((char*)&m_pPalette[i], sizeof(RGBQUAD));
				}
			}

			byte * pPNewData = new byte[bitmapInfoHeader.biSizeImage];
			memset(pPNewData, 0, bitmapInfoHeader.biSizeImage);
			int pixelLeft = startColpixel * bitmapInfoHeader.biBitCount % 8;
			byte op1 = 0;
			byte op2 = 0;
			if (bitmapInfoHeader.biBitCount == 1)
			{
				for (int kk = 7; kk >= pixelLeft; kk--)
				{
					op2 = op2 + (1 << kk);
				}
				for (int kk = 0; kk < pixelLeft; kk++)
				{
					op1 = op1 + (1 << kk);
				}
			}

			for (int i = 0; i < bitmapInfoHeader.biHeight; i++)
			{
				int j = 0;
				while (j < bitmapInfoHeader.biWidth)
				{
					if (bitmapInfoHeader.biBitCount == 1)
					{
						//单字节中，高位存储低位像素
						int index = totlrowLen * lineBytes + i * lineBytes + (startColpixel + j) / 8;
						pPNewData[i * align4BytesPerRow + j / 8] = ((m_pData[index] & op1) <<(8- pixelLeft)) + ((m_pData[index + 1] & op2) >> (pixelLeft));
						j=j+8;
					}
					//if (bitmapInfoHeader.biBitCount == 1 ||
					//	bitmapInfoHeader.biBitCount == 4)
					//{
					//	byte tempv = m_pData[(totlrowLen+i) * lineBytes + startByte + j/];
					//	tempv = (tempv & op1);
					//	pPNewData[i * align4BytesPerRow + j] = tempv;
					//	if (pixelLeft != 0)
					//	{
					//		pPNewData[i * align4BytesPerRow + j] = (m_pData[(totlrowLen + i) * lineBytes + startByte + j + 1] & op2);
					//	}                                
					//	j++;
					//}
					//else
					//{
					//	pPNewData[i * align4BytesPerRow + j] = m_pData[(totlrowLen + i) * lineBytes + startByte + j];
					//	j++;
					//}
				}
			}

			startColpixel = startColpixel + colLen;
			outstream.write((char *)pPNewData, bitmapInfoHeader.biSizeImage);
			outstream.close();
			delete[] pPNewData;
		}

		totlrowLen += rowLen;
	}

	return true;
}

bool ZJLBitmap::SplitBitmapByCustom(int startx, int starty, int width, int height)
{
	return false;
}

