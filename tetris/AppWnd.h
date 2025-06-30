#pragma once
//////////////////////////////////////////////////////////////////////////////
class CAppWnd : public CFrameWnd
{
DECLARE_MESSAGE_MAP ()
public:
	CAppWnd();
	~CAppWnd() {}

	//system and user event handlers
	afx_msg void OnStartGame();
	afx_msg void OnRotateLeft();
	afx_msg void OnRotateRight();
	afx_msg void OnDrop();
	afx_msg void OnPause();
	afx_msg void OnExit();
	afx_msg void OnAbout();
	afx_msg void OnClose();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGameOptions();
	afx_msg void OnRating();
	afx_msg void OnClearRating();
	afx_msg void OnTabChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTabChanging(NMHDR* pNMHDR, LRESULT* pResult);

	//timer events
	afx_msg void OnTimer(UINT_PTR nIDEvent);

private:
	//tabs
	enum : int
	{
		TAB_GAME,
		TAB_SCORES,

		TAB_COUNT
	};

	//score list columns
	enum : int
	{
		COLUMN_RANK,
		COLUMN_SCORE,
		COLUMN_DATE,

		COLUMN_COUNT
	};

	//timers
	CTimer ClockTimer{ CLOCK_TIMER_ID };
	CMoveKeyTimer LeftKeyTimer;
	CMoveKeyTimer RightKeyTimer;
	CMoveKeyTimer DownKeyTimer;

	//counters
	CGameTime GameTime; //time counter
	CTickCounter TCTime;  //time view update
	CGameTick TCTick;  //game tick event

	//game
	enum : int
	{
		//game state
		GS_NO_GAME,
		GS_GAME_OVER,
		GS_RUNNING,
		GS_PAUSED
	};
	int GameState{GS_NO_GAME};

	//score
	int Score{0};
	int Lines{0};

	//tabs
	CTabCtrl TabCtrl;
	CListCtrl ScoreTab;
	CGameTab GameTab;

	void UpdateScoresList();

	void StartGame();
	void StopGame(int game_state);
	void Pause(bool pause = true);

	void ProcessResult(int engine_result);
	bool QueryEndGame();
	void Exit();

	//dialogs
	bool ShowQeuryMessage(LPCTSTR message);
	void ShowWarningMessage(LPCTSTR message);
};
//////////////////////////////////////////////////////////////////////////////