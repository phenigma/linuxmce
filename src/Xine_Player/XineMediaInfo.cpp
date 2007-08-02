#include "PlutoUtils/StringUtils.h"

#include "XineMediaInfo.h"

string XineMediaInfo::ToString()
{
    string sResult;
    sResult += StringUtils::itos(m_iSpeed);
    sResult += ",";
    sResult += FormatCurrentTime();
    sResult += ",";
    sResult += FormatTotalTime();
    sResult += ",";
    sResult += StringUtils::itos(m_iStreamID);
    sResult += ",";
    sResult += StringUtils::itos(m_iTitle);
    sResult += ",";
    sResult += StringUtils::itos(m_iChapter);
    sResult += ",";
    sResult += StringUtils::itos(m_iPK_Disc);
    sResult += ",";
    sResult += m_sFileName;

    return sResult;
}

bool XineMediaInfo::FromString(string sSource)
{
    return false;
}

string XineMediaInfo::FormatCurrentTime(bool bShowMilliseconds/*=true*/)
{
    return FormatTime(m_iPositionInMilliseconds, bShowMilliseconds);
}

string XineMediaInfo::FormatTotalTime(bool bShowMilliseconds/*=true*/)
{
    return FormatTime(m_iTotalLengthInMilliseconds, bShowMilliseconds);
}

XineMediaInfo::XineMediaInfo()
{
    m_iSpeed = 0;
    m_iPositionInMilliseconds = 0;
    m_iTotalLengthInMilliseconds = 0;
    m_iStreamID = 0;
    m_iTitle = -1;
    m_iChapter = -1;
    m_iPK_Disc = -1;
}

string XineMediaInfo::FormatTime(int iTime, bool bShowMilliseconds)
{
    const int iMaxSize = 32;
    char buf[iMaxSize];
    
    if (bShowMilliseconds)
        snprintf(buf, iMaxSize, "%02i:%02i:%02i.%03i",  iTime / 1000 / 3600, iTime / 1000 % 3600 / 60, iTime / 1000 % 3600 % 60, iTime % 1000);
    else
        snprintf(buf, iMaxSize, "%02i:%02i:%02i",  iTime / 1000 / 3600, iTime / 1000 % 3600 / 60, iTime / 1000 % 3600 % 60);
    
    return buf;
}