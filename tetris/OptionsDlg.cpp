#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "options.h"
#include "optionsdlg.h"

extern COptions Options;
//////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////
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
	CSliderCtrl* speed_slider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_KEYSPEED_SLIDER));
	speed_slider->SetRange(KEYBOARD_MAX_SPEED, KEYBOARD_MIN_SPEED, TRUE);
	Options.KeyboardSpeed = std::clamp<int>(Options.KeyboardSpeed, KEYBOARD_MAX_SPEED, KEYBOARD_MIN_SPEED);
	speed_slider->SetPos(Options.KeyboardSpeed);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void COptionsDlg::OnOK()
{
	CSliderCtrl* speed_slider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_KEYSPEED_SLIDER));
	Options.KeyboardSpeed = speed_slider->GetPos();
	CDialog::OnOK();
}
void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Slider(pDX, IDC_KEYSPEED_SLIDER, Options.KeyboardSpeed);
}
//////////////////////////////////////////////////////////////////////////////


