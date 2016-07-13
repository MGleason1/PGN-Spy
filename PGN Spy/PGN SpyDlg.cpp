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
#include "Analysis.h"
#include "AnalysisDlg.h"
#include "ResultsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
   CAboutDlg();

// Dialog Data
   enum { IDD = IDD_ABOUTBOX };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPGNSpyDlg dialog




CPGNSpyDlg::CPGNSpyDlg(CWnd* pParent /*=NULL*/)
   : CDialog(CPGNSpyDlg::IDD, pParent)
   , m_sInputFile("")
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPGNSpyDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_INPUTANALYSE, m_sInputFile);
   DDX_Text(pDX, IDC_PLAYER, m_vAnalysisSettings.m_sPlayerName);
   DDX_Text(pDX, IDC_ENGINE, m_vAnalysisSettings.m_sEnginePath);
   DDX_Text(pDX, IDC_SEARCHDEPTH, m_vAnalysisSettings.m_iSearchDepth);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iSearchDepth, 1, 50);
   DDX_Text(pDX, IDC_BOOKDEPTH, m_vAnalysisSettings.m_iBookDepth);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iBookDepth, 0, 30);
   DDX_Text(pDX, IDC_NUMTHREADS, m_vAnalysisSettings.m_iNumThreads);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iNumThreads, 1, 128);
   DDX_Check(pDX, IDC_EXCLUDEFORCED, m_vAnalysisSettings.m_bExcludeForcedMoves);
   DDX_Text(pDX, IDC_MINTIME, m_vAnalysisSettings.m_iMinTime);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iMinTime, 1, 60000);
   DDX_Text(pDX, IDC_MAXTIME, m_vAnalysisSettings.m_iMaxTime);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iMaxTime, 1, 60000);
   DDX_Text(pDX, IDC_HASHSIZE, m_vAnalysisSettings.m_iHashSize);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iHashSize, 1, 8192);
   DDX_Text(pDX, IDC_FORCEDMOVECUTOFF, m_vAnalysisSettings.m_iForcedMoveCutoff);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iForcedMoveCutoff, 25, 10000);
   DDX_Check(pDX, IDC_INCLUDEONLYUNCLEAR, m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions);
   DDX_Text(pDX, IDC_UNCLEARPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iUnclearPositionCutoff);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iUnclearPositionCutoff, 25, 10000);
   DDX_Text(pDX, IDC_EQUALPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iEqualPositionThreshold);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iEqualPositionThreshold, 25, 10000);
   DDX_Text(pDX, IDC_LOSINGPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iLosingThreshold);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iLosingThreshold, 25, 10000);
   DDX_Text(pDX, IDC_BLUNDERTHRESHOLD, m_vAnalysisSettings.m_iBlunderThreshold);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iBlunderThreshold, 25, 10000);
   DDX_Text(pDX, IDC_VARIATIONS, m_vAnalysisSettings.m_iNumVariations);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iNumVariations, 1, 10);
}

BEGIN_MESSAGE_MAP(CPGNSpyDlg, CDialog)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   //}}AFX_MSG_MAP
   ON_BN_CLICKED(IDC_BROWSEANALYSE, &CPGNSpyDlg::OnBnClickedBrowseanalyse)
   ON_BN_CLICKED(IDC_BROWSEENGINE, &CPGNSpyDlg::OnBnClickedBrowseengine)
   ON_BN_CLICKED(IDC_RUNANALYSIS, &CPGNSpyDlg::OnBnClickedRunanalysis)
   ON_BN_CLICKED(IDC_HELPPLAYER, &CPGNSpyDlg::OnBnClickedHelpplayer)
   ON_BN_CLICKED(IDC_HELPDEPTH, &CPGNSpyDlg::OnBnClickedHelpdepth)
   ON_BN_CLICKED(IDC_HELPBOOKDEPTH, &CPGNSpyDlg::OnBnClickedHelpbookdepth)
   ON_BN_CLICKED(IDC_HELPTHREADS, &CPGNSpyDlg::OnBnClickedHelpthreads)
   ON_BN_CLICKED(IDC_HELPMINTIME, &CPGNSpyDlg::OnBnClickedHelpmintime)
   ON_BN_CLICKED(IDC_HELPMAXTIME, &CPGNSpyDlg::OnBnClickedHelpmaxtime)
   ON_BN_CLICKED(IDC_HELPPHASHSIZE, &CPGNSpyDlg::OnBnClickedHelpphashsize)
   ON_BN_CLICKED(IDC_SAVESETTINGS, &CPGNSpyDlg::OnBnClickedSavesettings)
   ON_BN_CLICKED(IDC_FORCEDMOVEHELP, &CPGNSpyDlg::OnBnClickedForcedmovehelp)
   ON_BN_CLICKED(IDC_UNCLEARPOSITIONHELP, &CPGNSpyDlg::OnBnClickedUnclearpositionhelp)
   ON_BN_CLICKED(IDC_EQUALPOSITIONHELP, &CPGNSpyDlg::OnBnClickedEqualpositionhelp)
   ON_BN_CLICKED(IDC_LOSINGTHRESHOLDHELP, &CPGNSpyDlg::OnBnClickedLosingthresholdhelp)
   ON_BN_CLICKED(IDC_BLUNDERTHRESHOLDHELP, &CPGNSpyDlg::OnBnClickedBlunderthresholdhelp)
   ON_BN_CLICKED(IDC_NUMVARIATIONSHELP, &CPGNSpyDlg::OnBnClickedNumvariationshelp)
   ON_BN_CLICKED(IDC_EXCLUDEFORCED, &CPGNSpyDlg::OnBnClickedExcludeforced)
   ON_BN_CLICKED(IDC_INCLUDEONLYUNCLEAR, &CPGNSpyDlg::OnBnClickedIncludeonlyunclear)
END_MESSAGE_MAP()


// CPGNSpyDlg message handlers

BOOL CPGNSpyDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   if (!m_vAnalysisSettings.LoadSettings()) //load settings from file
      m_vAnalysisSettings = CAnalysisSettings(); //failed to load, so restore defaults
   UpdateData(FALSE);

   DisableInvalidSettings();

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPGNSpyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialog::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPGNSpyDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPGNSpyDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

void CPGNSpyDlg::OnBnClickedBrowseanalyse()
{
   if (!UpdateData())
      return;
   CFileDialog vFileDialog(TRUE,"pgn","*.pgn",OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT,"Portable Game Notation file (*.pgn)|*.pgn|All files (*.*)|*.*||",this);
   if (vFileDialog.DoModal() != IDOK)
      return;
   m_sInputFile = vFileDialog.GetPathName();
   UpdateData(FALSE);
}

void CPGNSpyDlg::OnBnClickedBrowseengine()
{
   if (!UpdateData())
      return;
   CFileDialog vFileDialog(TRUE,"exe","*.exe",OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT,"Chess Engines (*.exe)|*.exe|All files (*.*)|*.*||",this);
   if (vFileDialog.DoModal() != IDOK)
      return;
   m_vAnalysisSettings.m_sEnginePath = vFileDialog.GetPathName();
   UpdateData(FALSE);
}

bool CPGNSpyDlg::ConvertFileForAnalysis(CString OUT &sConvertedFile)
{
   sConvertedFile = GetConvertedPGNFilePath();
   CFile vFile;
   if (!vFile.Open(sConvertedFile, CFile::modeCreate | CFile::modeWrite))
   {
      MessageBox("Failed to create temporary output file.","PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   vFile.Close();

   CString sCommandLine = " -Wuci \"-o" + sConvertedFile + "\" \"" + m_sInputFile + "\"";

   PROCESS_INFORMATION vProcessInfo;
   STARTUPINFO vStartupInfo = {0};
   vStartupInfo.cb = sizeof(vStartupInfo);
   if (!CreateProcess(GetConverterFilePath(), sCommandLine.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &vStartupInfo, &vProcessInfo))
   {
      sCommandLine.ReleaseBuffer();
      MessageBox("Failed to launch converter.  Please ensure it is in the same folder as PGN Spy, with the file name \"pgn-extract.exe\".","PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   sCommandLine.ReleaseBuffer();
   WaitForSingleObject(vProcessInfo.hProcess, INFINITE);
   CloseHandle(vProcessInfo.hProcess);
   CloseHandle(vProcessInfo.hThread);

   return true;
}

void CPGNSpyDlg::OnBnClickedRunanalysis()
{
   if (!ValidateSettings())
      return;

   //validate file paths
   if (!PathFileExists(m_sInputFile))
   {
      MessageBox("The specified input file does not exist.", "PGN Spy", MB_ICONEXCLAMATION);
      return;
   }

   CString sTemporaryFile;
   if (!ConvertFileForAnalysis(sTemporaryFile))
   {
      MessageBox("Failed to convert the pgn file into the appropriate format for analysis.","PGN Spy", MB_ICONEXCLAMATION);
      return;
   }

   //file is converted; now process it
   CAnalysisDlg vAnalyserDlg;
   vAnalyserDlg.m_sConvertedPGN = sTemporaryFile;
   vAnalyserDlg.m_vAnalysisSettings = m_vAnalysisSettings;
   vAnalyserDlg.DoModal();

   //delete temporary file
   DeleteFile(sTemporaryFile);

   if (vAnalyserDlg.m_avGames.GetSize() == 0)
      return;
   
   //now launch the window to process and display the results
   CResultsDlg vResultsDlg;
   vResultsDlg.m_avGames.Copy(vAnalyserDlg.m_avGames);
   vResultsDlg.m_vSettings = m_vAnalysisSettings;
   vResultsDlg.DoModal();
}

void CPGNSpyDlg::OnBnClickedSavesettings()
{
   if (!ValidateSettings())
      return;

   if (!m_vAnalysisSettings.SaveSettings())
      MessageBox("Failed to save settings.", "PGN Spy", MB_ICONEXCLAMATION);
   else
      MessageBox("Settings saved.", "PGN Spy", MB_ICONINFORMATION);
}

bool CPGNSpyDlg::ValidateSettings()
{
   if (!UpdateData())
      return false;

   if (m_vAnalysisSettings.m_iMinTime > m_vAnalysisSettings.m_iMaxTime)
   {
      MessageBox("The minimum time for analysis must not exceed the maximum time.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   if (m_vAnalysisSettings.m_iEqualPositionThreshold > m_vAnalysisSettings.m_iLosingThreshold)
   {
      MessageBox("The equal position threshold must not exceed the losing position threshold.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   if (!PathFileExists(m_vAnalysisSettings.m_sEnginePath))
   {
      MessageBox("The specified engine does not exist.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   SYSTEM_INFO vSysInfo;
   GetSystemInfo(&vSysInfo);
   if (m_vAnalysisSettings.m_iNumThreads > (int)vSysInfo.dwNumberOfProcessors)
   {
      MessageBox("You have entered more threads than the number of processors present in your system.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   return true;
}

void CPGNSpyDlg::OnBnClickedHelpplayer()
{
   CString sMessage = "If a player name is entered, statistics for the specified player will be reported.  Games excluding "
                      "this player will be ignored.\n\nIf no player name is entered, aggregate statistics for all players "
                      "will be reported.  This is useful for establishing baselines.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpdepth()
{
   CString sMessage = "Specify the minimum number of plies to search for each position.  A ply is half of a move; i.e. "
                      "one white move, or one black move.\n\nIf this depth is not reached within the minimum time, the "
                      "search will continue until either this depth is reached or the maximum time has been reached.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpbookdepth()
{
   CString sMessage = "Specify the number of opening moves to exclude.  A move consists of one white move and one black "
                      "move, so entering five means ignoring five moves for each side.\n"
                      "\n"
                      "Opening moves are better excluded from calculations, as a player who has studied an opening may "
                      "well be able to reproduce main-line moves from memory, without the use of an engine.  Also, static "
                      "resources such as books and databases are usually allowed in correspondence chess, so even a weak "
                      "player may legitimately play main-line moves early in the game without the use of an engine.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpthreads()
{
   CString sMessage = "Specify the number of threads to use for analysis.\n\nTo aid consistency of results, the engine "
                      "will only be allowed to use a single thread for each position.  However, if there are multiple "
                      "games being analysed on a multi-core machine, several games may be processed simultaneously to "
                      "enable analysis to be completed more quickly.\n\nNote: if you leave this at the default value, "
                      "this will monopolise all available processing power.  If you want to be able to use your computer "
                      "for other purposes while this is running, it is recommended that you decrease this value.\n"
                      "\n"
                      "Also note that the architecture of chess engines is not usually very well suited to hyperthreading.  "
                      "It is therefore recommended that this not be set to a number larger than the number of physical cores "
                      "in your machine.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpmintime()
{
   CString sMessage = "Specify the minimum time (in milliseconds) to spend analysing each position.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpmaxtime()
{
   CString sMessage = "Specify the maximum time (in milliseconds) to spend analysing each position.  This will force "
                      "analysis to stop, even if the minimum search depth has not yet been reached.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedHelpphashsize()
{
   CString sMessage = "Specify the size of the engine's memory cache (in MB).\n"
                      "\n"
                      "See documentation for your engine for optimal values for this setting.\n"
                      "\n"
                      "Note: if using multiple threads, remember that each thread will have its own hash.  As this is "
                      "stored in memory, it is recommended that you ensure that the total hash size for threads does "
                      "not exceed the RAM that is physically present on your machine; you should also leave sufficient "
                      "for the operating system and any other software currently running.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedForcedmovehelp()
{
   CString sMessage = "For T1/T2/T3/etc. analysis, moves where the next-best move are evaluated to be worse than the "
                      "move in question by more than the specified threshold will be excluded from analysis.  This avoids "
                      "flagging obvious recaptures and other moves that a strong player would usually be expected to find.  "
                      "Values are in centipawns.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedUnclearpositionhelp()
{
   CString sMessage = "For T1/T2/T3/etc. analysis, moves where the next-best move is evaluated to be worse than the "
                      "first-choice move by more than the specified threshold will be excluded from analysis.  Values "
                      "are in centipawns.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedEqualpositionhelp()
{
   CString sMessage = "Positions where neither side is ahead by more than the specified threshold will be analysed.  "
                      "This is to help detect cheaters who stop cheating once they are ahead.\n\nThese results will be "
                      "reported separately from those for losing positions.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedLosingthresholdhelp()
{
   CString sMessage = "Positions where the player behind by more than the equal position threshold and less than the "
                      "losing position threshold will be analysed.  This is to help detect cheaters who only cheat once "
                      "they start to lose.\n\nThese results will be reported separately from those for equal positions.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedBlunderthresholdhelp()
{
   CString sMessage = "Moves that are evaluated to be worse than the best move by the specified threshold will be "
                      "counted as blunders.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedNumvariationshelp()
{
   CString sMessage = "Specify the number of top engine moves to be compared to the actual move played in each position.  "
                      "This is to help detect cheaters who regularly play a second-choice move, or who use a different "
                      "engine or engine settings.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CPGNSpyDlg::OnBnClickedExcludeforced()
{
   DisableInvalidSettings();
}

void CPGNSpyDlg::OnBnClickedIncludeonlyunclear()
{
   DisableInvalidSettings();
}

void CPGNSpyDlg::DisableInvalidSettings()
{
   UpdateData();
   //disable/enable controls as appropriate
   GetDlgItem(IDC_UNCLEARPOSITIONTHRESHOLD)->EnableWindow(m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions);
   GetDlgItem(IDC_FORCEDMOVECUTOFF)->EnableWindow(m_vAnalysisSettings.m_bExcludeForcedMoves);

   //if controls are disabled, reset disabled values to ensure we don't get validation failures triggered by bad numbers in the disabled values
   if (!m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions)
      m_vAnalysisSettings.m_iUnclearPositionCutoff = 100;
   if (!m_vAnalysisSettings.m_bExcludeForcedMoves)
      m_vAnalysisSettings.m_iForcedMoveCutoff = 50;
   UpdateData(FALSE);
}