#include <fstream>
#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char *argv[]){
	if(argc<1){
 		cout<<"Usage: "<< argv[0]
 				<<" fileWhatToFilter"<<endl;
 		return 1;
 	}
	char del[16384],str[16384];
	ifstream f( argv[1] );
//	ifstream f( "2.txt" );
	if(!f)
		return 1;
	f.getline( del, sizeof(del) );
//	cerr<<del<<endl;
	int i=0;
//	ifstream cin( "crew.txt" );
	cin.getline( str, sizeof(str) );
	while( !cin.eof() ){
		i=strncmp(str, del, 13);
		if(0==i){
			cout<<str<<endl;
			if(!(f.getline( del, sizeof(del) ))) {
				break;
			}
		}else if (i>0){
			if(f.getline( del, sizeof(del) )) {
				continue;
			}else{
				break;
			}
		}
		cin.getline( str, sizeof(str) );
	}
	return 0;
}
