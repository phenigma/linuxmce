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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
#define MaxBuf 17

using namespace std;

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
	link="wget --user-agent=\"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.4) Gecko/20060406 Firefox/1.5.0.4 (Debian-1.5.dfsg+1.5.0.4-1)\" -O out.xml ";
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
		cout<<row[0]<<"\t"<<version<<endl;
		return (int)(strcmp(version.c_str(),row[0])>0);
	}
	return 2;
}

int main(int argc, char *argv[]){
	int res;
	MYSQL *mysql;
	MYSQL_RES *mres;
	MYSQL_ROW row;
	string host="dcerouter",user="root",passwd="",db="pluto_main";
	string query,query1;
	const string head="INSERT INTO `Software`(Iconstr, Title, Description, HomeURL, Category, Downloadurl, RepositoryName, PackageName, Misc, Version, Target, Importance, PC_Type, Required_Version_Min, Required_Version_Max, FK_Device) VALUES";
	mysql=mysql_init(NULL);
	if(!mysql_real_connect(mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),0,NULL,0)){
		cout<<"Error connecting to db:"<<mysql_error(mysql)<<endl;
		return false;
	}
	res=system("wget --user-agent=\"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.0.4) Gecko/20060406 Firefox/1.5.0.4 (Debian-1.5.dfsg+1.5.0.4-1)\" -O search.html \"http://www.google.com/search?q=10faostb\"");
	if (!res){
		FILE *fd,*fxml;
		dxml_element *packages,*package;
		fd=fopen("search.html","r");
		if (!fd){
			cout<<"Error opening file search.html\n";
			return false;
		}
		query="select PK_Device from Device where IPaddress is not NULL and IPaddress<>\'\'";
		vector<string> vPK_Device;
		if(mysql_real_query(mysql,query.c_str(),(unsigned int) strlen(query.c_str()))==0&&(mres=mysql_store_result(mysql))) {
			while (row=mysql_fetch_row(mres))
				vPK_Device.push_back(row[0]);
		}
		string link,packageName,version;
		while (!findURL(fd,link)){
//      cout << link.c_str() << endl;
      res=system(link.c_str());
			if(!res){
				fxml=fopen("out.xml","r");
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
					query="";
					switch(IsExists(mysql,packageName,version)){
						case -1:
							return false;
						case 0:
							package=package->next;
							continue;
						case 1:
							query="UPDATE `Software` set ";
							query+=getParamValue("Iconstr=\"","icon",package);
							query+=getParamValue("\",Title=\"", "title",package);
							query+=getParamValue("\",Description=\"", "description",package);
							query+=getParamValue("\",HomeURL=\"", "homeurl",package);
							query+=getParamValue("\",Category=\"", "category",package);
							query+=getParamValue("\",Downloadurl=\"", "downloadurl",package);
							query+=getParamValue("\",RepositoryName=\"", "repositoryname",package);
							query+=getParamValue("\",Misc=\"", "misc",package);
							query+=getParamValue("\",Version=\"", "version",package);
							query+=getParamValue("\",Target=\"", "target",package);
							query+=getParamValue("\",Importance=\"", "importance",package);
							query+=getParamValue("\",PC_Type=\"", "PC_Type",package);
							query+=getParamValue("\",Required_Version_Min=\"", "Required_Version_Min",package);
							query+=getParamValue("\",Required_Version_Max=\"", "Required_Version_Max",package);
							query+="\" WHERE PackageName=\""+packageName+"\"";
							break;
						case 2:
							query1=getParamValue("(\"","icon",package);
							query1+=getParamValue("\",\"", "title",package);
							query1+=getParamValue("\",\"", "description",package);
							query1+=getParamValue("\",\"", "homeurl",package);
							query1+=getParamValue("\",\"", "category",package);
							query1+=getParamValue("\",\"", "downloadurl",package);
							query1+=getParamValue("\",\"", "repositoryname",package);
							query1+=getParamValue("\",\"", "packagename",package);
							query1+=getParamValue("\",\"", "misc",package);
							query1+=getParamValue("\",\"", "version",package);
							query1+=getParamValue("\",\"", "target",package);
							query1+=getParamValue("\",\"", "importance",package);
							query1+=getParamValue("\",\"", "PC_Type",package);
							query1+=getParamValue("\",\"", "Required_Version_Min",package);
							query1+=getParamValue("\",\"", "Required_Version_Max",package);
							for(size_t s=0;s<vPK_Device.size();++s)
								query+=query1+"\","+vPK_Device[s]+"),";
							query=head+query.substr(0,query.length()-1);
					}
					if(query.length()){
						cout<<query<<endl;
						if(mysql_real_query(mysql,query.c_str(),(unsigned int) strlen(query.c_str()))){
							cout<<"Error making query: "<<mysql_error(mysql)<<endl;
							return false;
						}
					}
					package=package->next;
				}
				dxml_free_xml(packages);
			}
		}
		fclose(fd);
	}
	return true;
}
