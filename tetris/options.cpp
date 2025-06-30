#include "stdafx.h"
#pragma hdrstop
#include "options.h"

int COptions::GetInt(LPCTSTR section, LPCTSTR key, int def_value) const
{
	ASSERT(IniFileName);
	return ::GetPrivateProfileInt(section, key, def_value, IniFileName);
}
void COptions::WriteInt(LPCTSTR section, LPCTSTR key, int value) const
{
	ASSERT(IniFileName);
	TCHAR value_string[16];
	_sntprintf_s<16>(value_string, 15, _T("%d"), value);
	::WritePrivateProfileString(section, key, value_string, IniFileName);
}
void COptions::Read(LPCTSTR ini_file_name)
{
	GetCurrentDirectory(MAX_PATH, IniFileName);
	lstrcat(IniFileName, _T("\\"));
	lstrcat(IniFileName, ini_file_name);
	LayoutX = GetInt(_T("Layuot"), _T("LayoutX"), 100);
	LayoutY = GetInt(_T("Layuot"), _T("LayoutY"), 100);
	ShowGrid = GetInt(_T("Layuot"), _T("ShowGrid"), 1);
	
	KeyboardSpeed = GetInt(_T("Game"), _T("KeyboardSpeed"), 75);
	if(KeyboardSpeed > KEYBOARD_MIN_SPEED)
		KeyboardSpeed = KEYBOARD_MIN_SPEED;
	else if(KeyboardSpeed < KEYBOARD_MAX_SPEED)
		KeyboardSpeed = KEYBOARD_MAX_SPEED;
}
void COptions::Write()
{
	WriteInt(_T("Layuot"), _T("LayoutX"), LayoutX);
	WriteInt(_T("Layuot"), _T("LayoutY"), LayoutY);
	WriteInt(_T("Layuot"), _T("ShowGrid"), ShowGrid);
	WriteInt(_T("Game"), _T("KeyboardSpeed"), KeyboardSpeed);
}