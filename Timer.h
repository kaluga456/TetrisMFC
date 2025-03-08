#pragma once

//timer IDs
enum : UINT_PTR
{
	MOVE_TIMER_ID = 94567,
	CLOCK_TIMER_ID = 73966,
};

//intervals, ms
constexpr UINT CLOCK_INTERVAL = 50;
constexpr UINT UPDATE_INTERVAL = 1000;

//tick counter for clock timer
class CTickCounter
{
public:
	explicit CTickCounter(UINT interval = 0) : Interval(interval), PausedTicks{0} {}

	void Start(DWORD ticks, UINT interval) { Ticks = ticks; Interval = interval; PausedTicks = 0; }
	void Stop() { Interval = 0; }

	bool Check(DWORD ticks);

	void Pause(DWORD ticks);
	void Resume(DWORD ticks);

protected:
	UINT Interval{ 0 };
	DWORD Ticks{ 0 };
	DWORD PausedTicks{ 0 };
};

//game tick counter
constexpr UINT MIN_TICK_INTERVAL = CLOCK_INTERVAL;
constexpr UINT MAX_TICK_INTERVAL = 1000;
constexpr UINT SPEED_BOOST_TIME = 600 * 1000; //time from game start to max speed
constexpr UINT SPEED_BOOST_INTERVAL = SPEED_BOOST_TIME / 100; //increase game speed interval
constexpr UINT SPEED_BOOST_VAL = MAX_TICK_INTERVAL / 100;
class CGameTick : public CTickCounter
{
public:
	CGameTick() : CTickCounter(MAX_TICK_INTERVAL) {}

	bool Check(DWORD ticks);
	void Pause(DWORD ticks);
	void Resume(DWORD ticks);

	UINT GetSpeed() const; //in %

private:
	DWORD BoostTicks{ 0 };
	DWORD PausedBoostTicks{ 0 };
};

class CGameTime
{
public:
	CGameTime() : Ticks{ 0 }, PausedTicks{ 0 }, Time{ 0 } {}

	void Start(DWORD ticks) { Ticks = ticks;  PausedTicks = 0; Time = 0; }
	void Update(DWORD ticks);
	void Pause(DWORD ticks) { PausedTicks = ticks - Ticks; }
	void Resume(DWORD ticks) { Ticks = ticks - PausedTicks; }

	UINT Get() const { return Time; }

private:
	DWORD Ticks;
	DWORD PausedTicks{ 0 };
	UINT Time; //ms
};


//window timer
class CTimer
{
public:
	explicit CTimer(UINT_PTR id) : Id{ id } {}
	virtual ~CTimer() { Kill(); }

	void SetParent(HWND parent);
	void Start(UINT interval);
	void Kill();

protected:
	HWND Parent{ nullptr };
	UINT_PTR Timer{ 0 };
	UINT_PTR Id{ 0 };
};

//move key repetition timer
class CMoveKeyTimer : public CTimer
{
public:
	CMoveKeyTimer() : CTimer(MOVE_TIMER_ID) {}
	~CMoveKeyTimer() { ::KillTimer(Parent, MOVE_TIMER_ID); }

	//access
	UINT GetMoveType() const;
	bool IsMoveKey(UINT key) const;

	//start|stop
	bool OnKeyDown(UINT key); //return true if timer started
	void OnKeyUp(UINT key);

protected:
	UINT Key{}; //pressed move key or 0
};

struct CGameTimer
{
public:
	void OnNewGame();
	void OnPause(bool pause);
	CTimeSpan GetGameSpan() const;
	CTime GetStartTime() const;
	CString GetGameSpanString() const;

private:
	CTime StartTime;
	CTime PauseTime;
	CTimeSpan PauseSpan;
};