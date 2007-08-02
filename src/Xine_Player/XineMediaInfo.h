#ifndef __XINE_MEDIA_INFO_H__
#define __XINE_MEDIA_INFO_H__

#include <string>
using namespace std;

// public class - nothing to hide

class XineMediaInfo 
{
public:
    int m_iSpeed;
    int m_iPositionInMilliseconds;
    int m_iTotalLengthInMilliseconds;
    int m_iStreamID;
    int m_iTitle;
    int m_iChapter;
    string m_sFileName;
    int m_iPK_Disc;
    
    
public:
    XineMediaInfo();
    
    string ToString();
    bool FromString(string sSource);

    string FormatCurrentTime(bool bShowMilliseconds=true);
    string FormatTotalTime(bool bShowMilliseconds=true);

    static string FormatTime(int iTime, bool bShowMilliseconds);
};

#endif
