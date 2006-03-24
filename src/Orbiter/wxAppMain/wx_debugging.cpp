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
#include "wx_event_thread.h"
#include "wx_thread_wrapper.h"
#include "wx_thread_bag.h"
#include "wxdialog_roomwizard.h"
#include "wxdialog_waitgrid.h"
#include "wxdialog_waitlist.h"
#include "wxdialog_waituser.h"
#include "wx_dialog_safe.h"

void _debug_show_dlg()
{
#ifdef USE_DEBUG_CODE
    wxDialog_RoomWizard *pWin = NULL;
    _WX_LOG_DBG("Safe_Show");
    pWin = Safe_CreateUnique<wxDialog_RoomWizard>();
    Safe_Show<wxDialog_RoomWizard>(pWin);
    _WX_LOG_DBG("Safe_Close");
    Safe_Close<wxDialog_RoomWizard>(pWin);
    _WX_LOG_DBG("Safe_Show Again");
    pWin = Safe_CreateUnique<wxDialog_RoomWizard>();
    Safe_Show<wxDialog_RoomWizard>(pWin);
    _WX_LOG_DBG("Safe_ShowModal");
    pWin = Safe_CreateUnique<wxDialog_RoomWizard>();
    Safe_ShowModal<wxDialog_RoomWizard>(pWin);
#endif // USE_DEBUG_CODE
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
                "str dddd",
            };
        static const int n=WXSIZEOF(aStr);
        static int iLoop=0;
        int i = iLoop % n;
        int nPercent = i * 100 / n;
        int nTimeoutSeconds = 5;
        static map<string, bool> mapStrBool;
        mapStrBool["aaaa"] = (i % 2);
        mapStrBool["bbbb"] = (i % 3);
        mapStrBool["cccc"] = (i % 4);
        mapStrBool["dddd"] = (i % 5);
        static map<int,string> mapIntStr;
        mapIntStr[10] = "aaaa";
        mapIntStr[20] = "bbbb";
        mapIntStr[30] = "cccc";
        mapIntStr[40] = "dddd";
        _WX_LOG_DBG("iLoop=%d, nPercent=%d, nTimeoutSeconds=%d", iLoop, nPercent, nTimeoutSeconds);
        // refresh update
        {
            wxDialog_WaitGrid *pwxDialog = ptr_wxDialogByType<wxDialog_WaitGrid>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitGrid");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], mapStrBool, nPercent);
            }
        }
        {
            wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitList");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], nPercent);
            }
        }
        {
            wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitUser");
                bOneActive = true;
                pwxDialog->NewDataRefresh(aStr[i], nTimeoutSeconds, &mapIntStr);
            }
        }
        iLoop++;
        if (bInThread)
        {
            wx_sleep(2);
        }
    } while (bInThread);
#endif // USE_DEBUG_CODE
};
