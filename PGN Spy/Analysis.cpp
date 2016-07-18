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

bool CAnalysisSettings::LoadSettingsFromFile()
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

bool CAnalysisSettings::SaveSettingsToFile()
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

bool CAnalysisSettings::LoadSettingsFromRegistry()
{
   //calc number of cores and divide by two; engines aren't great with hyper-threading; checking for
   //hyper-threading seems to be a little more complex, so just assume it's present for setting the default
   //subtract one to allow for the OS
   SYSTEM_INFO vSysInfo;
   GetSystemInfo(&vSysInfo);
   int iDefaultThreads = max((int)vSysInfo.dwNumberOfProcessors / 2, 1);

   m_bExcludeForcedMoves = theApp.GetProfileInt("PGNSpy", "ExcludeForcedMoves", 1) == 1;
   m_iForcedMoveCutoff = theApp.GetProfileInt("PGNSpy", "ForcedMoveCutoff", 50);
   m_bIncludeOnlyUnclearPositions = theApp.GetProfileInt("PGNSpy", "IncludeOnlyUnclearPositions", 1) == 1;
   m_iUnclearPositionCutoff = theApp.GetProfileInt("PGNSpy", "UnclearPositionCutoff", 100);
   m_iBlunderThreshold = theApp.GetProfileInt("PGNSpy", "BlunderThreshold", 100);
   m_iEqualPositionThreshold = theApp.GetProfileInt("PGNSpy", "EqualPositionThreshold", 200);
   m_iLosingThreshold = theApp.GetProfileInt("PGNSpy", "LosingThreshold", 500);
   m_iBookDepth = theApp.GetProfileInt("PGNSpy", "BookDepth", 10);
   m_sEnginePath = theApp.GetProfileString("PGNSpy", "EnginePath", "");
   m_iNumVariations = theApp.GetProfileInt("PGNSpy", "NumVariations", 3);
   m_iSearchDepth = theApp.GetProfileInt("PGNSpy", "SearchDepth", 20);
   m_iMaxTime = theApp.GetProfileInt("PGNSpy", "MaxTime", 20000);
   m_iMinTime = theApp.GetProfileInt("PGNSpy", "MinTime", 10000);
   m_iNumThreads = theApp.GetProfileInt("PGNSpy", "NumThreads", iDefaultThreads);
   m_iHashSize = theApp.GetProfileInt("PGNSpy", "HashSize", 24);
   return true;
}

bool CAnalysisSettings::SaveSettingsToRegistry()
{
   theApp.WriteProfileInt("PGNSpy", "ExcludeForcedMoves", m_bExcludeForcedMoves ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "ForcedMoveCutoff", m_iForcedMoveCutoff);
   theApp.WriteProfileInt("PGNSpy", "IncludeOnlyUnclearPositions", m_bIncludeOnlyUnclearPositions ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "UnclearPositionCutoff", m_iUnclearPositionCutoff);
   theApp.WriteProfileInt("PGNSpy", "BlunderThreshold", m_iBlunderThreshold);
   theApp.WriteProfileInt("PGNSpy", "EqualPositionThreshold", m_iEqualPositionThreshold);
   theApp.WriteProfileInt("PGNSpy", "LosingThreshold", m_iLosingThreshold);
   theApp.WriteProfileInt("PGNSpy", "BookDepth", m_iBookDepth);
   theApp.WriteProfileString("PGNSpy", "EnginePath", m_sEnginePath);
   theApp.WriteProfileInt("PGNSpy", "NumVariations", m_iNumVariations);
   theApp.WriteProfileInt("PGNSpy", "SearchDepth", m_iSearchDepth);
   theApp.WriteProfileInt("PGNSpy", "MaxTime", m_iMaxTime);
   theApp.WriteProfileInt("PGNSpy", "MinTime", m_iMinTime);
   theApp.WriteProfileInt("PGNSpy", "NumThreads", m_iNumThreads);
   theApp.WriteProfileInt("PGNSpy", "HashSize", m_iHashSize);
   return true;
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
