#ifndef ACTIONGROUP_H
#define ACTIONGROUP_H

typedef enum {
		agrt_none,agrt_CameraQuadView,agrt_CameraSingleView,
		agrt_SecurityPanel,agrt_UserStatus,agrt_MakeACall,agrt_NoCommands
} enumRecognizedCommandGroups;

class CommandGroup
{
public:
	int m_PK_CommandGroup;
	vector<class Command *> m_vectCommands;
	string m_Description;
	int m_PK_C_Array;
	enumRecognizedCommandGroups m_RecognizedTypes;

	CommandGroup(int PK_CommandGroup,int PK_C_Array=0) : 
		m_PK_CommandGroup(PK_CommandGroup), m_PK_C_Array(PK_C_Array), m_RecognizedTypes(agrt_none) {}

	~CommandGroup()
	{
		vector<class Command *>::iterator iA;

		for (iA = m_vectCommands.begin(); iA != m_vectCommands.end(); ++iA)
		{
			delete (*iA); 
		}
	}
};

#endif

