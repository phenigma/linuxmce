#ifndef FollowMe_Plugin_h
#define FollowMe_Plugin_h

namespace DCE
{
	/*	All Plug-ins that want to implement follow-me (Lighting, Media, Climate, Security and Telecom) are 
		derived from this base class and must implement ExecuteFollowMe which executes the actual 
		commands to move whatever is being moved.  This class implements HandleFollowMe which takes care of
		the common logic of determining when a user enters a room if he is the only one in the room, and the
		ExecuteFollowMe will happen immediately, or if someone else is in the room so this user will be put
		in the queue and the execute will take place after the user leaves.
	*/

	class FollowMe_Plugin
	{
	public:
		virtual void HandleFollowMe(class DeviceData_Router *pDevice_PriorDetector,class OH_Orbiter *pOH_Orbiter);
		virtual void ExecuteFollowMe(OH_Orbiter *pOH_Orbiter,class Room *pRoom_Prior,class Room *pRoom_Current)=0;
	};
}

#endif
