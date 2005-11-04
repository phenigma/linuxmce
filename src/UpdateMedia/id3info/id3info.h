#ifndef __ID3INFO_H_
#define __ID3INFO_H_

#include <map>
#include <string>
using namespace std;

void GetId3Info(string sFilename, map<int,string>& mapAttributes);
void SetId3Info(string sFilename, const map<int,string>& mapAttributes);

#endif
