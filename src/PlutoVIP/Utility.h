/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
// Utility.h: interface for the CUtility class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __UTILITY_H__
#define __UTILITY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CUtility  
{
public:
	void SlideWindow (HWND hWnd, LPRECT prc);
	CUtility();
	virtual ~CUtility();

private:
	int m_iSlideHide;
	int m_iSlideShow;
};

#endif // !defined(AFX_UTILITY_H__AB5CEE03_A955_11D1_B258_006097960BB7__INCLUDED_)
