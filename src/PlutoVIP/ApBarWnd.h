/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#ifndef __APBARWND_H__
#define __APBARWND_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ApBarWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppBarWnd window

class CAppBarWnd : public CWnd
{
public:
	CAppBarWnd();

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppBarWnd)
	//}}AFX_VIRTUAL

	virtual ~CAppBarWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAppBarWnd)
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APBARWND_H__AB5CEE04_A955_11D1_B258_006097960BB7__INCLUDED_)
