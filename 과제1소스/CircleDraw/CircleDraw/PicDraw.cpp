#include "pch.h"
#include "PicDraw.h"

#include <wingdi.h>


BEGIN_MESSAGE_MAP(CPicDraw, CStatic)
    ON_WM_PAINT() // WM_PAINT 메시지 매핑
    ON_MESSAGE(WM_USER_ACTION_DRAW, &CPicDraw::OnActionDraw) // 사용자 정의 메시지 매핑
    ON_MESSAGE(WM_USER_ACTION_STOP, &CPicDraw::OnActionStop) // 사용자 정의 메시지 매핑
END_MESSAGE_MAP()


CPicDraw::CPicDraw()
{
    // 필요 시 추가 초기화 코드
    m_nX1 = 0;
    m_nY1 = 0;
    m_nX2 = 0;
    m_nY2 = 0;
    m_nRadius = 0;
    m_bRunning = false;
    m_bImgDraw = false;
}

CPicDraw::~CPicDraw()
{
    ActionStop();
}

BOOL CPicDraw::IsRunning()
{
    return m_bRunning;
}

void CPicDraw::PicDraw(int x1, int y1, int nRadius)
{
    m_nX1 = x1;
    m_nY1 = y1;
    m_nRadius = nRadius;
    m_bImgDraw = false;
    Invalidate();
}

void CPicDraw::PicAction(int x1, int y1, int x2, int y2, int nRadius, const CString& strImgPath)
{
    m_nX1 = x1;
    m_nY1 = y1;
    m_nX2 = x2;
    m_nY2 = y2;
    m_nRadius = nRadius;
    m_strImgPath = strImgPath;
    m_nSteb = 0;
    m_bImgDraw = false;
    ActionStart();
}

void CPicDraw::PicOpen(const CString& strImgName)
{
    m_strImgName = strImgName;
    m_bImgDraw = true;

    Invalidate();
}

void CPicDraw::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);

    int savedDC = dc.SaveDC();
    dc.IntersectClipRect(&rect);

    // 배경 초기화 (시스템 기본 색상으로 채우기)
    CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetSysColorBrush(COLOR_BTNFACE));
    dc.FillRect(&rect, pBrush);

    if (m_bImgDraw)
    {
        CImage image;
        if (SUCCEEDED(image.Load(m_strImgName)))
        {
            image.BitBlt(dc.GetSafeHdc(), 0, 0);

            // 원의 중심 찾기
            POINT ptCenter = FindCircleCenter(image);

            // 원의 중심에 X와 좌표값 표시
            DrawCenterPoint(&dc, rect, ptCenter.x, ptCenter.y);
            image.Destroy();
        }
    }
    else
    {
        CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
        CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
        CPen* pOldPen = dc.SelectObject(&pen);

        dc.Ellipse(m_nX1 - m_nRadius, m_nY1 - m_nRadius, m_nX1 + m_nRadius, m_nY1 + m_nRadius);

        dc.SelectObject(pOldPen);
        dc.SelectObject(pOldBrush);

        if (m_bRunning)
        {
            m_memDC.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

            // 파일로 저장하기
            CString fileName;
            fileName.Format(_T("%s\\image_step_%03d.jpg"), (LPCTSTR)m_strImgPath, m_nSteb);

            CImage image;
            image.Attach((HBITMAP)m_bitmap.GetSafeHandle());
            image.Save(fileName, Gdiplus::ImageFormatJPEG);
            image.Detach();
            image.Destroy();
        }
    }

    dc.RestoreDC(savedDC);
}

void ThreadProcess(CWnd* pWnd)
{
    CPicDraw* pPicDraw = (CPicDraw*)pWnd;
    pPicDraw->ThreadWorker();
}

void CPicDraw::ActionStart()
{
    if (m_bRunning)
        return; // 이미 실행 중이면 중복 실행 방지


    m_bRunning = true;

    if (!m_memDC.GetSafeHdc())
    {
        CRect rect;
        GetClientRect(&rect);
        CClientDC dc(this);
        m_memDC.CreateCompatibleDC(&dc);
        m_bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
        m_pOldBitmap = m_memDC.SelectObject(&m_bitmap);
    }

    m_workerThread = std::thread(ThreadProcess, this);
}


void CPicDraw::ThreadWorker()
{
    const int nStepCount = 20; // 이동을 20 단계로 나눔
    const float dx = (float)(m_nX2 - m_nX1) / nStepCount;
    const float dy = (float)(m_nY2 - m_nY1) / nStepCount;

    int nCurrentX = m_nX1;
    int nCurrentY = m_nY1;
    for (int i = 0; i <= nStepCount; i++)
    {
        if (!m_bRunning)
            break; // 중단 요청 시 종료

        m_nX1 = nCurrentX + (int)(i * dx);
        m_nY1 = nCurrentY + (int)(i * dy);
        m_nSteb = i;
        SendMessage(WM_USER_ACTION_DRAW, 0, 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    PostMessage(WM_USER_ACTION_STOP, 0, 0);
}

void CPicDraw::ActionStop()
{
    if (m_workerThread.joinable())
    {
        m_workerThread.join(); // 쓰레드 종료 대기
    }

    if (m_memDC.GetSafeHdc())
    {
        m_memDC.SelectObject(m_pOldBitmap);
        m_bitmap.DeleteObject();
        m_memDC.DeleteDC();
    }

    m_bRunning = false; // 이동 중단
}

// 사용자 정의 메시지 핸들러
LRESULT CPicDraw::OnActionDraw(WPARAM wParam, LPARAM lParam)
{
    Invalidate();
    UpdateWindow();
    return 0;
}

LRESULT CPicDraw::OnActionStop(WPARAM wParam, LPARAM lParam)
{
    ActionStop();
    return 0;
}



POINT CPicDraw::FindCircleCenter(const CImage& image)
{
    POINT ptT = { 0, 0 };
    POINT ptL = { 0, 0 };
    POINT ptB = { 0, 0 };
    POINT ptR = { 0, 0 };

    int nWidth = image.GetWidth();
    int nHeight = image.GetHeight();
    int nBpp = image.GetBPP();

    if (nBpp != 24 && nBpp != 32)
    {
        AfxMessageBox(_T("Only 24-bit or 32-bit images are supported."));
        return { -1, -1 };
    }

    BYTE* pBits = (BYTE*)image.GetBits();
    int nPitch = image.GetPitch(); 

    int x, y;
    // 원 상단 찾기
    float fSumX = 0, fSumY = 0, nCount = 0;
    for (y = 0; y < nHeight; y++)
    {
        for (x = 0; x < nWidth; x++)
        {
            BYTE* pixel = (pBits + y * nPitch) + x * (nBpp / 8); // 현재 픽셀 주소
            // 원 찾기
            if ((pixel[2] < 0x80) && (pixel[1] < 0x80) && (pixel[0] < 0x80))
            {
                fSumX += x;
                nCount++;
            }
        }
        if (nCount)
            break;
    }

    if (nCount > 0)
    {
        ptT.x = (int)round(fSumX / nCount);
        ptT.y = y;
    }

    // 원 왼쪽 찾기
    nCount = 0;
    for (x = 0; x < nWidth; x++)
    {
        for (y = 0; y < nHeight; y++)
        {
            BYTE* pixel = (pBits + y * nPitch) + x * (nBpp / 8); // 현재 픽셀 주소
            // 원 찾기
            if ((pixel[2] < 0x80) && (pixel[1] < 0x80) && (pixel[0] < 0x80))
            {
                fSumY += y;
                nCount++;
            }
        }
        if (nCount)
            break;
    }

    if (nCount > 0)
    {
        ptL.x = x;
        ptL.y = (int)round(fSumY / nCount);
    }


    // 원 하단 찾기
    fSumX = 0, fSumY = 0, nCount = 0;
    for (y = nHeight - 1; y >= 0; y--)
    {
        for (x = 0; x < nWidth; x++)
        {
            BYTE* pixel = (pBits + y * nPitch) + x * (nBpp / 8); // 현재 픽셀 주소
            // 원 찾기
            if ((pixel[2] < 0x80) && (pixel[1] < 0x80) && (pixel[0] < 0x80))
            {
                fSumX += x;
                nCount++;
            }
        }
        if (nCount)
            break;
    }

    if (nCount > 0)
    {
        ptB.x = (int)round(fSumX / nCount);
        ptB.y = y;
    }

    // 원 오른쪽 찾기
    nCount = 0;
    for (x = nWidth - 1; x >= 0; x--)
    {
        for (y = 0; y < nHeight; y++)
        {
            BYTE* pixel = (pBits + y * nPitch) + x * (nBpp / 8); // 현재 픽셀 주소
            // 원 찾기
            if ((pixel[2] < 0x80) && (pixel[1] < 0x80) && (pixel[0] < 0x80))
            {
                fSumY += y;
                nCount++;
            }
        }
        if (nCount)
            break;
    }

    if (nCount > 0)
    {
        ptR.x = x;
        ptR.y = (int)round(fSumY / nCount);
    }


    CRect rect;
    GetClientRect(&rect);

    POINT ptCenter = { -1, -1 };

    if (abs(ptT.x - ptB.x) <= 3)
        ptCenter.x = (int)round(((float)ptT.x + ptB.x) / 2);
    else
    {
        if (ptT.y == 0)
            ptCenter.x = ptB.x;
        else
            ptCenter.x = ptT.x;
    }
    if (abs(ptL.y - ptR.y) <= 3)
        ptCenter.y = (int)round(((float)ptL.y + ptR.y) / 2);
    else
    {
        if (ptL.x == 0)
            ptCenter.y = ptR.y;
        else
            ptCenter.y = ptL.y;
    }

    return ptCenter;
}

void CPicDraw::DrawCenterPoint(CDC* pDC, CRect rcDraw, int nCenterX, int nCenterY)
{
    CPen pen(PS_SOLID, 5, RGB(0, 0, 0));
    CPen* pOldPen = pDC->SelectObject(&pen);

    const int nSize = 10;
    pDC->MoveTo(nCenterX - nSize, nCenterY - nSize);
    pDC->LineTo(nCenterX + nSize, nCenterY + nSize);

    pDC->MoveTo(nCenterX - nSize, nCenterY + nSize);
    pDC->LineTo(nCenterX + nSize, nCenterY - nSize);

    CString coords;
    coords.Format(_T("(%d, %d)"), nCenterX, nCenterY);
    if (nCenterX > rcDraw.right - 100)
        nCenterX -= 100;
    if (nCenterY > rcDraw.bottom - 40)
        nCenterY -= 40;
    pDC->TextOutW(nCenterX + nSize, nCenterY + nSize, coords);

    pDC->SelectObject(pOldPen);
}

