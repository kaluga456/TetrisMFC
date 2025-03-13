#pragma once


// COptionsDlg dialog

class COptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(COptionsDlg)
public:
	COptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~COptionsDlg();
	virtual BOOL OnInitDialog();
// Dialog Data
	enum { IDD = IDD_OPTIONS };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
};
