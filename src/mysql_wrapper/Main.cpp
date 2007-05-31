#include <iostream>
#include <map>
#include <mysql/mysql.h>
#include "ServerSocket.h"
#include "SocketException.h"
#include "TLV.h"

#define DEBUG

using namespace std;
	
const int default_port = 3306;

int main(int argc, char **argv) {
	int port = default_port;
	string unix_socket = "/tmp/db_wrapper"; // default socket name
	string host, user, pass, db;
	
	// parsing parameters
	int i;	
	while ( (i=getopt(argc, argv, "h:u:p:D:P:s:")) != -1 ) {
		if (!optarg) {
			cout << "Parameter -" << (char) i << " must have a value" << endl;
			continue;
		}

		switch (i) {
			case 'h':
				host = optarg;
				break;

			case 'u':
				user = optarg;
				break;

			case 'p':
				pass = optarg;
				break;

			case 'D':
				db = optarg;
				break;
				
			case 'P':
				port = atoi(optarg);
				if (!port)
				{
					cout << "Setting port to default value: " << default_port << endl;
					port = default_port;
				}
				break;

			case 's':
				unix_socket = optarg;
				break;

			default:
				cout << "Unknown parameter: -" << (char) i << endl;
		}
	}

	bool isConnected = false;
	
	MYSQL mysql;
	mysql_init(&mysql);
	
	unsigned long result_id=0;
	map<unsigned long, MYSQL_RES*> my_results;
	
	if ( !mysql_real_connect(&mysql, host.c_str(), user.c_str(), pass.c_str(), db.c_str(), port, NULL, 0) )
	{
		cout << "FAILED: " << "Failed to connect to database: Error: " << mysql_error(&mysql) << endl;
		return 1;
	}
	
	try {
		ServerSocket server(unix_socket);
		
		cout << "CONNECT: " << unix_socket << endl;
		isConnected = true;

		bool runServer = true;
		
		while ( runServer )
		{
			ServerSocket new_sock;
			server.accept ( new_sock );
			
			bool runConnection = true;

			try
			{
				while ( runConnection )
				{
					TLV tlv;					
					new_sock >> tlv;
					
					switch (tlv.type)
					{
						case TLV::typeNone:
						{
							cout << "Client connection broken, exiting" << endl;
							runConnection = false;
							runServer = false;
						}
						break;
						
						case TLV::typeFunction_RealQuery:
						{
#ifdef DEBUG
							cout << "Req4 mysql_real_query: " << tlv.length << endl;
							cout << "Query text (maybe truncated): ";
							for (int i=0; i<tlv.length; i++)
							{
								cout << tlv.value[i];
							}
							cout << endl;
#endif
							
							uint iRes = mysql_real_query(&mysql, tlv.value, tlv.length);
#ifdef DEBUG
							if (iRes == 0)
								cout << "Query executed OK" << endl;
							else
								cout << "Failed to execute query: Error: " << mysql_error(&mysql) << endl;
#endif
							
							bool bRes = (iRes == 0);
							TLV result(bRes);
							new_sock << result;
						}
						break;
						
						case TLV::typeFunction_Query:
						{
#ifdef DEBUG
							cout << "Req4 mysql_query: " << tlv.length << endl;
#endif

							char *query = new char[tlv.length+1];
							memcpy(query, tlv.value, tlv.length);
							query[tlv.length] = 0;
							
#ifdef DEBUG
							cout << "Query text: " << query << endl;
#endif

							uint iRes = mysql_query(&mysql, query);
#ifdef DEBUG
							if (iRes == 0)
								cout << "Query executed OK" << endl;
							else
								cout << "Failed to execute query: Error: " << mysql_error(&mysql) << endl;
#endif
							delete[] query;

							bool bRes = (iRes == 0);
							TLV result(bRes);
							new_sock << result;
						}
						break;
						
						case TLV::typeFunction_RealEscapeString:
						{
#ifdef DEBUG
							cout << "Req4 mysql_real_escape_string: " << tlv.length << endl;
							cout << "String text (maybe truncated): " << tlv.value << endl;
#endif
							char *to = new char[tlv.length*2+1];
							uint iRes = mysql_real_escape_string(&mysql, to, tlv.value, tlv.length);

#ifdef DEBUG
							cout << "String text (escaped): " << to << endl;
							cout << "length: " << iRes << endl;
#endif

							TLV answer(TLV::typeData_CHARArray, to);
							new_sock << answer;
							delete[] to;
						}
						break;
						
						case TLV::typeFunction_AffectedRows:
						{
#ifdef DEBUG
							cout << "Req4 mysql_affected_rows: " << tlv.length << endl;
#endif
							long iRes = mysql_affected_rows(&mysql);
							
							TLV result(iRes);
							new_sock << result;
						}
						break;

						case TLV::typeFunction_FieldCount:
						{
#ifdef DEBUG
							cout << "Req4 mysql_field_count: " << tlv.length << endl;
#endif
							uint iRes = mysql_field_count(&mysql);
							
							TLV result(iRes);
							new_sock << result;
						}
						break;
						
						case TLV::typeFunction_InsertID:
						{
#ifdef DEBUG
							cout << "Req4 mysql_insert_id: " << tlv.length << endl;
#endif
							unsigned long iRes = mysql_insert_id(&mysql);
							
							TLV result(iRes);
							new_sock << result;
						}
						break;
						
						case TLV::typeFunction_Error:
						{
#ifdef DEBUG
							cout << "Req4 mysql_error: " << tlv.length << endl;
#endif
							const char *error = mysql_error(&mysql);
							
							TLV result(TLV::typeData_CHARArray, error, strlen(error));
							new_sock << result;
						}
						break;
						
						case TLV::typeFunction_StoreResult:
						{
#ifdef DEBUG
							cout << "Req4 mysql_store_result: " << tlv.length << endl;
#endif
							MYSQL_RES * my_result = mysql_store_result(&mysql);
							bool isOK = (my_result != NULL);
							
							TLV result(isOK);
							new_sock << result;
							
							if (isOK)
							{
								result_id++;
								my_results[result_id] = my_result;
#ifdef DEBUG
								cout << "Result ID: " << result_id << endl;
#endif
								TLV result0(result_id);
								new_sock << result0;
								
								unsigned long rows_count = my_result->row_count;
#ifdef DEBUG
								cout << "Rows in result: " << rows_count << endl;
#endif
								TLV result1( rows_count );
								new_sock << result1;
								
								unsigned long fields_count = my_result->field_count;
#ifdef DEBUG
								cout << "Fields in result: " << fields_count << endl;
#endif
								TLV result2( fields_count );
								new_sock << result2;
							}
						}
						break;
						
						case TLV::typeFunction_FreeResult:
						{
#ifdef DEBUG
							cout << "Req4 mysql_free_result: " << tlv.length << endl;
#endif
							unsigned long target_id = tlv.to_ulong();
							if (target_id == -1)
							{
#ifdef DEBUG
								cout << "ERROR: no such result set" << endl;
#endif
							}
							else
							{
								map<unsigned long, MYSQL_RES*>::iterator i = my_results.find(target_id);
								if ( i == my_results.end() )
								{
#ifdef DEBUG
									cout << "ERROR: no such result set" << endl;
#endif
								}
								else
								{
									MYSQL_RES *to_free = i->second;
									my_results.erase(i);
									mysql_free_result(to_free);
#ifdef DEBUG
									cout << "Freed result set ID: " << target_id <<  endl;
#endif
								}
							}
						}
						break;
						
						case TLV::typeData_PING:
						{
#ifdef DEBUG
							cout << "Req4 ping: " << tlv.length << endl;
#endif
							TLV reply(TLV::typeData_PONG);
							new_sock << reply;
						}
						break;
						
						case TLV::typeFunction_FetchRow:
						{
#ifdef DEBUG
							cout << "Req4 mysql_fetch_row: " << tlv.length << endl;
#endif
							int target_id = tlv.to_ulong();
							if (target_id == -1)
							{
#ifdef DEBUG
								cout << "ERROR: no such result set" << endl;
#endif
							}
							else
							{
								map<unsigned long, MYSQL_RES*>::iterator i = my_results.find(target_id);
								if ( i == my_results.end() )
								{
#ifdef DEBUG
									cout << "ERROR: no such result set" << endl;
#endif
								}
								else
								{
									MYSQL_ROW row = mysql_fetch_row(i->second);
							
									if (row)
									{
										uint nFields = mysql_num_fields(i->second);
										unsigned long *lengths = mysql_fetch_lengths(i->second);
								
										uint data_length = sizeof(uint);
								
										for (uint i=0; i<nFields; i++)
										{
										data_length += sizeof(bool);
										data_length += sizeof(uint);
										data_length += lengths[i];
										}
								
										char *data = new char[data_length];
										char *p = data;
										memcpy(p, &nFields, sizeof(uint));
										p += sizeof(uint);
								
										for (uint i=0; i<nFields; i++)
										{
										bool isNULL = (row[i] == NULL);
										memcpy(p, &isNULL, sizeof(bool));
										p += sizeof(bool);
									
										memcpy(p, &lengths[i], sizeof(uint));
										p += sizeof(uint);
									
										memcpy(p, row[i], lengths[i]);
										p += lengths[i];
										}
								
										TLV result(TLV::typeData_ROW, data, data_length);
										new_sock << result;
								
#ifdef DEBUG
										/*
										cout << "built packet with length: " << data_length << endl;
										for (int i=0; i<data_length; i++)
										{
										printf("%02X ", data[i]);
										}
										cout << endl;
										*/
#endif
								
										delete[] data;
									}
									else
									{
										TLV result(false);
										new_sock << result;
									}
								}
							}
								
							
						}
						break;
						
						case TLV::typeFunction_FetchLengths:
						{
#ifdef DEBUG
							cout << "Req4 mysql_fetch_lengths: " << tlv.length << endl;
#endif
							
							int target_id = tlv.to_ulong();
							if (target_id == -1)
							{
#ifdef DEBUG
								cout << "ERROR: no such result set" << endl;
#endif
							}
							else
							{
								map<unsigned long, MYSQL_RES*>::iterator i = my_results.find(target_id);
								if ( i == my_results.end() )
								{
#ifdef DEBUG
									cout << "ERROR: no such result set" << endl;
#endif
								}
								else
								{
									uint nFields = mysql_num_fields(i->second);
									unsigned long *lengths = mysql_fetch_lengths(i->second);
							
									uint data_length = sizeof(unsigned long)*nFields;
									char *data = new char[data_length];
									memcpy(data, lengths, data_length);
							
									TLV result(TLV::typeData_ULONGArray, data, data_length);
									new_sock << result;
							
#ifdef DEBUG
									/*
									cout << "built packet with length: " << data_length << endl;
									for (int i=0; i<data_length; i++)
									{
										printf("%02X ", data[i]);
									}
							
									cout << endl;
									*/
#endif
							
									delete[] data;
								}
							}
							
						}
						break;
												
						case TLV::typeCommand_SHUTDOWN:
						{
							cout << "Shutdown requested" << endl;
							runConnection = false;
							runServer = false;
							unlink(unix_socket.c_str());
						}
						break;
						
						default:
#ifdef DEBUG
							cout << "not implemented TLV type: " << tlv.type << endl;
#endif
						break;
					}
					
				}
			}
			catch ( SocketException& ) {}
		}
	}
	catch (SocketException &e)
	{
		cout << "CONNECT ERROR: Exception was caught:" << e.description() << " Exiting.";
	}
	
	return 0;
}
