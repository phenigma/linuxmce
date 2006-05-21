#ifndef PostCreateOptions_h
#define PostCreateOptions_h

using namespace std;

class Database_pluto_main;
class Row_Device;

namespace DCE
{
	class OH_Orbiter;

	/** @brief
    */
    class PostCreateOptions
    {
		Database_pluto_main *m_pDatabase_pluto_main;
		class Router *m_pRouter;
		class Orbiter_Plugin *m_pOrbiter_Plugin;
	public:
		PostCreateOptions(Database_pluto_main *pDatabase_pluto_main,Router *pRouter);

		void PostCreateDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_AlarmPanel(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateSecurityDevice(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_NetworkStorage(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
		void PostCreateDevice_Cameras(Row_Device *pRow_Device, OH_Orbiter *pOH_Orbiter);
	};
}

#endif
