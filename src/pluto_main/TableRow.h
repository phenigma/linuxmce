#ifndef __TableRow_H_
#define __TableRow_H_

struct SingleLongKey
{
	long int pk;
	SingleLongKey(long int in);
};

struct SingleLongKey_Less
{			
	bool operator()(const SingleLongKey &key1, const SingleLongKey &key2) const;
};

struct DoubleLongKey
{
	long int pk1,pk2;
	DoubleLongKey(long int in1,long int in2);
};

struct DoubleLongKey_Less
{			
	bool operator()(const DoubleLongKey &key1, const DoubleLongKey &key2) const;
};

struct TripleLongKey
{
	long int pk1,pk2,pk3;
	TripleLongKey(long int in1,long int in2,long int in3);
};

struct TripleLongKey_Less
{			
	bool operator()(const TripleLongKey &key1, const TripleLongKey &key2) const;
};

struct SingleStringKey
{
	string pk;
	SingleStringKey(string in);
};

struct SingleStringKey_Less
{			
	bool operator()(const SingleStringKey &key1, const SingleStringKey &key2) const;
};

class TableRow
{
protected:
	bool is_deleted;
	bool is_added;
	bool is_modified;
public:
	bool is_modified_get() { return is_modified; }
	bool is_added_get() { return is_added; }
	bool is_deleted_get() { return is_deleted; }
};

class TableBase
{
public:
	vector<class TableRow*> addedRows;
	vector<class TableRow*> deleted_addedRows;	
};

class SingleLongKeyBase
{
public:
	map<SingleLongKey, class TableRow*, SingleLongKey_Less> cachedRows;
	map<SingleLongKey, class TableRow*, SingleLongKey_Less> deleted_cachedRows;
};
class DoubleLongKeyBase
{
public:
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less> cachedRows;
	map<DoubleLongKey, class TableRow*, DoubleLongKey_Less> deleted_cachedRows;
};
class TripleLongKeyBase
{
public:
	map<TripleLongKey, class TableRow*, TripleLongKey_Less> cachedRows;
	map<TripleLongKey, class TableRow*, TripleLongKey_Less> deleted_cachedRows;
};
class SingleStringKeyBase
{
public:
	map<SingleStringKey, class TableRow*, SingleStringKey_Less> cachedRows;
	map<SingleStringKey, class TableRow*, SingleStringKey_Less> deleted_cachedRows;
};

#endif
