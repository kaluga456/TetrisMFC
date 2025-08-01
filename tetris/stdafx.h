#pragma once

//STL
#include <random>
#include <algorithm>
#include <string>
#include <vector>

//SQLite
#include "sqlite3.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN			// Exclude rarely-used stuff from Windows headers
#endif

//target version
#include <SDKDDKVer.h>

//MFC
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <atltime.h>
#include <afxcmn.h>
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#undef min
#undef max

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
