#include "PlutoUtils/CommonIncludes.h"	
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "PhoneDetection_Simulate.h"
#include "BD/PhoneDevice.h"

bool PhoneDetection_Simulate::ScanningLoop()
{
	// I just read that sleep causes all the threads to sleep???  But I don't see a pthread_sleep to make
	// just this one sleep
	Sleep(500);
#ifdef WIN32
	FILE *file = fopen("C:\\Phones.txt","r");
#else
	FILE *file = fopen("/home/Phones.txt","r");
#endif
	if( file )
	{
		while(true)
		{
			// The file will be in the format:
			// Bluetooth ID \t MacAddress \t LinkQuality \t	IPAddress \t Port \n
			// Where IPAddress & Port is for testing purposes only with socket connection
			char Line[100]="";
			fgets(Line,100,file);
			string sLine(Line);

			string::size_type pos=0;
			string BlueToothID = StringUtils::Tokenize(sLine,"\t",pos);
			string MacAddress = StringUtils::Tokenize(sLine,"\t",pos);
			string LinkQuality = StringUtils::Tokenize(sLine,"\t",pos);
			string IPAddress = StringUtils::Tokenize(sLine,"\t",pos);
			string Port = StringUtils::Tokenize(sLine,"\t",pos);

			if ( MacAddress.length()>10 )
			{
				// See if this is a new device
				PLUTO_SAFETY_LOCK(mm,m_MapMutex);

				PhoneDevice *pDNew = new PhoneDevice(BlueToothID,MacAddress,atoi(LinkQuality.c_str()));

				PhoneDevice *pDExisting = m_mapPhoneDevice_Detected_Find(pDNew->m_iMacAddress);
				if( pDExisting && abs(pDExisting->m_iLinkQuality-pDNew->m_iLinkQuality)<10 )
				{
					// nothing to do
					delete pDNew;
					continue;
				}
				mm.Release();

				if( !pDExisting )
					Intern_NewDeviceDetected(pDNew);
				else
					Intern_SignalStrengthChanged(pDNew);
			}
			else
				break;
		}
		fclose(file);
	}

	Sleep(1000);  // Don't this run wild
	return !m_bAbortScanLoop;
}
