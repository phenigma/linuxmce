#pragma once
#include "afxwin.h"


// CVIPVariableDlg dialog

class CVIPVariableDlg : public CDialog
{
	DECLARE_DYNAMIC(CVIPVariableDlg)
	class VIPVariable *m_pVariable;

public:
	CVIPVariableDlg(class VIPVariable *pVariable);   // standard constructor
	virtual ~CVIPVariableDlg();

// Dialog Data
	enum { IDD = IDD_VIPVARIABLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbPredefinedVariables;
	CString m_CustomVariableDescription;
	BOOL m_Encrypt;
	BOOL m_PhoneSetsValue;
	CComboBox m_Format;
	CString m_sValue;

	void AddVariableIfNotThere(string Description,int VariableID);
	void AddFormat(string Description,int FormatID);

	afx_msg void OnEnChangeVarCustomVariable();
	afx_msg void OnCbnSelchangeVarPredefined();
	afx_msg void OnBnClickedOk();
};
