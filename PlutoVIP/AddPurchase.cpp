// AddPurchase.cpp : implementation file
//

#include "stdafx.h"
#include "PlutoVIP.h"
#include "AddPurchase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddPurchase dialog


CAddPurchase::CAddPurchase(CWnd* pParent /*=NULL*/)
	: CDialog(CAddPurchase::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddPurchase)
	m_Purchase = 0.0;
	//}}AFX_DATA_INIT
}


void CAddPurchase::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddPurchase)
	DDX_Text(pDX, IDC_PURCHASE, m_Purchase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddPurchase, CDialog)
	//{{AFX_MSG_MAP(CAddPurchase)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddPurchase message handlers
