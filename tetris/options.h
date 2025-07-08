#pragma once
//////////////////////////////////////////////////////////////////////////////
//ms, repetition interval for move shape keys
enum : UINT
{
	KEYBOARD_MAX_SPEED = 100,
	KEYBOARD_MIN_SPEED = 160,
	KEYBOARD_DEF_SPEED = 130
};
//////////////////////////////////////////////////////////////////////////////
struct COptions
{
public:
	int LayoutX;
	int LayoutY;
	int KeyboardSpeed;
	bool ShowGrid{true};

	void Read(HKEY reg_key);
	void Write(HKEY reg_key) const;

private:
	int ReadInt(CRegKey& rk, LPCTSTR key, int def_value = 0);
	void WriteInt(CRegKey& rk, LPCTSTR key, int value) const;
};
//////////////////////////////////////////////////////////////////////////////