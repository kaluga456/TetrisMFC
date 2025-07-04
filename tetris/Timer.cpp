#include "stdafx.h"
#pragma hdrstop
#include "options.h"
#include "Timer.h"

extern COptions Options;
//////////////////////////////////////////////////////////////////////////////
//CTickCounter
bool CTickCounter::Check(ULONGLONG ticks)
{
	if (0 == Interval || Interval > static_cast<ULONGLONG>(ticks - Ticks))
		return false;
	Ticks = ticks;
	return true;
}
void CTickCounter::Pause(ULONGLONG ticks)
{ 
	if (0 == Interval)
		return;
	PausedTicks = ticks - Ticks; 
}
void CTickCounter::Resume(ULONGLONG ticks)
{ 
	if (0 == Interval)
		return;
	Ticks = ticks - PausedTicks; 
}
//////////////////////////////////////////////////////////////////////////////
//CGameTick
bool CGameTick::Check(ULONGLONG ticks)
{
	const ULONGLONG d = static_cast<ULONGLONG>(ticks - BoostTicks);
	if (d >= SPEED_BOOST_INTERVAL)
	{
		if (Interval > MIN_TICK_INTERVAL)
			Interval -= MAX_TICK_INTERVAL / 100;
		BoostTicks = ticks;
	}

	return CTickCounter::Check(ticks);
}
void CGameTick::Pause(ULONGLONG ticks)
{
	PausedBoostTicks = ticks - BoostTicks;
	CTickCounter::Pause(ticks); 
}
void CGameTick::Resume(ULONGLONG ticks)
{ 
	BoostTicks = ticks - PausedBoostTicks;
	CTickCounter::Resume(ticks);
}
int CGameTick::GetSpeed() const
{
	const int result = 100 - static_cast<int>(100.f * static_cast<float>(Interval) / MAX_TICK_INTERVAL);
	if (result < 0) return 0;
	if (result > 100) return 100;
	return result;
}
//////////////////////////////////////////////////////////////////////////////
//CGameTime
void CGameTime::Update(ULONGLONG ticks)
{
	const ULONGLONG d = static_cast<ULONGLONG>(ticks - Ticks);
	Time += static_cast<UINT>(d);
	Ticks = ticks;
}
//////////////////////////////////////////////////////////////////////////////
//CTimer
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
//////////////////////////////////////////////////////////////////////////////
//CMoveKeyTimer
bool CMoveKeyTimer::Start()
{
	//already started
	if (Timer)
		return false;

	CTimer::Start(Options.KeyboardSpeed);
	return true;
}
//////////////////////////////////////////////////////////////////////////////