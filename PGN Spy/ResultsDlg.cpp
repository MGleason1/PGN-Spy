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
}


BEGIN_MESSAGE_MAP(CResultsDlg, CDialogEx)
   ON_BN_CLICKED(IDC_ABOUT, &CResultsDlg::OnBnClickedAbout)
   ON_BN_CLICKED(IDC_SAVEDATA, &CResultsDlg::OnBnClickedSavedata)
END_MESSAGE_MAP()

BOOL CResultsDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   CalculateStats();

   return TRUE;  // return TRUE  unless you set the focus to a control
}

// CResultsDlg message handlers


void CResultsDlg::OnBnClickedAbout()
{
   CString sMessage;
   sMessage = "T1/T2/T3/etc: These stats display information about how often a player's moves matched the top\n"
              "one, two, three, etc., engine moves.  A high number could be an indicator of possible engine use.\n"
              "\n"
              ">N CP Loss: This indicates how often a player's moves were worse than the top engine move by the\n"
              "specified value (in centipawns).  A low number could be an indicator of possible engine use.\n"
              "\n"
              "CP loss: This indicates how much worse a player's moves were when compared to the top engine move.\n"
              "A low number could be an indicator of possible engine use.  Values are in centipawns (1/100 of a pawn).\n"
              "\n"
              "These values MUST NOT be taken as evidence of cheating on their own, without proper statistical analysis,\n"
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
   sReport = "Event\tDate\tWhite\tBlack\tResult\tTime Control\tMove #\tMove Played\tDepth Searched\tT-number";
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
         sLine = pGame->m_sEvent + "\t" + pGame->m_sDate + "\t" + pGame->m_sWhite + "\t" + pGame->m_sBlack + "\t" + pGame->m_sResult + "\t" + pGame->m_sTimeControl + "\t";

         //got all game data, now get move data
         //move number and coordinates
         sText.Format("%i\t", iPosition + m_vEngineSettings.m_iBookDepth + 1);
         sLine += sText + pPosition->m_avTopMoves[pPosition->m_iMovePlayed].m_sMove + "\t";
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

void CResultsDlg::CalculateStats()
{
   m_vUndecidedPositions.Initialize(m_vEngineSettings);
   m_vLosingPositions.Initialize(m_vEngineSettings);

   //calculate results
   for (int iGame = 0; iGame < m_avGames.GetSize(); iGame++)
   {
      for (int iPosition = 0; iPosition < m_avGames[iGame].m_avPositions.GetSize(); iPosition++)
      {
         CPosition *pPosition = &m_avGames[iGame].m_avPositions[iPosition];
         if (pPosition->IsEqualPosition(m_vAnalysisSettings.m_iEqualPositionThreshold))
            m_vUndecidedPositions.AddPosition(*pPosition, m_vAnalysisSettings);
         if (pPosition->IsLosingPosition(m_vAnalysisSettings.m_iEqualPositionThreshold, m_vAnalysisSettings.m_iLosingThreshold))
            m_vLosingPositions.AddPosition(*pPosition, m_vAnalysisSettings);
      }
   }

   m_vUndecidedPositions.FinaliseStats();
   m_vLosingPositions.FinaliseStats();

   //Now dump results to text
   m_sResults.Format("%i games\r\n\r\n", m_avGames.GetSize());
   m_sResults += "UNDECIDED POSITIONS\r\n";
   m_sResults += m_vUndecidedPositions.GetResultsText() + "\r\n";
   m_sResults += "LOSING POSITIONS\r\n";
   m_sResults += m_vLosingPositions.GetResultsText();

   UpdateData(FALSE);
}