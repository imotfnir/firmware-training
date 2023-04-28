
// fat32_formatterDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "fat32_formatter.h"
#include "fat32_formatterDlg.h"
#include "afxdialogex.h"

#include <Windows.h>
#include <sstream>

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

// Cfat32formatterDlg dialog

Cfat32formatterDlg::Cfat32formatterDlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_FAT32_FORMATTER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cfat32formatterDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ReadDisk, readDiskButton);
	DDX_Control(pDX, IDC_COMBO_ClUSTER_SIZE, custerSizeComboBox);
	DDX_Control(pDX, IDC_EDIT_FAT_OFFSET, fatOffset);
	DDX_Control(pDX, IDC_EDIT_PARTITION_OFFSET, partitionOffset);
	DDX_Control(pDX, IDC_CHECK_MBR, mbrCheckBox);
	DDX_Control(pDX, IDC_COMBO_DISK_PATH, diskPathComboBox);
}

BEGIN_MESSAGE_MAP(Cfat32formatterDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_ReadDisk, &Cfat32formatterDlg::OnBnClickedReaddisk)
ON_CBN_SELCHANGE(IDC_COMBO_ClUSTER_SIZE, &Cfat32formatterDlg::OnCbnSelchangeComboClusterSize)

ON_BN_CLICKED(IDC_BUTTON_SHOW_CONFIG, &Cfat32formatterDlg::OnBnClickedButtonShowConfig)
ON_EN_CHANGE(IDC_EDIT_FAT_OFFSET, &Cfat32formatterDlg::OnEnChangeEditFatOffset)
ON_EN_CHANGE(IDC_EDIT_PARTITION_OFFSET, &Cfat32formatterDlg::OnEnChangeEditPartitionOffset)
ON_BN_CLICKED(IDC_CHECK_MBR, &Cfat32formatterDlg::OnBnClickedCheckMbr)
ON_CBN_SELCHANGE(IDC_COMBO_DISK_PATH, &Cfat32formatterDlg::OnCbnSelchangeComboDiskPath)
END_MESSAGE_MAP()

// Cfat32formatterDlg message handlers

BOOL Cfat32formatterDlg::OnInitDialog()
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
	DWORD driveMask = GetLogicalDrives();

	custerSizeComboBox.AddString(_T("512 Bytes"));
	custerSizeComboBox.AddString(_T("1024 Bytes"));
	custerSizeComboBox.AddString(_T("2048 Bytes"));
	custerSizeComboBox.AddString(_T("4096 Bytes"));
	custerSizeComboBox.AddString(_T("8192 Bytes"));
	custerSizeComboBox.AddString(_T("16384 Bytes"));
	custerSizeComboBox.SelectString(-1, _T("8192 Bytes"));

	for (INT i = 0; i < 26; i++)
	{
		if (driveMask & (1 << i))
		{
			CHAR driveLabel[4] = {'A' + (CHAR)i, ':', '\\', '\0'};
			CHAR deviceName[7] = {'\\', '\\', '.', '\\', 'A' + (CHAR)i, ':', '\0'};
			UINT type = GetDriveTypeA(driveLabel);
			if (type == DRIVE_FIXED || type == DRIVE_REMOVABLE)
			{
				OutputDebugStringA(deviceName);
				diskPathComboBox.AddString((CString)deviceName);
			}
		}
	}

	fatOffset.SetWindowText(_T("4"));
	partitionOffset.SetWindowText(_T("0"));
	partitionOffset.EnableWindow(false);

	return TRUE; // return TRUE  unless you set the focus to a control
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

void Cfat32formatterDlg::OnCbnSelchangeComboClusterSize()
{
	CString value;
	custerSizeComboBox.GetLBText(custerSizeComboBox.GetCurSel(), value);
	fileSystemConfig.clusterSizeInByte = _ttoi(value);
}

void Cfat32formatterDlg::OnBnClickedButtonShowConfig()
{
	TRACE(_T("Have MBR: %d\n"), fileSystemConfig.isMBR);
	TRACE(_T("Cluster Size: %d Bytes\n"), fileSystemConfig.clusterSizeInByte);
	TRACE(_T("Offset of FAT Table : % d Sector\n"), fileSystemConfig.fat32ReversedSizeInSector);
	TRACE(_T("Offset of partition : %d Sector\n"), fileSystemConfig.offsetOfPartitionInSector);
	TRACE(_T("Disk Path : %s\n"), fileSystemConfig.diskPath);

	return;
}

void Cfat32formatterDlg::OnEnChangeEditFatOffset()
{
	CString value;
	fatOffset.GetWindowText(value);
	fileSystemConfig.fat32ReversedSizeInSector = _ttoi(value);
}

void Cfat32formatterDlg::OnEnChangeEditPartitionOffset()
{
	CString value;
	partitionOffset.GetWindowText(value);
	fileSystemConfig.offsetOfPartitionInSector = _ttoi(value);
}

void Cfat32formatterDlg::OnBnClickedCheckMbr()
{
	if (mbrCheckBox.GetCheck())
	{
		CString value;

		fileSystemConfig.isMBR = true;
		partitionOffset.EnableWindow(true);

		partitionOffset.GetWindowText(value);
		fileSystemConfig.offsetOfPartitionInSector = _ttoi(value);

		return;
	}
	fileSystemConfig.isMBR = false;
	partitionOffset.EnableWindow(false);
	fileSystemConfig.offsetOfPartitionInSector = 0;
}

void Cfat32formatterDlg::OnCbnSelchangeComboDiskPath()
{
	CString value;
	diskPathComboBox.GetLBText(diskPathComboBox.GetCurSel(), value);
	fileSystemConfig.diskPath = value;
}

void Cfat32formatterDlg::OnBnClickedReaddisk()
{
	if (!fileSystemConfig.IsConfigValid())
	{
		return;
	}

	HANDLE storageDevice = CreateFile(
		fileSystemConfig.diskPath,
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (storageDevice == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox(_T("Failed to open drive"), MB_ICONWARNING | MB_OK);
		TRACE(_T("Failed to open drive\n"));
		return;
	}

	if (!fileSystemConfig.InitConfig(storageDevice))
	{
		return;
	}

	DeviceLock(storageDevice);

	InitMbrStructure(storageDevice, fileSystemConfig);
	InitFat32BootSector(storageDevice, fileSystemConfig);
	InitFat32FsInfo(storageDevice, fileSystemConfig);
	InitFat32FatStructure(storageDevice, fileSystemConfig);
	ClearRootDirectory(storageDevice, fileSystemConfig);

	DeviceUnLock(storageDevice);
	CloseHandle(storageDevice);

	return;
}
