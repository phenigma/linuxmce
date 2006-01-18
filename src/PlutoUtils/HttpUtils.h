#include <map>
#include <string>
#include <vector>

using namespace std;

string HttpPost(string sURL, const vector<string>& vectHeaders, const map<string, string>& mapParams, 
				string User = "", string Password = "", int nPort = 80);
