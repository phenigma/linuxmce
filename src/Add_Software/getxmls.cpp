/*
getxmls
Copyright (C) 2004 Pluto, Inc., a Florida Corporation

www.plutohome.com		

Phone: +1 (877) 758-8648

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 

See the GNU General Public License for more details.
*/

#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <vector>

#include "../PlutoUtils/uuencode.h"
#include "../DCE/Logger.h"
#include "../PlutoUtils/StringUtils.h"

#include "PackageXMLParser.h"
#include "DatabaseHelper.h"
#include "XMLFinder.h"

#define MaxBuf 17
#define UserAgent "--user-agent=\"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.4) Gecko/20060406 Firefox/1.5.0.4 (Debian-1.5.dfsg+1.5.0.4-1)\""
namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
using namespace std;

const string templBegin[2]={"<a class=l href=\"","<em class=yschurl>"};
const string templEnd[2]={"\">","</em>"};

int findURL(FILE *f,string &link,size_t searchEngine)
{
	char buf[MaxBuf+1];
	int pos,pos2;
	string buf2="";
	link=(string)"wget --timeout 60 --tries=1 "+UserAgent+" -O /tmp/out.html \"";
	while (fread(buf,MaxBuf,1,f)){
		buf[MaxBuf]='\0';
		buf2+=buf;
		pos=buf2.find(templBegin[searchEngine],0);
		if(pos!=-1){
			pos2=buf2.find(templEnd[searchEngine],pos+1);
			while(pos2==-1){
				fread(buf,MaxBuf,1,f);
				buf[MaxBuf]='\0';
				buf2+=buf;
				pos2=buf2.find(templEnd[searchEngine],pos+1);
			}
			link.append(buf2,pos+templBegin[searchEngine].length(),pos2-pos-templBegin[searchEngine].length());
			link+="\"";
			if(".xml"==link.substr(link.length()-5,4)){
				link+=" ; mv /tmp/out.html /tmp/out.xml";
				return 2;
			}
			if(1==searchEngine){
				pos=link.find("<wbr>");
				if(pos!=-1)
					link.erase(pos, 5);
			}
			return 1;
		}
		buf2=buf;
	}
	return 0;
}

int findXML(FILE *fpage)
{
	const string templ=">10faostb</a>";
	const string templ2="<a href=\"";
	const string templ3=".xml\"";
	char buf[MaxBuf+1];
	int pos,pos2,pos3;
	string buf2="";
	string link=(string)"wget --timeout 60 --tries=1 "+UserAgent+" -O /tmp/out.xml ";
	while (fread(buf,MaxBuf,1,fpage))
	{
		buf[MaxBuf]='\0';
		buf2+=buf;
		pos=buf2.find(templ,0);
		if(pos!=-1){
			fseek(fpage,-100,1);
			buf2="";
			while(fread(buf,MaxBuf,1,fpage)){
				buf[MaxBuf]='\0';
				buf2+=buf;
				pos2=buf2.find(templ2,0);
				if(pos2!=-1){
					pos3=buf2.find(templ3,pos2+1);
					while(pos3==-1){
						fread(buf,MaxBuf,1,fpage);
						buf[MaxBuf]='\0';
						buf2+=buf;
						pos3=buf2.find(templ3,pos2+1);
					}
					link.append(buf2,pos2+9,pos3+4-pos2-9);
					g_pPlutoLogger->Write(LV_WARNING, link.c_str());
					return system(link.c_str());
				}
			}
		}
		buf2=buf;
	}
	return 1;
}

void ProcessXML(string sFileName)
{
	g_pPlutoLogger->Write(LV_WARNING, "Processing XML %s", sFileName.c_str());

	string sXmlData;
	FileUtils::ReadTextFile(sFileName, sXmlData);
	sXmlData = StringUtils::Replace(sXmlData, "&", "&amp;");

	PackageXMLParser parser;
	parser.Parse(sXmlData);
	const list<PackageInfo>& listPackages = parser.GetParsedData();

	DatabaseHelper dbhelper;
	dbhelper.ProcessPackages(listPackages);
}

int main(int argc, char *argv[])
{
	g_pPlutoLogger = new FileLogger(stdout);
	int res;
	bool bError=false;
	string host="dcerouter",user="root",passwd="",db="pluto_main";
	vector<string> url;
	url.push_back("http://www.google.com/search?q=10faostb");
	url.push_back("http://search.yahoo.com/search?p=10faostb");
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-'){
			cerr<<"Unknown option"<<endl;
			bError=true;
			break;
		}
		char c=argv[i][1];
		switch(c){
			case 'h':host=argv[++i];break;
			case 'u':user=argv[++i];break;
			case 'p':passwd=argv[++i];break;
			case 'd':db=argv[++i];break;
			case 'U':url.push_back(argv[++i]);break;
			default:cerr<<"Unknown option"<<endl;
				bError=true;
		}
	}
	if(bError){
		cout << "Usage: "<< argv[0]
		<< " [-h MySql hostname] [-u MySql username] " << endl
			<< " [-p MySql password] [-d MySql database] " << endl
			<< " [-U URL of google search]" << endl;
		return false;
	}
	for(size_t noEngine=0;noEngine<url.size();noEngine++)
	{
		string sCmd = string("wget --timeout 60 --tries=1 ")+UserAgent+" -O /tmp/search.html \""+url[noEngine]+"\"";
		cout << sCmd << endl;
		res=system(sCmd.c_str());
		if (!res)
		{
			FILE *fsearch,*fpage;
			fsearch=fopen("/tmp/search.html","r");
			if (!fsearch)
			{
				cout<<"Error opening file search.html"<<endl;
				return false;
			}

			string link,packageName,version;
			int whatFind;
			while ((whatFind=findURL(fsearch,link,noEngine)))
			{
				cout<<link<<endl;
				res=system(link.c_str());
				if(!res)
				{
					if(1 == whatFind)
					{
						fpage=fopen("/tmp/out.html","r");
						if(!fpage){
							cout<<"Error opening file out.html"<<endl;
							return false;
						}

						if(findXML(fpage))
							continue;

                        ProcessXML("/tmp/out.xml");

					}
				}
			}
			fclose(fsearch);

			string sXMLUrl = XMLFinder::FindURLToXML("/tmp/search.html");

			if(!sXMLUrl.empty())
			{
				string sCommand = string("wget --timeout 60 --tries=1 ") + UserAgent + " -O /tmp/out.xml " + sXMLUrl;
				g_pPlutoLogger->Write(LV_WARNING, sCommand.c_str());

				if(!system(sCommand.c_str()))
					ProcessXML("/tmp/out.xml");
			}
		}
	}

	return true;
}
