// PackageListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WindowsInstaller.h"
#include "WindowsInstallerDlg.h"
#include "PackageListDialog.h"
#include ".\packagelistdialog.h"
#include "PlutoUtils/StringUtils.h"

#include "afxinet.h"

// CPackageListDialog dialog

IMPLEMENT_DYNAMIC(CPackageListDialog, CDialog)
CPackageListDialog::CPackageListDialog(CDialog *pPrevDialog, CString sPlutoHomeURL, CWnd* pParent /*=NULL*/)
	: CDialog(CPackageListDialog::IDD, pParent)
{
	m_pPrevDialog = pPrevDialog;
	m_sPlutoHomeURL = sPlutoHomeURL;
}

CPackageListDialog::~CPackageListDialog()
{
}

void CPackageListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS_PACK, m_Status);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_HEADER_PACK2, m_Header);
}


BEGIN_MESSAGE_MAP(CPackageListDialog, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(ID_PREV_PACK, OnBnClickedPrevPack)
	ON_BN_CLICKED(ID_NEXT_PACK, OnBnClickedNextPack)
	ON_BN_CLICKED(IDC_EXIT_PACK, OnBnClickedExitPack)
	ON_BN_CLICKED(ID_PACKAGES_PACK, OnBnClickedPackagesPack)
END_MESSAGE_MAP()


// CPackageListDialog message handlers

void CPackageListDialog::OnBnClickedCancel()
{
	if(HandleCancel(this))
		CloseAll();
}

void CPackageListDialog::OnBnClickedPrevPack()
{
	ShowWindow(SW_HIDE);

	ASSERT(NULL != m_pPrevDialog);
	m_pPrevDialog->ShowWindow(SW_SHOW);
}

void CPackageListDialog::OnBnClickedNextPack()
{
	// TODO: Add your control notification handler code here
}

void CPackageListDialog::OnBnClickedExitPack()
{
	if(HandleCancel(this))
		CloseAll();
}

void CPackageListDialog::OnBnClickedPackagesPack()
{
	m_Status.SetWindowText("Connecting to plutohome site...");

	CInternetSession inetSession;
	CStdioFile *psFile = inetSession.OpenURL(m_sPlutoHomeURL);

	m_Status.SetWindowText("Downloading the list with the software to be installed...");

	ULONGLONG iSize = psFile->GetLength();
	char *pBuffer = new char[(size_t)iSize];
	psFile->Read(pBuffer, (int)iSize);
	psFile->Close();
	delete psFile;

	//TODO: use pBuffer here

	delete pBuffer;

	m_list.AddString("Boo!");
	m_list.AddString("sadfasdf");
	m_list.AddString("asdfg");
	m_list.AddString("baubau");
	m_list.SetColumnWidth(300);

	CString sHeader = CString("Found the following ") + StringUtils::ltos(m_list.GetCount()).c_str() + " packages";
	m_Header.SetWindowText(sHeader);

	m_Status.SetWindowText("Done.");
}

void CPackageListDialog::CloseAll()
{
	OnCancel();
	((CWindowsInstallerDlg *)m_pPrevDialog)->CloseAll();
}