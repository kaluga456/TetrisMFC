#pragma once

constexpr UINT ACCELERATION = 3;
constexpr UINT APPWND_SPACING = 8;
constexpr UINT APPWND_WIDTH = (APPWND_SPACING + GAME_FIELD_VIEW_WIDTH + APPWND_SPACING + TEXT_VIEW_WIDTH + APPWND_SPACING);
constexpr UINT APPWND_HEIGHT = (APPWND_SPACING + GAME_FIELD_VIEW_HEIGHT + APPWND_SPACING);

enum : UINT_PTR
{
	MOVE_TIMER_ID = 94567,
	TIME_TIMER_ID = 56304,
	SPEED_TIMER_ID = 19483
};

//move key repetition timer
class CMoveKeyTimer
{
public:
	~CMoveKeyTimer() { ::KillTimer(Parent, MOVE_TIMER_ID); }

	void SetParent(HWND parent) { Parent = parent; }

	//access
	UINT GetMoveType() const;
	bool IsMoveKey(UINT key) const;

	//start|stop
	bool OnKeyDown(UINT key); //return true if timer started
	void OnKeyUp(UINT key);

private:
	HWND Parent{};
	UINT_PTR Timer{};
	UINT Key{}; //pressed move key or 0
};

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
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExit();
	afx_msg void OnAbout();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGameOptions();
	afx_msg void OnHighscores();

private:
	//timers
	int Speed; //msec
	UINT TimeTimerId;
	UINT SpeedTimerId;
	CMoveKeyTimer MoveKeyTimer;
	CGameTimer GameTimer;

	int GameState{GS_NO_GAME};
	CGameField GameField;

	//controls
	CTextView* ScoreLabel;
	CTextView* ScoreView;
	CTextView* LinesLabel;
	CTextView* LinesView;
	CTextView* BonusLabel;
	CTextView* BonusView;
	CTextView* TimeLabel;
	CTextView* TimeView;
	CGameFieldView* GameFieldView;
	CNextShapeView* NextShapeView;

	void Pause(bool pause);
	bool QueryFinishCurrentGame() const;
	void Exit();
};
