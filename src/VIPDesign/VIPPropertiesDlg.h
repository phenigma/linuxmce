#pragma once
#include "afxwin.h"


// CVIPPropertiesDlg dialog

class CVIPPropertiesDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPPropertiesDlg)
	class VIPMenuCollection *m_pMenuCollection;

public:
	CVIPPropertiesDlg(class VIPMenuCollection *pMenuCollection);   // standard constructor
	virtual ~CVIPPropertiesDlg();

// Dialog Data
	enum { IDD = IDD_PROPERTIES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_Width;
	CString m_Height;
	afx_msg void OnBnClickedOk();
	BOOL OnInitDialog();
	CComboBox m_StartingMenu;
	CString m_CollectionID;
};
