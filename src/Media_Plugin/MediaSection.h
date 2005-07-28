#ifndef MediaSection_h
#define MediaSection_h

class MediaSection
{
public:

	MediaSection() {
	}

	~MediaSection() {
	}

	map< int,int > m_mapPK_Attribute;  /** An external media identification script may set attributes here, PK_AttributeType=PK_Attribute */
    int m_mapPK_Attribute_Find(int PK_AttributeType) { map<int,int>::iterator it = m_mapPK_Attribute.find(PK_AttributeType); return it==m_mapPK_Attribute.end() ? NULL : (*it).second; }
};

#endif
