#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "random.h"
#include "about.h"
#include "options.h"
#include "BestScores.h"
#include "ScoresDB.h"
#include "logic.h"
#include "Timer.h"
#include "controls.h"
#include "optionsdlg.h"
#include "appwnd.h"

extern COptions Options;
extern CWinApp* ThisApp;
extern CAppWnd* AppWnd;
CGameField GameField;

static app::random_value<int> RandomColor(100, 255);

BEGIN_MESSAGE_MAP(CAppWnd, CWnd)
	ON_COMMAND(ID_NEW_GAME, OnNewGame)
	ON_COMMAND(ID_ROTATE_LEFT, OnRotateLeft)
	ON_COMMAND(ID_ROTATE_RIGHT, OnRotateRight)
	ON_COMMAND(ID_DROP, OnDrop)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_WM_TIMER()
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_GAME_OPTIONS, OnGameOptions)
	ON_COMMAND(ID_GAME_DELETESCORES, OnClearScores)
	ON_COMMAND(ID_HIGHSCORES, OnHighscores)
	ON_REGISTERED_MESSAGE(AFX_WM_CHANGING_ACTIVE_TAB, &CAppWnd::OnTabCtrl)
END_MESSAGE_MAP()

CAppWnd::CAppWnd() : CFrameWnd()
{
	LPCTSTR wndclass;
	try
	{
		wndclass = ::AfxRegisterWndClass(
			CS_HREDRAW | CS_VREDRAW, 
			::LoadCursor(NULL, IDC_ARROW), 
			NULL,
			//(HBRUSH)(COLOR_BTNFACE+1),
			::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_MAIN_ICON)));
	}
	catch(CResourceException* pEx)
	{
		pEx->ReportError();
		pEx->Delete();
		return;
	}
	CRect r(	Options.LayoutX, 
				Options.LayoutY, 
				Options.LayoutX + APPWND_WIDTH, 
				Options.LayoutY + APPWND_HEIGHT);	//client rectangle
	Create(wndclass, APP_NAME, WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_DLGFRAME, r, NULL, MAKEINTRESOURCE(IDR_MAIN_MENU), NULL);
	CalcWindowRect(r, CWnd::adjustBorder); //adjust window size

	//get menu height
	MENUBARINFO mbi{};
	mbi.cbSize = sizeof(MENUBARINFO);
	GetMenuBarInfo(OBJID_MENU, NULL, &mbi);
	r.bottom += mbi.rcBar.bottom - mbi.rcBar.top;
	MoveWindow(Options.LayoutX, Options.LayoutY, r.Width(), r.Height());
	LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR));

	//init game values
	GameState = GS_NO_GAME;
	GameField.Initialize(GetBLockContext, EventsProcedure);

	//init timers
	ClockTimer.SetParent(m_hWnd);
	MoveKeyTimer.SetParent(m_hWnd);

	//init tab control
	CRect client_rect;
	GetClientRect(&client_rect);
	TabCtrl.Create(CMFCTabCtrl::STYLE_3D, client_rect, this, IDC_TAB_CTRL, CMFCTabCtrl::LOCATION_TOP);
	TabCtrl.SetActiveTabBoldFont();
	GameTab.Create(_T(""), WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, CRect(0, 0, 0, 0), &TabCtrl);
	ScoreTab.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LV_VIEW_DETAILS, CRect(0, 0, 0, 0), &TabCtrl, IDC_SCORE_LIST);
	ScoreTab.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	TabCtrl.AddTab(&GameTab, _T("Game"));
	TabCtrl.AddTab(&ScoreTab, _T("Results"));
	
	//init score list columns
	ScoreTab.GetClientRect(client_rect);
	const int column_width = client_rect.Width() / COLUMN_COUNT;
	ScoreTab.InsertColumn(COLUMN_RANK, L"Rank", LVCFMT_LEFT, column_width);
	ScoreTab.InsertColumn(COLUMN_SCORE, L"Score", LVCFMT_LEFT, column_width);
	ScoreTab.InsertColumn(COLUMN_DATE, L"Date", LVCFMT_LEFT, column_width);

	//recalculate window height
	CRect rectTabAreaTop;
	CRect rectTabAreaBottom;
	TabCtrl.GetTabArea(rectTabAreaTop, rectTabAreaBottom);
	const int tab_height = rectTabAreaTop.Height() + rectTabAreaBottom.Height();
	CRect wnd_rect;
	GetWindowRect(wnd_rect);
	SetWindowPos(NULL, wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height() + tab_height, SWP_NOMOVE | SWP_NOOWNERZORDER);
	TabCtrl.GetWindowRect(wnd_rect);
	TabCtrl.SetWindowPos(NULL, wnd_rect.left, wnd_rect.top, wnd_rect.Width(), wnd_rect.Height() + tab_height, SWP_NOMOVE | SWP_NOOWNERZORDER);

	//load best scores
	try
	{
		CScoresDB scores_db;
		scores_db.Load(BestScores);
	}
	catch (CDBException& exc)
	{
		CString msg(L"Unable to load best scores:\n");
		msg += CString(exc.what());
		::AfxMessageBox(msg, MB_ICONWARNING | MB_OK);
	}

	//TEST: fill scores
	//time_t rt;
	//::time(&rt);
	//for (int i = 0; i < CBestScores::MAX_RESULTS; ++i)
	//{
	//	const int score = ::rand() % 200;
	//	rt = rt - static_cast<time_t>(::rand() & 3600);
	//  BestScores.add(score, rt);
	//}

	UpdateScoresList();
}
CAppWnd::~CAppWnd()
{
}
block_t CAppWnd::GetBLockContext()
{
	const int r = RandomColor.get();
	const int g = RandomColor.get();
	const int b = RandomColor.get();
	return static_cast<block_t>(RGB(r, g, b));
}
void CAppWnd::EventsProcedure(int event, int param)
{
	ASSERT(AppWnd);
	switch(event)
	{
	case CGameField::ON_NEW_SHAPE:
		AppWnd->OnNewShape();
		break;
	case CGameField::ON_SHAPE_MOVE:
		AppWnd->OnShapeMove();
		break;
	case CGameField::ON_SHAPE_LANDED:
		AppWnd->OnShapeLanded();
		break;
	case CGameField::ON_LINE_DELETE:
		AppWnd->OnLineDelete(param);
		break;
	case CGameField::ON_LINES_DELETE:
		AppWnd->OnLinesDelete(param);
		break;
	case CGameField::ON_GAME_OVER:
		AppWnd->OnGameOver();
		break;
	}
}
void CAppWnd::OnNewShape()
{
	GameTab.NextShapeView->SetShape(GameField.GetNextShape());
}
void CAppWnd::OnShapeMove()
{
	GameTab.GameFieldView->OnShapeMove();
}
void CAppWnd::OnShapeLanded()
{
	GameTab.GameFieldView->OnShapeLanded();
}
void CAppWnd::OnLineDelete(int y_coord)
{
	//TEST:
	//++CurrentScore;
	//GameFieldView->RePaint();
	//LinesView->SetText(CurrentScore);
}
void CAppWnd::OnLinesDelete(int lines_count)
{
	CurrentScore += lines_count;
	GameTab.GameFieldView->OnLinesDelete();
	GameTab.LinesView->SetText(CurrentScore);
}
void CAppWnd::OnGameOver()
{
	ClockTimer.Kill();
	GameState = GS_GAME_OVER;
	GameTab.GameFieldView->OnGameOver();
	if (BestScores.add(CurrentScore))
		UpdateScoresList();
	CurrentScore = 0;
}
void CAppWnd::OnNewGame()
{
	if (false == QueryEndGame())
		return;

	//init timers
	ClockTimer.Start(CLOCK_INTERVAL);
	const DWORD ticks = ::GetTickCount();
	TCTime.Start(ticks, UPDATE_INTERVAL);
	TCTick.Start(ticks, MAX_TICK_INTERVAL);
	GameTime.Start(ticks);
	
	CurrentScore = 0;
	GameField.OnNewGame();
	GameTab.GameFieldView->OnNewGame();
	GameState = GS_RUNNING;
	GameTab.LinesView->SetText(L"0");
	GameTab.SpeedView->SetText(L"+0%");
	GameTab.TimeView->SetTime(0);
}
void CAppWnd::OnRotateLeft()
{
	if(GameState == GS_RUNNING)
		GameField.OnMoveShape(MT_ROTATE_LEFT);
}
void CAppWnd::OnRotateRight()
{
	if(GameState == GS_RUNNING)
		GameField.OnMoveShape(MT_ROTATE_RIGHT);
}
void CAppWnd::OnDrop()
{
	if(GameState == GS_RUNNING)
		GameField.OnDrop();
}
static int CALLBACK ScoreListViewSort(LPARAM param1, LPARAM param2, LPARAM ascending)
{
	return param1 > param2;
}
void CAppWnd::UpdateScoresList()
{
	ScoreTab.DeleteAllItems();

	int rank = 1;
	for (const auto& i : BestScores)
	{
		const int score = i.first;
		const CDate& date = i.second;

		LVITEM lvi;
		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.mask = LVIF_TEXT | LVIF_PARAM;
		lvi.lParam = static_cast<LPARAM>(rank);

		CString str;
		str.Format(L"%u", rank);
		lvi.pszText = str.GetBuffer();
		const int item_index = ScoreTab.InsertItem(&lvi);

		str.Format(L"%u", score);
		ScoreTab.SetItemText(item_index, COLUMN_SCORE, str.GetString());

		CTime time(static_cast<__time64_t>(date));
		CString time_str = time.Format(L"%H:%M %d.%m.%Y");
		ScoreTab.SetItemText(item_index, COLUMN_DATE, time_str);

		++rank;
	}

	ScoreTab.SortItems(ScoreListViewSort, NULL);
}
void CAppWnd::Pause(bool pause)
{
	if(GS_PAUSED == GameState || GS_RUNNING == GameState)
	{
		const DWORD ticks = ::GetTickCount();
		if(pause)
		{
			GameState = GS_PAUSED;
			GameTime.Pause(ticks);
			TCTime.Pause(ticks);
			TCTick.Pause(ticks);
		}
		else
		{
			GameState = GS_RUNNING;
			GameTime.Resume(ticks);
			TCTime.Resume(ticks);
			TCTick.Resume(ticks);
		}

		//TODO:
		GameTab.GameFieldView->OnPause(pause);
	}
}
void CAppWnd::OnPause()
{
	if(GameState == GS_RUNNING)
		Pause(true);
	else if(GameState == GS_PAUSED)
		Pause(false);
}
void CAppWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case CLOCK_TIMER_ID:
	{
		if (GameState != GS_RUNNING)
			break;

		//TODO: keep focus on main window to process WM_KEY_DOWN/UP events
		SetFocus();

		const DWORD ticks = ::GetTickCount();
		if (TCTime.Check(ticks))
		{
			GameTime.Update(ticks);
			GameTab.TimeView->SetTime(GameTime.Get());

			const UINT speed = TCTick.GetSpeed();
			CString text;
			text.Format(L"+%u%%", speed);
			GameTab.SpeedView->SetText(text);
		}
		if (TCTick.Check(ticks))
		{
			GameField.OnTick();
		}

		break;
	}
	case MOVE_TIMER_ID:
	{
		//TODO: 3 timers for each move
		if (GS_RUNNING == GameState)
			GameField.OnMoveShape(MoveKeyTimer.GetMoveType());
		break;
	}
	}

	CFrameWnd::OnTimer(nIDEvent);
}
void CAppWnd::Exit()
{
	CRect r;
	GetWindowRect(&r);
	Options.LayoutX = r.left;
	Options.LayoutY = r.top;

	CFrameWnd::OnClose();
}
bool CAppWnd::QueryEndGame()
{
	if(GS_PAUSED == GameState || GS_RUNNING == GameState)
	{
		if(GS_RUNNING == GameState)
			OnPause();

		if (AfxMessageBox(_T("Stop current game?"), MB_OKCANCEL | MB_ICONEXCLAMATION, NULL) == IDOK)
		{
			if (BestScores.add(CurrentScore))
				UpdateScoresList();
			return true;
		}
		if(GS_PAUSED == GameState)
			OnPause();

		return false;
	}
	return true;
}
void CAppWnd::OnExit()
{
	if(GS_PAUSED == GameState || GS_RUNNING == GameState)
	{
		if(QueryEndGame())
			Exit();
	}
	else
		Exit();

	//save best scores
	try
	{
		CScoresDB scores_db;
		scores_db.Save(BestScores);
	}
	catch (CDBException& exc)
	{
		CString msg(L"Unable to save best scores:\n");
		msg += CString(exc.what());
		::AfxMessageBox(msg, MB_ICONWARNING | MB_OK);
	}
}
void CAppWnd::OnAbout()
{
	LPCTSTR about_msg = APP_NAME L"\nBuild: " APP_BUILD\
		L"\n\n"
		L"Controls:\n"
		L"Move shape:\tASD or arrow keys\n"
		L"Rotate shape:\tW or UP\n"
		L"Drop shape:\tSPACE\n";
	MessageBox(about_msg, _T("About"), MB_OK | MB_ICONINFORMATION);
}
void CAppWnd::OnClose()
{
	OnExit();
}
void CAppWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(GS_RUNNING == GameState)
	{
		if('W' == nChar)
			OnRotateLeft();
		else
		{
			if(MoveKeyTimer.OnKeyDown(nChar)) //make first move immediately
				GameField.OnMoveShape(MoveKeyTimer.GetMoveType());
		}
	}
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CAppWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MoveKeyTimer.OnKeyUp(nChar);
	CFrameWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
void CAppWnd::OnGameOptions()
{
	COptionsDlg options_dlg;
	options_dlg.DoModal();
}
void CAppWnd::OnHighscores()
{
	TabCtrl.SetActiveTab(TAB_SCORES);
}
void CAppWnd::OnClearScores()
{
	if (IDOK != ::AfxMessageBox(L"Delete best scores?", MB_ICONWARNING | MB_OKCANCEL))
		return;

	BestScores.clear();
	try
	{
		CScoresDB scores_db;
		scores_db.Clear();
	}
	catch (CDBException& exc)
	{
		CString msg(L"Unable to delete best scores:\n");
		msg += CString(exc.what());
		::AfxMessageBox(msg, MB_ICONWARNING | MB_OK);
	}

	UpdateScoresList();
}
LRESULT CAppWnd::OnTabCtrl(WPARAM wParam, LPARAM lParam)
{
	//pause/unpause game when tab switches
	OnPause();
	return FALSE;
}

