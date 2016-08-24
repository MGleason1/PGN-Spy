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

#pragma once

void AddStringIfNotFound(CString sValue, CStringArray &rasArray, CArray<int, int> &raiCountArray);

class CAnalysisSettings
{
public:
   CAnalysisSettings();
   bool LoadSettingsFromRegistry();
   bool SaveSettingsToRegistry();

   //analysis settings that are saved
   BOOL m_bExcludeForcedMoves;
   int m_iForcedMoveCutoff; //in centipawns, will exclude moves where the second move is this much worse than the best
   BOOL m_bIncludeOnlyUnclearPositions;
   int m_iUnclearPositionCutoff; //in centipawns, will exclude moves where the Tn position is this much worse than the best
   int m_iEqualPositionThreshold; //in centipawns, restricts analysis to positions where neither side is winning by more than this value
   int m_iLosingThreshold; //in centipawns, restricts analysis to positions where the side in question is losing by more than the equal position threshold, but by less than this value

   //temporary filters
   CString m_sPlayerName; //cannot be changed if player name is specified in engine settings
   CString m_sOpponentName;
   CString m_sEvent;
   int m_iMoveNumMin; //must be greater than the engine setting
   int m_iMoveNumMax;
   bool m_bWhiteOnly;
   bool m_bBlackOnly;
};

class CEngineSettings
{
public:
   CEngineSettings();
   bool LoadSettingsFromRegistry();
   bool SaveSettingsToRegistry();

   //engine settings
   CString m_sEnginePath;
   int m_iNumVariations;
   int m_iSearchDepth;
   int m_iMaxTime;
   int m_iMinTime;
   int m_iNumThreads;
   int m_iHashSize; //in MB
   int m_iBookDepth;

   //analysis settings that are fed to the analyser
   CString m_sPlayerName;
};

class CMove
{
public:
   CMove();
   CString m_sMove;
   int m_iDepth;
   int m_iTime;
   int m_iScore;
};

class CPosition
{
public:
   CPosition();
   CPosition(const CPosition &rSrc);
   CPosition &operator=(const CPosition &rSrc);
   bool IsForcedMove(int iVariation, int iForcedMoveThreshold);
   bool IsUnclearPosition(int iVariation, int iUnclearPositionThreshold);
   bool IsEqualPosition(int iEqualPositionThreshold);
   bool IsLosingPosition(int iEqualPositionThreshold, int iLosingPositionThreshold);
   int GetCentipawnLoss();
   CArray<CMove, CMove> m_avTopMoves;
   int m_iMovePlayed;
   bool m_bWhite;
};

class CGame
{
public:
   CGame() {}
   CGame(const CGame &rSrc);
   CGame &operator=(const CGame &rSrc);
   bool LoadGame(CString sGameText);

   CArray<CPosition, CPosition> m_avPositions;
   CString m_sEvent;
   CString m_sDate;
   CString m_sWhite;
   CString m_sBlack;
   CString m_sResult;
   CString m_sTimeControl;
};

class CStats
{
public:
   CStats();
   int m_iNumVariations;

   int m_iNumPositions;
   CArray<int, int> m_aiTValues; //T1/T2/T3/etc. totals
   CArray<int, int> m_aiTMoves; //T1/T2/T3/etc. totals
   int m_iTotalCentipawnLoss;
   CArray<int, int> m_aiCentipawnLosses; //for calculating std deviation
   int m_i0CPLoss;
   int m_i10CPLoss;
   int m_i25CPLoss;
   int m_i50CPLoss;
   int m_i100CPLoss;
   int m_i200CPLoss;
   int m_i300CPLoss;
   int m_i500CPLoss;

   double m_dAvgCentipawnLoss;
   double m_dCentipawnLossStdDeviation;

   void ZeroAll();
   void Initialize(const CEngineSettings &vSettings);
   void AddPosition(CPosition &vPosition, const CAnalysisSettings &vSettings);
   void FinaliseStats();
   CString GetResultsText();
};

class CGamePGN
{
public:
   CGamePGN() {}
   CString m_sPGNText;
   CString m_sWhite;
   CString m_sBlack;
   CString m_sFileName;
};
