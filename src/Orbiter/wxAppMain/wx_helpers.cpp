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
    wxString sEvent;
    sEvent.Printf(
        "event{id=%d, pObj=%p, type=%d}",
        event.GetId(),
        pwxObject,
        event.GetEventType()
        );
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
    return wxString::Format(
        "_str_event(%s%s%s)",
        sEvent.c_str(),
        sWindow.c_str(),
        sCommandEvent.c_str()
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
    if (event_type == wxEVTC_DIALOG)
        _WX_LOG_NFO("wx_post_event(%s)", _str_event_dialog(event));
    else if (event_type == wxEVTC_THREAD)
        _WX_LOG_NFO("wx_post_event(%s)", _str_event_thread(event));
    else
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
void wxAppSetReady()
{
    if (! g_bAppInitialized)
    {
        _WX_LOG_NFO();
        g_bAppInitialized = true;
    }
}
bool wxAppIsReady()
{
    return g_bAppInitialized = true;
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
    _COND_RET(pGrid);
    pGrid->ClearGrid();
    pGrid->DeleteCols(0, pGrid->GetNumberCols());
    pGrid->DeleteRows(0, pGrid->GetNumberRows());
}

void wx_Align(wxGrid *pGrid, wxAlignment horiz/*=wxALIGN_CENTRE*/, wxAlignment vert/*=wxALIGN_CENTRE*/)
{
    _COND_RET(pGrid);
    pGrid->SetDefaultCellAlignment(horiz, vert);
}

void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg)
{
    _COND_RET(pGrid);
    pGrid->SetDefaultCellBackgroundColour(bg);
    pGrid->SetDefaultCellTextColour(fg);
}

void wx_Set_Color(wxGrid *pGrid, wxColour bg, wxColour fg, wxColour line)
{
    _COND_RET(pGrid);
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
    int widthCol = widthMax - (WX_GridLineSize * nColumnCount);
    // - WX_ScrollBarSize
    for (int i=0; i<nColumnCount; i++)
    {
        if (i == idxResizableColumn)
            continue;
        widthCol -= pGrid->GetColSize(i);
    }
    pGrid->SetColSize(idxResizableColumn, widthCol);
    pGrid->Refresh();
}
