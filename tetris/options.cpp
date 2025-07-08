#include "stdafx.h"
#pragma hdrstop
#include "options.h"
//////////////////////////////////////////////////////////////////////////////
int COptions::ReadInt(CRegKey& rk, LPCTSTR key, int def_value /*= 0*/)
{
	DWORD result{0};
	if (ERROR_SUCCESS == rk.QueryDWORDValue(key, result))
		return static_cast<int>(result);
	return def_value;
}
void COptions::WriteInt(CRegKey& rk, LPCTSTR key, int value) const
{
	rk.SetDWORDValue(key, static_cast<DWORD>(value));
}
void COptions::Read(HKEY reg_key)
{
	CRegKey rk;
	rk.Attach(reg_key);

	LayoutX = ReadInt(rk, _T("LayoutX"), 100);
	LayoutY = ReadInt(rk, _T("LayoutY"), 100);
	ShowGrid = ReadInt(rk, _T("ShowGrid"), TRUE);

	KeyboardSpeed = ReadInt(rk, _T("KeyboardSpeed"), KEYBOARD_DEF_SPEED);
	KeyboardSpeed = std::clamp<int>(KeyboardSpeed, KEYBOARD_MAX_SPEED, KEYBOARD_MIN_SPEED);

	rk.Detach();
}
void COptions::Write(HKEY reg_key) const
{
	CRegKey rk;
	rk.Attach(reg_key);

	WriteInt(rk, _T("LayoutX"), LayoutX);
	WriteInt(rk, _T("LayoutY"), LayoutY);
	WriteInt(rk, _T("ShowGrid"), ShowGrid);
	WriteInt(rk, _T("KeyboardSpeed"), KeyboardSpeed);

	rk.Detach();
}
//////////////////////////////////////////////////////////////////////////////