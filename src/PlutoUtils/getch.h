#ifndef getch_h
#define getch_h
#ifndef WIN32
// Note: this conflicts with getch from nCurses (but we don't use nCurses anyway)
int getch(int iTimeoutInSeconds=0);
int getche(int iTimeoutInSeconds=0);
#endif
#endif
