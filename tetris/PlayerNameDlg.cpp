#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "PlayerNameDlg.h"

// CPlayerNameDlg dialog
IMPLEMENT_DYNAMIC(CPlayerNameDlg, CDialog)
CPlayerNameDlg::CPlayerNameDlg(CWnd* pParent) : 
	CDialog(CPlayerNameDlg::IDD, pParent)
{
}
CPlayerNameDlg::~CPlayerNameDlg()
{
}
void CPlayerNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CPlayerNameDlg, CDialog)
END_MESSAGE_MAP()
BOOL CPlayerNameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("Enter your name"));
	CEdit* edit = static_cast<CEdit*>(GetDlgItem(IDC_PLAYER_NAME));
	edit->SetFocus();
	edit->SetLimitText(24);
	return FALSE;  // return TRUE unless you set the focus to a control
}

void CPlayerNameDlg::OnOK()
{
	CEdit* edit = static_cast<CEdit*>(GetDlgItem(IDC_PLAYER_NAME));
	edit->GetWindowText(PlayerName);
	CDialog::OnOK();
}
