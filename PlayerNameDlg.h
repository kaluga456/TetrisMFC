#pragma once


// CPlayerNameDlg dialog
class CPlayerNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayerNameDlg)
public:
	CPlayerNameDlg(CWnd* pParent);   // standard constructor
	virtual ~CPlayerNameDlg();
// Dialog Data
	enum { IDD = IDD_PLAYER_NAME };
	CString PlayerName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
