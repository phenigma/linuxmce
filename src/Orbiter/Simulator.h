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
	int m_iNumberOfButtonsPerClick;
	bool m_bGenerateMouseClicks;
	bool m_bGenerateKeyboardEvents;
	int m_iKeysSetToGenerate;
	string m_sHomeScreen;

	bool m_bStopGeneratorThread;
	long m_dwStartGeneratorThreadDelay;

public: //methods

	static Simulator *GetInstance();
	~Simulator();

	void LoadConfigurationFile(string sConfigurationFile);
	void SaveConfigurationFile(string sConfigurationFile);

	void StartRandomEventGenerator(long dwDelay = 0);
	void StopRandomEventGenerator();

	static void SimulateActionDelay(long delay);
};
}
//------------------------------------------------------------------------------------------------------
#endif
