
// fat32_formatter.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cfat32formatterApp:
// See fat32_formatter.cpp for the implementation of this class
//

class Cfat32formatterApp : public CWinApp
{
public:
	Cfat32formatterApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cfat32formatterApp theApp;
