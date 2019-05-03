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

// ResultsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PGN Spy.h"
#include "ResultsDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include <fstream>

// CResultsDlg dialog

IMPLEMENT_DYNAMIC(CResultsDlg, CDialogEx)

CResultsDlg::CResultsDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(IDD_RESULTS, pParent)
   , m_sResults(_T(""))
{

}

CResultsDlg::~CResultsDlg()
{
}

void CResultsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_RESULTS, m_sResults);
   DDX_Check(pDX, IDC_EXCLUDEFORCED, m_vAnalysisSettings.m_bExcludeForcedMoves);
   DDX_Check(pDX, IDC_INCLUDEONLYUNCLEAR, m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions);
   DDX_Text(pDX, IDC_FORCEDMOVECUTOFF, m_vAnalysisSettings.m_iForcedMoveCutoff);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iForcedMoveCutoff, 25, 10000);
   DDX_Text(pDX, IDC_UNCLEARPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iUnclearPositionCutoff);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iUnclearPositionCutoff, 25, 10000);
   DDX_Text(pDX, IDC_LOSINGPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iLosingThreshold);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iLosingThreshold, 25, 10000);
   DDX_Text(pDX, IDC_EQUALPOSITIONTHRESHOLD, m_vAnalysisSettings.m_iEqualPositionThreshold);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iEqualPositionThreshold, 25, 10000);
   DDX_Text(pDX, IDC_MOVERANGEMIN, m_vAnalysisSettings.m_iMoveNumMin);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iMoveNumMin, 1, 200);
   DDX_Text(pDX, IDC_MOVERANGEMAX, m_vAnalysisSettings.m_iMoveNumMax);
   DDV_MinMaxInt(pDX, m_vAnalysisSettings.m_iMoveNumMax, 1, 10000);
   DDX_CBString(pDX, IDC_PLAYER, m_vAnalysisSettings.m_sPlayerName);
   DDV_MaxChars(pDX, m_vAnalysisSettings.m_sPlayerName, 300);
   DDX_CBString(pDX, IDC_OPPONENT, m_vAnalysisSettings.m_sOpponentName);
   DDV_MaxChars(pDX, m_vAnalysisSettings.m_sOpponentName, 300);
   DDX_CBString(pDX, IDC_EVENT, m_vAnalysisSettings.m_sEvent);
   DDV_MaxChars(pDX, m_vAnalysisSettings.m_sEvent, 300);
   DDX_Control(pDX, IDC_PLAYER, m_vPlayers);
   DDX_Control(pDX, IDC_OPPONENT, m_vOpponents);
   DDX_Control(pDX, IDC_EVENT, m_vEvents);
   DDX_Check(pDX, IDC_INCLUDELOSING, m_vAnalysisSettings.m_bIncludeLosing);
   DDX_Check(pDX, IDC_INCLUDEWINNING, m_vAnalysisSettings.m_bIncludeWinning);
   DDX_Check(pDX, IDC_INCLUDEPOSTLOSING, m_vAnalysisSettings.m_bIncludePostLosing);
   DDX_Check(pDX, IDC_INCLUDEWINS, m_vAnalysisSettings.m_bIncludeWins);
   DDX_Check(pDX, IDC_INCLUDELOSSES, m_vAnalysisSettings.m_bIncludeLosses);
   DDX_Check(pDX, IDC_INCLUDEDRAWS, m_vAnalysisSettings.m_bIncludeDraws);
}


BEGIN_MESSAGE_MAP(CResultsDlg, CDialogEx)
   ON_BN_CLICKED(IDC_ABOUT, &CResultsDlg::OnBnClickedAbout)
   ON_BN_CLICKED(IDC_SAVEDATA, &CResultsDlg::OnBnClickedSavedata)
   ON_BN_CLICKED(IDC_FORCEDMOVEHELP, &CResultsDlg::OnBnClickedForcedmovehelp)
   ON_BN_CLICKED(IDC_UNCLEARPOSITIONHELP, &CResultsDlg::OnBnClickedUnclearpositionhelp)
   ON_BN_CLICKED(IDC_EQUALPOSITIONHELP, &CResultsDlg::OnBnClickedEqualpositionhelp)
   ON_BN_CLICKED(IDC_HELPPLAYER, &CResultsDlg::OnBnClickedHelpplayer)
   ON_BN_CLICKED(IDC_HELPOPPONENT, &CResultsDlg::OnBnClickedHelpopponent)
   ON_BN_CLICKED(IDC_HELPEVENT, &CResultsDlg::OnBnClickedHelpevent)
   ON_BN_CLICKED(IDC_HELPMOVERANGEMIN, &CResultsDlg::OnBnClickedHelpmoverangemin)
   ON_BN_CLICKED(IDC_HELPMOVERANGEMAX, &CResultsDlg::OnBnClickedHelpmoverangemax)
   ON_BN_CLICKED(IDC_LOSINGTHRESHOLDHELP, &CResultsDlg::OnBnClickedLosingthresholdhelp)
   ON_BN_CLICKED(IDC_SAVESETTINGS, &CResultsDlg::OnBnClickedSavesettings)
   ON_BN_CLICKED(IDC_RECALCULATE, &CResultsDlg::OnBnClickedRecalculate)
   ON_BN_CLICKED(IDC_SAVERESULTS, &CResultsDlg::OnBnClickedSaveresults)
   ON_BN_CLICKED(IDC_EXCLUDEFORCED, &CResultsDlg::OnBnClickedExcludeforced)
   ON_BN_CLICKED(IDC_INCLUDEONLYUNCLEAR, &CResultsDlg::OnBnClickedIncludeonlyunclear)
   ON_BN_CLICKED(IDC_HELPINCLUDE, &CResultsDlg::OnBnClickedHelpinclude)
   ON_BN_CLICKED(IDC_PERGAMEEXPORT, &CResultsDlg::OnBnClickedPerGameExport)
   ON_BN_CLICKED(IDC_LOADANDMERGERESULTS, &CResultsDlg::OnBnClickedLoadAndMergeResults)
END_MESSAGE_MAP()

BOOL CResultsDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   LoadPlayerAndEventLists();
   DisableInvalidSettings();

   CalculateStats();
   
   return TRUE;  // return TRUE  unless you set the focus to a control
}

// CResultsDlg message handlers


void CResultsDlg::OnBnClickedAbout()
{
   CString sMessage;
   sMessage = "T1/T2/T3/etc: These stats display information about how often a player's moves matched the top "
              "one, two, three, etc., engine moves.  A high number could be an indicator of possible engine use.\n"
              "\n"
              ">N CP Loss: This indicates how often a player's moves were worse than the top engine move by the "
              "specified value (in centipawns).  A low number could be an indicator of possible engine use.\n"
              "\n"
              "CP loss: This indicates how much worse a player's moves were when compared to the top engine move. "
              "A low number could be an indicator of possible engine use.  Values are in centipawns (1/100 of a pawn).\n"
              "\n"
              "These values MUST NOT be taken as evidence of cheating on their own, without proper statistical analysis, "
              "comparison to appropriate benchmarks, and consideration of other evidence.\n"
              "\n"
              "Binomial confidence intervals can be calculated at :"
              "\nhttp://statpages.info/confint.html#Binomial";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}

void CResultsDlg::OnBnClickedSavedata()
{
   CFileDialog vFileDialog(FALSE, _T("tab"), _T("*.tab"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Tab-delimited files (*.tab)|*.tab|All files (*.*)|*.*||"), this);
   if (vFileDialog.DoModal() != IDOK)
      return;
   CString sFilePath = vFileDialog.GetPathName();
   CString sReport, sLine, sText;
   sReport = "Event\tDate\tWhite\tBlack\tWhite Rating\tBlack Rating\tResult\tTime Control\tMove #\tPlayer\tMove Played\tDepth Searched\tT-number";
   for (int i = 0; i < m_vEngineSettings.m_iNumVariations+1; i++)
   {
      sText.Format("\tT%i move eval", i + 1);
      sReport += sText;
   }
   sReport += "\tNon-top move eval";
   for (int iGame = 0; iGame < m_avGames.GetSize(); iGame++)
   {
      CGame *pGame = &m_avGames[iGame];
      for (int iPosition = 0; iPosition < pGame->m_avPositions.GetSize(); iPosition++)
      {
         //first get general game data
         CPosition *pPosition = &pGame->m_avPositions[iPosition];
         //the \' before the result is so Excel won't be stupid and treat 1-0 as a date - 1/1/2000
         sLine = pGame->m_sEvent + "\t" + pGame->m_sDate + "\t" + pGame->m_sWhite + "\t" + pGame->m_sBlack + "\t" + pGame->m_sWhiteElo + "\t" + pGame->m_sBlackElo + "\t\'" + pGame->m_sResult + "\t" + pGame->m_sTimeControl + "\t";

         //got all game data, now get move data
         //move number and coordinates
         sText.Format("%i\t", (iPosition/2) + m_vEngineSettings.m_iBookDepth + 1);
         sLine += sText;
         sText = (pPosition->m_bWhite) ? "White" : "Black";
         sLine += sText + "\t" + pPosition->m_avTopMoves[pPosition->m_iMovePlayed].m_sMove + "\t";
         //max depth searched
         int iMaxDepth = 0;
         for (int i = 0; i < pPosition->m_avTopMoves.GetSize(); i++)
            iMaxDepth = max(iMaxDepth, pPosition->m_avTopMoves[i].m_iDepth);
         sText.Format("%i\t", iMaxDepth);
         sLine += sText;
         //T-number of move
         sText.Format("%i\t", pPosition->m_iMovePlayed + 1);
         sLine += sText;
         //evaluation of variants
         int iMove = 0;
         for (; iMove < pPosition->m_avTopMoves.GetSize(); iMove++)
         {
            sText.Format("%i\t", pPosition->m_avTopMoves[iMove].m_iScore);
            sLine += sText;
         }
         for (; iMove < m_vEngineSettings.m_iNumVariations + 1; iMove++)
            sLine += "N/A\t";
         sReport += "\r\n" + sLine;
      }
   }

   CFile vFile;
   if (!vFile.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
   {
      MessageBox("Failed to create output file.", "PGN Spy", MB_ICONEXCLAMATION);
      return;
   }

   vFile.Write(sReport.GetBuffer(), sReport.GetLength());
   sReport.ReleaseBuffer();
   vFile.Close();
   MessageBox("File saved.", "PGN Spy", MB_ICONINFORMATION);

   ShellExecute(NULL, "open", sFilePath, NULL, NULL, SW_MAXIMIZE);
}

bool CResultsDlg::IncludeGameInStats(const CGame &vGame, bool &bExcludeWhite, bool &bExcludeBlack)
{
   bExcludeWhite = m_vAnalysisSettings.m_bBlackOnly;
   bExcludeBlack = m_vAnalysisSettings.m_bWhiteOnly;

   //exclude games without the right players involved
   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty() &&
      m_vAnalysisSettings.m_sPlayerName.CompareNoCase(vGame.m_sWhite) != 0 &&
      m_vAnalysisSettings.m_sPlayerName.CompareNoCase(vGame.m_sBlack) != 0)
      return false;
   if (!m_vAnalysisSettings.m_sOpponentName.IsEmpty() &&
      m_vAnalysisSettings.m_sOpponentName.CompareNoCase(vGame.m_sWhite) != 0 &&
      m_vAnalysisSettings.m_sOpponentName.CompareNoCase(vGame.m_sBlack) != 0)
      return false;
   if (!m_vAnalysisSettings.m_sEvent.IsEmpty() &&
      m_vAnalysisSettings.m_sEvent.CompareNoCase(vGame.m_sEvent) != 0)
      return false;

   if (!m_vAnalysisSettings.m_bIncludeDraws && vGame.m_sResult.CompareNoCase("1/2-1/2") == 0)
      return false;

   if (!m_vAnalysisSettings.m_bIncludeLosses)
   {
      if (vGame.m_sResult.CompareNoCase("1-0") == 0)
         bExcludeBlack = true;
      else if (vGame.m_sResult.CompareNoCase("0-1") == 0)
         bExcludeWhite = true;
   }
   if (!m_vAnalysisSettings.m_bIncludeWins)
   {
      if (vGame.m_sResult.CompareNoCase("1-0") == 0)
         bExcludeWhite = true;
      else if (vGame.m_sResult.CompareNoCase("0-1") == 0)
         bExcludeBlack = true;
   }

   if (bExcludeWhite && bExcludeBlack)
      return false;

   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty())
   {
      if (bExcludeWhite && vGame.m_sWhite.CompareNoCase(m_vAnalysisSettings.m_sPlayerName) == 0)
         return false;
      if (bExcludeBlack && vGame.m_sBlack.CompareNoCase(m_vAnalysisSettings.m_sPlayerName) == 0)
         return false;
   }
   return true;
}

bool CResultsDlg::IncludePositionInStats(const CGame &vGame, const CPosition &vPosition, int iMoveNum, bool bExcludeWhite, bool bExcludeBlack)
{
   if (iMoveNum < m_vAnalysisSettings.m_iMoveNumMin || iMoveNum > m_vAnalysisSettings.m_iMoveNumMax)
      return false;
   if ((bExcludeWhite && vPosition.m_bWhite) || (bExcludeBlack && !vPosition.m_bWhite))
      return false;

   //check player names
   CString sPlayerName = (vPosition.m_bWhite) ? vGame.m_sWhite : vGame.m_sBlack;
   CString sOpponentName = (vPosition.m_bWhite) ? vGame.m_sBlack : vGame.m_sWhite;
   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty() &&
      m_vAnalysisSettings.m_sPlayerName.CompareNoCase(sPlayerName) != 0)
      return false;
   if (!m_vAnalysisSettings.m_sOpponentName.IsEmpty() &&
      m_vAnalysisSettings.m_sOpponentName.CompareNoCase(sOpponentName) != 0)
      return false;
   return true;
}

void CResultsDlg::CalculateStats()
{
   m_vUndecidedPositions.Initialize(m_vEngineSettings);
   m_vLosingPositions.Initialize(m_vEngineSettings);
   m_vWinningPositions.Initialize(m_vEngineSettings);
   m_vPostLosingPositions.Initialize(m_vEngineSettings);

   int iGamesInSubset = 0;
   //calculate results
   for (int iGame = 0; iGame < m_avGames.GetSize(); iGame++)
   {
      bool bExcludeWhite, bExcludeBlack;
      if (!IncludeGameInStats(m_avGames[iGame], bExcludeWhite, bExcludeBlack))
         continue;

      iGamesInSubset++;
      int iMoveNum = m_vEngineSettings.m_iBookDepth;
      bool bFoundLosingPositionWhite = false;
      bool bFoundLosingPositionBlack = false;
      for (int iPosition = 0; iPosition < m_avGames[iGame].m_avPositions.GetSize(); iPosition++)
      {
         CPosition *pPosition = &m_avGames[iGame].m_avPositions[iPosition];
         if (pPosition->m_bWhite || !m_vEngineSettings.m_sPlayerName.IsEmpty())
            iMoveNum++; //increment move if we're looking at a white move, or if engine only analysed one player's games
         if (!IncludePositionInStats(m_avGames[iGame], *pPosition, iMoveNum, bExcludeWhite, bExcludeBlack))
            continue;

         if (pPosition->IsEqualPosition(m_vAnalysisSettings.m_iEqualPositionThreshold))
            m_vUndecidedPositions.AddPosition(*pPosition, m_vAnalysisSettings);
         if (pPosition->IsLosingPosition(m_vAnalysisSettings.m_iEqualPositionThreshold, m_vAnalysisSettings.m_iLosingThreshold))
         {
            m_vLosingPositions.AddPosition(*pPosition, m_vAnalysisSettings);
            if (pPosition->m_bWhite)
               bFoundLosingPositionWhite = true;
            else
               bFoundLosingPositionBlack = true;
         }
         if (pPosition->IsWinningPosition(m_vAnalysisSettings.m_iEqualPositionThreshold, m_vAnalysisSettings.m_iLosingThreshold))
            m_vWinningPositions.AddPosition(*pPosition, m_vAnalysisSettings);
         if (!pPosition->IsExcludedPosition(m_vAnalysisSettings.m_iLosingThreshold) &&
            (pPosition->m_bWhite && bFoundLosingPositionWhite) || (!pPosition->m_bWhite && bFoundLosingPositionBlack))
         {
            //if we've found a losing position and we're not now in a totally dead position, we've got a
            //post-losing position, useful for catching someone who cheats to save a lost game.
            m_vPostLosingPositions.AddPosition(*pPosition, m_vAnalysisSettings);
         }
      }
   }

   m_vUndecidedPositions.FinaliseStats();
   m_vLosingPositions.FinaliseStats();
   m_vWinningPositions.FinaliseStats();
   m_vPostLosingPositions.FinaliseStats();

   //Now dump results to text
   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty())
      m_sResults.Format("%s, %i games\r\n\r\n", m_vAnalysisSettings.m_sPlayerName, iGamesInSubset);
   else
      m_sResults.Format("%i games\r\n\r\n", iGamesInSubset);
   m_sResults += "UNDECIDED POSITIONS\r\n";
   m_sResults += m_vUndecidedPositions.GetResultsText();
   if (m_vAnalysisSettings.m_bIncludeLosing)
   {
      m_sResults += "\r\nLOSING POSITIONS\r\n";
      m_sResults += m_vLosingPositions.GetResultsText();
   }
   if (m_vAnalysisSettings.m_bIncludeWinning)
   {
      m_sResults += "\r\nWINNING POSITIONS\r\n";
      m_sResults += m_vWinningPositions.GetResultsText();
   }
   if (m_vAnalysisSettings.m_bIncludePostLosing)
   {
      m_sResults += "\r\nPOST-LOSING POSITIONS\r\n";
      m_sResults += m_vPostLosingPositions.GetResultsText();
   }

   UpdateData(FALSE);
}

void CResultsDlg::OnBnClickedForcedmovehelp()
{
   CString sMessage = "For T1/T2/T3/etc. analysis, moves where the next-best move are evaluated to be worse than the "
                      "move in question by more than the specified threshold will be excluded from analysis.  This avoids "
                      "flagging obvious recaptures and other moves that a strong player would usually be expected to find.  "
                      "Values are in centipawns.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedUnclearpositionhelp()
{
   CString sMessage = "For T1/T2/T3/etc. analysis, moves where the next-best move is evaluated to be worse than the "
                      "first-choice move by more than the specified threshold will be excluded from analysis.  Values "
                      "are in centipawns.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedEqualpositionhelp()
{
   CString sMessage = "Positions where neither side is ahead by more than the specified threshold will be analysed.  "
                      "This is to help detect cheaters who stop cheating once they are ahead.\n\nThese results will be "
                      "reported separately from those for losing positions.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedHelpplayer()
{
   CString sMessage = "If a player name is entered, statistics for the specified player will be reported.  Games excluding "
                      "this player will be ignored.\n\nIf no player name is entered, aggregate statistics for all players "
                      "will be reported.  This is useful for establishing baselines.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedHelpopponent()
{
   CString sMessage = "If a player name is entered, statistics for all moves played against the specified player will "
                      "be reported.  Games excluding this player will be ignored.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedHelpevent()
{
   CString sMessage = "If an event name is entered, statistics for moves played during the specified event will be reported.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedHelpmoverangemin()
{
   CString sMessage = "Moves before the specified move number will be excluded from analysis.  This must be greater "
                      "than the book depth specified in engine settings when the analysis was run.\n\n"
                      "Note: this counts one move for each side as a single move.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedHelpmoverangemax()
{
   CString sMessage = "Moves after the specified move number will be excluded from analysis.\n\n"
                      "Note: this counts one move for each side as a single move.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedLosingthresholdhelp()
{
   CString sMessage = "Positions where the player behind by more than the equal position threshold and less than the "
                      "losing position threshold will be analysed.  This is to help detect cheaters who only cheat once "
                      "they start to lose.\n\nThese results will be reported separately from those for equal positions.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedSavesettings()
{
   if (!ValidateSettings())
      return;

   if (!m_vAnalysisSettings.SaveSettingsToRegistry())
      MessageBox("Failed to save settings.", "PGN Spy", MB_ICONEXCLAMATION);
   else
      MessageBox("Settings saved.", "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedRecalculate()
{
   if (!ValidateSettings())
      return;

   CalculateStats();
}


void CResultsDlg::OnBnClickedSaveresults()
{
   CFileDialog vFileDialog(FALSE, _T("xml"), _T("*.xml"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("PGN Spy files (*.xml)|*.xml|All files (*.*)|*.*||"), this);
   if (vFileDialog.DoModal() != IDOK)
      return;
   CString sFilePath = vFileDialog.GetPathName();
   if (SaveGameArrayToFile(sFilePath, m_avGames, m_vEngineSettings))
      MessageBox(_T("Results saved."), _T("PGN Spy"), MB_ICONINFORMATION);
   else
      MessageBox(_T("Failed to save results."), _T("PGN Spy"), MB_ICONEXCLAMATION);
}

bool CResultsDlg::ValidateSettings()
{
   if (!UpdateData())
      return false;

   if (m_vAnalysisSettings.m_iMoveNumMin <= m_vEngineSettings.m_iBookDepth)
   {
      MessageBox("The move range minimum must be greater than the engine book depth.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   if (m_vAnalysisSettings.m_iMoveNumMin > m_vAnalysisSettings.m_iMoveNumMax)
   {
      MessageBox("The move range minimum must not be greater than the move range maximum.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }
   if (m_vAnalysisSettings.m_iEqualPositionThreshold > m_vAnalysisSettings.m_iLosingThreshold)
   {
      MessageBox("The equal position threshold must not exceed the losing position threshold.", "PGN Spy", MB_ICONEXCLAMATION);
      return false;
   }

   m_vAnalysisSettings.m_bWhiteOnly = IsDlgButtonChecked(IDC_WHITEMOVES) == BST_CHECKED;
   m_vAnalysisSettings.m_bBlackOnly = IsDlgButtonChecked(IDC_BLACKMOVES) == BST_CHECKED;

   return true;
}

void CResultsDlg::DisableInvalidSettings()
{
   UpdateData();
   //disable/enable controls as appropriate
   GetDlgItem(IDC_UNCLEARPOSITIONTHRESHOLD)->EnableWindow(m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions);
   GetDlgItem(IDC_FORCEDMOVECUTOFF)->EnableWindow(m_vAnalysisSettings.m_bExcludeForcedMoves);
   GetDlgItem(IDC_PLAYER)->EnableWindow(m_vEngineSettings.m_sPlayerName.IsEmpty());//player setting should be disabled if specified in engine settings

   //if controls are disabled, reset disabled values to ensure we don't get validation failures triggered by bad numbers in the disabled values
   if (!m_vAnalysisSettings.m_bIncludeOnlyUnclearPositions)
      m_vAnalysisSettings.m_iUnclearPositionCutoff = 100;
   if (!m_vAnalysisSettings.m_bExcludeForcedMoves)
      m_vAnalysisSettings.m_iForcedMoveCutoff = 50;
   UpdateData(FALSE);
}

void CResultsDlg::LoadPlayerAndEventLists()
{
   m_vPlayers.ResetContent();
   m_vOpponents.ResetContent();
   m_vEvents.ResetContent();
   CStringArray asPlayers, asEvents;
   CArray<int, int> aiPlayers, aiEvents;
   for (int iGame = 0; iGame < m_avGames.GetSize(); iGame++)
   {
      AddStringIfNotFound(m_avGames[iGame].m_sWhite, asPlayers, aiPlayers);
      AddStringIfNotFound(m_avGames[iGame].m_sBlack, asPlayers, aiPlayers);
      AddStringIfNotFound(m_avGames[iGame].m_sEvent, asEvents, aiEvents);
   }
   for (int i = 0; i < asPlayers.GetSize(); i++)
      m_vPlayers.AddString(asPlayers[i]);
   for (int i = 0; i < asPlayers.GetSize(); i++)
   {
      if (m_vEngineSettings.m_sPlayerName.CompareNoCase(asPlayers[i]) != 0)
         m_vOpponents.AddString(asPlayers[i]);
   }
   for (int i = 0; i < asEvents.GetSize(); i++)
      m_vEvents.AddString(asEvents[i]);

   if (!m_vAnalysisSettings.LoadSettingsFromRegistry())
      m_vAnalysisSettings = CAnalysisSettings();
   m_vAnalysisSettings.m_iMoveNumMin = m_vEngineSettings.m_iBookDepth + 1;
   m_vAnalysisSettings.m_sPlayerName = m_vEngineSettings.m_sPlayerName;
   if (!m_vEngineSettings.m_sPlayerName.IsEmpty())
      m_vPlayers.SelectString(-1, m_vAnalysisSettings.m_sPlayerName);
   CheckDlgButton(IDC_ALLMOVES, BST_CHECKED);
   CheckDlgButton(IDC_WHITEMOVES, BST_UNCHECKED);
   CheckDlgButton(IDC_BLACKMOVES, BST_UNCHECKED);

   UpdateData(FALSE);
}

void CResultsDlg::OnBnClickedExcludeforced()
{
   DisableInvalidSettings();
}

void CResultsDlg::OnBnClickedIncludeonlyunclear()
{
   DisableInvalidSettings();
}


void CResultsDlg::OnBnClickedHelpinclude()
{
   CString sMessage = "Losing positions are positions where the player in question is losing by more than the undecided "
                      "position threshold but less than the losing position threshold.  Winning positions are positions "
                      "where the player in question is winning by more than the undecided position threshold but less "
                      "than the losing position threshold.  Post-losing positions are positions where the player in "
                      "question was losing earlier in the game.  Positions where either player is losing by more than "
                      "the losing position threshold will always be excluded.";
   MessageBox(sMessage, "PGN Spy", MB_ICONINFORMATION);
}


void CResultsDlg::OnBnClickedPerGameExport()
{
   //Required stats:
   //number of undecided positions
   //T(n) num
   //T(n) denom
   //=0 CP loss num
   //>10 CP loss num
   //>25 CP loss num
   //>50 CP loss num
   //>100 CP loss num
   //>200 CP loss num
   //>500 CP loss num
   //CP loss mean
   CFileDialog vFileDialog(FALSE, _T("tab"), _T("*.tab"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Tab-delimited files (*.tab)|*.tab|All files (*.*)|*.*||"), this);
   if (vFileDialog.DoModal() != IDOK)
      return;
   CString sFilePath = vFileDialog.GetPathName();

   CString sReport, sText;
   sReport = "Event\tDate\tPlayer\tColour\tResult\tTime Control\tUndecided positions\tRating";
   for (int i = 0; i < m_vEngineSettings.m_iNumVariations; i++)
   {
      sText.Format("\tT%i moves\tT%i positions\tT%i%%", i + 1, i + 1, i + 1);
      sReport += sText;
   }
   sReport += "\t=0 CP loss num\t>0 CP loss num\t>10 CP loss num\t>25 CP loss num\t>50 CP loss num\t>100 CP loss num\t>200 CP loss num\t>500 CP loss num\tCP loss mean";
   int iGamesInSubset = 0;
   for (int iGame = 0; iGame < m_avGames.GetSize(); iGame++)
   {
      CGame *pGame = &m_avGames[iGame];
      bool bExcludeWhite, bExcludeBlack;
      if (!IncludeGameInStats(*pGame, bExcludeWhite, bExcludeBlack))
         continue;
      iGamesInSubset++;

	  CStats vWhiteUndecidedPositions;
	  CStats vBlackUndecidedPositions;
	  vWhiteUndecidedPositions.Initialize(m_vEngineSettings);
	  vBlackUndecidedPositions.Initialize(m_vEngineSettings);

      int iMoveNum = m_vEngineSettings.m_iBookDepth;
      for (int iPosition = 0; iPosition < pGame->m_avPositions.GetSize(); iPosition++)
      {
         CPosition *pPosition = &pGame->m_avPositions[iPosition];
         if (pPosition->m_bWhite || !m_vEngineSettings.m_sPlayerName.IsEmpty())
            iMoveNum++; //increment move if we're looking at a white move, or if engine only analysed one player's games
         if (!IncludePositionInStats(*pGame, *pPosition, iMoveNum, bExcludeWhite, bExcludeBlack))
            continue;
		 if (pPosition->IsEqualPosition(m_vAnalysisSettings.m_iEqualPositionThreshold)) {
			 if (pPosition->m_bWhite) {
				 vWhiteUndecidedPositions.AddPosition(*pPosition, m_vAnalysisSettings);
			 }
			 else {
				 vBlackUndecidedPositions.AddPosition(*pPosition, m_vAnalysisSettings);
			 }

		 }
      }
      vWhiteUndecidedPositions.FinaliseStats();
      vBlackUndecidedPositions.FinaliseStats();

      //first get header information
	  CString sLine, whiteHeader, blackHeader;
	  whiteHeader = pGame->m_sEvent + "\t" + pGame->m_sDate + "\t" + pGame->m_sWhite + "\tWhite";
	  blackHeader = pGame->m_sEvent + "\t" + pGame->m_sDate + "\t" + pGame->m_sBlack + "\tBlack";

      sLine = "\t\'" + pGame->m_sResult; //the \' is so that Excel doesn't interpret 1-0 as a date - 1/1/2000
      sLine += "\t" + pGame->m_sTimeControl;

      sText.Format("\t%i", vWhiteUndecidedPositions.m_iNumPositions);
	  whiteHeader += sLine +  sText + "\t" + pGame->m_sWhiteElo;

      sText.Format("\t%i", vBlackUndecidedPositions.m_iNumPositions);
	  blackHeader += sLine +  sText + "\t" + pGame->m_sBlackElo;

	  //Now get the stats (T-stats, CP loss, etc.)
      sReport += "\r\n" + whiteHeader + generatePlayerGameStats(vWhiteUndecidedPositions);
      sReport += "\r\n" + blackHeader + generatePlayerGameStats(vBlackUndecidedPositions);
   }

   CFile vFile;
   if (!vFile.Open(sFilePath, CFile::modeCreate | CFile::modeWrite))
   {
      MessageBox("Failed to create output file.", "PGN Spy", MB_ICONEXCLAMATION);
      return;
   }

   vFile.Write(sReport.GetBuffer(), sReport.GetLength());
   sReport.ReleaseBuffer();
   vFile.Close();
   MessageBox("File saved.", "PGN Spy", MB_ICONINFORMATION);

   ShellExecute(NULL, "open", sFilePath, NULL, NULL, SW_MAXIMIZE);
}

CString CResultsDlg::generatePlayerGameStats(CStats& vUndecidedPositions) {
	//Generate a row for the CSV File
	//get T-stats
	CString gameStatsOutput = "";
	CString gameStats;

	//now get T-stats
	for (int i = 0; i < m_vEngineSettings.m_iNumVariations; i++)
	{
		double dTVal = 0;
		if (vUndecidedPositions.m_aiTMoves[i] > 0)
			dTVal = ((double)vUndecidedPositions.m_aiTValues[i] / (double)vUndecidedPositions.m_aiTMoves[i]) * 100.0;
		gameStats.Format("\t%i\t%i\t%.2f%%", vUndecidedPositions.m_aiTValues[i], vUndecidedPositions.m_aiTMoves[i], dTVal);
		gameStatsOutput += gameStats;
	}

	//get CP loss values
	gameStats.Format("\t%f", (vUndecidedPositions.m_iNumPositions - vUndecidedPositions.m_i0CPLoss) / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i0CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i10CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i25CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i50CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i100CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i200CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%f", vUndecidedPositions.m_i500CPLoss / (double)vUndecidedPositions.m_iNumPositions);
	gameStatsOutput += gameStats;
	gameStats.Format("\t%.2f", vUndecidedPositions.m_dAvgCentipawnLoss);
	gameStatsOutput += gameStats;
	return gameStatsOutput;
}

void CResultsDlg::OnBnClickedLoadAndMergeResults()
{
   CFileDialog vFileDialog(TRUE, "xml", "*.xml", OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_DONTADDTORECENT, "PGN Spy files (*.xml)|*.xml|All files (*.*)|*.*||", this);
   if (vFileDialog.DoModal() != IDOK)
      return;                      
   CArray <CGame, CGame> avGames;
   CEngineSettings vOtherEngineSettings;
   if (!LoadGameArrayFromFile(vFileDialog.GetPathName(), avGames, vOtherEngineSettings))
   {
      MessageBox(_T("Failed to load game file."), _T("PGN Spy"), MB_ICONEXCLAMATION);
      return;
   }
   CString sWarning;
   CEngineSettings vCompatibleSettings = m_vEngineSettings.MakeCompatible(vOtherEngineSettings, sWarning);
   if (!sWarning.IsEmpty())
   {
      CString sMessage = "Engine setting compatibility warnings were encountered.  Do you wish to continue?\r\n\r\n" + sWarning;
      if (MessageBox(sMessage, "PGN Spy", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
         return;
   }
   m_vEngineSettings = vCompatibleSettings;
   m_avGames.Append(avGames);
   LoadPlayerAndEventLists();
   CalculateStats();
   MessageBox("Analysis results successfully loaded and merged.", "PGN Spy", MB_ICONINFORMATION);
}


