#pragma once


// CVIPMenuDlg dialog

class CVIPMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPMenuDlg)
	class VIPMenu *m_pMenu;

public:
	CVIPMenuDlg(class VIPMenu *pMenu);   // standard constructor
	virtual ~CVIPMenuDlg();

// Dialog Data
	enum { IDD = IDD_VIPMENU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_Description;
	afx_msg void OnBnClickedOk();
};
