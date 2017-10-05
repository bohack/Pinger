#include "stdafx.h"
#include "cbtext.h"
#include <memory.h>

/* Code snippet to set clipboard text.
 * From "Developing Professional Applications for Windows 95
 * and Windows NT Using MFC" by Marshall Brain/ Lance Lovette */

BOOL CBText::SetText(CString s)
{
	HGLOBAL temp;
	CWinApp *app=AfxGetApp();
	CFrameWnd *fw=(CFrameWnd *)app->m_pMainWnd;
	LPTSTR str;

	if (!::OpenClipboard(fw->m_hWnd))
		return FALSE;
	::EmptyClipboard();
	temp = GlobalAlloc(GHND, s.GetLength()+1);
	if (temp == NULL)
	{
		CloseClipboard();
		return FALSE;
	}
	str=(char *)GlobalLock(temp);
	memcpy(str,LPCTSTR(s), s.GetLength()+1);
	GlobalUnlock((void *)temp);
	SetClipboardData(CF_TEXT,temp);
	CloseClipboard();
	return TRUE;
}