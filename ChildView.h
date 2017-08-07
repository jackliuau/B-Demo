// ChildView.h : interface of the CChildView class

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <gdiplus.h>
#include <gdiplusbrush.h>
#include <gdipluscolor.h>
#include <gdiplusenums.h>
#include <gdiplusgraphics.h>

#include <math.h>

using namespace Gdiplus;
using Gdiplus::PointF;

using std::vector;
using std::string;

const char pt_delimiter = ';';
const char xy_delimiter = ',';

struct _index_
{
    _index_()
    {
        _ii_ = -1;
        _jj_ = -1;
    }

    _index_(int ii, int jj)
    {
        _ii_ = ii;
        _jj_ = jj;
    }

    int _ii_;
    int _jj_;
};


struct CtrlPoints
{
    void* powner;
    vector<PointF> vecPts;

    void Init(void* po)
    {
        powner = po;
        vecPts.clear();
    }

    void FromString(string& str)
    { // won't do robustness check ---- assuming the string comes in with correct format!
        // 11,22; 11,33; 245,566
        vecPts.clear();

        if (str.find(pt_delimiter) == std::string::npos ||
            str.find(xy_delimiter) == std::string::npos)
        {
            return;
        }

        std::istringstream iss(str);
        std::string s;
        while (std::getline(iss, s, pt_delimiter))
        {
            PointF pt;
            pt.X = (float) atof(s.substr(0, s.find(xy_delimiter)).c_str());
            pt.Y = (float) atof(s.substr(s.find(xy_delimiter) + 1).c_str());
            vecPts.push_back(pt);
        }
    }

    string ToString()
    {
        std::ostringstream oss;
        for (vector<PointF>::iterator itPt = vecPts.begin(); itPt != vecPts.end(); itPt ++)
        {
            oss << itPt->X << xy_delimiter << itPt->Y << pt_delimiter;
        }
        return oss.str();
    }

    int IsPointSelectable(CPoint pt)
    {
        const int pt_on_distance = 5;

        int jj = 0;
        for (vector<PointF>::iterator itCtrlPoint = vecPts.begin(); itCtrlPoint != vecPts.end(); itCtrlPoint++, jj++)
        {
            if (abs(itCtrlPoint->X - pt.x) < pt_on_distance &&
                abs(itCtrlPoint->Y - pt.y) < pt_on_distance)
            {
                return jj;
            }
        }

        return -1;
    }

    void Offset(CPoint ptOffset)
    {
        for (vector<PointF>::iterator itCtrlPoint = vecPts.begin(); itCtrlPoint != vecPts.end(); itCtrlPoint++)
        {
            itCtrlPoint->X += ptOffset.x;
            itCtrlPoint->Y += ptOffset.y;
        }
    }

    bool operator == (CtrlPoints cps)
    {
        return IsEqualTo(cps);
    }

    bool IsEqualTo(CtrlPoints cps)
    {
        if (vecPts.size() != cps.vecPts.size())
        {
            return false;
        }

        for (int ii=0; ii < (int) vecPts.size(); ii++)
        {
            if (!vecPts[ii].Equals(cps.vecPts[ii]))
            {
                return false;
            }
        }

        return true;
    }

    bool IsLinear()
    {
        if (vecPts.size() <= 2)
        {
            return true;
        }

        CPoint pt0((long)vecPts[0].X, (long)vecPts[0].Y);
        CPoint pt1((long)vecPts[1].X, (long)vecPts[1].Y);
        for (size_t i = 2; i < vecPts.size(); i++)
        {
            CPoint pt2((long)vecPts[i].X, (long)vecPts[i].Y);
            // pt2-pt0
            if (pt1.x * (pt2.y - pt0.y) - pt0.x * (pt2.y - pt0.y) != pt2.x * (pt1.y - pt0.y) - pt0.x * (pt1.y - pt0.y))
            {
                return false;
            }

            pt0 = pt1;
            pt1 = pt2;
        }

        return true;
    }

};

struct CtrlGroups
{
    CtrlPoints _cps; // currently editing one.
    vector<CtrlPoints> vecGrps;

    void Init(void* po)
    {
        _cps.Init(po);
        vecGrps.clear();
    }

    void FromString(string& str)
    { // won't do robustness check ---- assuming the string comes in with correct format!
        // 11,22; 11,33; 245,566
        // 11,22; 11,33; 245,566
        _cps.vecPts.clear();
        vecGrps.clear();

        if (str.find(pt_delimiter) == std::string::npos ||
            str.find(xy_delimiter) == std::string::npos)
        {
            return;
        }

        std::istringstream iss(str);
        std::string s;
        while (std::getline(iss, s))
        {
            CtrlPoints cps;
            cps.Init(_cps.powner);
            cps.FromString(s);
            if (cps.vecPts.size() > 0)
            {
                vecGrps.push_back(cps);
            }
        }
    }

    string ToString()
    {
        std::ostringstream oss;
        for (vector<CtrlPoints>::iterator itGrp = vecGrps.begin(); itGrp != vecGrps.end(); itGrp ++)
        {
            oss << itGrp->ToString() << std::endl;
        }
        oss << _cps.ToString() << std::endl;
        return oss.str();
    }

    _index_ IsPointSelectable(CPoint pt)
    {
        int ii = 0;
        for (vector<CtrlPoints>::iterator itCtrlPoints = vecGrps.begin(); itCtrlPoints != vecGrps.end(); itCtrlPoints++, ii++)
        {
            int jj = itCtrlPoints->IsPointSelectable(pt);
            if (jj != -1)
            {
                return _index_(ii, jj);
            }
        }

        int jj = _cps.IsPointSelectable(pt);
        if (jj != -1)
        {
            return _index_(-1, jj);
        }

        return _index_(-1, -1);
    }

    void Offset(CPoint ptOffset)
    {
        for (vector<CtrlPoints>::iterator itCtrlPoints = vecGrps.begin(); itCtrlPoints != vecGrps.end(); itCtrlPoints++)
        {
            itCtrlPoints->Offset(ptOffset);
        }
    }

    bool operator == (CtrlGroups cgs)
    {
        return IsEqualTo(cgs);
    }

    bool IsEqualTo(CtrlGroups cgs)
    {
        if (vecGrps.size() != cgs.vecGrps.size())
        {
            return false;
        }

        if (!_cps.IsEqualTo(cgs._cps))
        {
            return false;
        }

        return std::equal(vecGrps.begin(), vecGrps.begin() + vecGrps.size(), cgs.vecGrps.begin());
    }

};

struct PascalTriangle
{
    vector<vector<long long>> vvll; // stores half of the symmetrical triangle.

    PascalTriangle()
    {
        vector<long long> vll;
        vll.push_back(1LL);
        vvll.push_back(vll);
    }

    inline long long Get(int n, int i)
    {
        if (n >= (int)vvll.size())
        {
            vector<long long> vll;
            vll.push_back(1LL);
            for (int ii = 1; ii <= n / 2; ii++)
            {
                vll.push_back(Get(n - 1, ii - 1) + Get(n - 1, ii));
            }
            vvll.push_back(vll);
        }

        if (i > n / 2)
        {
            i = n - i;
        }
        return vvll[n][i];
    }
};


// CChildView window

class CChildView : public CWnd
{
public:
    CChildView();
    virtual ~CChildView();

    void FillMark(PointF pt, int sz, COLORREF clr);
    void FillMark(CPoint pt, int sz, COLORREF clr);

    void DrawMark(PointF pt, int sz, COLORREF clr);
    void DrawMark(CPoint pt, int sz, COLORREF clr);

    void DrawLine(bool bTrulyDrawing, PointF pt1, PointF pt2, Gdiplus::Color gclr);
    void DrawLine(bool bTrulyDrawing, CPoint pt1, CPoint pt2, COLORREF clr);

    static unsigned __stdcall DrawBSpline_auto(void *pvoid);
    void DrawBSpline(CtrlPoints cps, int iOffsetDraw, DWORD dwSleep = 0);

    static unsigned __stdcall DrawBezier_auto(void *pvoid);
    void DrawBezier(CtrlPoints cps, int iOffsetDraw, DWORD dwSleep = 0);

    void DrawFittingCurve(CtrlPoints& cps, int iOffsetDraw, bool bAutoDraw);
    void DrawCtrlPoints(CtrlPoints cps, bool bEnded);

    void ClearClient();

    void ReDrawAll(bool bAutoDraw = false);

public:
    void Init();

    void LoadCGSFromFile(string& strFileName);
    void SaveCGSIntoFile(string& strFileName);

    bool GetBezierSwitch() {return m_bBezierIsOn;}
    bool GetBSplineSwitch() {return m_bBSplineIsOn;}
    bool GetOffsetDrawSwitch() {return m_bOffsetDrawIsOn;}

    void SwitchBezier();
    void SwitchBSpline();
    void SwitchOffsetDraw();

    string& GetFileName() { return m_strFileName; }

    bool HasCGSChanged();

    void DeleteLast();

    double N_(int k, int i, double u);
    double N1(int i, double u);
    double N2(int i, double u);
    double N3(int i, double u);

protected:
    CtrlGroups m_cgs;
    CtrlGroups m_cgs_copy;

    string m_strFileName;

    bool m_bBezierIsOn;
    bool m_bBSplineIsOn;
    bool m_bOffsetDrawIsOn;

    bool m_bLBtnClickInClient;

    bool m_bDraggable;
    bool m_bDragging;

    _index_  m_idxDragging;

    CMutex mtx_pascal_triangle;
    PascalTriangle  pascal_triangle;

    static const int LBTN_CLICK_TIMER_ID = 8;
    static const int RBTN_CLICK_TIMER_ID = 9;

    static const int OFFSET_DRAW_OFFSET = 15;

    CMutex mtx_gdiplus_graphics;
    Gdiplus::Graphics* m_pGdiplusGraphics;

    std::vector<HANDLE> m_vecAutoDrawThreadHandles;

protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    DECLARE_MESSAGE_MAP()

    afx_msg void OnPaint();

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short iWheel, CPoint point);

    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

    afx_msg void OnTimer(UINT nIDEvent);
};
