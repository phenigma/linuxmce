#pragma once


// CResetEmulatorDlg dialog

typedef CEdit * CEditPtr;

class CResetEmulatorDlg : public CDialog
{
	DECLARE_DYNAMIC(CResetEmulatorDlg)
	CEditPtr *m_EditPointers;
	class VIPMenuCollection *m_pCollection;
	bool m_bReportVariablesOnly;

public:
	CResetEmulatorDlg(class VIPMenuCollection *pCollection,bool bReportVariablesOnly=false);   // standard constructor
	virtual ~CResetEmulatorDlg();

// Dialog Data
	enum { IDD = IDD_RESET_EMULATOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
};
