// ZWaveTest.cpp : Defines the entry point for the console application.
//

#include "DCE/Logger.h"
#include "DCE/ServerLogger.h"
#include "Serial/SerialPort.h" 
using namespace DCE;

namespace DCE
{
	Logger *g_pPlutoLogger = NULL;
}

void *MyFunc(void *)
{
	g_pPlutoLogger->Write(LV_WARNING, "Intr-un thread, MyFunc %d", 4);

	try
	{
		CSerialPort serprt("COM1", 9600, epbsN81); 
		char mybuf[100];
		serprt.Read(mybuf, 100);
	}
	catch(...)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Invalid port.");	
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	g_pPlutoLogger = new FileLogger("ZWaveText.log");

	pthread_t t;
	pthread_create(&t, NULL, &MyFunc, NULL); 

	Sleep(10000);

	return 0;
}


