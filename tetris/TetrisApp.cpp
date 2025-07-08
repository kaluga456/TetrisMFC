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
//////////////////////////////////////////////////////////////////////////////
COptions Options;
CTetrisApp TetrisApp;
//////////////////////////////////////////////////////////////////////////////
BOOL CTetrisApp::InitInstance()
{
	CWinApp::InitInstance();

	//NOTE: app title defined in resource string AFX_IDS_APP_TITLE
	SetRegistryKey(_T("Kaluga456"));
	Options.Read(GetAppRegistryKey());

    m_pMainWnd = new CAppWnd;
	m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}
int CTetrisApp::ExitInstance()
{
	Options.Write(GetAppRegistryKey());
	return CWinApp::ExitInstance();
}
//////////////////////////////////////////////////////////////////////////////