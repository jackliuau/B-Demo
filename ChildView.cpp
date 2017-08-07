// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "jkl6plus.h"
#include "ChildView.h"


#include <fstream>
#include <streambuf>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
    if (m_pGdiplusGraphics)
    {
        delete m_pGdiplusGraphics;
    }
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
    ON_WM_PAINT()
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_TIMER()
END_MESSAGE_MAP()

// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
    if (!CWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle |= WS_EX_CLIENTEDGE;
    cs.style &= ~WS_BORDER;
    cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
        ::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

    m_pGdiplusGraphics = NULL;

    Init();

    return TRUE;
}

void CChildView::OnPaint() 
{
    CPaintDC dc(this); // IMPORTANT! DON'T DELETE!

    if (!m_pGdiplusGraphics)
    {
        m_pGdiplusGraphics = Gdiplus::Graphics::FromHDC(this->GetDC()->m_hDC); // new a Gdiplus::Graphics.
        m_pGdiplusGraphics->SetSmoothingMode(SmoothingModeHighQuality);

        //Graphics graphics(this->GetDC()->m_hDC);

        //// Create an Image object from a PNG file.
        //Image image(L"C:\\(JVL)\\work\\jkl6plus\\train_france.jpg");

        //// Create a Graphics object that is associated with the image.
        //Graphics* imageGraphics = Graphics::FromImage(&image);

        //// Draw the altered image.
        //graphics.DrawImage(&image, 30, 20);

        //delete imageGraphics;
    }

    ReDrawAll();

    // below code is used to verify GdiPlus AntiAliasing drawing.   JKL
    //float x = 100.0F;
    //DrawLine(PointF(x, 100), PointF(x, 300), RGB(200, 0, 0));
    //x += 100;
    //DrawLine(PointF(x, 100), PointF(x+0.2, 300), RGB(200, 0, 0));
    //x += 100;
    //DrawLine(PointF(x, 100), PointF(x+0.5, 300), RGB(200, 0, 0));
    //x += 100;
    //DrawLine(PointF(x, 100), PointF(x+0.8, 300), RGB(200, 0, 0));
    //x += 100;
    //DrawLine(PointF(x, 100), PointF(x + 1.0, 300), RGB(200, 0, 0));

    // Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
    CString  cstr;
    cstr.Format(" JKL6PLUS -- Bezier demo -- %s    [%d, %d]", m_strFileName.c_str(), point.x, point.y);
    (AfxGetMainWnd( ))->SetWindowText(cstr);

    if (m_bDraggable)
        m_bDragging = true;

    if (m_bDragging)
    {
        if (m_idxDragging._jj_ >= 0)
        {
            if (m_idxDragging._ii_ >= 0)
            {
                if (nFlags&MK_CONTROL)
                {
                    CPoint ptOrig((long)m_cgs.vecGrps[m_idxDragging._ii_].vecPts[m_idxDragging._jj_].X,
                                  (long)m_cgs.vecGrps[m_idxDragging._ii_].vecPts[m_idxDragging._jj_].Y);
                    CPoint ptDelta = point - ptOrig;
                    m_cgs.vecGrps[m_idxDragging._ii_].Offset(ptDelta);
                }
                else // dragging 1 point.
                {
                    m_cgs.vecGrps[m_idxDragging._ii_].vecPts[m_idxDragging._jj_].X = (float)point.x;
                    m_cgs.vecGrps[m_idxDragging._ii_].vecPts[m_idxDragging._jj_].Y = (float)point.y;
                }
            }
            else
            {
                if (nFlags&MK_CONTROL)
                {
                    CPoint ptOrig((long)m_cgs._cps.vecPts[m_idxDragging._jj_].X, (long)m_cgs._cps.vecPts[m_idxDragging._jj_].Y);
                    CPoint ptDelta = point - ptOrig;
                    m_cgs._cps.Offset(ptDelta);
                }
                else // dragging 1 point.
                {
                    m_cgs._cps.vecPts[m_idxDragging._jj_].X = (float)point.x;
                    m_cgs._cps.vecPts[m_idxDragging._jj_].Y = (float)point.y;
                }
            }

            ReDrawAll();
        }
    }

    //HCURSOR hCursor = (m_cgs.IsPointSelectable(point)._jj_ >= 0) ? LoadCursorFromFile(".//res//cursor1.cur") : LoadCursorFromFile(".//res//cursor2.cur");
    HCURSOR hCursor = (m_cgs.IsPointSelectable(point)._jj_ >= 0) ? LoadCursor(NULL, IDC_SIZEALL) : LoadCursor(NULL, IDC_CROSS);
    SetCursor(hCursor);
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short iWheel, CPoint point)
{
    return TRUE;
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}
void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCursor(LoadCursor(NULL, IDC_CROSS));

    m_bLBtnClickInClient = true;

    m_idxDragging = m_cgs.IsPointSelectable(point);
    if (m_idxDragging._jj_ >= 0)
    {
        m_bDraggable = true;
    }
    else 
    {
        m_bDraggable = false;
    }
}
void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
    SetCursor(LoadCursor(NULL, IDC_SIZEALL));

    m_bDraggable = false;

    if (m_bDragging)
    {
        m_bDragging = false;
        return;
    }

    if (m_bLBtnClickInClient)
    {
        m_bLBtnClickInClient = false;

        m_cgs._cps.vecPts.push_back(PointF((float)point.x, (float)point.y));

        ReDrawAll();
    }
}


void CChildView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    ::KillTimer(this->GetSafeHwnd(), RBTN_CLICK_TIMER_ID);

    ReDrawAll(true);
}
void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
    ::SetTimer(this->GetSafeHwnd(), RBTN_CLICK_TIMER_ID, ::GetDoubleClickTime(), NULL); // 200 ~ 900
}
void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
}


void CChildView::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == LBTN_CLICK_TIMER_ID)
    {
    }
    else if (nIDEvent == RBTN_CLICK_TIMER_ID)
    {
        ::KillTimer(this->GetSafeHwnd(), RBTN_CLICK_TIMER_ID);

        if (m_cgs._cps.vecPts.size() > 0)
        {
            m_cgs.vecGrps.push_back(m_cgs._cps);
            m_cgs._cps.vecPts.clear();
        }

        ReDrawAll();
    }

    CWnd::OnTimer(nIDEvent);
}


void CChildView::FillMark(PointF pt, int sz, COLORREF clr)
{
    FillMark(CPoint((int)pt.X, (int)pt.Y), sz, clr);
}
void CChildView::FillMark(CPoint pt, int sz, COLORREF clr)
{
    CDC* pdc = this->GetDC();

    CRect rct(pt.x - sz, pt.y - sz, pt.x + sz + 1, pt.y + sz + 1);
    pdc->FillRect(&rct, &CBrush(clr));

    this->ReleaseDC(pdc);
}

void CChildView::DrawMark(PointF pt, int sz, COLORREF clr)
{
    DrawMark(CPoint((int)pt.X, (int)pt.Y), sz, clr);
}
void CChildView::DrawMark(CPoint pt, int sz, COLORREF clr)
{
    CDC* pdc = this->GetDC();

    CPen pen(PS_GEOMETRIC | PS_DOT, 1, clr);
    CPen* pOldPen = pdc->SelectObject(&pen);

    pdc->MoveTo(pt.x - sz, pt.y - sz);
    pdc->LineTo(pt.x + sz, pt.y - sz);
    pdc->LineTo(pt.x + sz, pt.y + sz);
    pdc->LineTo(pt.x - sz, pt.y + sz);
    pdc->LineTo(pt.x - sz, pt.y - sz);

    pdc->SelectObject(pOldPen);

    this->ReleaseDC(pdc);
}

void CChildView::DrawLine(bool bTrulyDrawing, PointF pt1, PointF pt2, Gdiplus::Color gclr)
{
    if (!bTrulyDrawing)
        return;

    PointF pts[2] = { pt1, pt2 };

    Pen redPen(gclr, 1.6f);

    mtx_gdiplus_graphics.Lock();
    m_pGdiplusGraphics->DrawLines(&redPen, pts, 2);
    mtx_gdiplus_graphics.Unlock();
}
void CChildView::DrawLine(bool bTrulyDrawing, CPoint pt1, CPoint pt2, COLORREF clr)
{
    if (!bTrulyDrawing)
        return;

    CDC* pdc = this->GetDC();

    CPen pen(PS_GEOMETRIC | PS_DOT, 1, clr);
    CPen* pOldPen = pdc->SelectObject(&pen);

    pdc->MoveTo(pt1);
    pdc->LineTo(pt2);

    pdc->SetPixel(pt2, clr); // since LineTo doesn't set the exact pixel.

    pdc->SelectObject(pOldPen);

    this->ReleaseDC(pdc);
}

inline PointF Offset(PointF pt, int offset)
{
    const short x_offset_direction = 1;
    const short y_offset_direction = 1;

    pt.X += x_offset_direction * offset;
    pt.Y += y_offset_direction * offset;
    return pt;
}
inline CPoint Offset(CPoint pt, int offset)
{
    const short x_offset_direction = 1;
    const short y_offset_direction = 1;

    pt.Offset(x_offset_direction * offset, y_offset_direction * offset);
    return pt;
}

inline CPoint Ptf2Pt(PointF ptf)
{
    return CPoint((int) ptf.X, (int) ptf.Y);
}

unsigned __stdcall CChildView::DrawBSpline_auto(void *pvoid)
{
    CtrlPoints* pcps = (CtrlPoints*)pvoid;
    CChildView* powner = (CChildView*)(pcps->powner);

    DWORD dwSleep = 20 / pcps->vecPts.size();
    powner->DrawBSpline(*pcps, powner->m_bOffsetDrawIsOn ? powner->OFFSET_DRAW_OFFSET : 0, dwSleep);

    return 0;
}

void CChildView::DrawBSpline(CtrlPoints cps, int iOffsetDraw, DWORD dwSleep)
{
    int nSize = cps.vecPts.size();
    if (nSize < 2)
    {
        return;
    }

    Gdiplus::Color clr = Gdiplus::Color::Cyan;

    vector<PointF> vecPts;
    vecPts.push_back(*cps.vecPts.begin());
    vecPts.insert(vecPts.end(), cps.vecPts.begin(), cps.vecPts.end());
    vecPts.push_back(*cps.vecPts.rbegin());

    PointF ptFrom = *vecPts.begin();
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, ptFrom, Offset(ptFrom, iOffsetDraw), clr);

    PointF ptTo = *vecPts.rbegin();
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, ptTo, Offset(ptTo, iOffsetDraw), clr);

    if (cps.IsLinear())
    {
        DrawLine(true, ptFrom, ptTo, clr);
        if (iOffsetDraw > 0)
            DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTo, iOffsetDraw), clr);

        return;
    }

    const int iCurrentK = 3;
    for (double u = iCurrentK; u <= vecPts.size(); u += 0.05)
    {
        if (!m_bBSplineIsOn)
            return;

        PointF ptTemp(0, 0);
        for (int i = 0; i < (int)vecPts.size(); i++)
        {
            float fN_ = (float) N_(iCurrentK, i, u);

            ptTemp.X += (vecPts[i].X * fN_);
            ptTemp.Y += (vecPts[i].Y * fN_);
        }

        DrawLine(m_bBSplineIsOn, ptFrom, ptTemp, clr);
        if (iOffsetDraw > 0)
            DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTemp, iOffsetDraw), clr);

        //if (i == TOTALNUMBER / 2)
        {
            //DrawMark(ptTemp, 3);
        }

        ptFrom = ptTemp;

        ::Sleep(dwSleep);
    }

    ptTo = *vecPts.rbegin();

    DrawLine(m_bBSplineIsOn, ptFrom, ptTo, clr);
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTo, iOffsetDraw), clr);
}


unsigned __stdcall CChildView::DrawBezier_auto(void *pvoid)
{
    CtrlPoints* pcps = (CtrlPoints*)pvoid;
    CChildView* powner = (CChildView*)(pcps->powner);

    DWORD dwSleep = 20 / pcps->vecPts.size();
    powner->DrawBezier(*pcps, powner->m_bOffsetDrawIsOn ? powner->OFFSET_DRAW_OFFSET : 0, dwSleep);

    return 0;
}

void CChildView::DrawBezier(CtrlPoints cps, int iOffsetDraw, DWORD dwSleep)
{
    int nSize = cps.vecPts.size();
    if (nSize < 2)
    {
        return;
    }

    Gdiplus::Color clr = Gdiplus::Color::Gold;

    PointF ptFrom = *cps.vecPts.begin();
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, ptFrom, Offset(ptFrom, iOffsetDraw), clr);

    PointF ptTo = *cps.vecPts.rbegin();
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, ptTo, Offset(ptTo, iOffsetDraw), clr);

    if (cps.IsLinear())
    {
        DrawLine(true, ptFrom, ptTo, clr);
        if (iOffsetDraw > 0)
            DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTo, iOffsetDraw), clr);

        return;
    }

    const int TOTALNUMBER = 20 * nSize;
    const double STEP = 1.0 / TOTALNUMBER;

    for (int i = 1; i < TOTALNUMBER; i++)
    {
        if (!m_bBezierIsOn)
            return;

        double t = STEP * i;
        double _t = 1.0 - t;

        //double Bt = (1.0 - t)*(1.0 - t)*(1.0 - t)*pt[0] + 3.0*(1.0 - t)*(1.0 - t)*t*pt[1] + 3.0*(1.0 - t)*t*t*pt[2] + t*t*t*pt[3];

        double xBt = 0.0;
        double yBt = 0.0;
        for (int j = 0; j < nSize; j++)
        {
            mtx_pascal_triangle.Lock();
            double ptv = pascal_triangle.Get(nSize - 1, j) * pow(_t, nSize - j - 1) * pow(t, j);
            mtx_pascal_triangle.Unlock();

            xBt += (ptv * cps.vecPts[j].X);
            yBt += (ptv * cps.vecPts[j].Y);
        }

        ptTo.X = (float)(xBt + 0.0); // ????? rounding ?????
        ptTo.Y = (float)(yBt + 0.0); // ????? rounding ?????

        DrawLine(m_bBezierIsOn, ptFrom, ptTo, clr);
        if (iOffsetDraw > 0)
            DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTo, iOffsetDraw), clr);

        if (i == TOTALNUMBER / 2)
        {
            //DrawMark(ptTo, 2);
        }

        ptFrom = ptTo;

        ::Sleep(dwSleep);
    }

    ptTo = *cps.vecPts.rbegin();

    DrawLine(m_bBezierIsOn, ptFrom, ptTo, clr);
    if (iOffsetDraw > 0)
        DrawLine(m_bOffsetDrawIsOn, Offset(ptFrom, iOffsetDraw), Offset(ptTo, iOffsetDraw), clr);
}

void CChildView::DrawFittingCurve(CtrlPoints& cps, int iOffsetDraw, bool bAutoDraw)
{
    int nSize = cps.vecPts.size();
    if (nSize < 2)
    {
        return;
    }

    if (bAutoDraw)
    {
        if (m_bBezierIsOn)
            m_vecAutoDrawThreadHandles.push_back((HANDLE)_beginthreadex(NULL, 0, (&CChildView::DrawBezier_auto), &cps, 0, NULL));
        if (m_bBSplineIsOn)
            m_vecAutoDrawThreadHandles.push_back((HANDLE)_beginthreadex(NULL, 0, (&CChildView::DrawBSpline_auto), &cps, 0, NULL));
    }
    else
    {
        if (m_bBezierIsOn)
            DrawBezier(cps, iOffsetDraw);
        if (m_bBSplineIsOn)
            DrawBSpline(cps, iOffsetDraw);
    }
}

void CChildView::DrawCtrlPoints(CtrlPoints cps, bool bEnded)
{
    if (cps.vecPts.size() <= 0)
    {
        return;
    }

    CDC* pdc = this->GetDC();

    vector<PointF>::iterator itPt = cps.vecPts.begin();

    pdc->MoveTo((int)itPt->X, (int)itPt->Y);

    DrawMark(*itPt, 3, RGB(250, 50, 50));
    FillMark(*itPt, 2, RGB(50, 250, 50));

    for (itPt +=1; itPt != cps.vecPts.end(); itPt++)
    {
        pdc->LineTo((int)itPt->X, (int)itPt->Y);
        pdc->SetPixel((int)itPt->X, (int)itPt->Y, RGB(255, 0, 0)); // since LineTo() doesn't set the exact pixel.

        DrawMark(*itPt, 3, RGB(250, 50, 50));
    }

    if (bEnded)
    {
        FillMark(*cps.vecPts.rbegin(), 2, RGB(50, 50, 250));
    }

    this->ReleaseDC(pdc);
}

void CChildView::ClearClient()
{
    CDC* pdc = this->GetDC();

    CRect rctClient;
    this->GetClientRect(&rctClient);
    pdc->FillRect(&rctClient, &CBrush(RGB(20, 20, 30)));

    this->ReleaseDC(pdc);
}

void CChildView::ReDrawAll(bool bAutoDraw)
{
    if (!m_vecAutoDrawThreadHandles.empty())
    {
        ::WaitForMultipleObjects(m_vecAutoDrawThreadHandles.size(), &m_vecAutoDrawThreadHandles[0], true, 500);

        m_vecAutoDrawThreadHandles.clear();
    }

    ClearClient();

    // draw CtrlPoints first, then FittingCurves, so that there is no FittingCurves being ruined by CtrlPoints.

    for (vector<CtrlPoints>::iterator itCtrlPoints = m_cgs.vecGrps.begin(); itCtrlPoints != m_cgs.vecGrps.end(); itCtrlPoints++)
    {
        DrawCtrlPoints(*itCtrlPoints, true);
    }

    DrawCtrlPoints(m_cgs._cps, false);

    if (m_bBezierIsOn || m_bBSplineIsOn)
    {
        for (vector<CtrlPoints>::iterator itCtrlPoints = m_cgs.vecGrps.begin(); itCtrlPoints != m_cgs.vecGrps.end(); itCtrlPoints++)
        {
            DrawFittingCurve(*itCtrlPoints, m_bOffsetDrawIsOn ? OFFSET_DRAW_OFFSET : 0, bAutoDraw);
        }

        DrawFittingCurve(m_cgs._cps, m_bOffsetDrawIsOn ? 5 : 0, bAutoDraw);
    }




    //Gdiplus::Graphics graphics(this->GetDC()->m_hDC);

    // Set the smoothing mode to SmoothingModeHighSpeed, and fill an ellipse.
    //graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);
    //graphics.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 220, 0)), 0, 0, 200, 100);

    // Set the smoothing mode to SmoothingModeHighQuality, and fill an ellipse.
    //graphics.SetSmoothingMode(SmoothingModeHighQuality);
    //graphics.FillEllipse(&SolidBrush(Color(255, 0, 0, 0)), 200, 0, 200, 100);
}

void CChildView::Init()
{
    m_cgs.Init(this);
    m_cgs_copy.Init(this);

    m_strFileName = "";

    m_bBezierIsOn = true;
    m_bBSplineIsOn = true;
    m_bOffsetDrawIsOn = true;

    m_bLBtnClickInClient = false;

    m_bDraggable = false;
    m_bDragging = false;

    m_vecAutoDrawThreadHandles.clear();
}

void CChildView::LoadCGSFromFile(string& strFileName)
{
    m_strFileName = strFileName;

    std::ifstream ifs(strFileName.c_str());
    std::string strf((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    ifs.close();

    m_cgs.FromString(strf);
    m_cgs_copy = m_cgs;

    ReDrawAll(true);
}

void CChildView::SaveCGSIntoFile(string& strFileName)
{
    m_strFileName = strFileName;

    std::ofstream ofs(strFileName.c_str());
    ofs << m_cgs.ToString();
    ofs.close();

    m_cgs_copy = m_cgs;


    //{
    //    std::ostringstream oss;
    //    for (int nn = 0; nn < 50; nn++)
    //    {
    //        for (int ii = 0; ii <= nn; ii++)
    //        {
    //            oss << pascal_triangle.Get(nn, ii) << ',';
    //        }
    //        oss << std::endl;
    //    }

    //    ofs << oss.str();
    //}
}

void CChildView::SwitchBezier()
{
    m_bBezierIsOn = !m_bBezierIsOn;

    //if (!m_bBezierIsOn)
    //    m_bTerminateAllDrawinghread = true;

    ReDrawAll(true);
}

void CChildView::SwitchBSpline()
{
    m_bBSplineIsOn = !m_bBSplineIsOn;

    ReDrawAll(true);
}

void CChildView::SwitchOffsetDraw()
{
    m_bOffsetDrawIsOn = !m_bOffsetDrawIsOn;

    ReDrawAll(true);
}

bool CChildView::HasCGSChanged()
{
    return !(m_cgs.IsEqualTo(m_cgs_copy));
}

void CChildView::DeleteLast()
{
    if (m_cgs._cps.vecPts.size() > 0)
    {
        m_cgs._cps.vecPts.erase(m_cgs._cps.vecPts.begin() + m_cgs._cps.vecPts.size() - 1);
    }
    else if (m_cgs.vecGrps.size() > 0)
    {
        m_cgs.vecGrps.erase(m_cgs.vecGrps.begin() + m_cgs.vecGrps.size() - 1);
    }

    ReDrawAll();
}


double CChildView::N_(int k, int i, double u)
{
    switch (k)
    {
    case 1:
        return N1(i, u);
    case 2:
        return N2(i, u);
    case 3:
        return N3(i, u);
    }

    return 1.0f;
}

double CChildView::N1(int i, double u)
{
    double t = u - i;

    if (0 <= t && t < 1)
        return t;
    if (1 <= t && t < 2)
        return 2 - t;
    return 0;
}

double CChildView::N2(int i, double u)
{
    double t = u - i;

    if (0 <= t && t < 1)
        return 0.5*t*t;
    if (1 <= t && t < 2)
        return 3 * t - t*t - 1.5;
    if (2 <= t && t < 3)
        return 0.5*pow(3 - t, 2);
    return 0;
}

double CChildView::N3(int i, double u)
{
    double t = u - i;
    double a = 1.0 / 6.0;

    if (0 <= t && t < 1)
        return a*t*t*t;
    if (1 <= t && t < 2)
        return a*(-3 * pow(t - 1, 3) + 3 * pow(t - 1, 2) + 3 * (t - 1) + 1);
    if (2 <= t && t < 3)
        return a*(3 * pow(t - 2, 3) - 6 * pow(t - 2, 2) + 4);
    if (3 <= t && t < 4)
        return a*pow(4 - t, 3);
    return 0;
}
