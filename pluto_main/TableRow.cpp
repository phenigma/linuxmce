// If using the thread logger, these generated classes create lots of activity
#ifdef NO_SQL_THREAD_LOG
#undef THREAD_LOG
#endif

#ifdef WIN32
#include <winsock.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <mysql.h>

using namespace std;
#include "PlutoUtils/StringUtils.h"
#include "TableRow.h"

SingleLongKey::SingleLongKey(long int in_pk)
{
	pk = in_pk;
}
bool SingleLongKey_Less::operator()(const SingleLongKey &key1, const SingleLongKey &key2) const
{
if (key1.pk!=key2.pk)
return key1.pk<key2.pk;
else
return false;	
}

DoubleLongKey::DoubleLongKey(long int in_pk1,long int in_pk2)
{
	pk1 = in_pk1;
	pk2 = in_pk2;
}
bool DoubleLongKey_Less::operator()(const DoubleLongKey &key1, const DoubleLongKey &key2) const
{
	if (key1.pk1!=key2.pk1)
		return key1.pk1<key2.pk1;
	else
		if (key1.pk2!=key2.pk2)
			return key1.pk2<key2.pk2;
		else
			return false;	
}


TripleLongKey::TripleLongKey(long int in_pk1,long int in_pk2,long int in_pk3)
{
	pk1 = in_pk1;
	pk2 = in_pk2;
	pk3 = in_pk3;
}
bool TripleLongKey_Less::operator()(const TripleLongKey &key1, const TripleLongKey &key2) const
{
	if (key1.pk1!=key2.pk1)
		return key1.pk1<key2.pk1;
	else
		if (key1.pk2!=key2.pk2)
			return key1.pk2<key2.pk2;
		else
			if (key1.pk3!=key2.pk3)
				return key1.pk3<key2.pk3;
			else
				return false;	
}


SingleStringKey::SingleStringKey(string in_pk)
{
	pk = in_pk;
}
bool SingleStringKey_Less::operator()(const SingleStringKey &key1, const SingleStringKey &key2) const
{
if (key1.pk!=key2.pk)
return key1.pk<key2.pk;
else
return false;	
}
