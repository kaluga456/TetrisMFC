#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "about.h"
#include "options.h"
#include "statistics.h"
#include "logic.h"
#include "Timer.h"
#include "controls.h"
#include "StatisticsDlg.h"
#include "PlayerNameDlg.h"
#include "optionsdlg.h"
#include "appwnd.h"

extern COptions Options;
extern CStatistic Statistics;
extern CAppWnd* pAppWnd;


BEGIN_MESSAGE_MAP(CAppWnd, CWnd)
	ON_WM_PAINT()
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
	ON_COMMAND(ID_HIGHSCORES, OnHighscores)
END_MESSAGE_MAP()

CAppWnd::CAppWnd() : CFrameWnd()
{
	LPCTSTR wndclass;
	try
	{
		wndclass = AfxRegisterWndClass(
			CS_HREDRAW | CS_VREDRAW, 
			::LoadCursor(NULL, IDC_ARROW), 
			(HBRUSH)(COLOR_BTNFACE+1),
			::LoadIcon(NULL, MAKEINTRESOURCE(ID_MAIN_ICON)));
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
	GameField.Initialize(EventsProcedure);

	//init timers
	ClockTimer.SetParent(m_hWnd);
	MoveKeyTimer.SetParent(m_hWnd);

	//create controls
	GameFieldView = new CGameFieldView(this, 
		APPWND_PADDING, 
		APPWND_PADDING, &GameField);
	NextShapeView = new CNextShapeView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING,
		APPWND_PADDING );
	LinesLabel = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 1*APPWND_PADDING + 1*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"LINES");
	LinesView = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 1*APPWND_PADDING + 2*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
	SpeedLabel = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 3*APPWND_PADDING + 3*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"SPEED");
	SpeedView = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 3*APPWND_PADDING + 4*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
	TimeLabel = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 5*APPWND_PADDING + 5*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"TIME");
	TimeView = new CTextView(this, 
		APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING, 
		NEXT_SHAPE_VIEW_HEIGHT + 5*APPWND_PADDING + 6*TEXT_VIEW_HEIGHT, 
		TEXT_VIEW_WIDTH, TEXT_VIEW_HEIGHT, L"");
}
CAppWnd::~CAppWnd()
{
	delete LinesView;
	delete LinesLabel;
	delete SpeedView;
	delete SpeedLabel;
	delete TimeLabel;
	delete TimeView;
	delete GameFieldView;
	delete NextShapeView;
}
void CAppWnd::EventsProcedure(int event, int param)
{
	ASSERT(pAppWnd);
	switch(event)
	{
	case CGameField::ON_NEW_SHAPE:
		pAppWnd->OnNewShape();
		break;
	case CGameField::ON_SHAPE_MOVE:
		pAppWnd->OnShapeMove();
		break;
	case CGameField::ON_SHAPE_LANDED:
		pAppWnd->OnShapeLanded();
		break;
	case CGameField::ON_LINES_DELETE:
		pAppWnd->OnLinesDelete(param);
		break;
	case CGameField::ON_GAME_OVER:
		pAppWnd->OnGameOver();
		break;
	}
}
void CAppWnd::OnNewShape()
{
	NextShapeView->SetShape(GameField.GetNextShape());
}
void CAppWnd::OnShapeMove()
{
	GameFieldView->OnShapeMove();
}
void CAppWnd::OnShapeLanded()
{
	GameFieldView->OnShapeLanded();	
}
void CAppWnd::OnLinesDelete(int lines_count)
{
	GameFieldView->OnLinesDelete();
	LinesView->SetText(GameField.GetLinesCount());
	CString s;
	s.Format(_T("+%d%%"), GameField.GetBonusPercent());
	SpeedView->SetText(s);
}
void CAppWnd::OnGameOver()
{
	ClockTimer.Kill();
	GameState = GS_GAME_OVER;
	GameFieldView->OnGameOver();
	if(GameField.Score > Statistics.GetWorstScore())	//add player to statistics
	{
		CString player;
		CStatisticRecord record;
		CPlayerNameDlg dlg(this);
		dlg.DoModal();
		record.PlayerName = dlg.PlayerName;
		record.Score = GameField.Score;
		record.Lines = GameField.LinesCount;
		record.Bonus = GameField.BonusPercent;
		record.Time = GameTimer.GetGameSpan();
		record.Date = GameTimer.GetStartTime();
		Statistics.AddRecord(record);
		CStatisticsDlg statdlg(this);
		statdlg.DoModal();
	}
}
void CAppWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	GameFieldView->OnPaint(&dc);
	NextShapeView->OnPaint(&dc);
	LinesView->OnPaint(&dc);
	LinesLabel->OnPaint(&dc);
	SpeedView->OnPaint(&dc);
	SpeedLabel->OnPaint(&dc);
	TimeLabel->OnPaint(&dc);
	TimeView->OnPaint(&dc);
}
void CAppWnd::OnNewGame()
{
	if (false == QueryFinishCurrentGame())
		return;

	//init timers
	ClockTimer.Start(CLOCK_INTERVAL);
	const DWORD ticks = ::GetTickCount();
	TCTime.Start(ticks, UPDATE_INTERVAL);
	TCTick.Start(ticks, MAX_TICK_INTERVAL);
	GameTime.Start(ticks);
		
	GameField.OnNewGame();
	GameFieldView->OnNewGame();
	NextShapeView->OnPause(false);
	GameState = GS_RUNNING;
	LinesView->SetText(L"0");
	SpeedView->SetText(L"+0%");
	TimeView->SetTime(0);
}
void CAppWnd::OnRotateLeft()
{
	if(GameState == GS_RUNNING)
		GameField.OnShapeMove(MT_ROTATE_LEFT);
}
void CAppWnd::OnRotateRight()
{
	if(GameState == GS_RUNNING)
		GameField.OnShapeMove(MT_ROTATE_RIGHT);
}
void CAppWnd::OnDrop()
{
	if(GameState == GS_RUNNING)
		GameField.OnDrop();
}
void CAppWnd::Pause(bool pause)
{
	if((GameState == GS_PAUSED)||(GameState == GS_RUNNING))
	{
		if (pause)
		{
			GameState = GS_PAUSED;

			const DWORD ticks = ::GetTickCount();
			GameTime.Pause(ticks);
			TCTime.Pause(ticks);
			TCTick.Pause(ticks);
		}
		else
		{
			GameState = GS_RUNNING;

			const DWORD ticks = ::GetTickCount();
			GameTime.Resume(ticks);
			TCTime.Resume(ticks);
			TCTick.Resume(ticks);
		}
		GameTimer.OnPause(pause);
		GameFieldView->OnPause(pause);
		NextShapeView->OnPause(pause);
	}
}
void CAppWnd::OnPause()
{
	if (GameState == GS_RUNNING)
		Pause(true);
	else if (GameState == GS_PAUSED)
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

		const DWORD ticks = ::GetTickCount();
		if (TCTime.Check(ticks))
		{
			GameTime.Update(ticks);
			TimeView->SetTime(GameTime.Get());

			const UINT speed = TCTick.GetSpeed();
			CString text;
			text.Format(L"+%u%%", speed);
			SpeedView->SetText(text);
		}
		if (TCTick.Check(ticks))
		{
			GameField.OnTimer();
		}

		break;
	}
	case MOVE_TIMER_ID:
	{
		//TODO: 3 timers for each move
		if (GS_RUNNING == GameState)
			GameField.OnShapeMove(MoveKeyTimer.GetMoveType());
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
bool CAppWnd::QueryFinishCurrentGame() const
{
	if((GameState == GS_PAUSED)||(GameState == GS_RUNNING))
	{
		if(AfxMessageBox(_T("Finish current game?"), MB_OKCANCEL|MB_ICONEXCLAMATION, NULL)==IDOK)
			return true;
		return false;
	}
	return true;
}
void CAppWnd::OnExit()
{
	if((GameState == GS_PAUSED)||(GameState == GS_RUNNING))
	{
		if(QueryFinishCurrentGame())
			Exit();
	}
	else
		Exit();
}
void CAppWnd::OnAbout()
{
	MessageBox(APP_NAME L"\nBuild: " APP_BUILD, _T("About"), MB_OK | MB_ICONINFORMATION);
}
void CAppWnd::OnClose()
{
	OnExit();
}
void CAppWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (GS_RUNNING == GameState)
	{
		if ('W' == nChar)
			OnRotateLeft();
		else
		{
			if(MoveKeyTimer.OnKeyDown(nChar)) //make first move immediately
				GameField.OnShapeMove(MoveKeyTimer.GetMoveType());
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
	CStatisticsDlg dlg;
	dlg.DoModal();
}