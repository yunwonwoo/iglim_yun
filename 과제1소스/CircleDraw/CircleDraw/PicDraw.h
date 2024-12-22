#pragma once
#include <afxwin.h>
#include <thread>

#define WM_USER_ACTION_DRAW (WM_USER + 1) // Action Draw
#define WM_USER_ACTION_STOP (WM_USER + 2) // Action Stop

class CPicDraw : public CStatic
{
public:
	afx_msg void OnPaint(); // WM_PAINT 메시지 처리
protected:
	afx_msg LRESULT OnActionDraw(WPARAM wParam, LPARAM lParam); // 사용자 정의 메시지 핸들러
	afx_msg LRESULT OnActionStop(WPARAM wParam, LPARAM lParam); // 사용자 정의 메시지 핸들러

	DECLARE_MESSAGE_MAP()

public:
	CPicDraw(); // 기본 생성자
	~CPicDraw();

	BOOL IsRunning();
	void PicDraw(int x1, int y1, int nRadius);
	void PicAction(int x1, int y1, int x2, int y2, int nRadius, const CString& strImgPath);
	void PicOpen(const CString& strImgName);

	void ActionStart();
	void ActionStop();
	void ThreadWorker();

protected:
	int m_nX1;
	int m_nY1;
	int m_nX2;
	int m_nY2;

	int m_nRadius;
	bool m_bRunning; // 이동 작업 중인지 상태 플래그
	bool m_bImgDraw;
	CString m_strImgPath;
	CString m_strImgName;
	std::thread m_workerThread;
	int m_nSteb;

	CDC m_memDC;
	CBitmap m_bitmap;
	CBitmap* m_pOldBitmap = nullptr;

	POINT FindCircleCenter(const CImage& image);
	void DrawCenterPoint(CDC* pDC, CRect rcDraw, int nCenterX, int nCenterY);
};

