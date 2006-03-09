//
// Author : C Remus
//
// Changed by : ...
//

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wx_debugging.h"
#endif

#include "wx/wxprec.h"
#ifdef __BORLANDC__
#pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx_debugging.h"
#include "wxevent_thread.h"
#include "wxthread_wrapper.h"
#include "wxthread_bag.h"
#include "wx_win_thread.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"

void _debug_show_dlg()
{
    wxDialog_RoomWizard *pWin = NULL;
    _WX_LOG_DBG("wxDialog_Show");
    wxDialog_Show<wxDialog_RoomWizard>(pWin = wxDialog_CreateUnique<wxDialog_RoomWizard>(SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME,SYMBOL_WXDIALOG_ROOMWIZARD_TITLE));
    _WX_LOG_DBG("wxDialog_Close");
    wxDialog_Close<wxDialog_RoomWizard>(pWin);
    _WX_LOG_DBG("wxDialog_ShowModal");
    wxDialog_ShowModal<wxDialog_RoomWizard>(pWin = wxDialog_CreateUnique<wxDialog_RoomWizard>(SYMBOL_WXDIALOG_ROOMWIZARD_IDNAME,SYMBOL_WXDIALOG_ROOMWIZARD_TITLE));
    wxUnusedVar(pWin);
}

void _debug_thread_block()
{
    _WX_LOG_DBG();
    _debug_show_dlg();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 5; i++ )
    {
        wx_post_event_thread(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_block()", ptr_wxThread_Cmd());
        wx_sleep(1);
    }
    _WX_LOG_DBG(";;");
}

void _debug_thread_nonblock()
{
    _WX_LOG_DBG();
    _debug_show_dlg();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 10; i++ )
    {
        if ( wxIdleThreadShouldStop() )
        {
            _WX_LOG_DBG("Should Stop Now");
            return;
        }
        wx_post_event_thread(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_nonblock()", ptr_wxThread_Cmd());
        wx_sleep(0, 500);
    }
    _WX_LOG_DBG(";;");
}

void _debug_refresh_update()
{
#ifdef USE_DEBUG_CODE
    bool bInThread = (! ::wxIsMainThread());
    bool bOneActive = true;
    do
    {
        if ( wxIdleThreadShouldStop() || (! bOneActive) )
            break;
        bOneActive = false;
        // initialization
        static string aStr[] =
            {
                "str aaaa",
                "str bbbb",
                "str cccc",
            };
        static int n=WXSIZEOF(aStr);
        static int i=0;
        if (i >= n)
            i = 0;
        static int nPercent = i * 10;
        static int nTimeoutSeconds = 5;
        static map<string, bool> mapStrBool;
        mapStrBool["aaaa"] = (i % 2);
        mapStrBool["bbbb"] = (i % 2);
        mapStrBool["cccc"] = (i % 2);
        static map<int,string> mapIntStr;
        mapIntStr[10] = "aaaa";
        mapIntStr[20] = "bbbb";
        mapIntStr[30] = "cccc";
        _WX_LOG_DBG("nPercent=%d, nTimeoutSeconds=%d", nPercent, nTimeoutSeconds);
        // refresh update
        {
            wxDialog_WaitGrid *pwxDialog = (wxDialog_WaitGrid *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITGRID_TITLE);
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitGrid");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], mapStrBool, nPercent);
            }
        }
        {
            wxDialog_WaitList *pwxDialog = (wxDialog_WaitList *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITLIST_TITLE);
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitList");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], nPercent);
            }
        }
        {
            wxDialog_WaitUser *pwxDialog = (wxDialog_WaitUser *)wxWindow::FindWindowByName(SYMBOL_WXDIALOG_WAITUSER_TITLE);
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitUser");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], nTimeoutSeconds, &mapIntStr);
            }
        }
        i++;
        if (bInThread)
        {
            wx_sleep(2);
        }
    } while (bInThread);
#endif // USE_DEBUG_CODE
};
