// Pinger.h : main header file for the PINGER application
//

#if !defined(AFX_PINGER_H__0788E7E7_71EF_11D2_B95A_0060083F73D6__INCLUDED_)
#define AFX_PINGER_H__0788E7E7_71EF_11D2_B95A_0060083F73D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPingerApp:
// See Pinger.cpp for the implementation of this class
//

class CPingerApp : public CWinApp
{
public:
	CPingerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPingerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPingerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINGER_H__0788E7E7_71EF_11D2_B95A_0060083F73D6__INCLUDED_)
