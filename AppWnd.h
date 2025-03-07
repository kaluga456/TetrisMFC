#pragma once

constexpr UINT ACCELERATION = 3;
constexpr UINT APPWND_PADDING = 8;
constexpr UINT APPWND_WIDTH = (APPWND_PADDING + GAME_FIELD_VIEW_WIDTH + APPWND_PADDING + TEXT_VIEW_WIDTH + APPWND_PADDING);
constexpr UINT APPWND_HEIGHT = (APPWND_PADDING + GAME_FIELD_VIEW_HEIGHT + APPWND_PADDING);

class CAppWnd : public CFrameWnd
{
DECLARE_MESSAGE_MAP ()
public:
	CAppWnd();
	~CAppWnd();

	static void EventsProcedure(int event, int param);

	//GameField event handlers
	void OnNewShape();
	void OnShapeMove();
	void OnShapeLanded();
	void OnLinesDelete(int lines_count);
	void OnGameOver();

	//system and user event handlers
	afx_msg void OnPaint();
	afx_msg void OnNewGame();
	afx_msg void OnRotateLeft();
	afx_msg void OnRotateRight();
	afx_msg void OnDrop();
	afx_msg void OnPause();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnExit();
	afx_msg void OnAbout();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGameOptions();
	afx_msg void OnHighscores();

private:
	//timers
	CMoveKeyTimer MoveKeyTimer;
	CTimer ClockTimer{ CLOCK_TIMER_ID };

	//DEPRECATE:
	CGameTimer GameTimer;

	//counters
	CGameTime GameTime; //time counter
	CTickCounter TCTime;  //time view update
	CGameTick TCTick;  //game tick event

	int GameState{GS_NO_GAME};
	CGameField GameField;

	//controls
	CTextView* LinesLabel;
	CTextView* LinesView;
	CTextView* SpeedLabel;
	CTextView* SpeedView;
	CTextView* TimeLabel;
	CTextView* TimeView;
	CGameFieldView* GameFieldView;
	CNextShapeView* NextShapeView;

	void Pause(bool pause);
	bool QueryFinishCurrentGame() const;
	void Exit();
};
