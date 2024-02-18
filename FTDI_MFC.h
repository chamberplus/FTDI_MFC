
// FTDI_MFC.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CFTDIMFCApp:
// See FTDI_MFC.cpp for the implementation of this class
//

class CFTDIMFCApp : public CWinApp
{
public:
	CFTDIMFCApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CFTDIMFCApp theApp;
