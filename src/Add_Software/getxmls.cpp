/*
	webinst
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
// as 'dancer-xml' doesn't have the extern "C" linkage, we do it manually
struct dxml_element;

extern "C" dxml_element * dxml_read_xml(FILE*f/** File pointer to the XML 
		file*/);

extern "C" char * dxml_get_PCDATA_bysimplepath(dxml_element * e /** element 
 */ , const char * path /** path to PCDATA */);

extern "C" dxml_element * dxml_get_element_byname(dxml_element * e/** element 
 */, const char * name /** The name of the element */);
extern "C"
{
	#include <dancer-xml.h>
	#include <mysql/mysql.h>
}
#include "../PlutoUtils/uuencode.h"
#define MaxBuf 17

using namespace std;

static int pr2six[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, 
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, 
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

string getParamValue(const char *pref,const char *paramName,dxml_element *package){
	string tmp=pref;
	dxml_element *param=dxml_get_element_byname(package->child, paramName);
	tmp+=(param&&(param->child))?param->child->element_name:"";
	return tmp;
}

int findURL(FILE *f,string &link){
	const string templ="<a class=l href=\"";
	const string templ2="\">";
	char buf[MaxBuf+1];
	int pos,pos2;
	static string buf2="";
	link="wget --user-agent=\"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.4) Gecko/20060406 Firefox/1.5.0.4 (Debian-1.5.dfsg+1.5.0.4-1)\" -O /tmp/out.xml ";
	while (fread(buf,MaxBuf,1,f)){
		buf[MaxBuf]='\0';
		buf2+=buf;
		pos=buf2.find(templ,0);
		if(pos!=-1){
			pos2=buf2.find(templ2,pos+1);
			while(pos2==-1){
				fread(buf,MaxBuf,1,f);
				buf[MaxBuf]='\0';
				buf2+=buf;
				pos2=buf2.find(templ2,pos+1);
			}
			link.append(buf2,pos+MaxBuf,pos2-pos-MaxBuf);
			buf2.assign(buf2,pos2+1,buf2.length()-pos2);
			return 0;
		}
		buf2=buf;
	}
	return 1;
}

int IsExists(MYSQL *mysql,const string packageName,const string version){
	return 2;
	string query="SELECT Version FROM `Software` where PackageName=\""+packageName+"\"";
	MYSQL_RES *res;
	MYSQL_ROW row;
	if(mysql_real_query(mysql,query.c_str(),(unsigned int) strlen(query.c_str()))){
		cout<<query<<endl;
		cout<<"Error making query: "<<mysql_error(mysql)<<endl;
		return -1;
	}
	res=mysql_store_result(mysql);
	if(row=mysql_fetch_row(res)){
		return (int)(strcmp(version.c_str(),row[0])>0);
	}
	return 2;
}

int decode(char *&dest,dxml_element *package){
	string str=getParamValue("","icon",package);
	size_t Size = str.length();
	char *pDataEncoded=new char[Size];
	char *d,*s;
	d=pDataEncoded;
	s=(char *)str.c_str();
	while(*s){
		if(pr2six[(int)*s]!=-1)
			*d++=*s;
		s++;
	}
	*d='\0';
	Size = strlen(pDataEncoded);
	dest=new char[Size];
	int Bytes=0;
	if(Size)
		Bytes=Ns_HtuuDecode((unsigned char *) pDataEncoded, (unsigned char *) dest, Size);
	delete pDataEncoded;
	return Bytes;
}

int main(int argc, char *argv[]){
	int res;
	MYSQL *mysql;
	MYSQL_RES *mres;
	MYSQL_ROW row;
	string host="dcerouter",user="root",passwd="",db="pluto_main";
	string sQuery,query1,sHeadQuery;
	char *query, *pDataDecoded, *pGoodData;
	int length=0,i,Bytes;
	const string head="INSERT INTO `Software`(Iconstr, Title, Description, HomeURL, Category, Downloadurl, RepositoryName, PackageName, Misc, Version, Target, Importance, PC_Type, Required_Version_Min, Required_Version_Max, FK_Device) VALUES";
	mysql=mysql_init(NULL);
	if(!mysql_real_connect(mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),0,NULL,0)){
		cout<<"Error connecting to db:"<<mysql_error(mysql)<<endl;
		return false;
	}
	if(argc>1){
		string url=argv[1];
	}else{
		url="http://www.google.com/search?q=10faostb";
	}
	res=system((string("wget --user-agent=\"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.4) Gecko/20060406 Firefox/1.5.0.4 (Debian-1.5.dfsg+1.5.0.4-1)\" -O /tmp/search.html \"")+url+"\"").c_str());
	res=0;
	if (!res){
		FILE *fd,*fxml;
		dxml_element *packages,*package;
		fd=fopen("/tmp/search.html","r");
		if (!fd){
			cout<<"Error opening file search.html\n";
			return false;
		}
		query="select PK_Device from Device where IPaddress is not NULL and IPaddress<>\'\'";
		vector<string> vPK_Device;
		if(mysql_real_query(mysql,query,(unsigned int) strlen(query))==0&&(mres=mysql_store_result(mysql))) {
			while (row=mysql_fetch_row(mres))
				vPK_Device.push_back(row[0]);
		}
		string link,packageName,version;
		while (!findURL(fd,link)){
//			res=system(link.c_str());
			if(!res){
				fxml=fopen("/tmp/out.xml","r");
				if (!fxml){
					cout<<"Error opening file out.xml:"<<mysql_error(mysql)<<endl;
					return false;
				}
				packages=dxml_read_xml(fxml);
				fclose(fxml);
				if (!packages || !(packages->child)){
					cout<<"Error parsing xml\n";
					return false;
				}
				package=packages->child;
				while(package){
					packageName=getParamValue("", "packagename",package);
					version=getParamValue("", "version",package);
					if(!packageName.length()||!version.length())
						continue;
					sQuery="";
					query="";
					switch(IsExists(mysql,packageName,version)){
						case -1:
							return false;
						case 0:
							package=package->next;
							continue;
						case 1:
							sHeadQuery="UPDATE `Software` set Iconstr=\"";
							Bytes=decode(pDataDecoded,package);
							pGoodData=new char[Bytes*2+1];
							length=Bytes=mysql_real_escape_string(mysql, pGoodData, pDataDecoded,Bytes);
							delete pDataDecoded;
							sQuery=getParamValue("\",Title=\"", "title",package);
							sQuery+=getParamValue("\",Description=\"", "description",package);
							sQuery+=getParamValue("\",HomeURL=\"", "homeurl",package);
							sQuery+=getParamValue("\",Category=\"", "category",package);
							sQuery+=getParamValue("\",Downloadurl=\"", "downloadurl",package);
							sQuery+=getParamValue("\",RepositoryName=\"", "repositoryname",package);
							sQuery+=getParamValue("\",Misc=\"", "misc",package);
							sQuery+=getParamValue("\",Version=\"", "version",package);
							sQuery+=getParamValue("\",Target=\"", "target",package);
							sQuery+=getParamValue("\",Importance=\"", "importance",package);
							sQuery+=getParamValue("\",PC_Type=\"", "PC_Type",package);
							sQuery+=getParamValue("\",Required_Version_Min=\"", "Required_Version_Min",package);
							sQuery+=getParamValue("\",Required_Version_Max=\"", "Required_Version_Max",package);
							sQuery+="\" WHERE PackageName=\""+packageName+"\"";
							length+=sHeadQuery.length();
							length+=sQuery.length();
							query=new char[length];
							memcpy(query, sHeadQuery.c_str(), sHeadQuery.length());
							i=sHeadQuery.length();
							memcpy(query+i, pGoodData, Bytes);
							i+=Bytes;
							memcpy(query+i, sQuery.c_str(), sQuery.length());
							delete pGoodData;
							break;
						case 2:
							Bytes=decode(pDataDecoded,package);
							pGoodData=new char[Bytes*2+1];
							length=Bytes=mysql_real_escape_string(mysql, pGoodData, pDataDecoded,Bytes);
							delete pDataDecoded;
							sQuery=getParamValue("\",\"", "title",package);
							sQuery+=getParamValue("\",\"", "description",package);
							sQuery+=getParamValue("\",\"", "homeurl",package);
							sQuery+=getParamValue("\",\"", "category",package);
							sQuery+=getParamValue("\",\"", "downloadurl",package);
							sQuery+=getParamValue("\",\"", "repositoryname",package);
							sQuery+=getParamValue("\",\"", "packagename",package);
							sQuery+=getParamValue("\",\"", "misc",package);
							sQuery+=getParamValue("\",\"", "version",package);
							sQuery+=getParamValue("\",\"", "target",package);
							sQuery+=getParamValue("\",\"", "importance",package);
							sQuery+=getParamValue("\",\"", "PC_Type",package);
							sQuery+=getParamValue("\",\"", "Required_Version_Min",package);
							sQuery+=getParamValue("\",\"", "Required_Version_Max",package);
							length+=sQuery.length();
							i=0;
							for(size_t s=0;s<vPK_Device.size();++s)
								i+=vPK_Device[s].length()+3;
							length=(length+3)*vPK_Device.size()+head.length()-1+i;
							query=new char[length];
							i=0;
							memcpy(query, head.c_str(), head.length());
							i=head.length();
							for(size_t s=0;s<vPK_Device.size();++s){
								memcpy(query+i, "(\"", 2);
								i+=2;
								memcpy(query+i, pGoodData, Bytes);
								i+=Bytes;
								memcpy(query+i, sQuery.c_str(), sQuery.length());
								i+=sQuery.length();
								memcpy(query+i, ((string)"\","+vPK_Device[s]+")").c_str(), vPK_Device[s].length()+3);
								i+=vPK_Device[s].length()+3;
								if(s<vPK_Device.size()-1)
									memcpy(query+i++, ",", 1);
							}
							delete pGoodData;
					}
					if(length){
						if(mysql_real_query(mysql,query,(unsigned int) length)){
							cout<<"Error making query: "<<mysql_error(mysql)<<endl;
							return false;
						}
						delete query;
					}
					package=package->next;
				}
				dxml_free_xml(packages);
			}
		}
		fclose(fd);
	}
	system("rm -f /tmp/search.html /tmp/out.xml");
	return true;
}
