#ifndef _LIST_HOVER_H_
#define _LIST_HOVER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListHover.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListHover window

class CListHover : public CListCtrl
{
// Construction
public:
	CListHover();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListHover)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetParent(CWnd* pParent);
	virtual ~CListHover();

	// Generated message map functions
protected:
	CWnd* m_pParent;
	//{{AFX_MSG(CListHover)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int Treat;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _LIST_HOVER_H_
