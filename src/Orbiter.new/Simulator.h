#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__
//------------------------------------------------------------------------------------------------------
#include "pthread.h"
#include <string>
#include <map>
using namespace std;
//------------------------------------------------------------------------------------------------------
namespace DCE
{

class Simulator
{
	static Simulator* m_pInstance;

	pthread_t m_GeneratorThreadID;

	Simulator(); //private constructor

	map<string,string> m_mapParameters;
	string m_mapParameters_Find(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it == m_mapParameters.end() ? NULL : (*it).second; }
	bool m_mapParameters_Exists(string Token) {	map<string,string>::iterator it = m_mapParameters.find( Token ); return it != m_mapParameters.end(); }

	inline int ReadInteger(string sToken, int iDefaultValue = 0);
	inline string ReadString(string sToken, string sDefaultValue = "");

public: //data

	class Orbiter *m_pOrbiter;

	bool m_bEnableGenerator;
	long m_dwDelayMin;
	long m_dwDelayMax;
	long m_dwTimeout;
	int m_iNumberOfButtonsPerClick;
	bool m_bGenerateMouseClicks;
	int m_iMilisecondsTransition;
	int m_iMilisecondsHighLight;
	bool m_bGenerateKeyboardEvents;
	int m_iKeysSetToGenerate;
	string m_sHomeScreen;

	bool m_bTryToDetermineAutomatically;
	string m_sDeviceID;
	string m_sRouterIP;

	bool m_bStopGeneratorThread;
	long m_dwStartGeneratorThreadDelay;

	bool m_bFullScreen;
	/** Enable/disable OpenGL drawing effects */
	bool m_bUseOpenGL;
	bool m_bLogToServer;

	bool m_bIsRunning;

	string m_sConfigurationFile;

public: //methods

	static Simulator *GetInstance();
	static void Cleanup();
	~Simulator();

	void LoadConfigurationFile(string sConfigurationFile);
	void SaveConfigurationFile(string sConfigurationFile = "");

	void StartRandomEventGenerator();
	void StopRandomEventGenerator();
	static bool IsRunning() { return NULL != m_pInstance && m_pInstance->m_bIsRunning; }

	static void SimulateActionDelay(long delay);

	int GetPreseededInstallValue(string Token);
	bool PreseededInstallValueDefined(string Token);

};
}
//------------------------------------------------------------------------------------------------------
#endif
