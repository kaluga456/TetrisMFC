#pragma once
//////////////////////////////////////////////////////////////////////////////
//TODO: shape movement types
enum : int
{
	MT_UNDEFINED = 0,

	MT_MOVE_DOWN,
	MT_MOVE_RIGHT,
	MT_MOVE_LEFT,

	MT_ROTATE_LEFT,
	MT_ROTATE_RIGHT
};
//////////////////////////////////////////////////////////////////////////////
//timer IDs
enum : UINT_PTR
{
	CLOCK_TIMER_ID = 73966,
	LEFT_KEY_TIMER_ID = 94568,
	RIGHT_KEY_TIMER_ID = 94569,
	DOWN_KEY_TIMER_ID = 94570,
};
//////////////////////////////////////////////////////////////////////////////
//intervals, ms
constexpr UINT CLOCK_INTERVAL = 50;
constexpr UINT UPDATE_INTERVAL = 1000;
//////////////////////////////////////////////////////////////////////////////
//tick counter for clock timer
class CTickCounter
{
public:
	explicit CTickCounter(UINT interval = 0) : Interval(interval), PausedTicks{0} {}

	void Start(ULONGLONG ticks, UINT interval) { Ticks = ticks; Interval = interval; PausedTicks = 0; }
	void Stop() { Interval = 0; }

	bool Check(ULONGLONG ticks);

	void Pause(ULONGLONG ticks);
	void Resume(ULONGLONG ticks);

protected:
	UINT Interval{ 0 };
	ULONGLONG Ticks{ 0 };
	ULONGLONG PausedTicks{ 0 };
};
//////////////////////////////////////////////////////////////////////////////
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

	bool Check(ULONGLONG ticks);
	void Pause(ULONGLONG ticks);
	void Resume(ULONGLONG ticks);

	int GetSpeed() const; //in %

private:
	ULONGLONG BoostTicks{ 0 };
	ULONGLONG PausedBoostTicks{ 0 };
};
//////////////////////////////////////////////////////////////////////////////
class CGameTime
{
public:
	CGameTime() : Ticks{ 0 }, PausedTicks{ 0 }, Time{ 0 } {}

	void Start(ULONGLONG ticks) { Ticks = ticks;  PausedTicks = 0; Time = 0; }
	void Update(ULONGLONG ticks);
	void Pause(ULONGLONG ticks) { PausedTicks = ticks - Ticks; }
	void Resume(ULONGLONG ticks) { Ticks = ticks - PausedTicks; }

	UINT Get() const { return Time; }

private:
	ULONGLONG Ticks;
	ULONGLONG PausedTicks{ 0 };
	UINT Time; //ms
};
//////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////
//move key repetition timer
class CMoveKeyTimer : public CTimer
{
public:
	explicit CMoveKeyTimer(UINT_PTR id) : CTimer(id) {}
	~CMoveKeyTimer() { ::KillTimer(Parent, Id); }

	//access
	UINT GetMoveType() const;
	bool IsMoveKey(UINT key) const;

	//start|stop
	bool Start();
	bool OnKeyDown(UINT key); //return true if timer started
	void OnKeyUp(UINT key);

protected:
	UINT Key{}; //pressed move key or 0
};
//////////////////////////////////////////////////////////////////////////////