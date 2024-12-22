
// CircleDrawDlg.h: 헤더 파일
//

#pragma once

#include "PicDraw.h"

// CCircleDrawDlg 대화 상자
class CCircleDrawDlg : public CDialogEx
{
// 생성입니다.
public:
	CCircleDrawDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CIRCLEDRAW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL IsRunning();
	CString GetImgPath();
	void ValueCheck(int* pValue);
	afx_msg void OnBnClickedBtnDraw();
	afx_msg void OnBnClickedBtnAction();
	afx_msg void OnBnClickedBtnOpen();
	int m_nX1;
	int m_nY1;
	int m_nX2;
	int m_nY2;

	int m_nRadius;
protected:
	CPicDraw m_PicDraw;
public:
	afx_msg void OnEnChangeEditX1();
	afx_msg void OnEnChangeEditY1();
	afx_msg void OnEnChangeEditX2();
	afx_msg void OnEnChangeEditY2();
};
