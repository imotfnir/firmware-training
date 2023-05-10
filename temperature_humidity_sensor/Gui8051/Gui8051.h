
// Gui8051.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGui8051App:
// See Gui8051.cpp for the implementation of this class
//

class CGui8051App : public CWinApp
{
public:
	CGui8051App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CGui8051App theApp;
