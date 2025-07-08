#pragma once
//////////////////////////////////////////////////////////////////////////////
class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)
	DECLARE_MESSAGE_MAP()

public:
	enum { IDD = IDD_OPTIONS };

	COptionsDlg(CWnd* pParent = NULL);
	virtual ~COptionsDlg();

	BOOL OnInitDialog() override;
	void OnOK() override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
};
//////////////////////////////////////////////////////////////////////////////