#pragma once

//ms, repetition interval for move shape keys
enum : UINT
{
	KEYBOARD_MAX_SPEED = 50,
	KEYBOARD_MIN_SPEED = 150
};

struct COptions
{
public:
	void Read(LPCTSTR ini_file_name);
	void Write();
	int LayoutX;
	int LayoutY;
	int KeyboardSpeed;
	bool ShowGrid{true};

private:
	TCHAR IniFileName[MAX_PATH];
	int GetInt(LPCTSTR section, LPCTSTR key, int def_value) const;
	void WriteInt(LPCTSTR section, LPCTSTR key, int value) const;
};