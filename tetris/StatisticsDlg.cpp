#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "statistics.h"
#include ".\statisticsdlg.h"

extern CStatistic Statistics;

IMPLEMENT_DYNAMIC(CStatisticsDlg, CDialog)
CStatisticsDlg::CStatisticsDlg(CWnd* pParent) : CDialog(CStatisticsDlg::IDD, pParent)
{
}
CStatisticsDlg::~CStatisticsDlg()
{
}
void CStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CStatisticsDlg, CDialog)
END_MESSAGE_MAP()
// CStatisticsDlg message handlers

BOOL CStatisticsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("High scores"));
	CListCtrl* list = static_cast<CListCtrl*>(GetDlgItem(IDC_HIGH_SCORES_LIST)); 
	list->InsertColumn(0, _T("#"), LVCFMT_LEFT, 20);
	list->InsertColumn(1, _T("Player"), LVCFMT_LEFT, 100);
	list->InsertColumn(2, _T("Score"), LVCFMT_LEFT, 50);
	list->InsertColumn(3, _T("Lines"), LVCFMT_LEFT, 50);
	list->InsertColumn(4, _T("Bonus"), LVCFMT_LEFT, 50);
	list->InsertColumn(5, _T("Time"), LVCFMT_LEFT, 50);
	list->InsertColumn(6, _T("Date"), LVCFMT_LEFT, 80);
	CStatisticRecord record;
	CString s;
	for(int i=0;i<BEST_PLAYERS_MAX_COUNT;i++)
	{
		Statistics.GetRecord(i, record);
		if(record.Score > 0)
		{
			s.Format(_T("%d"), i+1);
			list->InsertItem(i, s.GetString());
			list->SetItemText(i, 1, record.PlayerName.GetString());
			s.Format(_T("%d"), record.Score);
			list->SetItemText(i, 2, s.GetString());
			s.Format(_T("%d"), record.Lines);
			list->SetItemText(i, 3, s.GetString());
			s.Format(_T("%d%%"), record.Bonus);
			list->SetItemText(i, 4, s.GetString());
			list->SetItemText(i, 5, record.Time.Format(_T("%M:%S")));
			list->SetItemText(i, 6, record.Date.Format(_T("%d %b %Y")));
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}
