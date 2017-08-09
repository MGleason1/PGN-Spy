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


// CPGNSpyDlg dialog
class CPGNSpyDlg : public CDialog
{
// Construction
public:
   CPGNSpyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
   enum { IDD = IDD_PGNSPY_DIALOG };

   protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
   HICON m_hIcon;

   // Generated message map functions
   virtual BOOL OnInitDialog();
   afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
   afx_msg void OnPaint();
   afx_msg HCURSOR OnQueryDragIcon();
   DECLARE_MESSAGE_MAP()
public:
   CString m_sInputFile;
   CEngineSettings m_vEngineSettings;
   afx_msg void OnBnClickedBrowseanalyse();
   afx_msg void OnBnClickedBrowseengine();
   afx_msg void OnBnClickedRunanalysis();
   bool ConvertFileForAnalysis(CString OUT &sConvertedFile);
   afx_msg void OnBnClickedHelpplayer();
   afx_msg void OnBnClickedHelpdepth();
   afx_msg void OnBnClickedHelpbookdepth();
   afx_msg void OnBnClickedHelpthreads();
   afx_msg void OnBnClickedHelpmintime();
   afx_msg void OnBnClickedHelpmaxtime();
   afx_msg void OnBnClickedHelpphashsize();
   afx_msg void OnBnClickedSavesettings();
   bool ValidateSettings();
   afx_msg void OnBnClickedForcedmovehelp();
   afx_msg void OnBnClickedUnclearpositionhelp();
   afx_msg void OnBnClickedEqualpositionhelp();
   afx_msg void OnBnClickedLosingthresholdhelp();
   afx_msg void OnBnClickedNumvariationshelp();
   afx_msg void OnBnClickedLoadresults();
};
