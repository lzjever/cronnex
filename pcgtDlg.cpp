
// pcgtDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "pcgt.h"
#include "pcgtDlg.h"
#include "afxdialogex.h"
#include "CPU6502.h"
#include "Bus.h"
#include <iostream>
#include <fstream>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CpcgtDlg 对话框



CpcgtDlg::CpcgtDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PCGT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CpcgtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CPU_INFO, m_edit_cpu_info);
}

BEGIN_MESSAGE_MAP(CpcgtDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CpcgtDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CpcgtDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CpcgtDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CpcgtDlg 消息处理程序

BOOL CpcgtDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	ncpu.connect(&nbus);
	ncpu.rst();
	streampos size;
	char* memblock;
	ifstream file("E:\\works\\pcgt\\test_bin\\6502_functional_test.bin", ios::in | ios::binary | ios::ate);
	if (file.is_open())
	{
		size = file.tellg();
		memblock = new char[size];
		file.seekg(0, ios::beg);
		file.read(memblock, size);
		file.close();

		cout << "the entire file content is in memory";
		memcpy(nbus.memory_, memblock, size);

		delete[] memblock;
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CpcgtDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CpcgtDlg::OnPaint()
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
HCURSOR CpcgtDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CpcgtDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	USES_CONVERSION;
	for (int i = 0; i < 10000000; i++)
		ncpu.ticktock();
	m_edit_cpu_info.SetWindowTextW(A2W(ncpu.cpu_status()));
}


void CpcgtDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	uint16_t a = 0x00a5;
	uint16_t b = 0x0074;
	uint16_t  s_b = (0x80 | ~b + 1);
	uint16_t c = a - b;
	uint16_t d = a + (int16_t)s_b;
	(void)d;
}


void CpcgtDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	USES_CONVERSION;
	for (int i = 0; i < 1; i++)
		ncpu.ticktock();
	m_edit_cpu_info.SetWindowTextW(A2W(ncpu.cpu_status()));
}
