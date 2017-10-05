// PingerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Pinger.h"
#include "PingerDlg.h"
#include "sockeng.h"
#include "cbtext.h"
#include "HelpDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DELAY 100

int gRunning=0;
int gResolving=0;
int totalThreads=0;
int sem=0;

struct ThreadArgs
{
	HWND win;
	long NumPasses;
	long Start,End,Timeout;
	long Delay;
};

struct ResolvArgs
{
	HWND win;
	long address;
	int index;
};

/* Our pinging thread entry point */

UINT Scan(LPVOID pParam)
{
	struct ThreadArgs *ta;
	ta=(struct ThreadArgs *)pParam;

	gRunning=1;
	
	::PostMessage(ta->win,WM_THREADSTARTING,0,0);

	sockeng *s=new sockeng(ta->Timeout,ta->Start,ta->End,
		ta->NumPasses,ta->Delay);

	s->ping(ta->win);
	
	::PostMessage(ta->win,WM_THREADDONE,0,0);

	delete s;
	return 0;
}

/* Our host resolving thread entry point */

UINT ResolveHost(LPVOID pParam)
{
	struct ResolvArgs *ta;
	struct hostent *hp;
	struct in_addr ip;
	ta=(struct ResolvArgs *)pParam;	
	
	ip.S_un.S_addr=ta->address;

	totalThreads++;

	/* makeshift semaphore :< */
	/* god i wish i knew what i was doing */
	/* wierd stuff can happen beacuse it's not atomic, but it 
	 * doesn't really matter */

	while ((sem>30) && (gRunning==1))
	{
		Sleep(1000);
	}
	sem++;

	if (!gRunning)
	{
		free(ta);
		totalThreads--;
		sem--;
		return 0;
	}

	hp=gethostbyaddr((char *)&(ta->address),4,AF_INET);
	if (!hp)
	{
		::PostMessage(ta->win,WM_THREADRESOLVED,
			(long)strdup(inet_ntoa(ip)),NULL);
		free(ta);
		totalThreads--;
		sem--;
		return 0;
	}

	if (hp->h_name)
	{
		::PostMessage(ta->win,WM_THREADRESOLVED,
			(long)strdup(inet_ntoa(ip)),
			(long)strdup(hp->h_name));
	}
	else
	{
		::PostMessage(ta->win,WM_THREADRESOLVED,
			(long)strdup(inet_ntoa(ip)),NULL);
	}
	free(ta);
	totalThreads--;
	sem--;
	return 0;
}

void AddStringSorted(CListBox *cl,char *s)
{
	CString walker;
	unsigned long ip,wip;
	unsigned char o1,o2,o3,o4;
	int i;

	sscanf(s,"%d.%d.%d.%d",&o1,&o2,&o3,&o4);
	ip=(o1<<24)+(o2<<16)+(o3<<8)+o4;

	for (i=0;i<cl->GetCount();i++)
	{
		cl->GetText(i,walker);
		/* This is so horifically stupid...
		 * however, I'm tired so fuck it */

		sscanf(walker,"%d.%d.%d.%d",&o1,&o2,&o3,&o4);
		wip=(o1<<24)+(o2<<16)+(o3<<8)+o4;
	
		if (wip>ip) break;
	}
	cl->InsertString(i,s);
}

LRESULT CPingerDlg::OnStartup(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);
	CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);

	statusBar->SetWindowText("Initializing Socket Engine...");

	return 0L;
}

LRESULT CPingerDlg::OnDone(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);
	MSG msg;	
	
	if (gResolving==1 && gRunning==1)
	{
		statusBar->SetWindowText("Resolving hostnames...");
		while ((totalThreads>0) && (gRunning==1))
		{
			// Goddamn this is sloppy
			GetMessage(&msg,NULL,0,0);
			{
				TranslateMessage(&msg); // Translate virt. key codes 
				DispatchMessage(&msg); // Dispatch msg. to window 
			}
		} 
	}

	statusBar->SetWindowText("");

	CButton *cb=(CButton *)GetDlgItem(IDC_SCAN);
	cb->SetWindowText("Ping");

	gRunning=0;

	return 0L;
}

LRESULT CPingerDlg::OnPing(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);

	static char bob[8192];
	sprintf(bob,"Pinging %d.%d.%d.%d",(lParam&255),(lParam>>8)&255,
		(lParam>>16)&255,(lParam>>24)&255);

	statusBar->SetWindowText(bob);

	return 0L;
}

LRESULT CPingerDlg::OnHostAlive(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);
	CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);
	
	if (!gResolving)
	{
		static char bob[256];
		sprintf(bob,"%d.%d.%d.%d",(lParam&255),(lParam>>8)&255,
			(lParam>>16)&255,(lParam>>24)&255);
		AddStringSorted(list,bob);		
	}
	else
	{
		struct ResolvArgs *ta;
		ta=(struct ResolvArgs *)malloc(sizeof (struct ResolvArgs));
	
		ta->win=GetSafeHwnd();
		ta->address=lParam;

		CWinThread *pThread = AfxBeginThread(ResolveHost, ta,
			THREAD_PRIORITY_NORMAL);	
	}
	return 0L;
}

LRESULT CPingerDlg::OnResolved(WPARAM wParam, LPARAM lParam)
{
	CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);

	static char bob[512];
	
	if (lParam)
		sprintf(bob,"%s   \t%s",(char*)wParam,(char *)lParam);
	else
		sprintf(bob,"%s",(char*)wParam);	
	
	if (gRunning)
		AddStringSorted(list,bob);		
	
	return 0L;
}

LRESULT CPingerDlg::OnListen(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);

	statusBar->SetWindowText("Listening for pings...");

	return 0L;
}

LRESULT CPingerDlg::OnError(WPARAM wParam, LPARAM lParam)
{
	CStatic *statusBar=(CStatic *)GetDlgItem(IDC_STATUS);

	statusBar->SetWindowText("");

	CButton *cb=(CButton *)GetDlgItem(IDC_SCAN);
	cb->SetWindowText("Ping");

	gRunning=0;

	if (lParam)
	{
		CString msg((char *)lParam);
		MessageBox(msg,"Ping Error",
				MB_ICONEXCLAMATION);
	}

	return 0L;
}

/////////////////////////////////////////////////////////////////////////////
// CPingerDlg dialog

CPingerDlg::CPingerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPingerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPingerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPingerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPingerDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPingerDlg, CDialog)
	//{{AFX_MSG_MAP(CPingerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_EN_CHANGE(IDC_O1, OnChangeO1)
	ON_EN_CHANGE(IDC_O2, OnChangeO2)
	ON_EN_CHANGE(IDC_O3, OnChangeO3)
	ON_BN_CLICKED(IDC_RESOLVE, OnResolve)
	ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_COPY, OnCopy)
	ON_WM_COMPAREITEM()
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
ON_MESSAGE(WM_THREADSTARTING, OnStartup)
ON_MESSAGE(WM_THREADDONE, OnDone)
ON_MESSAGE(WM_THREADERROR, OnError)
ON_MESSAGE(WM_THREADPING, OnPing)
ON_MESSAGE(WM_THREADHOSTALIVE, OnHostAlive)
ON_MESSAGE(WM_THREADLISTEN, OnListen)
ON_MESSAGE(WM_THREADRESOLVED, OnResolved)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPingerDlg message handlers

BOOL CPingerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CEdit *ce;

	ce=(CEdit *)GetDlgItem(IDC_TIMEOUT);
	ce->SetWindowText("3000");

	ce=(CEdit *)GetDlgItem(IDC_PASSES);
	ce->SetWindowText("2");

	ce=(CEdit *)GetDlgItem(IDC_O4);
	ce->SetWindowText("1");

	ce=(CEdit *)GetDlgItem(IDC_O8);
	ce->SetWindowText("254");

	gRunning=0;
	gResolving=1;

	CButton *cb=(CButton *)GetDlgItem(IDC_RESOLVE);
	
	cb->SetCheck(1);

	srand(GetTickCount());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPingerDlg::OnOK()
{ 
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPingerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPingerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPingerDlg::OnClear() 
{
	CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);
	list->ResetContent();	
}


void CPingerDlg::OnChangeO1() 
{
	CString cstr;
	CEdit *pOct1=(CEdit *)GetDlgItem(IDC_O1);
	CEdit *pOct5=(CEdit *)GetDlgItem(IDC_O5);

	if (pOct1 && pOct5)
	{
		pOct1->GetWindowText(cstr);
		if (cstr)
			pOct5->SetWindowText(cstr);
	}	
}

void CPingerDlg::OnChangeO2() 
{
	CString cstr;
	CEdit *pOct2=(CEdit *)GetDlgItem(IDC_O2);
	CEdit *pOct6=(CEdit *)GetDlgItem(IDC_O6);

	if (pOct2 && pOct6)
	{
		pOct2->GetWindowText(cstr);
		if (cstr)
			pOct6->SetWindowText(cstr);
	}	
}

void CPingerDlg::OnChangeO3() 
{
	CString cstr;
	CEdit *pOct3=(CEdit *)GetDlgItem(IDC_O3);
	CEdit *pOct7=(CEdit *)GetDlgItem(IDC_O7);

	if (pOct3 && pOct7)
	{
		pOct3->GetWindowText(cstr);
		if (cstr)
			pOct7->SetWindowText(cstr);
	}		
}

void CPingerDlg::OnResolve() 
{
	CButton *cb=(CButton *)GetDlgItem(IDC_RESOLVE);
	
	gResolving=cb->GetCheck();
}

void CPingerDlg::OnScan() 
{
	struct ThreadArgs ta;
	CString o1,o2,o3,o4,o5,o6,o7,o8,passes,to,np,dl;

	if (gRunning==1)
	{
		gRunning=0;
		return;
	}

	CEdit *pFOct1=(CEdit *)GetDlgItem(IDC_O1);
	CEdit *pFOct2=(CEdit *)GetDlgItem(IDC_O2);
	CEdit *pFOct3=(CEdit *)GetDlgItem(IDC_O3);
	CEdit *pFOct4=(CEdit *)GetDlgItem(IDC_O4);
	CEdit *pTOct1=(CEdit *)GetDlgItem(IDC_O5);
	CEdit *pTOct2=(CEdit *)GetDlgItem(IDC_O6);
	CEdit *pTOct3=(CEdit *)GetDlgItem(IDC_O7);
	CEdit *pTOct4=(CEdit *)GetDlgItem(IDC_O8);

	pFOct1->GetWindowText(o1);
	pFOct2->GetWindowText(o2);
	pFOct3->GetWindowText(o3);
	pFOct4->GetWindowText(o4);
	pTOct1->GetWindowText(o5);
	pTOct2->GetWindowText(o6);
	pTOct3->GetWindowText(o7);
	pTOct4->GetWindowText(o8);

	CEdit *pTimeout=(CEdit *)GetDlgItem(IDC_TIMEOUT);
	CEdit *pPasses=(CEdit *)GetDlgItem(IDC_PASSES);

	pPasses->GetWindowText(passes);
	pTimeout->GetWindowText(to);

	ta.NumPasses=atoi(passes);
	ta.Timeout=atoi(to);
	ta.Delay=DELAY;

	char startIP[80],endIP[80];

	sprintf(startIP,"%s.%s.%s.%s",o1.GetBuffer(3),o2.GetBuffer(3)
		,o3.GetBuffer(3),o4.GetBuffer(3));
	sprintf(endIP,"%s.%s.%s.%s",o5.GetBuffer(3),o6.GetBuffer(3)
		,o7.GetBuffer(3),o8.GetBuffer(3));

	ta.Start=sockeng::getaddr(startIP);
	if (ta.Start==-1)
	{
		CString msg("There is a problem with your starting IP.");
		MessageBox(msg,"Data Validation Error",
			MB_ICONEXCLAMATION);
		return;
	}
	ta.End=sockeng::getaddr(endIP);
	if (ta.End==-1)
	{
		CString msg("There is a problem with your ending IP.");
		MessageBox(msg,"Data Validation Error",
			MB_ICONEXCLAMATION);
		return;
	}

	if ((ntohl(ta.End))<(ntohl(ta.Start)))
	{
		CString msg("Your ending IP is lower than your starting IP.");
		MessageBox(msg,"Data Validation Error",
			MB_ICONEXCLAMATION);
		return;
	}

	CButton *cb=(CButton *)GetDlgItem(IDC_SCAN);
	cb->SetWindowText("Stop");

	ta.win=GetSafeHwnd();

	totalThreads=0;
	sem=0;

	CWinThread *pThread = AfxBeginThread(Scan, &ta,
		THREAD_PRIORITY_NORMAL);	
}

void CPingerDlg::OnSave() 
{
	CFileDialog fileDialog(FALSE,
		"txt", NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||");

	if (fileDialog.DoModal() == IDOK)
	{
		CString filePathName=fileDialog.GetPathName();
		CFile file;

		if (!file.Open(filePathName,CFile::modeCreate|
			CFile::modeWrite))
		{
			CString msg("Can't Open "+ filePathName+
				"\nCheck the filename.");
			MessageBox(msg,"File Save Error",
				MB_ICONEXCLAMATION);
		}
		else
		{
			char item[8192];
			CString clip;

			CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);
			for (int i=0;i<list->GetCount();i++)
			{
				list->GetText(i,item);
				clip=clip+item+"\n";
			}
			file.Write(clip,strlen(clip));
			file.Close();
		}
	}
}

void CPingerDlg::OnCopy() 
{
	CBText t;
	char item[8192];
	CString clip;

	CListBox *list=(CListBox *)GetDlgItem(IDC_LIST);
	for (int i=0;i<list->GetCount();i++)
	{
		list->GetText(i,item);
		clip=clip+item+"\n";
	}
	t.SetText(clip);	
}

void CPingerDlg::OnAbout() 
{
	HelpDialog ch;
	ch.DoModal();
}
