#ifndef MYTHMAINWINDOWRESIZABLE_H
#define MYTHMAINWINDOWRESIZABLE_H

#include <libmyth/mythdialogs.h>

class MythMainWindowResizable: public MythMainWindow
{
public:
    MythMainWindowResizable(QWidget *parent = 0, const char *name = 0, bool modal = FALSE);

protected:
//     virtual void resizeEvent ( QResizeEvent * );
    virtual bool event ( QEvent * e );
    virtual bool x11Event ( XEvent * event);
};

#endif // MYTHMAINWINDOWRESIZABLE_H
