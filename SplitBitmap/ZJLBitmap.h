#pragma once
class ZJLBitmap
{
private:
	// λͼ�ļ�ͷ(bmp file header):�ṩ�ļ��ĸ�ʽ����С����Ϣ
	// λͼ��Ϣͷ(bitmap information):�ṩͼ�����ݵĳߴ硢λƽ������ѹ����ʽ����ɫ��������Ϣ
	// ��ɫ��(color palette):��ѡ����ʹ����������ʾͼ�񣬵�ɫ��������������Ӧ����ɫ��ӳ���
	// λͼ����(bitmap data):ͼ��������
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

