#ifndef MYTHMAINWINDOWRESIZABLE_H
#define MYTHMAINWINDOWRESIZABLE_H

#include <libmyth/mythdialogs.h>
#include <libmythtv/NuppelVideoPlayer.h>

#include <qlayout.h>



class MythMainWindowResizable: public MythMainWindow
{
    QGridLayout         *m_pLayout;
    NuppelVideoPlayer   *m_pNuppelVideoPlayer;

public:
    MythMainWindowResizable(QWidget *parent = 0, const char *name = 0, bool modal = FALSE);

    void setNuppelPlayerToResize(NuppelVideoPlayer *pNuppelVideoPlayer);

protected:
    virtual void resizeEvent ( QResizeEvent * );
    virtual bool event ( QEvent * e );
    virtual bool x11Event ( XEvent * event);

    QWidget *getTvPlaybackWindow();
};

#endif // MYTHMAINWINDOWRESIZABLE_H
