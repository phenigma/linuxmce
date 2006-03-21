#include "StdAfx.h"
#include ".\Database.h"


DatabaseWrapper::DatabaseWrapper()
{
	HRESULT hr;

	try
	{
		hr = m_pConnect.CreateInstance(__uuidof(Connection));
		hr = m_pRecordset.CreateInstance(__uuidof(Recordset));
	}
	catch(_com_error& ce)
	{
		errDesc = (char *) ce.Description();
	}
}

DatabaseWrapper::~DatabaseWrapper(void)
{
	close();
}

void DatabaseWrapper::close()
{
	HRESULT hr;

	try
	{
		m_pConnect->Close();
	}
	catch( _com_error &ce )
	{
	}
}

bool DatabaseWrapper::connect(string dns)
{	
	HRESULT hr;

	try
	{
		hr = m_pConnect->Open( dns.c_str(), "" , "", 0 );
		if( FAILED(hr) )
			return false;
	}
	catch(_com_error& ce)
    {
		errDesc = (char *) ce.Description();
		return false;
    }
	return true;
}

bool DatabaseWrapper::executeSql(string szCommand,bool bReturn)
{
	variant_t var;
	BSTR sqlCommand;
	HRESULT hr;

	USES_CONVERSION;
	sqlCommand = A2W( szCommand.c_str() );

	try
	{
		if( bReturn )
			m_pRecordset = m_pConnect->Execute( sqlCommand, &var, adOptionUnspecified );
		else
			m_pConnect->Execute( sqlCommand, &var, adExecuteNoRecords );

		if(  var.iVal > 0 && bReturn )
			hr = m_pRecordset->MoveFirst();
	}
	catch(_com_error& ce)
    {
		errDesc = (char *) ce.Description();
		return false;
    }

	return true;
}

string DatabaseWrapper::getFieldValue(string fieldName)
{
	int nCount = 0;
	_variant_t name = fieldName.c_str();
	_variant_t value;

	try
	{
		m_pFields = m_pRecordset->GetFields();
		nCount = m_pFields->GetCount();
		m_pField = m_pFields->GetItem( name );
		value = m_pField->GetValue();
	}
	catch(_com_error& ce)
    {
		errDesc = (char *) ce.Description();
		return false;
    }

	return string( (char *) value.bstrVal);
}



///////////////////////Util functions
////////////////////////////////////////////////////////

bool DatabaseWrapper::constructInsert(string databaseName,vector<string> params,vector<string> values,
									vector<bool> isString,string &resSql)
{
	vector<string>::const_iterator it;
	vector<bool>::const_iterator it2;

	if( (params.size() != values.size()) || (params.size() != isString.size()) )
		return false;
	if( (params.size() == 0) && (values.size() == 0) ) 
		return false;

	resSql = "INSERT INTO " + databaseName + "(";
	for(it =params.begin();it!=params.end();it++)
		resSql += *it + ",";
	resSql[resSql.size()-1] = ')';

	resSql += " VALUES (";
	for(it=values.begin(),it2=isString.begin();it!=values.end();it++,it2++)
	{
		if( *it2 )
			resSql += "'" + *it + "',";
		else
			resSql += *it + ",";
	}
	resSql[resSql.size()-1] = ')';
	
	return true;
}

bool DatabaseWrapper::constructSelect(string databaseName,string condition,string &resSql)
{
	resSql = string("SELECT * FROM ") + databaseName.c_str() + " ";
	resSql += string("WHERE ") + condition.c_str();
	return true;
}
