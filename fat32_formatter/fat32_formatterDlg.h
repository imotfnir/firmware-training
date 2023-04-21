
// fat32_formatterDlg.h : header file
//

#pragma once

#include "CFileSystemConfig.h"

// Cfat32formatterDlg dialog
class Cfat32formatterDlg : public CDialogEx
{
// Construction
public:
	Cfat32formatterDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FAT32_FORMATTER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReaddisk();
	afx_msg void OnCbnSelchangeComboClusterSize();
	afx_msg void OnBnClickedButtonShowConfig();
	CButton readDiskButton;
	CButton mbrCheckBox;
	CComboBox custerSizeComboBox;
	CComboBox fileSystemComboBox;
	CEdit fatOffset;
	CEdit partitionOffset;
	CFileSystemConfig fileSystemConfig;
	afx_msg void OnEnChangeEditFatOffset();
	afx_msg void OnEnChangeEditPartitionOffset();
	afx_msg void OnBnClickedCheckMbr();
};
