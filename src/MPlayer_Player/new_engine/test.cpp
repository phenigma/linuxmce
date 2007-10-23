#include <iostream>

#include "MPlayerEngine.h"
#include "Log.h"

using namespace std;

int main() {
	MPlayerEngine *pEngine = new MPlayerEngine();
	Log("Created MPlayerEngine object");

	if ( pEngine->StartEngine() ) {
		Log("Engine started successfully");
		/*
		for (int i=0; i<10; i++)
		{
			string sLine = pEngine->ReadLine();
			std::cout << "Line " << i << ": " << sLine << endl;
		}
		*/
		sleep(5);
		Log("Playing mp3...");
		pEngine->Command("loadfile /home/fenikso/Music/SinnerMan.mp3");
		sleep(3);
		string sAudioCodec = pEngine->Ask("get_audio_codec", "AUDIO_CODEC");
		Log("Detected audio codec: "+sAudioCodec);
		sleep(10);
	}
	else {
		Log("Engine failed to start");
	}

	Log("Destroying engine");
	delete pEngine;
}

