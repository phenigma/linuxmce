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
#include "wx_safe_dialog.h"

wxArray_RoomItems _g_aRoomItems;

static string _g_aStr[] =
{
    "str aaaa",
    "str bbbb",
    "str cccc",
    "str dddd",
};
const int _g_nStr=WXSIZEOF(_g_aStr);
int _g_nPercent = 0;
int _g_nTimeoutSeconds = 5;
static map<string, bool> _g_mapStrBool;
static map<int,string> _g_mapIntStr;

void _debug_init()
{
    for (int idx=0; idx<_g_nStr; idx++)
    {
        _g_mapIntStr[(idx+1)*10] = _g_aStr[idx];
        _g_mapStrBool[_g_aStr[idx]] = (idx % (2+idx));
    }
    _g_aRoomItems.Add( RoomItem("Living Room", 1, 0) );
    _g_aRoomItems.Add( RoomItem("Bathroom", 2, 0) );
    _g_aRoomItems.Add( RoomItem("Bedroom", 4, 0) );
};

void * _debug_RoomWizard()
{
    return &_g_aRoomItems;
};

template <class wxClassName>
void _debug_show_dlg_safe(void *pData=NULL)
{
#ifdef USE_DEBUG_CODE
    wxClassName *pWin = NULL;
    _WX_LOG_DBG("Safe_Show");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Close");
    Safe_Close<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_Show Again");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_Show<wxClassName>(pWin);
    _WX_LOG_DBG("Safe_ShowModal");
    pWin = Safe_CreateUnique<wxClassName>(pData);
    Safe_ShowModal<wxClassName>(pWin);
#endif // USE_DEBUG_CODE
}

template <class wxClassName>
void _debug_show_dlg_pdac(void *pData=NULL)
{
#ifdef USE_DEBUG_CODE
    Data_Holder_Dialog data_holder_dialog(Get_Type<wxClassName>(), NULL, pData);
    _WX_LOG_DBG("Safe_Show");
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Create_Unique, &data_holder_dialog);
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Show, &data_holder_dialog);
    _WX_LOG_DBG("Safe_Close");
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Close, &data_holder_dialog);
    _WX_LOG_DBG("Safe_Show Again");
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Create_Unique, &data_holder_dialog);
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Show, &data_holder_dialog);
    _WX_LOG_DBG("Safe_ShowModal");
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_Create_Unique, &data_holder_dialog);
    Process_Dialog_Action(Get_Type<wxClassName>(), E_Action_ShowModal, &data_holder_dialog);
#endif // USE_DEBUG_CODE
}

void _debug_show_dlg_safe_all()
{
    _debug_show_dlg_safe<wxDialog_RoomWizard>(_debug_RoomWizard());
    _debug_show_dlg_safe<wxDialog_WaitGrid>();
    _debug_show_dlg_safe<wxDialog_WaitList>();
    _debug_show_dlg_safe<wxDialog_WaitUser>();
}

void _debug_show_dlg_pdac_all()
{
    _debug_show_dlg_pdac<wxDialog_RoomWizard>(_debug_RoomWizard());
    _debug_show_dlg_pdac<wxDialog_WaitGrid>();
    _debug_show_dlg_pdac<wxDialog_WaitList>();
    _debug_show_dlg_pdac<wxDialog_WaitUser>();
}

void _debug_show_dlg_all()
{
    _debug_show_dlg_safe_all();
    //_debug_show_dlg_pdac_all();
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
        static int iLoop=0;
        int i = iLoop % _g_nStr;
        _g_nPercent = i * 100 / _g_nStr;
        for (int idx=0; idx<_g_nStr; idx++)
        {
            _g_mapStrBool[_g_aStr[idx]] = (i % (2+idx));
        }
        _WX_LOG_DBG("iLoop=%d, nPercent=%d, nTimeoutSeconds=%d", iLoop, _g_nPercent, _g_nTimeoutSeconds);
        // refresh update
        {
            wxDialog_WaitGrid *pwxDialog = ptr_wxDialogByType<wxDialog_WaitGrid>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitGrid");
                bOneActive = true;
                wxDialog_WaitGrid::Data_Refresh data_refresh = { _g_aStr[i], _g_mapStrBool, _g_nPercent };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
            }
        }
        {
            wxDialog_WaitList *pwxDialog = ptr_wxDialogByType<wxDialog_WaitList>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitList");
                bOneActive = true;
                wxDialog_WaitList::Data_Refresh data_refresh = { _g_aStr[i], _g_nPercent };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
            }
        }
        {
            wxDialog_WaitUser *pwxDialog = ptr_wxDialogByType<wxDialog_WaitUser>();
            if (pwxDialog)
            {
                _WX_LOG_DBG("wxDialog_WaitUser");
                bOneActive = true;
                wxDialog_WaitUser::Data_Refresh data_refresh = { _g_aStr[i], _g_nTimeoutSeconds, _g_mapIntStr };
                Safe_Gui_Refresh(pwxDialog, &data_refresh);
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

void _debug_thread_block()
{
    _WX_LOG_DBG();
    _debug_show_dlg_all();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 5; i++ )
    {
        wx_post_event(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_block()", ptr_wxThread_Cmd());
        wx_sleep(1);
    }
    _WX_LOG_DBG(";;");
}

void _debug_thread_nonblock()
{
    _WX_LOG_DBG();
    _debug_show_dlg_all();
    _WX_LOG_DBG("Loop");
    for ( int i = 0; i < 10; i++ )
    {
        if ( wxIdleThreadShouldStop() )
        {
            _WX_LOG_DBG("Should Stop Now");
            return;
        }
        wx_post_event(g_pwxThread_Bag, wxEVTC_THREAD, wxThread_Cmd::E_EventIdle, "_debug_thread_nonblock()", ptr_wxThread_Cmd());
        wx_sleep(0, 500);
    }
    _WX_LOG_DBG(";;");
}
