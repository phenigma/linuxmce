#include <unistd.h>

#include <iostream>
#include <strstream>
#include <fstream>

#include <cstdlib>

#include <string>

#include "VIPShared/PhoneDevice.h"

using namespace std;

int main(int argc, char *argv[])
{
	if (argc<2)
	{
		cout << "convert_mac [Mac Address as X:X:X:X:X:X]|[Mac as large integer]" << endl;
		exit(0);
	}
	
	string Mac = argv[1];
	if( Mac.find(':')!=string::npos )
	{
		PhoneDevice pd("",Mac,0);
		cout << "Mac as large integer: " << pd.m_iMacAddress << endl;
	}
	else
	{
		PhoneDevice pd("",atof(Mac.c_str()),0);
		cout << "Mac as standard: " << pd.m_sMacAddress << endl;
	}
}

