#pragma once
#include "afxwin.h"


// CVIPInputDlg dialog

class CVIPInputDlg : public CDialog
{
	class VIPMenuInput *m_pInput;
	DECLARE_DYNAMIC(CVIPInputDlg)

public:
	CVIPInputDlg(class VIPMenuInput *pInput);   // standard constructor
	virtual ~CVIPInputDlg();

// Dialog Data
	enum { IDD = IDD_VIPINPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_NumberOfCharacters;
	BOOL m_AutoTerminate;
	BOOL m_ClearOnLoad;
	CComboBox m_cbVariable;
	CComboBox m_TextElement;
	afx_msg void OnBnClickedOk();
};
