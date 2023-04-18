
// fat32_formatterDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "fat32_formatter.h"
#include "fat32_formatterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// Cfat32formatterDlg dialog



Cfat32formatterDlg::Cfat32formatterDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FAT32_FORMATTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cfat32formatterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ReadDisk, ReadDisk);
	DDX_Control(pDX, IDC_COMBO_ClUSTER_SIZE, ClusterSizeCheckbox);
	DDX_Control(pDX, IDC_COMBO2, FileSystemCheckbox);
}

BEGIN_MESSAGE_MAP(Cfat32formatterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ReadDisk, &Cfat32formatterDlg::OnBnClickedReaddisk)
	ON_CBN_SELCHANGE(IDC_COMBO_ClUSTER_SIZE, &Cfat32formatterDlg::OnCbnSelchangeComboClusterSize)

END_MESSAGE_MAP()


// Cfat32formatterDlg message handlers

BOOL Cfat32formatterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ClusterSizeCheckbox.AddString(_T("512 Bytes"));
	ClusterSizeCheckbox.AddString(_T("1024 Bytes"));
	ClusterSizeCheckbox.AddString(_T("2048 Bytes"));
	ClusterSizeCheckbox.AddString(_T("4096 Bytes"));
	ClusterSizeCheckbox.AddString(_T("8192 Bytes"));
	ClusterSizeCheckbox.AddString(_T("16384 Bytes"));
	FileSystemCheckbox.AddString(_T("FAT32"));
	FileSystemCheckbox.AddString(_T("exFAT"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cfat32formatterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cfat32formatterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cfat32formatterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cfat32formatterDlg::OnBnClickedReaddisk()
{
	return;
}


void Cfat32formatterDlg::OnCbnSelchangeComboClusterSize()
{


}


