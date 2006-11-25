#include "BackEndFactory.h"

#include <stdlib.h>

#include "SDLFrontEnd.h"
#include "OpenGLFrontEnd.h"

/*static*/ BackEndFactory* BackEndFactory::Instance = NULL;

BackEndFactory::BackEndFactory(void)
{
	CurrentBackEnd = NULL;
}

BackEndFactory::~BackEndFactory()
{
}


BackEndFactory* BackEndFactory::GetInstance()
{
	if(!Instance)
		Instance = new BackEndFactory();
	return Instance;
}

void BackEndFactory::CleanUp()
{
	delete Instance;
	Instance = NULL;
}

GenericBackEnd* BackEndFactory::CreateBackEnd(int Type)
{
	switch(Type) {
	case Default:
		return new OpenGLFrontEnd();
	case Simple:
		return new SDLFrontEnd();
		break;
	}
	return new SDLFrontEnd();
}
