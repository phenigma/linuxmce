/*
	Tribunexml
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "DCE/Logger.h"
#include "PlutoUtils/StringUtils.h"
#include <vector>
#include <fstream>
#include <time.h>
#include <iostream>

extern "C"
{
#include <mysql/mysql.h>
}

string URL="ftp://big549bg:NOK248ti@ftp.tmstv.com/pub/";

namespace DCE
{
	Logger *g_pPlutoLogger;
}
using namespace DCE;
using namespace std;

int getURL(const string filename){
	return 0;
	string command=(string) "wget -O - \""+URL+filename+".Z\"|uncompress>/tmp/"+filename;
	cout<<command<<endl;
	return system(command.c_str());
}

int fillInsert(ifstream &f, string &result,short table=0) {
	if(f.eof())
		return false;
	string str="",buf="";
	char tmp[16384];
	memset(tmp, 0, sizeof(tmp));
	f.getline(tmp, sizeof(tmp));
	int field=0;
	if(*tmp==0)
		return false;
	buf=(string)tmp+"|";
	int num=buf.find("|");
	result+="('";
	while(num>=0){
		if ((2==table)&&(field>11))
			break;
		str=buf.substr(0,num);
		if((table!=1)||((1==table)&&(field!=0)))
			result+=StringUtils::SQLEscape(str)+"','";
		str="";
		if((unsigned)num+1<=buf.size())
			buf.erase(0,num+1);
		num=buf.find("|");
		field++;
	}
	result.erase(result.size()-2,2);
	result+=")";
	return true;
}

int main(int argc, char *argv[]) {
	g_pPlutoLogger = new FileLogger(stdout);
	MYSQL *mysql;
	string host="dcerouter",user="root",passwd="",db="tribune";
	bool bError=false;
	char c;
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-'){
			cerr<<"Unknown option\n";
			bError=true;
			break;
		}
		c=argv[i][1];
		switch(c){
			case 'h':host=argv[++i];break;
			case 'u':user=argv[++i];break;
			case 'p':passwd=argv[++i];break;
			case 'd':db=argv[++i];break;
			case 'U':URL=argv[++i];break;
			default:cerr<<"Unknown option\n";
			bError=true;
		}
	}
	if(bError){
		cout << "Usage: "<< argv[0]
				<< " [-h MySql hostname] [-u MySql username] " << endl
				<< "[-p MySql password] [-d MySql database] " << endl
				<< "[-U URL to download headend and lineup]" << endl;
		return false;
	}
	string query="";
	string head="INSERT INTO Headends( headend_id, community_name, county_name, county_size, st_county_code, state_served, zip_code, dma_code, dma_name, mso_code, dma_rank, headend_name, headend_location, mso_name, time_zone_code ) VALUES ";
//	string update=" ON DUPLICATE KEY UPDATE he_headend_id=VALUES(he_headend_id), he_community_name=VALUES(he_community_name), he_county_name=VALUES(he_county_name), he_county_size=VALUES(he_county_size), he_st_county_code=VALUES(he_st_county_code), he_state_served=VALUES(he_state_served), he_zip_code=VALUES(he_zip_code), he_dma_code=VALUES(he_dma_code), he_dma_name=VALUES(he_dma_name), he_mso_code=VALUES(he_mso_code), he_dma_rank=VALUES(he_dma_rank), he_headend_name=VALUES(he_headend_name), he_headend_location=VALUES(he_headend_location), he_mso_name=VALUES(he_mso_name), he_time_zone_code=VALUES(he_time_zone_code)";
	mysql=mysql_init(NULL);
	if(!mysql_real_connect(mysql,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),0,NULL,0)){
		cout<<"Error connecting to db:"<<mysql_error(mysql)<<endl;
		return false;
	}
	if(!getURL("headend.fsf")){
		ifstream f("/tmp/headend.fsf");
		if (!f){
			cerr<<"Error opening file /tmp/headend.fsf\n";
			return false;
		}
		while(fillInsert(f, query)){
			if(query.size()>=12800){
				query=head+query;//+update;
//				cout<<query<<endl;
				if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
					cout<<"query: "<<query<<endl
							<<"Error making query: "<<mysql_error(mysql)<<endl;
					return false;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;//+update;
//			cout<<query<<endl;
			if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
				cout<<"query: "<<query<<endl
						<<"Error making query: "<<mysql_error(mysql)<<endl;
				return false;
			}
			query="";
		}
	}
	head="INSERT INTO Lineups( FK_Headends, device, FK_Stations, tms_chan, service_tier, effective_date, expiration_date ) VALUES ";
//	update=" ON DUPLICATE KEY UPDATE cl_device=VALUES(cl_device), cl_station_num=VALUES(cl_station_num), cl_tms_chan=VALUES(cl_tms_chan), cl_service_tier=VALUES(cl_service_tier), cl_effective_date=VALUES(cl_effective_date), cl_expiration_date=VALUES(cl_expiration_date)";
	if(!getURL("lineup.fsf")){
		ifstream f("/tmp/lineup.fsf");
		if (!f){
			cerr<<"Error opening file /tmp/lineup.fsf\n";
			return false;
		}
		while(fillInsert(f, query,1)){
			if(query.size()>=12800){
				query=head+query;//+update;
//				cout<<query<<endl;
				if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
					cout<<"query: "<<query<<endl
							<<"Error making query: "<<mysql_error(mysql)<<endl;
					return false;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;//+update;
//			cout<<query<<endl;
			if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
				cout<<"query: "<<query<<endl
						<<"Error making query: "<<mysql_error(mysql)<<endl;
				return false;
			}
			query="";
		}
	}
	head="INSERT INTO Stations( PK_Stations, station_time_zone, station_name, station_call_sign, station_affil, station_city, station_state, station_zip_code, station_country, dma_name, dma_num, fcc_channel_num ) VALUES ";
	if(!getURL("statrec.txt")){
		ifstream f("/tmp/statrec.txt");
		if (!f){
			cerr<<"Error opening file /tmp/statrec.txt\n";
			return false;
		}
		while(fillInsert(f, query,2)){
			if(query.size()>=12800){
				query=head+query;//+update;
//				cout<<query<<endl;
				if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
					cout<<"query: "<<query<<endl
							<<"Error making query: "<<mysql_error(mysql)<<endl;
					return false;
				}
				query="";
			}else
				query+=",";
		}
		if(query.size()>0){
			query.erase(query.size()-1,1);
			query=head+query;//+update;
//			cout<<query<<endl;
			if( mysql_real_query( mysql,query.c_str(),(unsigned int) query.size() ) ){
				cout<<"query: "<<query<<endl
						<<"Error making query: "<<mysql_error(mysql)<<endl;
				return false;
			}
			query="";
		}
	}
	return true;
}
