// PingerDlg.h : header file
//

#if !defined(AFX_PINGERDLG_H__0788E7E9_71EF_11D2_B95A_0060083F73D6__INCLUDED_)
#define AFX_PINGERDLG_H__0788E7E9_71EF_11D2_B95A_0060083F73D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPingerDlg dialog

class CPingerDlg : public CDialog
{
// Construction
public:
	CPingerDlg(CWnd* pParent = NULL);	// standard constructor
	void CPingerDlg::OnOK();

// Dialog Data
	//{{AFX_DATA(CPingerDlg)
	enum { IDD = IDD_PINGER_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPingerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPingerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClear();
	afx_msg void OnChangeO1();
	afx_msg void OnChangeO2();
	afx_msg void OnChangeO3();
	afx_msg void OnResolve();
	afx_msg void OnScan();
	afx_msg void OnSave();
	afx_msg void OnCopy();
	afx_msg void OnAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	LRESULT OnStartup(WPARAM wParam, LPARAM lParam);
	LRESULT OnDone(WPARAM wParam, LPARAM lParam);
	LRESULT OnError(WPARAM wParam, LPARAM lParam);
	LRESULT OnPing(WPARAM wParam, LPARAM lParam);
	LRESULT OnHostAlive(WPARAM wParam, LPARAM lParam);
	LRESULT OnListen(WPARAM wParam, LPARAM lParam);
	LRESULT OnResolved(WPARAM wParam, LPARAM lParam);
};

#define WM_THREADSTARTING (WM_USER+1)
#define WM_THREADDONE (WM_USER+2)
#define WM_THREADERROR (WM_USER+3)
#define WM_THREADPING (WM_USER+4)
#define WM_THREADHOSTALIVE (WM_USER+5)
#define WM_THREADLISTEN (WM_USER+6)
#define WM_THREADRESOLVED (WM_USER+7)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PINGERDLG_H__0788E7E9_71EF_11D2_B95A_0060083F73D6__INCLUDED_)
