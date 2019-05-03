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

// AnalysisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PGN Spy.h"
#include "AnalysisDlg.h"
#include "afxdialogex.h"


// CAnalysisDlg dialog

IMPLEMENT_DYNAMIC(CAnalysisDlg, CDialogEx)

CAnalysisDlg::CAnalysisDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(IDD_ANALYSIS, pParent)
   , m_sStatus(_T(""))
   , m_iGamesWithErrors(0)
   , m_bShowResults(false)
   , m_bCancelled(false)
   , m_bStatusChanged(true)
   , m_iTargetThreads(0)
   , m_iMaxThreads(0)
{

}

CAnalysisDlg::~CAnalysisDlg()
{
}

void CAnalysisDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_STATUS, m_sStatus);
}


BEGIN_MESSAGE_MAP(CAnalysisDlg, CDialogEx)
   ON_BN_CLICKED(IDOK, &CAnalysisDlg::OnBnClickedOK)
   ON_BN_CLICKED(IDCANCEL, &CAnalysisDlg::OnBnClickedCancel)
   ON_WM_TIMER()
   ON_BN_CLICKED(IDC_DECREASETHREADS, &CAnalysisDlg::OnBnClickedDecreasethreads)
   ON_BN_CLICKED(IDC_INCREASETHREADS, &CAnalysisDlg::OnBnClickedIncreasethreads)
END_MESSAGE_MAP()


// CAnalysisDlg message handlers
BOOL CAnalysisDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   //use a timer - otherwise the status display doesn't seem to show up
   //maybe use OnPaint instead of OnInitDialog to avoid that?  Whatever, the timer works.
   SetTimer(0, 50, NULL);

   return TRUE;
}

void CAnalysisDlg::OnTimer(UINT_PTR nIDEvent)
{
   KillTimer(nIDEvent);

   CWaitCursor vWaitCursor;

   m_iTargetThreads = m_vEngineSettings.m_iNumThreads;

   SYSTEM_INFO vSysInfo;
   GetSystemInfo(&vSysInfo);
   m_iMaxThreads = (int)vSysInfo.dwNumberOfProcessors;
   UpdateThreadControlButtons();

   //do stuff
   ProcessGames();

   CDialog::OnTimer(nIDEvent);

   OnOK();//close the dialog
}

void CAnalysisDlg::UpdateThreadControlButtons()
{
   GetDlgItem(IDC_INCREASETHREADS)->EnableWindow(m_iTargetThreads < m_iMaxThreads);
   GetDlgItem(IDC_DECREASETHREADS)->EnableWindow(m_iTargetThreads > 0);
}

void CAnalysisDlg::OnBnClickedOK()
{
   //do nothing if they press enter
}

void CAnalysisDlg::OnBnClickedCancel()
{
   if (IDNO == MessageBox("Are you sure you wish to cancel?", "PGN Spy", MB_ICONQUESTION | MB_YESNO))
      return;
   
   if (IDYES == MessageBox("Would you like to save your analysis before cancelling?", "PGN Spy", MB_ICONQUESTION | MB_YESNO))
	   m_bSavePartialAnalysis = true;
   
   m_bCancelled = true;

   CDialogEx::OnCancel();
}

void CAnalysisDlg::UpdateDisplay()
{
   if (m_bStatusChanged)
      UpdateData(FALSE);
   m_bStatusChanged = false;

   MSG msg;
   while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      if (!AfxGetApp()->PreTranslateMessage(&msg))
      {
         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
   }
}

bool CAnalysisDlg::ProcessGames()
{
   CStdioFile vConvertedPGN;
   if (!vConvertedPGN.Open(m_sConvertedPGN, CFile::modeRead))
   {
      CString sMessage = "Failed to open converted PGN for analysis.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   m_sStatusHistory = "Reading pgn file...";
   m_sStatus = m_sStatusHistory;
   m_bStatusChanged = true;
   UpdateDisplay();
   //Pull games out of file
   bool bKeepLooping = true;
   CArray<CGamePGN, CGamePGN> avGamePGNs;
   while (bKeepLooping)
   {
      CString sLine;
      if (vConvertedPGN.ReadString(sLine))
      {
         if (avGamePGNs.GetSize() == 0 || sLine.Left(8).CompareNoCase("[Event \"") == 0)
         {
            //found a new game
            CGamePGN vGamePGN;
            vGamePGN.m_sPGNText = sLine + "\n";
            avGamePGNs.Add(vGamePGN);
         }
         else
         {
            //still loading the latest game
            avGamePGNs[avGamePGNs.GetUpperBound()].m_sPGNText.Append(sLine + "\n");
            if (sLine.Left(8).CompareNoCase("[White \"") == 0)
            {
               //found white player name
               int iLeftQuote, iRightQuote;
               iLeftQuote = sLine.Find("\"", 0);
               iRightQuote = sLine.Find("\"", iLeftQuote + 1);
               avGamePGNs[avGamePGNs.GetUpperBound()].m_sWhite = sLine.Mid(iLeftQuote + 1, iRightQuote - iLeftQuote - 1);
            }
            if (sLine.Left(8).CompareNoCase("[Black \"") == 0)
            {
               //found black player name
               int iLeftQuote, iRightQuote;
               iLeftQuote = sLine.Find("\"", 0);
               iRightQuote = sLine.Find("\"", iLeftQuote + 1);
               avGamePGNs[avGamePGNs.GetUpperBound()].m_sBlack = sLine.Mid(iLeftQuote + 1, iRightQuote - iLeftQuote - 1);
            }
         }
      }
      else
      {
         vConvertedPGN.Close();
         bKeepLooping = false;
      }
   }

   //Check if any games should be skipped
   if (!m_vEngineSettings.m_sPlayerName.IsEmpty())
   {
      for (int i = 0; i < avGamePGNs.GetSize(); i++)
      {
         if (avGamePGNs[i].m_sWhite.CompareNoCase(m_vEngineSettings.m_sPlayerName) != 0 &&
            avGamePGNs[i].m_sBlack.CompareNoCase(m_vEngineSettings.m_sPlayerName) != 0)
         {
            avGamePGNs.RemoveAt(i);
            i--;
         }
      }
   }

   if (avGamePGNs.GetSize() == 0)
   {
      CString sMessage = "No games to analyse.  If you have entered a player name, please ensure it is spelled correctly.";
      MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
      return true;
   }

   m_sStatusHistory = "Creating temporary files...\r\n" + m_sStatusHistory;
   m_sStatus = m_sStatusHistory;
   m_bStatusChanged = true;
   UpdateDisplay();
   //We now have a complete list of games; dump them to files
   for (int i = 0; i < avGamePGNs.GetSize(); i++)
   {
      avGamePGNs[i].m_sFileName = GetTemporaryPGNFilePath(i);
      CFile vFile;
      if (!vFile.Open(avGamePGNs[i].m_sFileName, CFile::modeCreate | CFile::modeWrite))
      {
         CString sMessage = "Failed to create temporary output file.";
         MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
         return false;
      }
      vFile.Write(avGamePGNs[i].m_sPGNText.GetBuffer(), avGamePGNs[i].m_sPGNText.GetLength());
      avGamePGNs[i].m_sPGNText.ReleaseBuffer();
      vFile.Close();
   }

   //We now have a complete set of files; time to process them

   CStringArray asResults;
   CArray<bool, bool> abErrors;
   asResults.SetSize(m_vEngineSettings.m_iNumThreads);
   abErrors.SetSize(m_vEngineSettings.m_iNumThreads);
   //initialize handles
   m_ahChildStdInRead.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahChildStdInWrite.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahChildStdOutRead.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahChildStdOutWrite.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahChildStdErrRead.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahChildStdErrWrite.SetSize(m_vEngineSettings.m_iNumThreads);
   m_ahProcesses.SetSize(m_vEngineSettings.m_iNumThreads);
   for (int i = 0; i < m_vEngineSettings.m_iNumThreads; i++)
   {
      m_ahChildStdInRead[i] = NULL;
      m_ahChildStdInWrite[i] = NULL;
      m_ahChildStdOutRead[i] = NULL;
      m_ahChildStdOutWrite[i] = NULL;
      m_ahChildStdErrRead[i] = NULL;
      m_ahChildStdErrWrite[i] = NULL;
      m_ahProcesses[i] = NULL;
      abErrors[i] = false;
   }

   int iActiveProcesses = 0;
   int iNextGame = 0;
   bool bThreadsStillRunning = false;
   m_bStatusChanged = true;
   while ((iNextGame < avGamePGNs.GetSize() || bThreadsStillRunning) && !m_bCancelled)
   {
      CString sTopLine;
      sTopLine.Format("Analysing games: %i currently active, %i of %i completed...\r\n", iActiveProcesses, iNextGame - iActiveProcesses, avGamePGNs.GetSize());
      m_sStatus = sTopLine + m_sStatusHistory;
      UpdateDisplay();

      bThreadsStillRunning = false; //we'll turn this on later if needed
                                    //check all processes to see if they're currently running
      for (int iCurThread = 0; iCurThread < m_vEngineSettings.m_iNumThreads; iCurThread++)
      {
         //if current process is free, kick off another process
         if (m_ahProcesses[iCurThread] == NULL)
         {
            if (iNextGame >= avGamePGNs.GetSize())
               continue; //don't start a new process if no games left
            abErrors[iCurThread] = false;
            //kick off process with next game
            if (!LaunchAnalyser(avGamePGNs[iNextGame], iCurThread))
            {
               //Failed to launch analyser; bail out
               return false;
            }

            iActiveProcesses++;
            bThreadsStillRunning = true; //we're launching a new thread
            iNextGame++;
            m_bStatusChanged = true;
            //bail out of loop; we'll check other processes next time through
            break;
         }
         //check if current process is complete
         else if (WaitForSingleObject(m_ahProcesses[iCurThread], 0) == WAIT_OBJECT_0)
         {
            //the process has completed; read all results
            ReadFromThread(iCurThread, asResults[iCurThread], abErrors[iCurThread]);

            //close handles
            CloseHandle(m_ahProcesses[iCurThread]);
            CloseHandle(m_ahChildStdInRead[iCurThread]);
            CloseHandle(m_ahChildStdInWrite[iCurThread]);
            CloseHandle(m_ahChildStdOutRead[iCurThread]);
            CloseHandle(m_ahChildStdOutWrite[iCurThread]);
            CloseHandle(m_ahChildStdErrRead[iCurThread]);
            CloseHandle(m_ahChildStdErrWrite[iCurThread]);
            m_ahProcesses[iCurThread] = NULL;
            m_ahChildStdInRead[iCurThread] = NULL;
            m_ahChildStdInWrite[iCurThread] = NULL;
            m_ahChildStdOutRead[iCurThread] = NULL;
            m_ahChildStdOutWrite[iCurThread] = NULL;
            m_ahChildStdErrRead[iCurThread] = NULL;
            m_ahChildStdErrWrite[iCurThread] = NULL;
            iActiveProcesses--;

            CString sStatusLine;
            //we have our results; now parse them
            if (abErrors[iCurThread])
            {
               m_iGamesWithErrors++; //skip games with errors
               sStatusLine = "Encountered game with error";
            }
            else
            {
               if (ProcessOutput(asResults[iCurThread]))
                  sStatusLine.Format("Finished game: %s v %s", m_avGames[m_avGames.GetUpperBound()].m_sWhite, m_avGames[m_avGames.GetUpperBound()].m_sBlack);
               else
                  sStatusLine = "Encountered game with error";
            }
            m_sStatusHistory = sStatusLine + "\r\n" + m_sStatusHistory;
            m_bStatusChanged = true;
            asResults[iCurThread].Empty();
            abErrors[iCurThread] = false;

            //check if we're supposed to be decrementing threads
            if (m_iTargetThreads < m_vEngineSettings.m_iNumThreads)
            {
               //remove array members for current thread
               asResults.RemoveAt(iCurThread);
               abErrors.RemoveAt(iCurThread);
               m_ahChildStdInRead.RemoveAt(iCurThread);
               m_ahChildStdInWrite.RemoveAt(iCurThread);
               m_ahChildStdOutRead.RemoveAt(iCurThread);
               m_ahChildStdOutWrite.RemoveAt(iCurThread);
               m_ahChildStdErrRead.RemoveAt(iCurThread);
               m_ahChildStdErrWrite.RemoveAt(iCurThread);
               m_ahProcesses.RemoveAt(iCurThread);

               //decrement thread count
               m_vEngineSettings.m_iNumThreads--;
            }
         }
         else
         {
            bThreadsStillRunning = true; //current thread is still running

                                         //read everything written so far to keep the pipe from filling up and locking the analyser
            ReadFromThread(iCurThread, asResults[iCurThread], abErrors[iCurThread]);
         }
      }
      Sleep(50); //so we don't eat lots of CPU trying to check on every process constantly

      //check if we've incremented the thread count
      if (m_iTargetThreads > m_vEngineSettings.m_iNumThreads)
      {
         m_vEngineSettings.m_iNumThreads++;
         //add a member to arrays and initialize values
         asResults.SetSize(m_vEngineSettings.m_iNumThreads);
         abErrors.SetSize(m_vEngineSettings.m_iNumThreads);
         //initialize handles
         m_ahChildStdInRead.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdInWrite.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdOutRead.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdOutWrite.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdErrRead.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdErrWrite.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahProcesses.SetSize(m_vEngineSettings.m_iNumThreads);
         m_ahChildStdInRead[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahChildStdInWrite[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahChildStdOutRead[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahChildStdOutWrite[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahChildStdErrRead[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahChildStdErrWrite[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         m_ahProcesses[m_vEngineSettings.m_iNumThreads - 1] = NULL;
         abErrors[m_vEngineSettings.m_iNumThreads - 1] = false;
      }
   }

   if (m_bCancelled)
   {
	  
      //close all active handles
      for (int i = 0; i < m_vEngineSettings.m_iNumThreads; i++)
      {
         if (m_ahProcesses[i])
         {
            CFile vInFile(m_ahChildStdInWrite[i]);
            vInFile.Write("cancel", 6);

            TerminateProcess(m_ahProcesses[i], 1);//shut down the analyser; the engine should close itself after a bit
            CloseHandle(m_ahProcesses[i]);
            CloseHandle(m_ahChildStdInRead[i]);
            CloseHandle(m_ahChildStdInWrite[i]);
            CloseHandle(m_ahChildStdOutRead[i]);
            CloseHandle(m_ahChildStdOutWrite[i]);
            CloseHandle(m_ahChildStdErrRead[i]);
            CloseHandle(m_ahChildStdErrWrite[i]);
         }
      }
   }

   //clean up temporary files
   for (int i = 0; i < avGamePGNs.GetSize(); i++)
      DeleteFile(avGamePGNs[i].m_sFileName);

   if (!m_bCancelled || m_bSavePartialAnalysis)
      m_bShowResults = true;

   CString sMessage;
   if (m_bCancelled)
      sMessage = "Analysis cancelled.";
   else if (m_iGamesWithErrors > 0)
      sMessage.Format("Errors were encountered on %i games out of a total %i to be analysed.", m_iGamesWithErrors, avGamePGNs.GetSize());
   else
      sMessage.Format("Analysis of %i games completed with no errors.", avGamePGNs.GetSize());
   MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);

   //caller will display results
   return true;
}

void CAnalysisDlg::ReadFromThread(int iThread, CString IN OUT &rsResults, bool IN OUT &rbError)
{
   if (m_ahChildStdOutRead[iThread] == NULL)
   {
      ASSERT(false);
      return;
   }
   CFile vOutFile(m_ahChildStdOutRead[iThread]);
   //we won't need to close these handles; they're closed elsewhere

   char sBuf[1001];
   ZeroMemory(sBuf, sizeof(sBuf));

   //check for failure
   DWORD iExitCode;
   GetExitCodeProcess(m_ahProcesses[iThread], &iExitCode);
   if (iExitCode != STILL_ACTIVE && iExitCode < 0)
   {
      //found error
      rbError = true;
   }

   //process hasn't terminated; check for error anyway
   DWORD iBytesAvailable;
   PeekNamedPipe(m_ahChildStdErrRead[iThread], NULL, NULL, NULL, &iBytesAvailable, NULL);
   if (iBytesAvailable > 0)
   {
      //Read error for debugging purposes - also to ensure pipe doesn't fill up and lock up the analyser
      CFile vErrFile(m_ahChildStdErrRead[iThread]);
      int iBytesRead = vErrFile.Read(sBuf, 1000);
      return;
   }

   //no failure, keep reading until we reach end of results or end of what's currently in the pipe
   while (rsResults.Find("</gamelist>") == -1)
   {
      //ensure there's still data to read
      DWORD iBytesAvailable;
      PeekNamedPipe(m_ahChildStdOutRead[iThread], NULL, NULL, NULL, &iBytesAvailable, NULL);
      if (iBytesAvailable <= 0)
         break; //hit end of pipe

      int iBytesRead = vOutFile.Read(sBuf, 1000);
      rsResults.Append(sBuf);
//       if (iBytesRead < 1000) //not needed if we're doing PeekNamedPipe
//          break; //hit end of pipe
      ZeroMemory(sBuf, sizeof(sBuf)); //initialise for next time round
   }
}

bool CAnalysisDlg::LaunchAnalyser(CGamePGN vGamePGN, int iCurThread)
{
   SECURITY_ATTRIBUTES vAttrib;
   vAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
   vAttrib.bInheritHandle = TRUE;
   vAttrib.lpSecurityDescriptor = NULL;

   //create std in pipe for writing to child process
   if (!CreatePipe(&m_ahChildStdInRead[iCurThread], &m_ahChildStdInWrite[iCurThread], &vAttrib, 0))
   {
      CString sMessage = "Failed to create pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdInWrite[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      CString sMessage = "Failed set up pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   //create std out pipe for reading from child process
   if (!CreatePipe(&m_ahChildStdOutRead[iCurThread], &m_ahChildStdOutWrite[iCurThread], &vAttrib, 0))
   {
      CString sMessage = "Failed to create pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdOutRead[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      CString sMessage = "Failed set up pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   //create std err pipe for reading from child process
   if (!CreatePipe(&m_ahChildStdErrRead[iCurThread], &m_ahChildStdErrWrite[iCurThread], &vAttrib, 0))
   {
      CString sMessage = "Failed to create pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdErrRead[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      CString sMessage = "Failed set up pipe to communicate with analyser.";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   PROCESS_INFORMATION vProcessInfo;
   STARTUPINFO vStartupInfo = { 0 };
   vStartupInfo.cb = sizeof(STARTUPINFO);
   vStartupInfo.hStdInput = m_ahChildStdInRead[iCurThread];
   vStartupInfo.hStdOutput = m_ahChildStdOutWrite[iCurThread];
   vStartupInfo.hStdError = m_ahChildStdErrWrite[iCurThread];
   vStartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
   vStartupInfo.wShowWindow = SW_HIDE;

   CString sCommandLine;
   CString sWhiteOrBlack = "";
   if (!m_vEngineSettings.m_sPlayerName.IsEmpty())
   {
      if (m_vEngineSettings.m_sPlayerName.CompareNoCase(vGamePGN.m_sWhite) == 0)
         sWhiteOrBlack = "--whiteonly ";
      else if (m_vEngineSettings.m_sPlayerName.CompareNoCase(vGamePGN.m_sBlack) == 0)
         sWhiteOrBlack = "--blackonly ";
      else
         ASSERT(false); //we should have discarded this game before this point
   }
   int iBookDepthPlies = m_vEngineSettings.m_iBookDepth * 2; //double book depth, since analyser uses plies, not moves
   sCommandLine.Format("--bookdepth %i --searchdepth %i --searchmaxtime %i --searchmintime %i --variations %i %s --setoption Hash %i --setoption Threads 1 --engine \"%s\" \"%s\"",
      iBookDepthPlies, m_vEngineSettings.m_iSearchDepth, m_vEngineSettings.m_iMaxTime,
      m_vEngineSettings.m_iMinTime, m_vEngineSettings.m_iNumVariations + 1, sWhiteOrBlack,
      m_vEngineSettings.m_iHashSize, m_vEngineSettings.m_sEnginePath, vGamePGN.m_sFileName);
   if (!CreateProcess(GetAnalyserFilePath(), sCommandLine.GetBuffer(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &vStartupInfo, &vProcessInfo))
   {
      sCommandLine.ReleaseBuffer();
      DWORD dwError = GetLastError();
      CString sMessage = "Failed to launch analyser.  Please ensure it is in the same folder as PGN Spy, with the file name \"pgn-extract.exe\".";
      MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   sCommandLine.ReleaseBuffer();
   m_ahProcesses[iCurThread] = vProcessInfo.hProcess;
   CloseHandle(vProcessInfo.hThread);
   return true;
}

bool CAnalysisDlg::ProcessOutput(CString sOutput)
{
   if (false)
   {
      //debugging code to dump sample xml output to a file
      CString sFileName = GetTemporaryPGNFilePath(1);
      sFileName.Replace(".pgn", ".xml");
      CFile vFile;
      if (!vFile.Open(sFileName, CFile::modeCreate | CFile::modeWrite))
      {
         CString sMessage = "Failed to create temporary output file.";
         MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION);
         return false;
      }

      vFile.Write(sOutput.GetBuffer(), sOutput.GetLength());
      sOutput.ReleaseBuffer();
      vFile.Close();
   }

   CGame vGame;
   if (vGame.LoadGame(sOutput))
      m_avGames.Add(vGame);
   else
   {
      m_iGamesWithErrors++;
      return false;
   }
   return true;
}


void CAnalysisDlg::OnBnClickedDecreasethreads()
{
   m_iTargetThreads--;
   if (m_iTargetThreads < 0)
   {
      ASSERT(false);
      m_iTargetThreads = 0;
   }
   CString sStatusLine;
   if (m_iTargetThreads > 0)
      sStatusLine.Format("Number of threads will be decreased to %i as active threads are completed.\r\n", m_iTargetThreads);
   else
      sStatusLine = "Analysis will be paused when all currently active threads are completed.\r\n";
   m_sStatusHistory = sStatusLine + m_sStatusHistory;
   m_bStatusChanged = true;
   UpdateThreadControlButtons();
}


void CAnalysisDlg::OnBnClickedIncreasethreads()
{
   m_iTargetThreads++;
   if (m_iTargetThreads > m_iMaxThreads)
   {
      ASSERT(false);
      m_iTargetThreads = m_iMaxThreads;
   }
   CString sStatusLine;
   sStatusLine.Format("Number of threads increased to %i.\r\n", m_iTargetThreads);
   m_sStatusHistory = sStatusLine + m_sStatusHistory;
   m_bStatusChanged = true;
   UpdateThreadControlButtons();
}

