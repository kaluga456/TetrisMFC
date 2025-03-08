#include "stdafx.h"
#pragma hdrstop
#include "logic.h"
#include "options.h"
#include "Timer.h"

extern COptions Options;

bool CTickCounter::Check(DWORD ticks)
{
	if (0 == Interval || Interval > static_cast<UINT>(ticks - Ticks))
		return false;
	Ticks = ticks;
	return true;
}
void CTickCounter::Pause(DWORD ticks)
{ 
	if (0 == Interval)
		return;
	PausedTicks = ticks - Ticks; 
}
void CTickCounter::Resume(DWORD ticks)
{ 
	if (0 == Interval)
		return;
	Ticks = ticks - PausedTicks; 
}
bool CGameTick::Check(DWORD ticks)
{
	//TODO: increase speed
	const UINT d = static_cast<UINT>(ticks - BoostTicks);
	if (d >= SPEED_BOOST_INTERVAL)
	{
		if(Interval > MIN_TICK_INTERVAL)
			Interval -= MAX_TICK_INTERVAL / 100;
		BoostTicks = ticks;
	}

	return CTickCounter::Check(ticks);
}
void CGameTick::Pause(DWORD ticks) 
{
	PausedBoostTicks = ticks - BoostTicks;
	CTickCounter::Pause(ticks); 
}
void CGameTick::Resume(DWORD ticks) 
{ 
	BoostTicks = ticks - PausedBoostTicks;
	CTickCounter::Resume(ticks);
}
UINT CGameTick::GetSpeed() const
{
	const UINT result = 100 - static_cast<UINT>(100. * static_cast<float>(Interval) / MAX_TICK_INTERVAL);
	if (result < 0) return 0;
	if (result > 100) return 100;
	return result;
}

void CGameTime::Update(DWORD ticks)
{
	const UINT d = static_cast<UINT>(ticks - Ticks);
	Time += d;
	Ticks = ticks;
}

void CTimer::SetParent(HWND parent)
{
	Kill();
	Parent = parent;
}
void CTimer::Start(UINT interval) 
{
	Kill();
	Timer = ::SetTimer(Parent, Id, interval, nullptr);
}
void CTimer::Kill() 
{
	if (nullptr == Parent || 0 == Timer)
		return;
	::KillTimer(Parent, Id);
	Timer = 0;
}

bool CMoveKeyTimer::IsMoveKey(UINT key) const
{
	return	VK_LEFT == key || 'A' == key ||
		VK_RIGHT == key || 'D' == key ||
		VK_DOWN == key || 'S' == key;
}
UINT CMoveKeyTimer::GetMoveType() const
{
	if (VK_LEFT == Key || 'A' == Key) return MT_MOVE_LEFT;
	if (VK_RIGHT == Key || 'D' == Key) return MT_MOVE_RIGHT;
	if (VK_DOWN == Key || 'S' == Key) return MT_MOVE_DOWN;
	return MT_UNDEFINED;
}
bool CMoveKeyTimer::OnKeyDown(UINT key)
{
	if (Timer) return false;
	if (false == IsMoveKey(key)) return false;
	if (key == Key) return false;

	ASSERT(0 == Timer);
	Key = key;
	Start(Options.KeyboardSpeed);
	return true;
}
void CMoveKeyTimer::OnKeyUp(UINT key)
{
	if (false == IsMoveKey(key)) return;
	Kill();
	Key = 0;
}

void CGameTimer::OnNewGame()
{
	StartTime = CTime::GetCurrentTime();
	PauseSpan = 0;
}
void CGameTimer::OnPause(bool pause)
{
	if (pause)	//pause
		PauseTime = CTime::GetCurrentTime();
	else		//resume
		PauseSpan += CTime::GetCurrentTime() - PauseTime;
}
CTimeSpan CGameTimer::GetGameSpan() const
{
	return CTime::GetCurrentTime() - PauseSpan - StartTime;
}
CTime CGameTimer::GetStartTime() const
{
	return StartTime;
}
CString CGameTimer::GetGameSpanString() const
{
	CTimeSpan game_span = CTime::GetCurrentTime() - PauseSpan - StartTime;
	return game_span.Format("%M:%S");
}