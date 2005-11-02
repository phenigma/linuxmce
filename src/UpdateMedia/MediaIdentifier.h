#ifndef __MEDIA_IDENTIFIER_H__
#define __MEDIA_IDENTIFIER_H_

#include <string>
#include <map>
using namespace std;
//-----------------------------------------------------------------------------------------------------
//  Media Identifier class (no instance can/must be made)
//  usage: MediaIdentifier::GetFileMediaType(string)
//-----------------------------------------------------------------------------------------------------
class MediaIdentifier
{
private:
	MediaIdentifier()  {} 
	~MediaIdentifier() {}
	
	static void Init();
	static map<string, int> m_mapMediaIdentifier;
	
public:
	static int GetFileMediaType(string sFileName);
};
//-----------------------------------------------------------------------------------------------------
#endif
