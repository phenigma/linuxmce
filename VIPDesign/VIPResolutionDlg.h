#pragma once
#include "afxwin.h"


// CVIPResolutionDlg dialog

class CVIPResolutionDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPResolutionDlg)
	class VIPMenuResolution *m_pResolution;

public:
	CVIPResolutionDlg(class VIPMenuResolution *pResolution);   // standard constructor
	virtual ~CVIPResolutionDlg();

// Dialog Data
	enum { IDD = IDD_VIPRESOLUTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_Y;
	BOOL m_N;
	BOOL m_E;
	BOOL m_F;
	BOOL m_U;
	BOOL m_D;
	BOOL m_L;
	BOOL m_R;
	BOOL m_0;
	BOOL m_1;
	BOOL m_2;
	BOOL m_3;
	BOOL m_4;
	BOOL m_5;
	BOOL m_6;
	BOOL m_7;
	BOOL m_8;
	BOOL m_9;
	CComboBox m_cbCriteriaVariable;
	CString m_CriteriaValue;
	BOOL m_HideApp;
	BOOL m_ReportVariables;
	CComboBox m_cbGoto;
	CComboBox m_cbActionVariable;
	CString m_ActionValue;
	afx_msg void OnBnClickedOk();
	CComboBox m_BasketAction;
	BOOL m_BasketToggle;
	CString m_Basket_ID;
	CString m_Basket_Description;
	CString m_Basket_Cost;
	CComboBox m_cbCloseRequest;
	BOOL m_C;
	CString m_sProgramName;
};
