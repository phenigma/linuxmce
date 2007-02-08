#ifndef MediaSection_h
#define MediaSection_h

#include <list>

typedef list<int> list_int;

class MediaSection
{
public:

	MediaSection() {
	}

	~MediaSection() {
	}

	map< int,list_int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    list_int *m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,list_int >::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : &((*it).second); }
};

#endif
