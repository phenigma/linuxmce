#ifndef BackEndFactory_H_
#define BackEndFactory_H_

#include "GenericBackEnd.h"

/**
 *	Default backend = OpenGL one
 *	Simple one = SDL 2 one
 */

enum BackEndType
{
	Default,
	Simple
};



class BackEndFactory
{
	BackEndFactory();

	static BackEndFactory* Instance;

	GenericBackEnd* CurrentBackEnd;
public:
	~BackEndFactory();

	static BackEndFactory* GetInstance();

	void CleanUp();

	GenericBackEnd* CreateBackEnd(int Type);




};

#endif