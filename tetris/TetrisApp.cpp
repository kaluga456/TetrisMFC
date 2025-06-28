#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "TetrisEngine.h"
#include "Graphics.h"
#include "Rating.h"
#include "options.h"
#include "Timer.h"
#include "AppWnd.h"
#include "tetrisapp.h"

COptions Options;
CTetrisApp TetrisApp;
CAppWnd* AppWnd;
CWinApp* ThisApp = &TetrisApp;

BOOL CTetrisApp::InitInstance()
{
	Options.Read(m_pszProfileName);

    m_pMainWnd = new CAppWnd;
	AppWnd = STATIC_DOWNCAST(CAppWnd, m_pMainWnd);
	m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
	hAccelTable = LoadAccelerators(m_hInstance, (LPCTSTR)IDR_ACCELERATOR);
    return TRUE;
}

int CTetrisApp::ExitInstance()
{
	Options.Write();
	return CWinApp::ExitInstance();
}
