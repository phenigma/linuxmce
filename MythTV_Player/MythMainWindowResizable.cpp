/*
    MythMainWindowResizable

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/


#include "MythMainWindowResizable.h"
#include "DCE/Logger.h"

#include <qlayout.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace DCE;

MythMainWindowResizable::MythMainWindowResizable(QWidget *parent, const char *name, bool modal)
    : MythMainWindow(parent, name, modal), m_pNuppelVideoPlayer(NULL) //m_pLayout(NULL)
{
    m_pLayout = new QGridLayout(this, 1, 1, 0, 0, "Layout");

//     new QFrame(this, "tv playback");

    g_pPlutoLogger->Write(LV_STATUS, "constructing");
}

void MythMainWindowResizable::resizeEvent ( QResizeEvent * event)
{
    QWidget::resizeEvent(event);

//     g_pPlutoLogger->Write(LV_STATUS,
//             "Resize event: %dx%d -> %dx%d",
//             event->oldSize().width(),
//             event->oldSize().height(),
//             event->size().width(),
//             event->size().height());

    QWidget *playbackWidget = getTvPlaybackWindow();

    if ( playbackWidget )
    {
        playbackWidget->resize(event->size());

        if ( m_pNuppelVideoPlayer )
        {
            m_pNuppelVideoPlayer->getVideoOutput()->StopEmbedding();
            m_pNuppelVideoPlayer->getVideoOutput()->EmbedInWidget(playbackWidget->winId(),
                    playbackWidget->x(), playbackWidget->y(),
                    event->size().width(), event->size().height());

/*            g_pPlutoLogger->Write(LV_STATUS, "Called for this %d, %dx%d %dx%d",
                    playbackWidget->winId(),
                    playbackWidget->x() + 10, playbackWidget->y() + 10,
                    event->size().width() - 20, event->size().height() - 20);*/
        }
        else
        {
//             g_pPlutoLogger->Write(LV_WARNING, "No Nuppel player ~!?");
        }
    }
    else
    {
//         g_pPlutoLogger->Write(LV_WARNING, "No playback widget !?");
    }
}

bool MythMainWindowResizable::event ( QEvent * e )
{
    bool result = QWidget::event(e);

//     g_pPlutoLogger->Write(LV_STATUS, "Got event of type: %d", e->type());

    if ( e->type() == QEvent::ChildInserted || e->type() == QEvent::ChildRemoved )
    {
        QChildEvent *childEvent = static_cast<QChildEvent*>(e);

        if ( childEvent->child()->inherits( "QWidget" ) )
        {
            QWidget *pWidget = static_cast<QWidget*>(childEvent->child());

            if ( childEvent->removed() )
                m_pLayout->remove(pWidget);
            else
            {
                pWidget->setMinimumSize(0, 0);
                m_pLayout->addWidget(pWidget, 0, 0);
            }
        }
    }

    QWidget *pInnerWidget = getTvPlaybackWindow();

    if  ( pInnerWidget )
    {
/*        g_pPlutoLogger->Write(LV_STATUS, "Inner child (%d), [%dx%d -> %dx%d]", pInnerWidget->winId(),
            pInnerWidget->x(), pInnerWidget->y(),
            pInnerWidget->width(), pInnerWidget->height());*/
    }
    else
    {
//         g_pPlutoLogger->Write(LV_STATUS, "No inner child ?!");
    }

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

/*    switch ( (*event).type )
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
            break;void setEventFilterOnChild(QObject *eventFilter);
    }
*/
    return result;
}

QWidget *MythMainWindowResizable::getTvPlaybackWindow()
{
    return (QWidget*)child("video playback window");
}

void MythMainWindowResizable::setNuppelPlayerToResize(NuppelVideoPlayer *pNuppelVideoPlayer)
{
    m_pNuppelVideoPlayer = pNuppelVideoPlayer;
}

