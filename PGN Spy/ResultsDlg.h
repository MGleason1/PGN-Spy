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

#include "Analysis.h"


// CResultsDlg dialog

class CResultsDlg : public CDialogEx
{
   DECLARE_DYNAMIC(CResultsDlg)

public:
   CResultsDlg(CWnd* pParent = NULL);   // standard constructor
   virtual ~CResultsDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
   enum { IDD = IDD_RESULTS };
#endif

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedAbout();
   afx_msg void OnBnClickedSavedata();
   void CalculateStats();
   CString m_sResults;
   CArray<CGame, CGame> m_avGames;
   CAnalysisSettings m_vSettings;
   CStats m_vEqualPositions;
   CStats m_vLosingPositions;
};