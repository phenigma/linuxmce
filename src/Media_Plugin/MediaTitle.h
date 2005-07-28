#ifndef MediaTitle_h
#define MediaTitle_h

#include "MediaSection.h"

class MediaTitle
{
public:
	deque<MediaSection *> m_dequeMediaSection;
	MediaTitle() {
	}

	~MediaTitle() {
		for(size_t s=0;s<m_dequeMediaSection.size();++s)
			if( m_dequeMediaSection[s] )
				delete m_dequeMediaSection[s];
	}

	map< int,int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    int m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,int>::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : (*it).second; }
};

#endif
