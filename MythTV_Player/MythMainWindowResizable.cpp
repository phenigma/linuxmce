#include "MythMainWindowResizable.h"
#include "DCE/Logger.h"

#include <qlayout.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace DCE;

MythMainWindowResizable::MythMainWindowResizable(QWidget *parent, const char *name, bool modal)
    : MythMainWindow(parent, name, modal)
{
    g_pPlutoLogger->Write(LV_STATUS, "constructing");
}

/*oid MythMainWindowResizable::resizeEvent ( QResizeEvent * event)
{
    g_pPlutoLogger->Write(LV_STATUS,
            "Resize event: %dx%d -> %dx%d",
            event->oldSize().width(),
            event->oldSize().height(),
            event->size().width(),
            event->size().height());
}*/

bool MythMainWindowResizable::event ( QEvent * e )
{
    bool result = QWidget::event(e);

    g_pPlutoLogger->Write(LV_STATUS, "Got event of type: %d", e->type());

/*    if ( e->type() == QEvent::Move )
    {
        QMoveEvent * moveEvent = static_cast<QMoveEvent*>(e);
        g_pPlutoLogger->Write(LV_STATUS,
            "Move event: %dx%d -> %dx%d",
                moveEvent->oldPos().x(),
                moveEvent->oldPos().y(),
                moveEvent->pos().x(),
                moveEvent->pos().y());
    }
//     if ( e->type() == QEvent::LayoutHint )
//     {
        if ( layout() == NULL )
            g_pPlutoLogger->Write(LV_STATUS, "Null layout?!?!");
        else
            g_pPlutoLogger->Write(LV_STATUS, "Resize mode: %d", layout()->resizeMode());
//     }

    g_pPlutoLogger->Write(LV_STATUS, "Internal Rect %dx%d -> %dx%d", rect().x(), rect().y(), rect().width(), rect().height());
    g_pPlutoLogger->Write(LV_STATUS, "Window state: %d %d %d %d", isMinimized(), isMaximized(), isVisible(), isFullScreen());*/
    return result;
}

bool MythMainWindowResizable::x11Event ( XEvent * event)
{
    bool result = QWidget::x11Event(event);

    switch ( (*event).type )
    {
        case ConfigureRequest:
            g_pPlutoLogger->Write(LV_STATUS, "Configure Request %dx%d -> %dx%d",
                            event->xconfigurerequest.x, event->xconfigurerequest.y,
                            event->xconfigurerequest.width, event->xconfigurerequest.height);
            break;
        case ConfigureNotify:
            g_pPlutoLogger->Write(LV_STATUS, "Configure Notify %dx%d -> %dx%d",
                            event->xconfigure.x, event->xconfigure.y,
                            event->xconfigure.width, event->xconfigure.height);
            break;
    }

    return result;
}

