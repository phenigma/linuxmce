#ifndef __POCKETFROG_WRAPPER_H__
#define __POCKETFROG_WRAPPER_H__

#include <PocketFrog.h>
using namespace Frog;

class PlutoGame : public Game
{
public:
	virtual void ButtonDown(int button);
	virtual void ButtonUp(int button);

	virtual bool PocketFrogButtonDown(int button) { return false;}
	virtual bool PocketFrogButtonUp(int button) { return false; }
};

#endif


