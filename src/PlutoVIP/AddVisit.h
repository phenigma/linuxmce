#if !defined(AFX_ADDVISIT_H__273364CA_6D6C_4CFF_9B3C_BDDF786CA48A__INCLUDED_)
#define AFX_ADDVISIT_H__273364CA_6D6C_4CFF_9B3C_BDDF786CA48A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddVisit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddVisit dialog

class CAddVisit : public CDialog
{
// Construction
public:
	CAddVisit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddVisit)
	enum { IDD = IDD_ADD_VISIT };
	int		m_Visit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddVisit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddVisit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDVISIT_H__273364CA_6D6C_4CFF_9B3C_BDDF786CA48A__INCLUDED_)
