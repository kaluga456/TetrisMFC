#pragma once

class CTetrisApp : public CWinApp
{
public:
    virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	ULONG_PTR gdiplusToken;
	HACCEL hAccelTable;
};