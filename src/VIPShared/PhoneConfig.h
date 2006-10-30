#ifndef PlutoConfig_H
#define PlutoConfig_H

class PhoneConfig {
public:
	typedef enum { mmImage=0, mmMenu } MenuMode;
protected:
	MenuMode m_MenuMode;
public:	
	PhoneConfig() { m_MenuMode = mmImage; }
	PhoneConfig( PhoneConfig& cfg ) { (*this)=cfg; }
	PhoneConfig& operator=( PhoneConfig& cfg ) { 
		m_MenuMode = cfg.m_MenuMode;
		return *this;
	}

	MenuMode GetMenuMode( ) { return m_MenuMode; }
	void SetMenuMode( MenuMode val ) { m_MenuMode = val; }

	void Write( SerializeClass& sc ){
		sc.Write_long( m_MenuMode );
	}
	void Read( SerializeClass& sc )
	{
		m_MenuMode = (MenuMode)sc.Read_long();
	}

};

#endif 

