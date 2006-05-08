//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_helpers.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_helpers.h"
#include "wx_event_dialog.h"
#include "wx_event_thread.h"
#include "wx_thread_wrapper.h"
#include "wx_thread_bag.h"
#include "wx_dialog_types.h"

#include "wx/regex.h"
#include "wx/grid.h"

wxString NoEsc(const wxString &str)
{
    wxString s = str;
    wxRegEx reNoEsc("\033[[0-9;]*m");
    _COND_RET(reNoEsc.IsValid(), str);
    reNoEsc.ReplaceAll(&s, "");
    return s;
}

wxString Str(int val)
{
    return wxString::Format("%d", val);
}

const char * _str_enum(wxSemaError value)
{
    switch (value)
    {
        CASE_const_ret_str(wxSEMA_NO_ERROR);
        CASE_const_ret_str(wxSEMA_INVALID);
        CASE_const_ret_str(wxSEMA_BUSY);
        CASE_const_ret_str(wxSEMA_TIMEOUT);
        CASE_const_ret_str(wxSEMA_OVERFLOW);
        CASE_const_ret_str(wxSEMA_MISC_ERROR);
        default:
            _WX_LOG_ERR("unknown value %d", value);
            break;
    }
    return wxString::Format("?%d?", value);
}

const char * _str_event(wxEvent &event)
{
    wxObject *pwxObject = event.GetEventObject();
    WXTYPE nType = event.GetEventType();
    int nId = event.GetId();
    bool bSkipped = event.GetSkipped();
    wxString sEvent;
    if (nType == wxEVTC_DIALOG)
    {
        sEvent.Printf(
            "event{id=%d[%s], type=%d[wxEVTC_DIALOG], pObj=%p, bSkipped=%d}",
            nId, _str_enum((E_ACTION_TYPE)nId),
            nType,
            pwxObject,
            bSkipped
            );
    }
    else if (nType == wxEVTC_THREAD)
    {
        sEvent.Printf(
            "event{id=%d[%s], type=%d[wxEVTC_THREAD], pObj=%p, bSkipped=%d}",
            nId, _str_enum((wxThread_Cmd::E_STATUS)nId),
            nType,
            pwxObject,
            bSkipped
            );
    }
    else
    {
        sEvent.Printf(
            "event{id=%d, type=%d, pObj=%p, bSkipped=%d}",
            nId,
            nType,
            pwxObject,
            bSkipped
            );
    }
    wxWindow *pwxWindow = wxDynamicCast(pwxObject, wxWindow);
    wxString sWindow;
    if (pwxWindow)
    {
        sWindow.Printf(
            ", pWin=%p{label='%s', ID=%d}",
            pwxWindow,
            pwxWindow->GetLabel().c_str(),
            pwxWindow->GetId()
            );
    }
    wxString sCommandEvent;
    if (event.IsCommandEvent())
    {
        wxCommandEvent &cmd_event = (wxCommandEvent &)event;
        sCommandEvent.Printf(
            ", cmd{str='%s', pData=%p, int=%ld}",
            cmd_event.GetString().c_str(),
            cmd_event.GetClientData(),
            cmd_event.GetInt()
            );
    }
    wxString sThread;
    if (! ::wxIsMainThread())
        sThread.Printf(", pwxThread=%p", wxThread::This());
    return wxString::Format(
        "%s%s%s%s",
        sEvent.c_str(),
        sCommandEvent.c_str(),
        sWindow.c_str(),
        sThread.c_str()
        );
}

void wx_sleep(unsigned long int n_seconds, unsigned long int n_milliseconds/*=0*/)
{
    if (::wxIsMainThread())
        ::wxMilliSleep(n_seconds * 1000 + n_milliseconds);
    else
        wxThread::Sleep(n_seconds * 1000 + n_milliseconds);
}

void wx_post_event(wxEvtHandler *pDestEvtHandler, WXTYPE event_type, int event_id, const char *sInfo/*=""*/, void *pData/*=NULL*/, int nInt/*=0*/)
{
    wxCommandEvent event( event_type, event_id );
    event.SetString(sInfo);
    event.SetClientData(pData);
    event.SetInt(nInt);
    _WX_LOG_NFO("wx_post_event(%s)", _str_event(event));
    ::wxPostEvent( pDestEvtHandler, event );
}

wxSemaError wx_semaphore_post(wxSemaphore &rSemaphore)
{
    wxSemaError semCode = rSemaphore.Post();
    if (semCode != wxSEMA_NO_ERROR)
    {
        _WX_LOG_ERR("Semaphore error %s", _str_enum(semCode));
    }
    return semCode;
}

wxSemaError wx_semaphore_wait(wxSemaphore &rSemaphore)
{
    // instead of : wxSemaError semCode = rSemaphore.Wait();
    wxSemaError semCode = wxSEMA_NO_ERROR;
    do
    {
        semCode = rSemaphore.WaitTimeout(WAIT_SEMAPHORE_MSEC);
        if (wxIdleThreadShouldStop())
        {
            _WX_LOG_WRN("Stop in semaphore wait");
            //rSemaphore.Post();
            break;
        }
    } while (semCode == wxSEMA_TIMEOUT);
    if (semCode != wxSEMA_NO_ERROR)
    {
        _WX_LOG_ERR("Semaphore error %s", _str_enum(semCode));
    }
    return semCode;
}

wxCondLocker::wxCondLocker(bool bCondition, type_ref_void_fn_void fn_launch_enter, type_ref_void_fn_void fn_launch_leave)
    : v_bLocked(bCondition)
, v_fn_launch_enter(fn_launch_enter)
, v_fn_launch_leave(fn_launch_leave)
{
    if (v_bLocked)
    {
        _WX_LOG_NFO("Locking");
        v_fn_launch_enter();
    }
}

wxCondLocker::~wxCondLocker()
{
    if (v_bLocked)
    {
        _WX_LOG_NFO("Unlocking");
        v_fn_launch_leave();
    }
}

bool g_bAppInitialized = false;
void App_SetReady()
{
    if (! g_bAppInitialized)
    {
        _WX_LOG_NFO();
        g_bAppInitialized = true;
    }
}
bool App_IsReady()
{
    return g_bAppInitialized = true;
}

int g_nExitCode = EXIT_SUCCESS;
bool g_bShouldExit = false;

int App_GetExitCode()
{
    //_WX_LOG_NFO("ExitCode == %d", g_nExitCode);
    return g_nExitCode;
}

void App_SetExitCode(int nExitCode)
{
    _WX_LOG_NFO("ExitCode: %d -> %d", g_nExitCode, nExitCode);
    g_nExitCode = nExitCode;
}

bool App_ShouldExit()
{
    return g_bShouldExit;
}

void App_SetShouldExit(bool bShouldExit)
{
    if (bShouldExit)
        _WX_LOG_NFO("app should exit soon");
    else
        _WX_LOG_NFO("app exit signal cleared");
    g_bShouldExit = bShouldExit;
}

bool wxIdleThreadShouldStop()
{
    if (::wxIsMainThread())
    {
        return false;
    }
    wxThread_Cmd *pwxThread_Cmd = ptr_wxThread_Cmd();
    if (pwxThread_Cmd == NULL)
    {
        return false;
    }
    if (pwxThread_Cmd->IsCancelled())
    {
        return true;
    }
    if (pwxThread_Cmd->TestDestroy())
    {
        return true;
    }
    return false;
}

void wx_Clear(wxGrid *pGrid)
{
    _COND_RET(pGrid != NULL);
    pGrid->ClearGrid();
    pGrid->DeleteCols(0, pGrid->GetNumberCols());
    pGrid->DeleteRows(0, pGrid->GetNumberRows());
}

void wx_Align(wxGrid *pGrid, wxAlignment horiz/*=wxALIGN_CENTRE*/, wxAlignment vert/*=wxALIGN_CENTRE*/)
{
    _COND_RET(pGrid != NULL);
    pGrid->SetDefaultCellAlignment(horiz, vert);
}

void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg)
{
    _COND_RET(pGrid != NULL);
    pGrid->SetDefaultCellBackgroundColour(bg);
    pGrid->SetDefaultCellTextColour(fg);
}

void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg, wxColour line)
{
    _COND_RET(pGrid != NULL);
    wx_Set_Color(pGrid, bg, fg);
    pGrid->SetGridLineColour(line);
}

int wx_GetTextExtent(wxWindow *pWindow, const wxArrayString &aStrings, int *pReturnHeight/*=NULL*/)
{
    int wMax = 0;
    int hMax = 0;
    int wCrt = 0;
    int hCrt = 0;
    size_t nCount = aStrings.GetCount();
    for (size_t i=0; i<nCount; i++)
    {
        pWindow->GetTextExtent(aStrings[i], &wCrt, &hCrt);
        wMax = max(wMax, wCrt);
        hMax = max(hMax, hCrt);
    }
    if (pReturnHeight)
        *pReturnHeight = hMax;
    return wMax;
}

void wx_Grid_Resize_Column(wxGrid *pGrid, int idxResizableColumn)
{
    int nColumnCount = pGrid->GetNumberCols();
    int widthMax = pGrid->GetClientSize().GetWidth();
    // will calculate the resizable column width
    int widthResizableColumn = widthMax - WX_GridLineSize; // substract once, will suffice
    for (int i=0; i<nColumnCount; i++)
    {
        if (i == idxResizableColumn)
            continue;
        widthResizableColumn -= pGrid->GetColSize(i);
    }
    // Sometimes wxGrid has trouble setting the scrollbars correctly
    // due to rounding errors: setting this to 1 can help
    pGrid->SetScrollLineX(1);
    pGrid->SetScrollLineY(1);
    pGrid->SetColSize(idxResizableColumn, widthResizableColumn);
    pGrid->Refresh();
}

void wx_CopyWindowAttributes(wxWindow *pWinDest, wxWindow *pWinSrc)
{
    _COND_RET(pWinDest);
    _COND_RET(pWinSrc);
    pWinDest->SetBackgroundColour(pWinSrc->GetBackgroundColour());
    pWinDest->SetForegroundColour(pWinSrc->GetForegroundColour());
    pWinDest->SetFont(pWinSrc->GetFont());
}

void wx_WindowStyle_Add(wxWindow *pWindow, long style)
{
    _COND_RET(pWindow);
    pWindow->SetWindowStyle(pWindow->GetWindowStyle() | style);
}

void wx_WindowStyle_Del(wxWindow *pWindow, long style)
{
    _COND_RET(pWindow);
    pWindow->SetWindowStyle(pWindow->GetWindowStyle() & !style);
}

wxString StrTimeHMS(int seconds)
{
    _COND_RET(seconds >= 0, "");
    int sec = seconds % 60;
    int hour = (seconds-sec) / 60;
    int min = hour % 60;
    hour = (hour-min) / 60;
    if (hour)
        return wxString::Format("%02d:%02d:%02d", hour, min, sec);
    else
        return wxString::Format("%02d:%02d", min, sec);
}
