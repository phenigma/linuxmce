#ifndef OH_User_h
#define OH_User_h


namespace DCE
{
	// Store an orbiter list within the handler
	class OH_User
	{
	public:
		int m_iPK_Users;
		bool m_bFollowMe_Lighting,m_bFollowMe_Media,m_bFollowMe_Climate,m_bFollowMe_Telecom,m_bFollowMe_Security;

		OH_User(int iPK_Users)
		{
			m_iPK_Users = iPK_Users;
			m_bFollowMe_Lighting=m_bFollowMe_Media=m_bFollowMe_Climate=m_bFollowMe_Telecom=m_bFollowMe_Security=false;
		}
	};
}

#endif
