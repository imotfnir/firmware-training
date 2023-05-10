
// Gui8051Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Gui8051.h"
#include "Gui8051Dlg.h"
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
	enum
	{
		IDD = IDD_ABOUTBOX
	};
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CGui8051Dlg dialog

CGui8051Dlg::CGui8051Dlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_GUI8051_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGui8051Dlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, ButtonUpdate);
}

BEGIN_MESSAGE_MAP(CGui8051Dlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CGui8051Dlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CGui8051Dlg message handlers

BOOL CGui8051Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu *pSysMenu = GetSystemMenu(FALSE);
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
	SetIcon(m_hIcon, TRUE);	 // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// TODO: Add extra initialization here

	return TRUE; // return TRUE  unless you set the focus to a control
}

void CGui8051Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGui8051Dlg::OnPaint()
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
HCURSOR CGui8051Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGui8051Dlg::OnBnClickedOk()
{
	// variables used with the com port
	HANDLE hComm;
	CString m_sComPort;
	char strBuffer[0x100] = { 0 };
	DCB m_dcb;
	DWORD iBytesRead;
	BOOL status;
	COMMTIMEOUTS timeouts = {0};
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_STATIC);


	m_sComPort = "Com5";

	hComm = CreateFile(m_sComPort,
					   GENERIC_READ | GENERIC_WRITE,
					   0,	 // exclusive access
					   NULL, // no security
					   OPEN_EXISTING,
					   0,	  // no overlapped I/O
					   NULL); // null template

	if (hComm == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			TRACE(_T("Cannot open port!\n"));
			return;
		}

		TRACE(_T("Invalid handle value!\n"));
		return;
	}

	SetupComm(hComm, 0x200, 0x200); // set buffer sizes

	status = GetCommState(hComm, &m_dcb);
	if (status == FALSE)
	{
		TRACE(_T("GetCommState failed\n"));
		goto __fail;
	}

	m_dcb.BaudRate = 9600;
	m_dcb.ByteSize = 8;
	m_dcb.Parity = NOPARITY;
	m_dcb.StopBits = ONESTOPBIT;
	m_dcb.fAbortOnError = TRUE;
	status = SetCommState(hComm, &m_dcb);
	if (status == FALSE)
	{
		TRACE(_T("SetCommState DCB failed\n"));
		goto __fail;
	}

	TRACE(_T("BardRate = %d\n", m_dcb.BaudRate));
	TRACE(_T("ByteSize = %d\n", m_dcb.ByteSize));
	TRACE(_T("Parity = %d\n", m_dcb.Parity));
	TRACE(_T("StopBits = %d\n", m_dcb.StopBits));
	TRACE(_T("fAbortOnError = %d\n", m_dcb.fAbortOnError));

	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (SetCommTimeouts(hComm, &timeouts) == 0)
	{
		TRACE("Error setting timeouts\n");
		goto __fail;
	}
	strBuffer[0] = 'a';
	if (!WriteFile(hComm, strBuffer, 1, &iBytesRead, NULL))
	{
		TRACE(_T("Write com port fail\n"));
	}
	do {
		if (!ReadFile(hComm, strBuffer, sizeof(strBuffer), &iBytesRead, NULL))
		{
			TRACE(_T("Read com port fail\n"));
		}
		//TRACE((CString)strBuffer);
		int nLength = MultiByteToWideChar(CP_ACP, 0, strBuffer, -1, NULL, 0);
		TCHAR* szBuffer = new TCHAR[nLength];
		MultiByteToWideChar(CP_ACP, 0, strBuffer, -1, szBuffer, nLength);
		pStatic->SetWindowText(szBuffer);
	} while (iBytesRead > 0);



__fail:
	CloseHandle(hComm);
	return;
}
