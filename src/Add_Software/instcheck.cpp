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
#include <mysql/mysql.h>
}

using namespace std;

int IsInstaled(string packageName){
	int res;
	if( (res=system(((string)"dpkg --get-selections |grep -w install|grep -w "+packageName+"|wc -l> /tmp/isinst.tmp").c_str())) ){
		cout<<"error res"<<endl;
		return -1;
	}
	FILE *fd;
	fd=fopen("/tmp/isinst.tmp","r");
	if(!fd){
		cout<<"Error opening file"<<endl;
		return -1;
	}
	char buf[2];
	if(fread(buf,1,1,fd)){
		buf[1]='\0';
		return atoi(buf);
	}
	cout<<"error reading file"<<endl;
	return -1;
}

int main(int argc, char *argv[]){
	if(argc!=2){
		cout<<"Usage: "<<argv[0]<<" PK_Device\n\twhere PK_Device is primary key of this media director PC\n\tin the table \"Device\" of the pluto db"<<endl;
		return false;
	}
	cout<<"Starting"<<endl;
	string sMD_ID=argv[1];
	MYSQL *mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int result;
	string host="dcerouter",user="root",passwd="",db="pluto_main";
	mysql=mysql_init(NULL);
	if(!mysql_real_connect(mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),0,NULL,0)){
		cout<<"Error connecting to db:"<<mysql_error(mysql)<<endl;
		return false;
	}
	string query="SELECT DISTINCT PackageName FROM Software JOIN Software_Device ON FK_Software=PK_Software WHERE FK_Device="+sMD_ID;
	if(mysql_real_query(mysql,query.c_str(),(unsigned int) strlen(query.c_str()))==0&&(res=mysql_store_result(mysql))) {
		while ( (row=mysql_fetch_row(res)) ){
			result=IsInstaled(row[0]);
			switch(result){
				case -1:
					return false;
				case 	0:
					query="No";
					break;
				case  1:
					query="Yes";
			}
			if(!query.length()){
				cout<<"error query"<<endl;
				return false;
			}
			query="UPDATE Software set Installation_status=\""+query+"\" where PackageName=\""+row[0]+"\" and FK_Device="+sMD_ID;
			if(mysql_real_query(mysql,query.c_str(),(unsigned int) strlen(query.c_str()))){
				cout<<query<<endl;
				cout<<"Error making query: "<<mysql_error(mysql)<<endl;
				return false;
			}
		}
	}else{
		cout<<query<<endl;
		cout<<"Error making query: "<<mysql_error(mysql)<<endl;
		return false;
	}
	cout<<"Finished"<<endl;
	return true;
}
