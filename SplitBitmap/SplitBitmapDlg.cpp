
// SplitBitmapDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "SplitBitmap.h"
#include "SplitBitmapDlg.h"
#include "afxdialogex.h"
#include "ZJLBitmap.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSplitBitmapDlg 对话框



CSplitBitmapDlg::CSplitBitmapDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SPLITBITMAP_DIALOG, pParent)
	, m_splitType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSplitBitmapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_AVERAGE_SPLIT, m_splitType);
}

BEGIN_MESSAGE_MAP(CSplitBitmapDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSplitBitmapDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSplitBitmapDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_BITMAP, &CSplitBitmapDlg::OnBnClickedButtonLoadBitmap)
	ON_BN_CLICKED(IDC_RADIO_AVERAGE_SPLIT, &CSplitBitmapDlg::OnClickedRadio)
	ON_COMMAND(IDC_RADIO_CUSTOM, &CSplitBitmapDlg::OnRadioCustom)
END_MESSAGE_MAP()


// CSplitBitmapDlg 消息处理程序

BOOL CSplitBitmapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	((CEdit *)GetDlgItem(IDC_EDIT_ROW))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_COLUMN))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_X))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_Y))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_WIDTH))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_HEIGHT))->EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSplitBitmapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSplitBitmapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSplitBitmapDlg::OnBnClickedOk()
{
	//分割图片
	CString strfilePath;
	GetDlgItem(IDC_EDIT_BITMAP_PATH)->GetWindowText(strfilePath);
	
	if (_access(strfilePath, 0) != 0)
	{
		MessageBox("文件不存在", "ZJLSplitBmp", MB_ICONERROR | MB_OK);
	}

	ZJLBitmap bitmap;
	bitmap.InitBitmap(strfilePath);

	if (m_splitType == 0)
	{
		CString str;
		((CEdit *)GetDlgItem(IDC_EDIT_ROW))->GetWindowText(str);
		int row = atoi(str.GetBuffer());
		((CEdit *)GetDlgItem(IDC_EDIT_COLUMN))->GetWindowText(str);
		int col = atoi(str.GetBuffer());
		if (!bitmap.SplitBitmapByRowAndCol(row, col))
		{
			MessageBox("分割失败","ZJLSplitBMP",MB_ICONERROR|MB_OK);
			return;
		}
		MessageBox("分割成功!");
	}
}


void CSplitBitmapDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CSplitBitmapDlg::OnBnClickedButtonLoadBitmap()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szFilter[] = "*.bmp||";
	CFileDialog fileDialog(TRUE, _T("bmp"), NULL, 0, szFilter, this);
	CString strfilePath = _T("");

	if (fileDialog.DoModal() == IDOK)
	{
		strfilePath = fileDialog.GetPathName();
	}
	GetDlgItem(IDC_EDIT_BITMAP_PATH)->SetWindowText(strfilePath);
}


void CSplitBitmapDlg::OnClickedRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit *)GetDlgItem(IDC_EDIT_ROW))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_COLUMN))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_X))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_Y))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_WIDTH))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_HEIGHT))->EnableWindow(FALSE);

}


void CSplitBitmapDlg::OnRadioCustom()
{
	// TODO: 在此添加命令处理程序代码
	((CEdit *)GetDlgItem(IDC_EDIT_ROW))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_COLUMN))->EnableWindow(FALSE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_X))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_START_Y))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_WIDTH))->EnableWindow(TRUE);
	((CEdit *)GetDlgItem(IDC_EDIT_HEIGHT))->EnableWindow(TRUE);

}
