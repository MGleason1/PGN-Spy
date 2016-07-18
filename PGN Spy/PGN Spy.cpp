// MIT License
// 
// Copyright(c) 2016 Michael J. Gleason
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "stdafx.h"
#include "PGN Spy.h"
#include "PGN SpyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPGNSpyApp

BEGIN_MESSAGE_MAP(CPGNSpyApp, CWinApp)
   ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPGNSpyApp construction

CPGNSpyApp::CPGNSpyApp()
{
   // TODO: add construction code here,
   // Place all significant initialization in InitInstance
}


// The one and only CPGNSpyApp object

CPGNSpyApp theApp;


// CPGNSpyApp initialization

BOOL CPGNSpyApp::InitInstance()
{
   // InitCommonControlsEx() is required on Windows XP if an application
   // manifest specifies use of ComCtl32.dll version 6 or later to enable
   // visual styles.  Otherwise, any window creation will fail.
   INITCOMMONCONTROLSEX InitCtrls;
   InitCtrls.dwSize = sizeof(InitCtrls);
   // Set this to include all the common control classes you want to use
   // in your application.
   InitCtrls.dwICC = ICC_WIN95_CLASSES;
   InitCommonControlsEx(&InitCtrls);

   CWinApp::InitInstance();

   AfxEnableControlContainer();

   // Standard initialization
   // If you are not using these features and wish to reduce the size
   // of your final executable, you should remove from the following
   // the specific initialization routines you do not need
   // Change the registry key under which our settings are stored
   // TODO: You should modify this string to be something appropriate
   // such as the name of your company or organization
   SetRegistryKey(_T("PGNSpy"));

   FindDataFolder();

   CPGNSpyDlg dlg;
   m_pMainWnd = &dlg;
   INT_PTR nResponse = dlg.DoModal();

   // Since the dialog has been closed, return FALSE so that we exit the
   //  application, rather than start the application's message pump.
   return FALSE;
}

CString CPGNSpyApp::FindDataFolder()
{
   HMODULE hModule;
   if (AfxGetApp())
      hModule = AfxGetInstanceHandle();
   else
      hModule = GetModuleHandle(NULL);

   char szAppPath[_MAX_PATH];
   GetModuleFileName(hModule,szAppPath,_MAX_PATH);
   while (szAppPath[strlen(szAppPath)-1] != '\\' && strlen(szAppPath) > 0)
      szAppPath[strlen(szAppPath)-1] = '\0';
   m_sDataFolder = CString(szAppPath);
   return m_sDataFolder;
}

CString GetConverterFilePath()
{
   return theApp.m_sDataFolder + "pgn-extract.exe";
}

CString GetAnalyserFilePath()
{
   return theApp.m_sDataFolder + "uci-analyser.exe";
}

CString GetSettingsFilePath()
{
   return theApp.m_sDataFolder + "settings.xml";
}

CString GetConvertedPGNFilePath()
{
   char sTempPath[500];
   char sFilePath[500];
   GetTempPath(500, sTempPath);
   GetTempFileName(sTempPath, "PGN", 0, sFilePath);
   return CString(sFilePath);
//    CTime vTime = CTime::GetCurrentTime();
//    return theApp.m_sDataFolder + vTime.Format("Temp %y%m%d%H%M%S.pgn");
}

CString GetTemporaryPGNFilePath(int i)
{
   char sTempPath[500];
   char sFilePath[500];
   GetTempPath(500, sTempPath);
   GetTempFileName(sTempPath, "PGN", 0, sFilePath);
   return CString(sFilePath);
//    CTime vTime = CTime::GetCurrentTime();
//    CString sCounter;
//    sCounter.Format(" %i.pgn", i);
//    return theApp.m_sDataFolder + vTime.Format("Temp %y%m%d%H%M%S") + sCounter;
}
