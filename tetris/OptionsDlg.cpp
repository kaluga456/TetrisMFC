#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "options.h"
#include "optionsdlg.h"

extern COptions Options;

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)
COptionsDlg::COptionsDlg(CWnd* pParent) : CDialog(COptionsDlg::IDD, pParent)
{
}
COptionsDlg::~COptionsDlg()
{
}
BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CSliderCtrl* ctrl = static_cast<CSliderCtrl*>(GetDlgItem(IDC_KEYSPEED_SLIDER));
	//CSliderCtrl* ctrl = STATIC_DOWNCAST(CSliderCtrl, GetDlgItem(IDC_KEYSPEED_SLIDER));
	ctrl->SetRange(KEYBOARD_MAX_SPEED, KEYBOARD_MIN_SPEED, TRUE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_SOUND_CHECK, Options.Sound);
	DDX_Slider(pDX, IDC_KEYSPEED_SLIDER, Options.KeyboardSpeed);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
END_MESSAGE_MAP()



