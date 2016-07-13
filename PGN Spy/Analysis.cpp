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
#include "Analysis.h"
#include "PGN Spy.h"
#include "Markup.h"

CAnalysisSettings::CAnalysisSettings()
{
   m_bExcludeForcedMoves = true;
   m_iForcedMoveCutoff = 50;
   m_bIncludeOnlyUnclearPositions = true;
   m_iUnclearPositionCutoff = 100;
   m_iBlunderThreshold = 100;
   m_iEqualPositionThreshold = 200;
   m_iLosingThreshold = 500;
   m_iBookDepth = 10;
   m_iNumVariations = 3;
   m_iSearchDepth = 20;
   m_iMaxTime = 20000;
   m_iMinTime = 10000;
   m_iHashSize = 24;

   //calc number of cores and divide by two; engines aren't great with hyper-threading; checking for
   //hyper-threading seems to be a little more complex, so just assume it's present for setting the default
   //subtract one to allow for the OS
   SYSTEM_INFO vSysInfo;
   GetSystemInfo(&vSysInfo);
   m_iNumThreads = max((int)vSysInfo.dwNumberOfProcessors / 2, 1);
}

bool CAnalysisSettings::LoadSettings()
{
   CMarkup vXML;
   CString sText;
   if (!vXML.Load(GetSettingsFilePath()))
      return false;

   if (!vXML.FindElem()) //find root <settings> elem
      return false;
   if (!vXML.IntoElem())
      return false;

   if (!vXML.FindElem("ExcludeForcedMoves"))
      return false;
   sText = vXML.GetData();
   m_bExcludeForcedMoves = sText.CompareNoCase("yes") == 0;

   if (!vXML.FindElem("ForcedMoveCutoff"))
      return false;
   sText = vXML.GetData();
   m_iForcedMoveCutoff = atoi(sText);

   if (!vXML.FindElem("IncludeOnlyUnclearPositions"))
      return false;
   sText = vXML.GetData();
   m_bIncludeOnlyUnclearPositions = sText.CompareNoCase("yes") == 0;

   if (!vXML.FindElem("UnclearPositionCutoff"))
      return false;
   sText = vXML.GetData();
   m_iUnclearPositionCutoff = atoi(sText);

   if (!vXML.FindElem("BlunderThreshold"))
      return false;
   sText = vXML.GetData();
   m_iBlunderThreshold = atoi(sText);

   if (!vXML.FindElem("EqualPositionThreshold"))
      return false;
   sText = vXML.GetData();
   m_iEqualPositionThreshold = atoi(sText);

   if (!vXML.FindElem("LosingThreshold"))
      return false;
   sText = vXML.GetData();
   m_iLosingThreshold = atoi(sText);

   if (!vXML.FindElem("BookDepth"))
      return false;
   sText = vXML.GetData();
   m_iBookDepth = atoi(sText);

   if (!vXML.FindElem("EnginePath"))
      return false;
   m_sEnginePath = vXML.GetData();

   if (!vXML.FindElem("NumVariations"))
      return false;
   sText = vXML.GetData();
   m_iNumVariations = atoi(sText);

   if (!vXML.FindElem("SearchDepth"))
      return false;
   sText = vXML.GetData();
   m_iSearchDepth = atoi(sText);

   if (!vXML.FindElem("MaxTime"))
      return false;
   sText = vXML.GetData();
   m_iMaxTime = atoi(sText);

   if (!vXML.FindElem("MinTime"))
      return false;
   sText = vXML.GetData();
   m_iMinTime = atoi(sText);

   if (!vXML.FindElem("NumThreads"))
      return false;
   sText = vXML.GetData();
   m_iNumThreads = atoi(sText);

   if (!vXML.FindElem("HashSize"))
      return false;
   sText = vXML.GetData();
   m_iHashSize = atoi(sText);

   return true;
}

bool CAnalysisSettings::SaveSettings()
{
   CMarkup vXML;

   if (!vXML.AddElem("settings"))
      return false;
   if (!vXML.IntoElem())
      return false;
   if (!vXML.AddElem("ExcludeForcedMoves", m_bExcludeForcedMoves ? "yes" : "no"))
      return false;
   if (!vXML.AddElem("ForcedMoveCutoff", m_iForcedMoveCutoff))
      return false;
   if (!vXML.AddElem("IncludeOnlyUnclearPositions", m_bIncludeOnlyUnclearPositions ? "yes" : "no"))
      return false;
   if (!vXML.AddElem("UnclearPositionCutoff", m_iUnclearPositionCutoff))
      return false;
   if (!vXML.AddElem("BlunderThreshold", m_iBlunderThreshold))
      return false;
   if (!vXML.AddElem("EqualPositionThreshold", m_iEqualPositionThreshold))
      return false;
   if (!vXML.AddElem("LosingThreshold", m_iLosingThreshold))
      return false;
   if (!vXML.AddElem("BookDepth", m_iBookDepth))
      return false;
   if (!vXML.AddElem("EnginePath", m_sEnginePath))
      return false;
   if (!vXML.AddElem("NumVariations", m_iNumVariations))
      return false;
   if (!vXML.AddElem("SearchDepth", m_iSearchDepth))
      return false;
   if (!vXML.AddElem("MaxTime", m_iMaxTime))
      return false;
   if (!vXML.AddElem("MinTime", m_iMinTime))
      return false;
   if (!vXML.AddElem("NumThreads", m_iNumThreads))
      return false;
   if (!vXML.AddElem("HashSize", m_iHashSize))
      return false;

   return vXML.Save(GetSettingsFilePath());
}

CMove::CMove()
{
   m_iDepth = 0;
   m_iTime = 0;
   m_iScore = 0;
}

CPosition::CPosition()
{
   m_iMovePlayed = 0;
}

CPosition::CPosition(const CPosition &rSrc)
{
   *this = rSrc;
}

CPosition &CPosition::operator=(const CPosition &rSrc)
{
   m_iMovePlayed = rSrc.m_iMovePlayed;
   m_avTopMoves.Copy(rSrc.m_avTopMoves);
   return *this;
}

bool CPosition::IsForcedMove(int iVariation, int iForcedMoveThreshold)
{
   //ensure there's at least one move not significantly worse than the current move
   if (m_avTopMoves.GetSize() <= iVariation + 1)
      return true; //not enough legal moves
   if (m_avTopMoves[iVariation].m_iScore > m_avTopMoves[iVariation + 1].m_iScore + iForcedMoveThreshold)
      return true;
   return false;
}

bool CPosition::IsUnclearPosition(int iVariation, int iUnclearPositionThreshold)
{
   //ensure n+1 move is not significantly worse than the first-choice move
   if (m_avTopMoves.GetSize() <= iVariation + 1)
      return false; //not enough legal moves
   if (m_avTopMoves[0].m_iScore > m_avTopMoves[iVariation + 1].m_iScore + iUnclearPositionThreshold)
      return false;
   return true;
}

bool CPosition::IsBlunder(int iBlunderThreshold)
{
   return GetCentipawnLoss() > iBlunderThreshold;
}

bool CPosition::IsEqualPosition(int iEqualPositionThreshold)
{
   return abs(m_avTopMoves[0].m_iScore) <= iEqualPositionThreshold;
}

bool CPosition::IsLosingPosition(int iEqualPositionThreshold, int iLosingPositionThreshold)
{
   if (IsEqualPosition(iEqualPositionThreshold))
      return false;
   return m_avTopMoves[0].m_iScore < 0 && abs(m_avTopMoves[0].m_iScore) <= iLosingPositionThreshold;
}

int CPosition::GetCentipawnLoss()
{
   //[0] will have the highest value; positive is winning, negative losing
   return m_avTopMoves[0].m_iScore - m_avTopMoves[m_iMovePlayed].m_iScore;
}

CGame::CGame(const CGame &rSrc)
{
   *this = rSrc;
}

CGame &CGame::operator=(const CGame &rSrc)
{
   m_sEvent = rSrc.m_sEvent;
   m_sDate = rSrc.m_sDate;
   m_sWhite = rSrc.m_sWhite;
   m_sBlack = rSrc.m_sBlack;
   m_sResult = rSrc.m_sResult;
   m_sTimeControl = rSrc.m_sTimeControl;
   m_avPositions.Copy(rSrc.m_avPositions);

   return *this;
}

bool CGame::LoadGame(CString sGameText)
{
   CMarkup vGame;
   if (!vGame.SetDoc(sGameText))
      return false;
   vGame.SetDocFlags(vGame.GetDocFlags() | CMarkup::MDF_COLLAPSEWHITESPACE);

   if (!vGame.FindElem()) //find root elem <gamelist>
      return false;
   if (!vGame.IntoElem())
      return false;
   if (!vGame.FindElem("game"))
      return false;
   if (!vGame.IntoElem())
      return false;
   if (!vGame.FindElem("tags"))
      return false;
   if (!vGame.IntoElem())
      return false;
   //load tags
   while (vGame.FindElem("tag"))
   {
      CString sName = vGame.GetAttrib("name");
      CString sValue = vGame.GetAttrib("value");
      if (sName.CompareNoCase("Event") == 0)
         m_sEvent = sValue;
      else if (sName.CompareNoCase("Date") == 0)
         m_sDate = sValue;
      else if (sName.CompareNoCase("White") == 0)
         m_sWhite = sValue;
      else if (sName.CompareNoCase("Black") == 0)
         m_sBlack = sValue;
      else if (sName.CompareNoCase("Result") == 0)
         m_sResult = sValue;
      else if (sName.CompareNoCase("TimeControl") == 0)
         m_sTimeControl = sValue;
      else
      {
         //ignore tag
      }
   }
   //done loading tags, step out
   if (!vGame.OutOfElem())
      return false;
   if (!vGame.FindElem("analysis"))
      return false;
   if (!vGame.IntoElem())
      return false;
   //load moves
   while (vGame.FindElem("move"))
   {
      CPosition vPosition;
      CString sMovePlayed;
      if (!vGame.IntoElem())
         return false;
      if (!vGame.FindElem("played"))
         return false;
      sMovePlayed = vGame.GetData();
      if (sMovePlayed.IsEmpty())
         return false;
      while (vGame.FindElem("evaluation"))
      {
         //find all moves and their evaluation
         CMove vMove;
         vMove.m_sMove = vGame.GetAttrib("move");
         if (vMove.m_sMove.CompareNoCase(sMovePlayed) == 0)
            vPosition.m_iMovePlayed = vPosition.m_avTopMoves.GetSize();//new move will be inserted at this point, so using GetSize will be valid
         CString sText;
         sText = vGame.GetAttrib("depth");
         vMove.m_iDepth = atoi(sText);
         sText = vGame.GetAttrib("time");
         vMove.m_iTime = atoi(sText);
         sText = vGame.GetAttrib("value");
         if (sText.Find("mate") == 0)
         {
            //sText will take the format "mate n", with n positive if we have a mate, and negative if we're being mated
            CString sMateMoves = sText.Right(sText.GetLength() - 5);
            int iMateMoves = atoi(sMateMoves);
            if (iMateMoves > 0)
               vMove.m_iScore = 1000;
            else
               vMove.m_iScore = -1000;
         }
         else
            vMove.m_iScore = atoi(sText);
         vPosition.m_avTopMoves.Add(vMove);
      }
      if (!vGame.OutOfElem())
         return false;
      ASSERT(vPosition.m_avTopMoves.GetSize()>0);
      m_avPositions.Add(vPosition);
   }
   return true;
}

CStats::CStats()
{
   m_iNumPositions = 0;
   m_iBlunders = 0;
   m_iTotalCentipawnLoss = 0;
   m_aiCentipawnLosses.RemoveAll();
   m_dAvgCentipawnLoss = 0;
   m_dCentipawnLossStdDeviation = 0;
}

void CStats::Initialize(const CAnalysisSettings &vSettings)
{
   m_aiTValues.SetSize(vSettings.m_iNumVariations + 1);
   m_aiTMoves.SetSize(vSettings.m_iNumVariations + 1);
   for (int i = 0; i < vSettings.m_iNumVariations + 1; i++)
   {
      m_aiTValues[i] = 0;
      m_aiTMoves[i] = 0;
   }
}

void CStats::AddPosition(CPosition &vPosition, const CAnalysisSettings &vSettings)
{
   m_iNumPositions++;
   if (vPosition.IsBlunder(vSettings.m_iBlunderThreshold))
      m_iBlunders++;
   m_iTotalCentipawnLoss += vPosition.GetCentipawnLoss();
   m_aiCentipawnLosses.Add(vPosition.GetCentipawnLoss());

   for (int i = 0; i < vSettings.m_iNumVariations; i++)
   {
      if (vSettings.m_bExcludeForcedMoves && vPosition.IsForcedMove(i, vSettings.m_iForcedMoveCutoff))
         break; //exclude forced moves
      if (vSettings.m_bIncludeOnlyUnclearPositions && !vPosition.IsUnclearPosition(i, vSettings.m_iUnclearPositionCutoff))
         break; //include only unclear positions

      //increment T1/T2/T3/etc. values as appropriate
      m_aiTMoves[i]++;
      if (vPosition.m_iMovePlayed <= i)
         m_aiTValues[i]++;
   }
}

void CStats::FinaliseStats()
{
   m_dAvgCentipawnLoss = (double)m_iTotalCentipawnLoss / (double)m_iNumPositions;

   double dTotalVariance = 0;
   for (int i = 0; i < m_iNumPositions; i++)
   {
      double dDiffFromMean = (double)m_aiCentipawnLosses[i] - m_dAvgCentipawnLoss;
      dTotalVariance += dDiffFromMean * dDiffFromMean;
   }
   double dVariance = dTotalVariance / (double)m_iNumPositions;
   m_dCentipawnLossStdDeviation = sqrt(dVariance);
}

CAnalyser::CAnalyser()
{
   m_iGamesWithErrors = 0;
}

bool CAnalyser::ProcessGames(CString sConvertedPGN)
{
   CStdioFile vConvertedPGN;
   if (!vConvertedPGN.Open(sConvertedPGN, CFile::modeRead))
   {
      m_sMessage = "Failed to open converted PGN for analysis.";
      return false;
   }

   //Pull games out of file
   bool bKeepLooping = true;
   CArray<CGamePGN, CGamePGN> avGames;
   while (bKeepLooping)
   {
      CString sLine;
      if (vConvertedPGN.ReadString(sLine))
      {
         if (avGames.GetSize() == 0 || sLine.Left(6).CompareNoCase("[Event") == 0)
         {
            //found a new game
            CGamePGN vGamePGN;
            vGamePGN.m_sPGNText = sLine + "\n";
            avGames.Add(vGamePGN);
         }
         else
         {
            //still loading the latest game
            avGames[avGames.GetUpperBound()].m_sPGNText.Append(sLine + "\n");
            if (sLine.Left(8).CompareNoCase("[White \"") == 0)
            {
               //found white player name
               int iLeftQuote, iRightQuote;
               iLeftQuote = sLine.Find("\"", 0);
               iRightQuote = sLine.Find("\"", iLeftQuote + 1);
               avGames[avGames.GetUpperBound()].m_sWhite = sLine.Mid(iLeftQuote + 1, iRightQuote - iLeftQuote - 1);
            }
            if (sLine.Left(8).CompareNoCase("[Black \"") == 0)
            {
               //found black player name
               int iLeftQuote, iRightQuote;
               iLeftQuote = sLine.Find("\"", 0);
               iRightQuote = sLine.Find("\"", iLeftQuote + 1);
               avGames[avGames.GetUpperBound()].m_sBlack = sLine.Mid(iLeftQuote + 1, iRightQuote - iLeftQuote - 1);
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
   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty())
   {
      for (int i = 0; i < avGames.GetSize(); i++)
      {
         if (avGames[i].m_sWhite.CompareNoCase(m_vAnalysisSettings.m_sPlayerName) != 0 &&
             avGames[i].m_sBlack.CompareNoCase(m_vAnalysisSettings.m_sPlayerName) != 0)
         {
            avGames.RemoveAt(i);
            i--;
         }
      }
   }

   if (avGames.GetSize() == 0)
   {
      m_sMessage = "No games to analyse.  If you have entered a player name, please ensure it is spelled correctly.";
      return true;
   }

   //We now have a complete list of games; dump them to files
   for (int i = 0; i < avGames.GetSize(); i++)
   {
      avGames[i].m_sFileName = GetTemporaryPGNFilePath(i);
      CFile vFile;
      if (!vFile.Open(avGames[i].m_sFileName, CFile::modeCreate | CFile::modeWrite))
      {
         m_sMessage = "Failed to create temporary output file.";
         return false;
      }
      vFile.Write(avGames[i].m_sPGNText.GetBuffer(), avGames[i].m_sPGNText.GetLength());
      avGames[i].m_sPGNText.ReleaseBuffer();
      vFile.Close();
   }

   //We now have a complete set of files; time to process them

   //initialize handles
   m_ahChildStdInRead.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahChildStdInWrite.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahChildStdOutRead.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahChildStdOutWrite.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahChildStdErrRead.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahChildStdErrWrite.SetSize(m_vAnalysisSettings.m_iNumThreads);
   m_ahProcesses.SetSize(m_vAnalysisSettings.m_iNumThreads);
   for (int i = 0; i < m_vAnalysisSettings.m_iNumThreads; i++)
   {
      m_ahChildStdInRead[i] = NULL;
      m_ahChildStdInWrite[i] = NULL;
      m_ahChildStdOutRead[i] = NULL;
      m_ahChildStdOutWrite[i] = NULL;
      m_ahChildStdErrRead[i] = NULL;
      m_ahChildStdErrWrite[i] = NULL;
      m_ahProcesses[i] = NULL;
   }

   int iActiveProcesses = 0;
   int iNextGame = 0;
   bool bThreadsStillRunning = false;
   CStringArray asResults;
   CArray<bool, bool> abErrors;
   asResults.SetSize(m_vAnalysisSettings.m_iNumThreads);
   abErrors.SetSize(m_vAnalysisSettings.m_iNumThreads);
   while (iNextGame < avGames.GetSize() || bThreadsStillRunning)
   {
      bThreadsStillRunning = false; //we'll turn this on later if needed
      //check all processes to see if they're currently running
      for (int iCurThread = 0; iCurThread < m_vAnalysisSettings.m_iNumThreads; iCurThread++)
      {
         //if current process is free, kick off another process
         if (m_ahProcesses[iCurThread] == NULL)
         {
            if (iNextGame >= avGames.GetSize())
               continue; //don't start a new process if no games left
            abErrors[iCurThread] = false;
            //kick off process with next game
            if (!LaunchAnalyser(avGames[iNextGame], iCurThread))
            {
               //Failed to launch analyser; bail out
               return false;
            }

            bThreadsStillRunning = true; //we're launching a new thread
            iNextGame++;
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

            //we have our results; now parse them
            if (abErrors[iCurThread])
               m_iGamesWithErrors++; //skip games with errors
            else
               ProcessOutput(asResults[iCurThread], m_vAnalysisSettings.m_bExcludeForcedMoves);
            asResults[iCurThread].Empty();
            abErrors[iCurThread] = false;
         }
         else
         {
            bThreadsStillRunning = true; //current thread is still running

            //read everything written so far to keep the pipe from filling up and locking the analyser
            ReadFromThread(iCurThread, asResults[iCurThread], abErrors[iCurThread]);
         }
      }
   }

   //clean up temporary files
   for (int i = 0; i < avGames.GetSize(); i++)
      DeleteFile(avGames[i].m_sFileName);

   if (m_iGamesWithErrors > 0)
      m_sMessage.Format("Errors were encountered on %i games out of a total %i to be analysed.", m_iGamesWithErrors, avGames.GetSize());
   else
      m_sMessage.Format("Analysis of %i games completed with no errors.", avGames.GetSize());

   //caller will display results
   return true;
}

void CAnalyser::ReadFromThread(int iThread, CString IN OUT &rsResults, bool IN OUT &rbError)
{
   if (m_ahChildStdOutRead[iThread] == NULL)
   {
      ASSERT(false);
      return;
   }
   CFile vOutFile(m_ahChildStdOutRead[iThread]);
   //we won't need to close these handles; they're closed elsewhere

   char sBuf[101];
   ZeroMemory(sBuf, sizeof(sBuf));

   //check for failure
   DWORD iExitCode;
   GetExitCodeProcess(m_ahProcesses[iThread], &iExitCode);
   if (iExitCode != STILL_ACTIVE && iExitCode < 0)
   {
      //found error
      rbError = true;

      DWORD iBytesAvailable;
      PeekNamedPipe(m_ahChildStdErrRead[iThread], NULL, NULL, NULL, &iBytesAvailable, NULL);
      if (iBytesAvailable > 0)
      {
         //Read error for debugging purposes
         CFile vErrFile(m_ahChildStdErrRead[iThread]);
         int iBytesRead = vOutFile.Read(sBuf, 100);
      }
      return;
   }

   //no failure, keep reading until we reach end of results or end of what's currently in the pipe
   while (rsResults.Find("</gamelist>") == -1)
   {
      if (iExitCode != STILL_ACTIVE)
      {
         //if process closed, ensure there's still data to read
         DWORD iBytesAvailable;
         PeekNamedPipe(m_ahChildStdOutRead[iThread], NULL, NULL, NULL, &iBytesAvailable, NULL);
         if (iBytesAvailable <= 0)
            break; //hit end of pipe
      }
      int iBytesRead = vOutFile.Read(sBuf, 100);
      rsResults.Append(sBuf);
      if (iBytesRead < 100)
         break; //hit end of pipe
      ZeroMemory(sBuf, sizeof(sBuf)); //initialise for next time round
   }
}

bool CAnalyser::LaunchAnalyser(CGamePGN vGamePGN, int iCurThread)
{
   SECURITY_ATTRIBUTES vAttrib;
   vAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
   vAttrib.bInheritHandle = TRUE;
   vAttrib.lpSecurityDescriptor = NULL;

   //create std in pipe for writing to child process
   if (!CreatePipe(&m_ahChildStdInRead[iCurThread], &m_ahChildStdInWrite[iCurThread], &vAttrib, 0))
   {
      m_sMessage = "Failed to create pipe to communicate with analyser.";
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdInWrite[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      m_sMessage = "Failed set up pipe to communicate with analyser.";
      return false;
   }

   //create std out pipe for reading from child process
   if (!CreatePipe(&m_ahChildStdOutRead[iCurThread], &m_ahChildStdOutWrite[iCurThread], &vAttrib, 0))
   {
      m_sMessage = "Failed to create pipe to communicate with analyser.";
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdOutRead[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      m_sMessage = "Failed set up pipe to communicate with analyser.";
      return false;
   }

   //create std err pipe for reading from child process
   if (!CreatePipe(&m_ahChildStdErrRead[iCurThread], &m_ahChildStdErrWrite[iCurThread], &vAttrib, 0))
   {
      m_sMessage = "Failed to create pipe to communicate with analyser.";
      return false;
   }
   if (!SetHandleInformation(m_ahChildStdErrRead[iCurThread], HANDLE_FLAG_INHERIT, 0))
   {
      m_sMessage = "Failed set up pipe to communicate with analyser.";
      return false;
   }

   PROCESS_INFORMATION vProcessInfo;
   STARTUPINFO vStartupInfo = {0};
   vStartupInfo.cb = sizeof(STARTUPINFO);
   vStartupInfo.hStdInput = m_ahChildStdInWrite[iCurThread];
   vStartupInfo.hStdOutput = m_ahChildStdOutWrite[iCurThread];
   vStartupInfo.hStdError = m_ahChildStdErrWrite[iCurThread];
   vStartupInfo.dwFlags = STARTF_USESTDHANDLES;

   CString sCommandLine;
   CString sWhiteOrBlack = "";
   if (!m_vAnalysisSettings.m_sPlayerName.IsEmpty())
   {
      if (m_vAnalysisSettings.m_sPlayerName.CompareNoCase(vGamePGN.m_sWhite))
         sWhiteOrBlack = "--whiteonly ";
      if (m_vAnalysisSettings.m_sPlayerName.CompareNoCase(vGamePGN.m_sBlack))
         sWhiteOrBlack = "--blackonly ";
      else
         ASSERT(false); //we should have discarded this game before this point
   }
   int iBookDepthPlies = m_vAnalysisSettings.m_iBookDepth * 2; //double book depth, since analyser uses plies, not moves
   sCommandLine.Format("--bookdepth %i --searchdepth %i --searchmaxtime %i --searchmintime %i --variations %i %s--setoption Hash %i --engine \"%s\" \"%s\"",
                       iBookDepthPlies, m_vAnalysisSettings.m_iSearchDepth, m_vAnalysisSettings.m_iMaxTime,
                       m_vAnalysisSettings.m_iMinTime, m_vAnalysisSettings.m_iNumVariations + 1, sWhiteOrBlack,
                       m_vAnalysisSettings.m_iHashSize, m_vAnalysisSettings.m_sEnginePath, vGamePGN.m_sFileName);
   if (!CreateProcess(GetAnalyserFilePath(), sCommandLine.GetBuffer(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &vStartupInfo, &vProcessInfo))
   {
      sCommandLine.ReleaseBuffer();
      DWORD dwError = GetLastError();
      m_sMessage = "Failed to launch analyser.  Please ensure it is in the same folder as PGN Spy, with the file name \"pgn-extract.exe\".";
      return false;
   }
   sCommandLine.ReleaseBuffer();
   m_ahProcesses[iCurThread] = vProcessInfo.hProcess;
   CloseHandle(vProcessInfo.hThread);
   return true;
}

void CAnalyser::ProcessOutput(CString sOutput, BOOL bExcludeForcedMoves)
{
//    if (false)
//    {
//       //debugging code to dump sample xml output to a file
//       CString sFileName = GetTemporaryPGNFilePath(1);
//       sFileName.Replace(".pgn", ".xml");
//       CFile vFile;
//       if (!vFile.Open(sFileName, CFile::modeCreate | CFile::modeWrite))
//       {
//          m_sMessage = "Failed to create temporary output file.";
//          return;
//       }
// 
//       vFile.Write(sOutput.GetBuffer(), sOutput.GetLength());
//       sOutput.ReleaseBuffer();
//       vFile.Close();
//    }

   CGame vGame;
   vGame.LoadGame(sOutput);
   m_avGames.Add(vGame);
}
