#ifndef CommonFunctions_h
#define CommonFunctions_h

#include <string>
#include <map>
using namespace std;

string int2string(int i);
string getTableFromForeignKey(string field,map<string,class TableInfo_Generator *> *mapTableNames);

#endif
