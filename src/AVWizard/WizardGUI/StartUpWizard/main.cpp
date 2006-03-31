#include <stdio.h>

#include "Wizard.h"

int main()
{
#ifdef DEBUG
	std::cout<<"Before start"<<std::endl;
#endif
	Wizard m_Wizard;

#ifdef DEBUG
	std::cout<<"SDL"<<std::endl;
#endif
	m_Wizard.StartSDLVideoMode(640, 480, false);	
#ifdef DEBUG
	std::cout<<"Before Loop"<<std::endl;
#endif
	m_Wizard.MainLoop();	

	return 0;
}