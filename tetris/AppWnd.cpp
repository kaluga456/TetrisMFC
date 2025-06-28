#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "random.h"
#include "about.h"
#include "options.h"
#include "Rating.h"
#include "ScoresDB.h"
#include "TetrisEngine.h"
#include "Timer.h"
#include "Graphics.h"
#include "optionsdlg.h"
#include "appwnd.h"

extern COptions Options;
extern CWinApp* ThisApp;
extern CAppWnd* AppWnd;

tetris::engine TetrisGame;
//////////////////////////////////////////////////////////////////////////////
//debug flags
#ifdef _DEBUG
//#define DEBUG_NO_TICKS
//#define DEBUG_SHAPE
//////////////////////////////////////////////////////////////////////////////
//debug shape generator
#ifdef DEBUG_SHAPE
class debug_shape_generator : public tetris::shape_generator_t
{
public:
	void generate(tetris::shape_t& shape) { shape.reset(tetris::SHAPE_TYPE_O, 0x808080); }
};
static debug_shape_generator debug_generator;
#endif //DEBUG_SHAPE
#endif //_DEBUG
//////////////////////////////////////////////////////////////////////////////
//shape generator
static app::random_value<int> RandomColor(100, 255);
static app::random_value<int> RandomShape(0, tetris::SHAPE_TYPE_COUNT - 1);
class CShapeGenerator : public tetris::shape_generator_t
{
public:
	void generate(tetris::shape_t& shape)
	{
		const int r = RandomColor.get();
		const int g = RandomColor.get();
		const int b = RandomColor.get();
		tetris::block_t color = static_cast<tetris::block_t>(RGB(r, g, b));
		shape.reset(RandomShape.get(), color);
	}
};
static CShapeGenerator ShapeGenerator;
//////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAppWnd, CWnd)
	ON_COMMAND(ID_NEW_GAME, OnStartGame)
	ON_COMMAND(ID_ROTATE_LEFT, OnRotateLeft)
	ON_COMMAND(ID_ROTATE_RIGHT, OnRotateRight)
	ON_COMMAND(ID_DROP, OnDrop)
	ON_COMMAND(ID_PAUSE, OnPause)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_GAME_OPTIONS, OnGameOptions)
	ON_COMMAND(ID_GAME_DELETESCORES, OnClearRating)
	ON_COMMAND(ID_HIGHSCORES, OnRating)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, &CAppWnd::OnTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_CTRL, &CAppWnd::OnTabChanging)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////
//CAppWnd
CAppWnd::CAppWnd() : CFrameWnd(), 
	LeftKeyTimer{ LEFT_KEY_TIMER_ID }, 
	RightKeyTimer{ RIGHT_KEY_TIMER_ID }, 
	DownKeyTimer{ DOWN_KEY_TIMER_ID }
{
	LPCTSTR wndclass;
	try
	{
		wndclass = ::AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL, IDC_ARROW), 
			NULL, ::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(ID_MAIN_ICON)));
	}
	catch(CResourceException* pEx)
	{
		pEx->ReportError();
		pEx->Delete();
		return;
	}
	CRect r(Options.LayoutX, 
			Options.LayoutY, 
			Options.LayoutX + CGameTab::GetWidth(),
			Options.LayoutY + CGameTab::GetHeight());	//client rectangle
	Create(wndclass, APP_NAME, WS_CAPTION|WS_MINIMIZEBOX|WS_SYSMENU|WS_DLGFRAME, r, NULL, MAKEINTRESOURCE(IDR_MAIN_MENU), NULL);
	CalcWindowRect(r, CWnd::adjustBorder); //adjust window size

	//init game logic
	GameState = GS_NO_GAME;
#ifdef DEBUG_SHAPE
	TetrisGame.set_generator(&debug_generator);
#else
	TetrisGame.set_generator(&ShapeGenerator);
#endif

	//init timers
	ClockTimer.SetParent(m_hWnd);
	LeftKeyTimer.SetParent(m_hWnd);
	RightKeyTimer.SetParent(m_hWnd);
	DownKeyTimer.SetParent(m_hWnd);

	//get menu height
	MENUBARINFO mbi{};
	mbi.cbSize = sizeof(MENUBARINFO);
	GetMenuBarInfo(OBJID_MENU, NULL, &mbi);
	const int menu_height = mbi.rcBar.bottom - mbi.rcBar.top;
	r.bottom += menu_height;
	LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR));

	//init tab control
	CRect client_rect;
	GetClientRect(&client_rect);
	TabCtrl.Create(TCS_TABS | TCS_FIXEDWIDTH | WS_CHILD | WS_VISIBLE, client_rect, this, IDC_TAB_CTRL);
	TabCtrl.InsertItem(TAB_GAME, _T("Game"));
	TabCtrl.InsertItem(TAB_SCORES, _T("Scores"));

	//recalculate size
	CRect tab_item_rect;
	TabCtrl.GetItemRect(TAB_GAME, &tab_item_rect);
	r.bottom += tab_item_rect.Height();
	MoveWindow(Options.LayoutX, Options.LayoutY, r.Width(), r.Height());
	TabCtrl.SetWindowPos(NULL, 0, 0, r.Width(), r.Height(), SWP_NOMOVE | SWP_NOOWNERZORDER);
	
	//init tabs
	GetClientRect(&client_rect);
	client_rect.top += tab_item_rect.Height();
	GameTab.Init(client_rect, &TabCtrl);
	ScoreTab.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LV_VIEW_DETAILS, client_rect, &TabCtrl, IDC_SCORE_LIST);
	ScoreTab.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	//init score list columns
	ScoreTab.GetClientRect(client_rect);
	const int column_width = client_rect.Width() / COLUMN_COUNT;
	ScoreTab.InsertColumn(COLUMN_RANK, L"Rank", LVCFMT_LEFT, column_width);
	ScoreTab.InsertColumn(COLUMN_SCORE, L"Score", LVCFMT_LEFT, column_width);
	ScoreTab.InsertColumn(COLUMN_DATE, L"Date", LVCFMT_LEFT, column_width);

	OnTabChanged(NULL, NULL);

	//load best scores
	try
	{
		CScoresDB scores_db;
		scores_db.Load(Rating);
	}
	catch (CDBException& exc)
	{
		ShowWarningMessage(CString(L"Unable to load best scores:\n") + CString(exc.what()));
	}

	//TEST: fill scores
	//time_t rt;
	//::time(&rt);
	//for (int i = 0; i < CRating::MAX_SCORES; ++i)
	//{
	//	const int score = ::rand() % 200;
	//	rt = rt - static_cast<time_t>(::rand() % 3600);
	//	Rating.add(score, rt);
	//}

	UpdateScoresList();

	::AfxSetWindowText(m_hWnd, APP_FULL_NAME);
}
void CAppWnd::OnGameOver()
{
	ClockTimer.Kill();
	GameState = GS_GAME_OVER;
	//OnStartGame.GameView.OnGameOver();
	if (Rating.add(CurrentScore))
		UpdateScoresList();
	CurrentScore = 0;
}
void CAppWnd::OnRotateLeft()
{
	if(GameState == GS_RUNNING)
		ProcessResult(TetrisGame.rotate_left());
}
void CAppWnd::OnRotateRight()
{
	if(GameState == GS_RUNNING)
		ProcessResult(TetrisGame.rotate_right());
}
void CAppWnd::OnDrop()
{
	if(GameState == GS_RUNNING)
		ProcessResult(TetrisGame.drop());
}
static int CALLBACK ScoreListViewSort(LPARAM param1, LPARAM param2, LPARAM ascending)
{
	return param1 > param2;
}
void CAppWnd::UpdateScoresList()
{
	ScoreTab.DeleteAllItems();

	int rank = 1;
	for (const auto& i : Rating)
	{
		const int score = i.Score;
		const CDate& date = i.Date;

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
void CAppWnd::OnStartGame()
{
	if (QueryEndGame())
		StartGame();
}
void CAppWnd::StartGame()
{
	TetrisGame.new_game();

	CurrentScore = 0;
	GameState = GS_RUNNING;

	//init timers
	ClockTimer.Start(CLOCK_INTERVAL);
	const ULONGLONG ticks = ::GetTickCount64();
	TCTime.Start(ticks, UPDATE_INTERVAL);
	TCTick.Start(ticks, MAX_TICK_INTERVAL);
	GameTime.Start(ticks);

	GameTab.GameView.SetMainText();
	GameTab.ShapeView.SetShape(&TetrisGame.get_next_shape());
	GameTab.SetScore(0);
	GameTab.SetSpeed(0);
	GameTab.SetTime(0);
}
void CAppWnd::StopGame(int game_state)
{
	LeftKeyTimer.Kill();
	RightKeyTimer.Kill();
	DownKeyTimer.Kill();

	if (GS_GAME_OVER == game_state)
	{
		GameTab.GameView.SetMainText(L"GAME OVER");
		GameState = GS_GAME_OVER;
	}
	else if (GS_NO_GAME == game_state)
	{
		TetrisGame.clear();
		GameTab.GameView.SetMainText();
		GameTab.ShapeView.SetShape();
		GameTab.SetScore();
		GameTab.SetSpeed();
		GameTab.SetTime();
		GameState = GS_NO_GAME;
	}
	else
		ASSERT(FALSE);
}
void CAppWnd::Pause(bool pause /*= true*/)
{
	if(GS_PAUSED == GameState || GS_RUNNING == GameState)
	{
		const ULONGLONG ticks = ::GetTickCount64();
		if(pause)
		{
			GameState = GS_PAUSED;
			GameTime.Pause(ticks);
			TCTime.Pause(ticks);
			TCTick.Pause(ticks);
			GameTab.GameView.SetMainText(L"PAUSED");
		}
		else
		{
			GameState = GS_RUNNING;
			GameTime.Resume(ticks);
			TCTime.Resume(ticks);
			TCTick.Resume(ticks);
			GameTab.GameView.SetMainText();
		}
	}
}
void CAppWnd::OnPause()
{
	if(GameState == GS_RUNNING)
		Pause(true);
	else if(GameState == GS_PAUSED)
		Pause(false);
}
void CAppWnd::ProcessResult(int engine_result)
{
	switch (engine_result)
	{
	case tetris::RESULT_NONE:
		break;
	case tetris::RESULT_SHAPE:
	{
		GameTab.ShapeView.SetShape(&TetrisGame.get_next_shape());
		const int score = TetrisGame.get_score();
		if (CurrentScore != score)
		{
			CurrentScore = score;
			GameTab.SetScore(score);
		}
		GameTab.GameView.RePaint();
		break;
	}
	case tetris::RESULT_CHANGED:
		GameTab.GameView.RePaint();
		break;
	case tetris::RESULT_GAME_OVER:
		StopGame(GS_GAME_OVER);
		break;
	default:
		ASSERT(0);
	}
}
void CAppWnd::Exit()
{
	//save options
	CRect r;
	GetWindowRect(&r);
	Options.LayoutX = r.left;
	Options.LayoutY = r.top;

	//save rating
	try
	{
		CScoresDB scores_db;
		scores_db.Save(Rating);
	}
	catch (CDBException& exc)
	{
		ShowWarningMessage(CString(L"Unable to save best scores:\n") + CString(exc.what()));
	}

	CFrameWnd::OnClose();
}
bool CAppWnd::QueryEndGame()
{
	if (GS_PAUSED == GameState || GS_RUNNING == GameState)
	{
		if (GS_RUNNING == GameState)
			OnPause();

		if (ShowQeuryMessage(L"Stop current game?"))
		{
			if (Rating.add(CurrentScore))
				UpdateScoresList();
			return true;
		}

		if (GS_PAUSED == GameState)
			OnPause();

		return false;
	}
	return true;
}
void CAppWnd::OnExit()
{
	if(QueryEndGame())
		Exit();
}
void CAppWnd::OnClose()
{
	OnExit();
}
void CAppWnd::OnGameOptions()
{
	Pause();
	COptionsDlg options_dlg;
	options_dlg.DoModal();
}
void CAppWnd::OnRating()
{
	TabCtrl.SetCurSel(TAB_SCORES);
	OnTabChanged(NULL, NULL);
}
void CAppWnd::OnClearRating()
{
	if(false == ShowQeuryMessage(L"Delete best scores?"))
		return;

	Rating.clear();
	UpdateScoresList();

	try
	{
		CScoresDB scores_db;
		scores_db.Clear();
	}
	catch (CDBException& exc)
	{
		ShowWarningMessage(CString(L"Unable to delete best scores:n") + CString(exc.what()));
	}
}
void CAppWnd::OnAbout()
{
	Pause();
	LPCTSTR about_msg = APP_FULL_NAME \
		L"\n\n"
		L"Controls:\n"
		L"Move shape:\tASD or arrow keys\n"
		L"Rotate shape:\tW or UP\n"
		L"Drop shape:\tSPACE\n";
	MessageBox(about_msg, _T("About"), MB_OK | MB_ICONINFORMATION);
}
void CAppWnd::OnTabChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	//TODO:
	RECT rc;
	TabCtrl.GetItemRect(0, &rc);
	const int selected_tab = TabCtrl.GetCurSel();

	if (TAB_GAME == selected_tab)
	{
		GameTab.Show(TRUE);
		//OnStartGame.SetWindowPos(NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		ScoreTab.ShowWindow(SW_HIDE);
	}
	else if (TAB_SCORES == selected_tab)
	{
		GameTab.Show(FALSE);
		//OnStartGame.ShowWindow(SW_HIDE);
		ScoreTab.SetWindowPos(NULL, rc.left + 1, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else
		ASSERT(FALSE);

	//TODO:
	//pause/unpause game when tab switches
	//OnPause();
}
void CAppWnd::OnTabChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//TODO:
	//const int selected_tab = TabCtrl.GetCurSel();
	//if (TAB_GAME == selected_tab)
	//{
	//	GameTab.Show(FALSE);
	//	//OnStartGame.ShowWindow(SW_HIDE);
	//}
	//else if (TAB_SCORES == selected_tab)
	//{
	//	ScoreTab.ShowWindow(SW_HIDE);
	//}
	//else
	//	ASSERT(FALSE);
}
bool CAppWnd::ShowQeuryMessage(LPCTSTR message)
{
	const int result = ::AfxMessageBox(message, MB_OKCANCEL | MB_ICONEXCLAMATION, NULL);
	return IDOK == result;
}
void CAppWnd::ShowWarningMessage(LPCTSTR message)
{
	::AfxMessageBox(message, MB_ICONWARNING | MB_OK);
}
void CAppWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GS_RUNNING == GameState)
	{
		switch (nChar)
		{
		case VK_UP:
		case 'W':
			OnRotateLeft();
			break;

		//make first move immediately
		case VK_LEFT:
		case 'A':
			if(LeftKeyTimer.Start())
				ProcessResult(TetrisGame.move_left());
			break;
		case VK_RIGHT:
		case 'D':
			if(RightKeyTimer.Start())
				ProcessResult(TetrisGame.move_right());
			break;
		case VK_DOWN:
		case 'S':
			if(DownKeyTimer.Start())
				ProcessResult(TetrisGame.move_down());
			break;
		}
	}
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CAppWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
	case 'A':
		LeftKeyTimer.Kill();
		break;
	case VK_RIGHT:
	case 'D':
		RightKeyTimer.Kill();
		break;
	case VK_DOWN:
	case 'S':
		DownKeyTimer.Kill();
		break;
	}

	CFrameWnd::OnKeyUp(nChar, nRepCnt, nFlags);
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

		const ULONGLONG ticks = ::GetTickCount64();
		if (TCTime.Check(ticks))
		{
			GameTime.Update(ticks);
			GameTab.SetTime(GameTime.Get());
			GameTab.SetSpeed(TCTick.GetSpeed()); //TODO: on speed change
		}
		if (TCTick.Check(ticks))
		{
#ifndef DEBUG_NO_TICKS
			ProcessResult(TetrisGame.move_down());
#endif
		}
		break;
	}
	case LEFT_KEY_TIMER_ID:
		if (GS_RUNNING == GameState)
			ProcessResult(TetrisGame.move_left());
		break;
	case RIGHT_KEY_TIMER_ID:
		if (GS_RUNNING == GameState)
			ProcessResult(TetrisGame.move_right());
		break;
	case DOWN_KEY_TIMER_ID:
		if (GS_RUNNING == GameState)
			ProcessResult(TetrisGame.move_down());
		break;

	default:
		ASSERT(FALSE);
	}

	CFrameWnd::OnTimer(nIDEvent);
}