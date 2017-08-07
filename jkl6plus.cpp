// jkl6plus.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "jkl6plus.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Cjkl6plusApp

BEGIN_MESSAGE_MAP(Cjkl6plusApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &Cjkl6plusApp::OnAppAbout)
    ON_COMMAND(ID_APP_BEZIER, &Cjkl6plusApp::OnAppBezier)
    ON_COMMAND(ID_APP_BSPLINE, &Cjkl6plusApp::OnAppBSpline)
    ON_COMMAND(ID_APP_OFFSET_DRAW, &Cjkl6plusApp::OnAppOffsetDraw)
    ON_COMMAND(ID_APP_DELETE_LAST, &Cjkl6plusApp::OnAppDeleteLast)
    ON_COMMAND(ID_APP_NEW, &Cjkl6plusApp::OnAppNew)
    ON_COMMAND(ID_APP_OPEN, &Cjkl6plusApp::OnAppOpen)
    ON_COMMAND(ID_APP_SAVE, &Cjkl6plusApp::OnAppSave)
    ON_COMMAND(ID_APP_SAVE_AS, &Cjkl6plusApp::OnAppSaveAs)
    ON_COMMAND(ID_APP_EXIT, &Cjkl6plusApp::OnAppExit)
END_MESSAGE_MAP()

// Cjkl6plusApp construction
Cjkl6plusApp theApp;

Cjkl6plusApp::Cjkl6plusApp()
{
}

// The one and only Cjkl6plusApp object

// Cjkl6plusApp initialization

BOOL Cjkl6plusApp::InitInstance()
{
    CWinApp::InitInstance();

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame)
        return FALSE;
    m_pMainWnd = pMainFrame;

    // create and load the frame with its resources
    pMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

    // The one and only window has been initialized, so show and update it
    pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
    pMainFrame->UpdateWindow();
    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

int Cjkl6plusApp::ExitInstance()
{
    Gdiplus::GdiplusShutdown(m_gdiplusToken);

    return CWinApp::ExitInstance();
}

// Cjkl6plusApp message handlers

void Cjkl6plusApp::OnAppAbout()
{
    CDialog dlg(IDD_ABOUTBOX);
    dlg.DoModal();
}

void Cjkl6plusApp::UpdateMenuItems(UINT uiID, CString strMenu, bool bl)
{
    if (bl)
        strMenu += "On";
    else
        strMenu += "Off";

    ((CMainFrame*)m_pMainWnd)->GetMenu()->ModifyMenu(uiID, MF_BYCOMMAND| MF_STRING, uiID, (LPCTSTR)strMenu);

    m_pMainWnd->DrawMenuBar();
}

void Cjkl6plusApp::OnAppBezier()
{
    ((CMainFrame*)m_pMainWnd)->m_ChildView.SwitchBezier();

    UpdateMenuItems(ID_APP_BEZIER, CString("Bezier:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetBezierSwitch());
}

void Cjkl6plusApp::OnAppBSpline()
{
    ((CMainFrame*)m_pMainWnd)->m_ChildView.SwitchBSpline();

    UpdateMenuItems(ID_APP_BSPLINE, CString("B-Spline:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetBSplineSwitch());
}

void Cjkl6plusApp::OnAppOffsetDraw()
{
    ((CMainFrame*)m_pMainWnd)->m_ChildView.SwitchOffsetDraw();

    UpdateMenuItems(ID_APP_OFFSET_DRAW, CString("Offset Draw:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetOffsetDrawSwitch());
}

void Cjkl6plusApp::OnAppDeleteLast()
{
    ((CMainFrame*)m_pMainWnd)->m_ChildView.DeleteLast();
}

void Cjkl6plusApp::OnAppNew()
{
    if (((CMainFrame*)m_pMainWnd)->m_ChildView.HasCGSChanged())
    {
        if (AfxMessageBox("Do you want to save current change?", MB_YESNO) == IDYES)
        {
            OnAppSave();
        }
    }

    ((CMainFrame*)m_pMainWnd)->m_ChildView.Init();
    ((CMainFrame*)m_pMainWnd)->m_ChildView.ReDrawAll();

    UpdateMenuItems(ID_APP_BEZIER, CString("Bezier:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetBezierSwitch());
    UpdateMenuItems(ID_APP_BSPLINE, CString("B-Spline:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetBSplineSwitch());
    UpdateMenuItems(ID_APP_OFFSET_DRAW, CString("Offset Draw:"), ((CMainFrame*)m_pMainWnd)->m_ChildView.GetOffsetDrawSwitch());
}

void Cjkl6plusApp::OnAppOpen()
{
    if (((CMainFrame*)m_pMainWnd)->m_ChildView.HasCGSChanged())
    {
        if (AfxMessageBox("Do you want to save current change?", MB_YESNO) == IDYES)
        {
            OnAppSave();
        }
    }

    CFileDialog fdlg(TRUE, "jkl6");
    if (fdlg.DoModal() == IDOK)
    {
        ((CMainFrame*)m_pMainWnd)->m_ChildView.LoadCGSFromFile(std::string(fdlg.GetPathName()));
    }
}

void Cjkl6plusApp::OnAppSave()
{
    std::string strFileName = ((CMainFrame*)m_pMainWnd)->m_ChildView.GetFileName();
    if (strFileName.length() >= 4)
    {
        ((CMainFrame*)m_pMainWnd)->m_ChildView.SaveCGSIntoFile(strFileName);
        AfxMessageBox("Saved.");
    }
    else // invalid strFileName.
    {
        OnAppSaveAs();
    }
}

void Cjkl6plusApp::OnAppSaveAs()
{
    CFileDialog fdlg(FALSE, "jkl6");
    if (fdlg.DoModal() == IDOK)
    {
        ((CMainFrame*)m_pMainWnd)->m_ChildView.SaveCGSIntoFile(std::string(fdlg.GetPathName()));
    }
}

void Cjkl6plusApp::OnAppExit()
{
    if (((CMainFrame*)m_pMainWnd)->m_ChildView.HasCGSChanged())
    {
        if (AfxMessageBox("Do you want to save current change?", MB_YESNO) == IDYES)
        {
            OnAppSave();
        }
    }

    CWinApp::OnAppExit();
}
