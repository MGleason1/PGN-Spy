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

void AddStringIfNotFound(CString sValue, CStringArray &rasArray, CArray<int, int> &raiCountArray)
{
   bool bFound = false;
   ASSERT(raiCountArray.GetSize() == rasArray.GetSize());
   for (int i = 0; i < rasArray.GetSize(); i++)
   {
      if (sValue.CompareNoCase(rasArray[i]) == 0)
      {
         raiCountArray[i]++; //increment counter
         return;
      }
   }

   //if we reach here, we didn't find it; add it
   rasArray.Add(sValue);
   raiCountArray.Add(1);
}

CAnalysisSettings::CAnalysisSettings()
{
   m_bExcludeForcedMoves = true;
   m_iForcedMoveCutoff = 50;
   m_bIncludeOnlyUnclearPositions = true;
   m_iUnclearPositionCutoff = 100;
   m_iEqualPositionThreshold = 200;
   m_iLosingThreshold = 500;
   m_bIncludeLosing = true;
   m_bIncludeWinning = false;
   m_bIncludePostLosing = false;

   //temporary filters
   m_iMoveNumMin = 11;
   m_iMoveNumMax = 10000;
   m_bWhiteOnly = false;
   m_bBlackOnly = false;
   m_bIncludeWins = true;
   m_bIncludeLosses = true;
   m_bIncludeDraws = true;
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
   m_iEqualPositionThreshold = theApp.GetProfileInt("PGNSpy", "EqualPositionThreshold", 200);
   m_iLosingThreshold = theApp.GetProfileInt("PGNSpy", "LosingThreshold", 500);
   m_bIncludeLosing = theApp.GetProfileInt("PGNSpy", "IncludeLosing", 1) == 1;
   m_bIncludeWinning = theApp.GetProfileInt("PGNSpy", "IncludeWinning", 0) == 1;
   m_bIncludePostLosing = theApp.GetProfileInt("PGNSpy", "IncludePostLosing", 0) == 1;
   return true;
}

bool CAnalysisSettings::SaveSettingsToRegistry()
{
   theApp.WriteProfileInt("PGNSpy", "ExcludeForcedMoves", m_bExcludeForcedMoves ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "ForcedMoveCutoff", m_iForcedMoveCutoff);
   theApp.WriteProfileInt("PGNSpy", "IncludeOnlyUnclearPositions", m_bIncludeOnlyUnclearPositions ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "UnclearPositionCutoff", m_iUnclearPositionCutoff);
   theApp.WriteProfileInt("PGNSpy", "EqualPositionThreshold", m_iEqualPositionThreshold);
   theApp.WriteProfileInt("PGNSpy", "LosingThreshold", m_iLosingThreshold);
   theApp.WriteProfileInt("PGNSpy", "IncludeLosing", m_bIncludeLosing ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "IncludeWinning", m_bIncludeWinning ? 1 : 0);
   theApp.WriteProfileInt("PGNSpy", "IncludePostLosing", m_bIncludePostLosing ? 1 : 0);
   return true;
}

CEngineSettings::CEngineSettings()
{
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

bool CEngineSettings::LoadSettingsFromRegistry()
{
   //calc number of cores and divide by two; engines aren't great with hyper-threading; checking for
   //hyper-threading seems to be a little more complex, so just assume it's present for setting the default
   //subtract one to allow for the OS
   SYSTEM_INFO vSysInfo;
   GetSystemInfo(&vSysInfo);
   int iDefaultThreads = max((int)vSysInfo.dwNumberOfProcessors / 2, 1);

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

bool CEngineSettings::SaveSettingsToRegistry()
{
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

CEngineSettings CEngineSettings::MakeCompatible(const CEngineSettings vOtherEngineSettings, CString &rsWarning) const
{
   CEngineSettings vEngineSettings = *this;
   rsWarning = "";

   //A mismatch on this setting should be resolved by blanking out the setting, but doesn't require a warning
   if (m_sPlayerName.CompareNoCase(vOtherEngineSettings.m_sPlayerName) != 0)
   {
      vEngineSettings.m_sPlayerName = "";
   }

   //A mismatch here needs resolved; we will only have partial data for the larger number of variations, so
   //simply ignore that data
   if (m_iNumVariations != vOtherEngineSettings.m_iNumVariations)
   {
      if (!rsWarning.IsEmpty())
         rsWarning += "\r\n";
      rsWarning = "The number of variations does not match.  Only the lower number will be preserved.";
      vEngineSettings.m_iNumVariations = min(m_iNumVariations, vOtherEngineSettings.m_iNumVariations);
   }

   //the user should be warned about mismatches on the following settings, but otherwise we can ignore it
   if (m_iSearchDepth != vOtherEngineSettings.m_iSearchDepth || m_iMaxTime != vOtherEngineSettings.m_iMaxTime || m_iMinTime != vOtherEngineSettings.m_iMinTime)
   {
      if (!rsWarning.IsEmpty())
         rsWarning += "\r\n";
      rsWarning += "The search depth and/or time settings do not match.";
   }
   if (m_iHashSize != vOtherEngineSettings.m_iHashSize)
   {
      if (!rsWarning.IsEmpty())
         rsWarning += "\r\n";
      rsWarning += "The hash size setting does not match.";
   }
   if (m_iBookDepth != vOtherEngineSettings.m_iBookDepth)
   {
      if (!rsWarning.IsEmpty())
         rsWarning += "\r\n";
      rsWarning += "The book depth setting does not match.";
   }
   if (m_sEnginePath.CompareNoCase(vOtherEngineSettings.m_sEnginePath) != 0)
   {
      CString sEngineName, sEngineNameOther;
      int iLastSlash = m_sEnginePath.ReverseFind('\\');
      if (iLastSlash != -1)
         sEngineName = m_sEnginePath.Right(m_sEnginePath.GetLength() - iLastSlash - 1);
      iLastSlash = vOtherEngineSettings.m_sEnginePath.ReverseFind('\\');
      if (iLastSlash != -1)
         sEngineNameOther = vOtherEngineSettings.m_sEnginePath.Right(vOtherEngineSettings.m_sEnginePath.GetLength() - iLastSlash - 1);

      if (sEngineName.CompareNoCase(sEngineNameOther) != 0)
      {
         if (!rsWarning.IsEmpty())
            rsWarning += "\r\n";
         rsWarning += "The engine does not match.";
      }
   }
   return vEngineSettings;
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
   m_bWhite = false;
}

CPosition::CPosition(const CPosition &rSrc)
{
   *this = rSrc;
}

CPosition &CPosition::operator=(const CPosition &rSrc)
{
   m_iMovePlayed = rSrc.m_iMovePlayed;
   m_bWhite = rSrc.m_bWhite;
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

bool CPosition::IsWinningPosition(int iEqualPositionThreshold, int iLosingPositionThreshold)
{
   if (IsEqualPosition(iEqualPositionThreshold))
      return false;
   return m_avTopMoves[0].m_iScore > 0 && abs(m_avTopMoves[0].m_iScore) <= iLosingPositionThreshold;
}

bool CPosition::IsExcludedPosition(int iLosingPositionThreshold)
{
   return abs(m_avTopMoves[0].m_iScore) > iLosingPositionThreshold;
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
   m_sWhiteElo = rSrc.m_sWhiteElo;
   m_sBlackElo = rSrc.m_sBlackElo;
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
	  else if (sName.CompareNoCase("WhiteElo") == 0)
		  m_sWhiteElo = sValue;
	  else if (sName.CompareNoCase("BlackElo") == 0)
		  m_sBlackElo = sValue;
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
      CString sText;
      if (!vGame.IntoElem())
         return false;
      if (!vGame.FindElem("played"))
         return false;
      sMovePlayed = vGame.GetData();
      if (sMovePlayed.IsEmpty())
         return false;
      if (!vGame.FindElem("white"))
         return false;
      sText = vGame.GetData();
      vPosition.m_bWhite = atoi(sText) == 1;
      while (vGame.FindElem("evaluation"))
      {
         //find all moves and their evaluation
         CMove vMove;
         vMove.m_sMove = vGame.GetAttrib("move");
         if (vMove.m_sMove.CompareNoCase(sMovePlayed) == 0)
            vPosition.m_iMovePlayed = vPosition.m_avTopMoves.GetSize();//new move will be inserted at this point, so using GetSize will be valid
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
         {
            vMove.m_iScore = atoi(sText);
            if (vMove.m_iScore > 1000)
               vMove.m_iScore = 1000;
            else if (vMove.m_iScore < -1000)
               vMove.m_iScore = -1000;
         }
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
   ZeroAll();
}

void CStats::ZeroAll()
{
   m_iNumVariations = 0;
   m_iNumPositions = 0;
   m_iTotalCentipawnLoss = 0;
   m_aiCentipawnLosses.RemoveAll();
   m_dAvgCentipawnLoss = 0;
   m_dCentipawnLossStdDeviation = 0;
   m_i0CPLoss = 0;
   m_i10CPLoss = 0;
   m_i25CPLoss = 0;
   m_i50CPLoss = 0;
   m_i100CPLoss = 0;
   m_i200CPLoss = 0;
   m_i500CPLoss = 0;
}

void CStats::Initialize(const CEngineSettings &vSettings)
{
   ZeroAll();
   m_iNumVariations = vSettings.m_iNumVariations;
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
   m_iTotalCentipawnLoss += vPosition.GetCentipawnLoss();

   int iCPLossIndex = 0;
   int iCPLoss = vPosition.GetCentipawnLoss();
   while (iCPLossIndex < m_aiCentipawnLosses.GetSize() && m_aiCentipawnLosses[iCPLossIndex] > iCPLoss)
      iCPLossIndex++; //sort cp loss array highest to lowest
   m_aiCentipawnLosses.InsertAt(iCPLossIndex, iCPLoss);

   if (iCPLoss > 0)
      m_i0CPLoss++;
   if (iCPLoss > 10)
      m_i10CPLoss++;
   if (iCPLoss > 25)
      m_i25CPLoss++;
   if (iCPLoss > 50)
      m_i50CPLoss++;
   if (iCPLoss > 100)
      m_i100CPLoss++;
   if (iCPLoss > 200)
      m_i200CPLoss++;
   if (iCPLoss > 500)
      m_i500CPLoss++;

   for (int i = 0; i < m_iNumVariations; i++)
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
   if (m_iNumPositions > 0)
      m_dAvgCentipawnLoss = (double)m_iTotalCentipawnLoss / (double)m_iNumPositions;
   else
      m_dAvgCentipawnLoss = 0;

   double dTotalVariance = 0;
   for (int i = 0; i < m_iNumPositions; i++)
   {
      double dDiffFromMean = (double)m_aiCentipawnLosses[i] - m_dAvgCentipawnLoss;
      dTotalVariance += dDiffFromMean * dDiffFromMean;
   }

   double dVariance;
   if (m_iNumPositions > 0)
      dVariance = dTotalVariance / (double)m_iNumPositions;
   else
      dVariance = 0;
   m_dCentipawnLossStdDeviation = sqrt(dVariance);
}

CString CStats::GetResultsText()
{
   CString sLine;
   CString sResults;
   sLine.Format("Positions: %i", m_iNumPositions);
   sResults = sLine + "\r\n";
   if (m_iNumPositions > 0)
   {
      //T-values
      for (int i = 0; i < m_iNumVariations; i++)
      {
         if (m_aiTMoves[i] == 0)
            sLine.Format("T%i: 0/0", i+1);
         else
         {
            double dFrac = ((double)m_aiTValues[i] / (double)m_aiTMoves[i]);
            double dStdError = sqrt(dFrac * (1 - dFrac) / m_aiTMoves[i]) * 100;
            sLine.Format("T%i: %i/%i; %.2f%% (std error %.2f)", i + 1, m_aiTValues[i], m_aiTMoves[i], dFrac*100.0, dStdError);
         }
         sResults += sLine + "\r\n";
      }
      //=0 CP loss
      {
         double dFrac = ((double)(m_iNumPositions - m_i0CPLoss) / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format("=0 CP loss: %i/%i; %.2f%% (std error %.2f)", m_iNumPositions - m_i0CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>0 CP loss
      {
         double dFrac = ((double)m_i0CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">0 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i0CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>10 CP loss
      {
         double dFrac = ((double)m_i10CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">10 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i10CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>25 CP loss
      {
         double dFrac = ((double)m_i25CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">25 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i25CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>50 CP loss
      {
         double dFrac = ((double)m_i50CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">50 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i50CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>100 CP loss
      {
         double dFrac = ((double)m_i100CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">100 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i100CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>200 CP loss
      {
         double dFrac = ((double)m_i200CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">200 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i200CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }
      //>500 CP loss
      {
         double dFrac = ((double)m_i500CPLoss / (double)m_iNumPositions);
         double dStdError = sqrt(dFrac * (1 - dFrac) / m_iNumPositions) * 100;
         sLine.Format(">500 CP loss: %i/%i; %.2f%% (std error %.2f)", m_i500CPLoss, m_iNumPositions, dFrac*100.0, dStdError);
         sResults += sLine + "\r\n";
      }

      sLine.Format("CP loss mean %.2f, std deviation %.2f", m_dAvgCentipawnLoss,m_dCentipawnLossStdDeviation);
      sResults += sLine + "\r\n";
   }
   return sResults;
}

bool LoadGameArrayFromFile(CString sFileName, CArray<CGame, CGame> &raGames, CEngineSettings &rvEngineSettings)
{
   raGames.RemoveAll();
   CMarkup vFile;
   if (!vFile.Load(sFileName))
      return false;
   if (!vFile.FindElem("Games"))
      return false;
   vFile.IntoElem();

   //engine settings
   if (!vFile.FindElem("EngineSettings"))
      return false;
   vFile.IntoElem();
   vFile.FindElem("EnginePath");
   rvEngineSettings.m_sEnginePath = vFile.GetData();
   vFile.FindElem("NumVariations");
   rvEngineSettings.m_iNumVariations = atoi(vFile.GetData());
   vFile.FindElem("SearchDepth");
   rvEngineSettings.m_iSearchDepth = atoi(vFile.GetData());
   vFile.FindElem("MinTime");
   rvEngineSettings.m_iMinTime = atoi(vFile.GetData());
   vFile.FindElem("MaxTime");
   rvEngineSettings.m_iMaxTime = atoi(vFile.GetData());
   vFile.FindElem("HashSize");
   rvEngineSettings.m_iHashSize = atoi(vFile.GetData());
   vFile.FindElem("BookDepth");
   rvEngineSettings.m_iBookDepth = atoi(vFile.GetData());
   vFile.FindElem("PlayerName");
   rvEngineSettings.m_sPlayerName = vFile.GetData();
   vFile.OutOfElem();

   //loop through all games
   while (vFile.FindElem("Game"))
   {
      CGame vGame;
      vFile.IntoElem();

      vFile.FindElem("Event");
      vGame.m_sEvent = vFile.GetData();
      vFile.FindElem("Date");
      vGame.m_sDate = vFile.GetData();
      vFile.FindElem("White");
      vGame.m_sWhite = vFile.GetData();
      vFile.FindElem("Black");
      vGame.m_sBlack = vFile.GetData();
      vFile.FindElem("Result");
      vGame.m_sResult = vFile.GetData();
      vFile.FindElem("TimeControl");
      vGame.m_sTimeControl = vFile.GetData();
	  vFile.FindElem("WhiteElo");
	  vGame.m_sWhiteElo = vFile.GetData();
	  vFile.FindElem("BlackElo");
	  vGame.m_sBlackElo = vFile.GetData();
      //loop through all positions
      vFile.FindElem("Positions");
      vFile.IntoElem();
      while (vFile.FindElem("Position"))
      {
         CPosition vPosition;
         vFile.IntoElem();

         vFile.FindElem("White");
         vPosition.m_bWhite = atoi(vFile.GetData()) == 1;
         vFile.FindElem("MovePlayed");
         vPosition.m_iMovePlayed = atoi(vFile.GetData());

         //loop through all moves
         vFile.FindElem("Moves");
         vFile.IntoElem();
         while (vFile.FindElem("Move"))
         {
            CMove vMove;
            vFile.IntoElem();
            vFile.FindElem("Move");
            vMove.m_sMove = vFile.GetData();
            vFile.FindElem("Depth");
            vMove.m_iDepth = atoi(vFile.GetData());
            vFile.FindElem("Time");
            vMove.m_iTime = atoi(vFile.GetData());
            vFile.FindElem("Score");
            vMove.m_iScore = atoi(vFile.GetData());

            vPosition.m_avTopMoves.Add(vMove);
            vFile.OutOfElem(); //</Move>
         }

         vGame.m_avPositions.Add(vPosition);
         vFile.OutOfElem(); //</Moves>
         vFile.OutOfElem(); //</Position>
      }

      vFile.OutOfElem(); //</Positions>
      raGames.Add(vGame);
      vFile.OutOfElem(); //</Game>
   }

   return true;
}

bool SaveGameArrayToFile(CString sFileName, const CArray<CGame, CGame> &raGames, CEngineSettings vEngineSettings)
{
   if (raGames.GetSize() == 0)
      return false;

   CMarkup vFile;
   vFile.AddElem("Games");
   vFile.IntoElem();

   //engine settings
   vFile.AddElem("EngineSettings");
   vFile.IntoElem();
   vFile.AddElem("EnginePath", vEngineSettings.m_sEnginePath);
   vFile.AddElem("NumVariations", vEngineSettings.m_iNumVariations);
   vFile.AddElem("SearchDepth", vEngineSettings.m_iSearchDepth);
   vFile.AddElem("MinTime", vEngineSettings.m_iMinTime);
   vFile.AddElem("MaxTime", vEngineSettings.m_iMaxTime);
   vFile.AddElem("HashSize", vEngineSettings.m_iHashSize);
   vFile.AddElem("BookDepth", vEngineSettings.m_iBookDepth);
   vFile.AddElem("PlayerName", vEngineSettings.m_sPlayerName);
   vFile.OutOfElem();

   //loop through all games
   for (int iGame = 0; iGame < raGames.GetSize(); iGame++)
   {
      vFile.AddElem("Game");
      vFile.IntoElem();
      vFile.AddElem("Event", raGames[iGame].m_sEvent);
      vFile.AddElem("Date", raGames[iGame].m_sDate);
      vFile.AddElem("White", raGames[iGame].m_sWhite);
      vFile.AddElem("Black", raGames[iGame].m_sBlack);
      vFile.AddElem("Result", raGames[iGame].m_sResult);
      vFile.AddElem("TimeControl", raGames[iGame].m_sTimeControl);
	  vFile.AddElem("WhiteElo", raGames[iGame].m_sWhiteElo);
	  vFile.AddElem("BlackElo", raGames[iGame].m_sBlackElo);
      //loop through all positions
      vFile.AddElem("Positions");
      vFile.IntoElem();
      for (int iPosition = 0; iPosition < raGames[iGame].m_avPositions.GetSize(); iPosition++)
      {
         vFile.AddElem("Position");
         vFile.IntoElem();
         vFile.AddElem("White", raGames[iGame].m_avPositions[iPosition].m_bWhite ? 1 : 0);
         vFile.AddElem("MovePlayed", raGames[iGame].m_avPositions[iPosition].m_iMovePlayed);

         //loop through all moves
         vFile.AddElem("Moves");
         vFile.IntoElem();
         for (int iMove = 0; iMove < raGames[iGame].m_avPositions[iPosition].m_avTopMoves.GetSize(); iMove++)
         {
            vFile.AddElem("Move");
            vFile.IntoElem();
            vFile.AddElem("Move", raGames[iGame].m_avPositions[iPosition].m_avTopMoves[iMove].m_sMove);
            vFile.AddElem("Depth", raGames[iGame].m_avPositions[iPosition].m_avTopMoves[iMove].m_iDepth);
            vFile.AddElem("Time", raGames[iGame].m_avPositions[iPosition].m_avTopMoves[iMove].m_iTime);
            vFile.AddElem("Score", raGames[iGame].m_avPositions[iPosition].m_avTopMoves[iMove].m_iScore);

            vFile.OutOfElem(); //</Move>
         }
         vFile.OutOfElem(); //</Moves>
         vFile.OutOfElem(); //</Position>
      }
      vFile.OutOfElem(); //</Positions>
      vFile.OutOfElem(); //</Game>
   }

   if (!vFile.Save(sFileName))
      return false;
   return true;
}

/*
<games>
   <enginesettings>
      <enginepath>path</enginepath>
      <numvariations>3</numvariations>
      <searchdepth>20</searchdepth>
      <maxtime>20000</maxtime>
      <mintime>10000</mintime>
      <hashsize>512</hashsize>
      <bookdepth>10</bookdepth>
      <playername></playername>
   </enginesettings>
   <game>
      <event>event</event>
      <date>date</date>
      <white>white</white>
      <black>black</black>
      <result>result</result>
      <timecontrol>timecontrol</timecontrol>
      <positions>
         <position>
            <white>1</white>
            <moveplayed>3<moveplayed>
            <moves>
               <move>
                  <move>e2e4</move>
                  <depth>19</depth>
                  <time>20000</time>
                  <score>35</score>
               </move>
            </moves>
         </position>
      </positions>
   </game>
</games>
*/