﻿
// CircleDrawDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "CircleDraw.h"
#include "CircleDrawDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCircleDrawDlg 대화 상자



CCircleDrawDlg::CCircleDrawDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CIRCLEDRAW_DIALOG, pParent)
	, m_nX1(0)
	, m_nY1(0)
	, m_nX2(0)
	, m_nY2(0)
	, m_nRadius(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCircleDrawDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X1, m_nX1);
	DDX_Text(pDX, IDC_EDIT_Y1, m_nY1);
	DDX_Text(pDX, IDC_EDIT_X2, m_nX2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_nY2);
}

BEGIN_MESSAGE_MAP(CCircleDrawDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_DRAW, &CCircleDrawDlg::OnBnClickedBtnDraw)
	ON_BN_CLICKED(IDC_BTN_ACTION, &CCircleDrawDlg::OnBnClickedBtnAction)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CCircleDrawDlg::OnBnClickedBtnOpen)
	ON_EN_CHANGE(IDC_EDIT_X1, &CCircleDrawDlg::OnEnChangeEditX1)
	ON_EN_CHANGE(IDC_EDIT_Y1, &CCircleDrawDlg::OnEnChangeEditY1)
	ON_EN_CHANGE(IDC_EDIT_X2, &CCircleDrawDlg::OnEnChangeEditX2)
	ON_EN_CHANGE(IDC_EDIT_Y2, &CCircleDrawDlg::OnEnChangeEditY2)
END_MESSAGE_MAP()


// CCircleDrawDlg 메시지 처리기

BOOL CCircleDrawDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	if (!m_PicDraw.SubclassDlgItem(IDC_PIC_DRAW, this))
	{
		AfxMessageBox(_T("Failed to subclass PicDraw"));
		return FALSE;
	}

	// 멤버 변수 초기값 설정
	m_nX1 = 10;
	m_nY1 = 10;
	m_nX2 = 590;
	m_nY2 = 590;

	m_nRadius = rand() % 50 + 10;
	// 컨트롤에 값 반영
	UpdateData(FALSE);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CCircleDrawDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CCircleDrawDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CCircleDrawDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CCircleDrawDlg::IsRunning()
{
	if (m_PicDraw.IsRunning())
	{
		MessageBox(_T("Action 처리중. 잠시후 다시 하세요"));
		return TRUE;
	}
	return FALSE;
}

CString CCircleDrawDlg::GetImgPath()
{
	// 실행 파일 경로 가져오기
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(nullptr, szPath, MAX_PATH);
	CString strImgPath = CString(szPath);
	strImgPath = strImgPath.Left(strImgPath.ReverseFind('\\')) + _T("\\image");

	// 폴더가 없으면 생성
	if (!PathFileExists(strImgPath))
	{
		CreateDirectory(strImgPath, nullptr);
	}
	return strImgPath;
}

void CCircleDrawDlg::OnBnClickedBtnDraw()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (IsRunning())
		return;
	UpdateData(TRUE); // 컨트롤에서 값 가져오기

	m_nRadius = rand() % 50 + 10;
	m_PicDraw.PicDraw(m_nX1, m_nY1, m_nRadius);
}


void CCircleDrawDlg::OnBnClickedBtnAction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (IsRunning())
		return;
	UpdateData(TRUE); // 컨트롤에서 값 가져오기

	CString strImgPath = GetImgPath();
	m_PicDraw.PicAction(m_nX1, m_nY1, m_nX2, m_nY2, m_nRadius, strImgPath);
}


void CCircleDrawDlg::OnBnClickedBtnOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (IsRunning())
		return;

	CString strImgPath = GetImgPath();

	CFileDialog fileDlg(TRUE, _T("jpg"), NULL, OFN_FILEMUSTEXIST, _T("Image Files (*.bmp; *.jpg)|*.bmp;*.jpg||"));
	fileDlg.m_ofn.lpstrInitialDir = strImgPath;

	if (fileDlg.DoModal() == IDOK)
	{
		CString filePath = fileDlg.GetPathName();
		m_PicDraw.PicOpen(filePath);
	}
}

void CCircleDrawDlg::ValueCheck(int* pValue)
{
	UpdateData(TRUE); // 컨트롤에서 값 가져오기
	if (*pValue < 0 || *pValue > 600)
	{
		AfxMessageBox(_T("입력값은 0에서 600 사이여야 합니다."));
		*pValue = 0;
		UpdateData(FALSE);
	}
}

void CCircleDrawDlg::OnEnChangeEditX1()
{
	ValueCheck(&m_nX1);
}


void CCircleDrawDlg::OnEnChangeEditY1()
{
	ValueCheck(&m_nY1);
}


void CCircleDrawDlg::OnEnChangeEditX2()
{
	ValueCheck(&m_nX2);
}


void CCircleDrawDlg::OnEnChangeEditY2()
{
	ValueCheck(&m_nY2);
}
