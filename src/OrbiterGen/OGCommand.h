#ifndef CGCommand_h
#define CGCommand_h

class CGCommand : public DesignObjCommand
{
public:
	class Row_CommandGroup_Command * m_drCommandGroup_Command;
	class Row_CommandGroup_D_Command * m_drCommandGroup_D_Command;
//		Row_DesignObjVariation * m_drDesignObjVariation;
//	vector<class CGCommandParm *> m_alParms;
	class Row_Command * m_drCommand;
//	int m_PK_Criteria;

	CGCommand(int PK_Command,class DesignObj_Generator *Parent);
	CGCommand(class Row_CommandGroup_Command * drAGA,DesignObj_Generator *Parent);
	CGCommand(class Row_CommandGroup_D_Command * drAGA,DesignObj_Generator *Parent);
	void SharedConstructor(DesignObj_Generator *Parent);
	void AddParm(class Row_CommandGroup_Command_CommandParameter * drAGACP,class DesignObj_Generator *Parent);
	void AddParm(class Row_CommandGroup_D_Command_CommandParameter * drAGACP,class DesignObj_Generator *Parent);
};

class CGZone : public DesignObjZone
{
public:
	class Row_CommandGroup * m_drCommandGroup;
	class Row_CommandGroup_D * m_drCommandGroup_D;

	CGZone() 
	{
		m_drCommandGroup_D=NULL;
		m_drCommandGroup=NULL;
	}

	CGZone(class Row_CommandGroup * drCommandGroup,class DesignObj_Generator *Parent);
	CGZone(class Row_CommandGroup_D * drCommandGroup_D,class DesignObj_Generator *Parent);
};

#endif
