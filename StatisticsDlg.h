#pragma once

class CStatisticsDlg : public CDialog
{
	DECLARE_DYNAMIC(CStatisticsDlg)
public:
	CStatisticsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatisticsDlg();
	enum { IDD = IDD_STATISTICS };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
