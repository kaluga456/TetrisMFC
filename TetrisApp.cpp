#include "stdafx.h"
#pragma hdrstop
#include "resource.h"
#include "logic.h"
#include "controls.h"
#include "statistics.h"
#include "options.h"
#include "Timer.h"
#include "AppWnd.h"
#include "tetrisapp.h"

COptions Options;
CStatistic Statistics;
CTetrisApp TetrisApp;
CAppWnd* pAppWnd;

BOOL CTetrisApp::InitInstance()
{
	Options.Read(m_pszProfileName);
    m_pMainWnd = new CAppWnd;
	pAppWnd = STATIC_DOWNCAST(CAppWnd, m_pMainWnd);
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
