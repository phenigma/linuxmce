#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "DCEConfig.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifdef WIN32
#include <direct.h>
#include <conio.h>
#define chdir _chdir  // Why, Microsoft, why?
#define mkdir _mkdir  // Why, Microsoft, why?
#else
#include <sys/wait.h>
#endif


using namespace std;
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger;
}

DCEConfig *g_dceConfig;
map<string,bool> g_mapBlocks;  // List of all the blocks to include

bool ReplaceTags(string &str,string sFilename);

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger(stdout);

	bool bError=false;
	string sInputPath,sExtensions,sConfFile;
	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c=argv[optnum][1];
		switch (c)
		{
		case 'p':
			sInputPath = argv[++optnum];
			break;
		case 'e':
			sExtensions = argv[++optnum];
			break;
		case 'c':
			sConfFile = argv[++optnum];
			break;
		default:
			cout << "Unknown: " << argv[optnum] << endl;
			bError=true;
			break;
		};
	}

	if ( bError || sInputPath.empty() || sExtensions.empty() || sConfFile.empty() )
	{
		cout << "MakeRelease_PrepFiles_PrepFiles" << endl
			<< "Usage: MakeRelease_PrepFiles_PrepFiles -p Path -e Extensions -c ConfFile" << endl;

		exit(0);
	}

	g_dceConfig = new DCEConfig(sConfFile);
	string sBlocks = g_dceConfig->ReadString("blocks");
	string::size_type pos=0;
	while( pos < sBlocks.size() )
		g_mapBlocks[ StringUtils::Tokenize(sBlocks,",",pos) ] = true;

	// FindFiles wants tab delimited list
	StringUtils::Replace(&sExtensions,",","\t");

	list<string> listFiles;
	FileUtils::FindFiles(listFiles,sInputPath,sExtensions,true,true);

	g_pPlutoLogger->Write(LV_STATUS,"Found %d files",(int) listFiles.size());

	for(list<string>::iterator it=listFiles.begin();it!=listFiles.end();++it)
	{
		if( it->find( "MakeRelease_PrepFiles" ) !=string::npos )
			continue;

		size_t s;
		char *pBuffer = FileUtils::ReadFileIntoBuffer(*it,s);
		if( pBuffer )
		{
			string str = pBuffer;
			if( ReplaceTags(str,*it) )
				FileUtils::WriteBufferIntoFile(*it,str.c_str(),str.size());
			delete pBuffer;
		}
	}

	return 0;
}

// tags can be <-mkr_b_blockname_b->  something <-mkr_b_blockname_e->  means the block in between should only be included if blockname is in the list
// tags can be <-mkr_B_blockname_b->  something <-mkr_B_blockname_e->  means the block in between should only be included if blockname is *not* in the list
// <-mkr_t_tagname-> will be substitude with tagname
bool ReplaceTags(string &str,string sFilename)
{
	bool bFound=false;
	string::size_type pos=0;
	while(true)
	{
		string::size_type tag_pos = str.find("<-mkr_",pos);
		if( tag_pos==string::npos )
			return bFound;

		bFound=true;
		string::size_type end_pos=str.find("->",tag_pos+1);
		if( end_pos==string::npos || end_pos - tag_pos < 12 )
		{
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find matching tag in %s",sFilename.c_str());
			return false;
		}
		char cType = str[ tag_pos+6 ];
		pos = tag_pos+1;

		switch( cType )
		{
		case 'b':
		case 'B':
			{
				string sTag = str.substr( tag_pos+8, end_pos-tag_pos-10 );
				char cBegin = str[ end_pos - 1 ];
				if( cBegin!='b' )
					continue;

				vector<string> vectBlocks;
				StringUtils::Tokenize(sTag,"|",vectBlocks);

				bool bFound=false;
				for(vector<string>::iterator itB = vectBlocks.begin(); itB != vectBlocks.end(); ++itB)
				{
					map<string,bool>::iterator it=g_mapBlocks.find(*itB);
					if( it!=g_mapBlocks.end() )
					{
						bFound=true;
						break;
					}
				}
				if( (cType=='b' && !bFound) || (cType=='B' && bFound) )
				{
					string s = " "; s[0]=cType;  // Stupid, but gcc doesn't like + cType??
					string sEndTag = "<-mkr_" + s + "_" + sTag + "_e->";
					string::size_type end_tag = str.find(sEndTag,tag_pos);
					if( end_tag==string::npos )
					{
						g_pPlutoLogger->Write(LV_CRITICAL,"Cannot find end block for %s in %s",sTag.c_str(),sFilename.c_str());
						continue;
					}
					str.replace(tag_pos,end_tag-tag_pos+sEndTag.size(),"");
				}
			}
			break;
		case 't':
			{
				string sTag = str.substr( tag_pos+8, end_pos-tag_pos-8 );
				string sValue = g_dceConfig->ReadString(sTag);
				str.replace(tag_pos,end_pos-tag_pos+2,sValue);
			}
			break;
		default:
			g_pPlutoLogger->Write(LV_CRITICAL,"Unknown tag type %c in %s",cType,sFilename.c_str());
			break;
		};
	}
}
