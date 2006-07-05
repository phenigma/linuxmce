#ifndef WizardRemote_H_
#define WizardRemote_H_

// TODO: Write beautifully

#include <string>
using namespace std;

// Class that wraps around the Remote Control process
class RemoteProcess
{
	public:
		RemoteProcess();
		~RemoteProcess();
		void Start(string sCmd);

	private:
		string m_sCmd;
		int pid;
};

#endif
