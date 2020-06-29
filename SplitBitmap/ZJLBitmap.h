#pragma once
class ZJLBitmap
{
private:
	// 位图文件头(bmp file header):提供文件的格式、大小等信息
	// 位图信息头(bitmap information):提供图像数据的尺寸、位平面数、压缩方式、颜色索引等信息
	// 调色板(color palette):可选，如使用索引来表示图像，调色板就是索引与其对应的颜色的映射表
	// 位图数据(bitmap data):图像数据区
	BITMAPFILEHEADER m_bitmapFileHeader;
	BITMAPINFOHEADER m_bitmapInfoHeader;
	RGBQUAD * m_pPalette;
	byte * m_pData;
	CString m_fileName;
	CString m_directory;
public:
	ZJLBitmap();
	~ZJLBitmap();

	bool InitBitmap(CString &bitmapFilePath);

	bool SplitBitmapByRowAndCol(int row, int col);

	bool SplitBitmapByCustom(int startx, int starty, int width, int height);
};

