#ifndef FollowMe_Plugin_h
#define FollowMe_Plugin_h

namespace DCE
{
	/*	All Plug-ins that want to implement follow-me (Lighting, Media, Climate, Security and Telecom) are 
		derived from this base class and must implement ExecuteFollowMe which executes the actual 
		commands to move whatever is being moved.  This class implements HandleFollowMe which takes care of
		the common logic of determining when a user enters a room if he is the only one in the room, and the
		FollowMe_LeftRoom and FollowMe_EnteredRoom will happen immediately, or if someone else is in the room so 
		this user will be put in the queue and the FollowMe_LeftRoom will happen immediate, but the 
		FollowMe_EnteredRoom will take place after the user leaves.
	*/

	/*	When a user enters a room, but another orbiter is already linked to that room, we create an 
		instance of this so we can keep track of it.
	*/

	class PendingMove
	{
	public:
		char m_cType;
		class OH_Orbiter *pOH_Orbiter;
		class Room *pRoom_Prior,*pRoom_Current;
	};

	class FollowMe_Plugin
	{
	public:
		// Some things, like media, climate, etc. can only have 1 thing going.  Others like Telecom don't have that restriction
		bool m_bOneAtATime;
		/* Type is L, C, M, S, or T for the various types of follow-me */
		virtual void HandleFollowMe(class DeviceData_Router *pDevice_PriorDetector,char Type, class OH_Orbiter *pOH_Orbiter);

		/*	When a user enters another room that's already occupied and his move is put in the queue, if he then cancels
			follow-me, we need to remove any such pending moves */
		void CancelPendingMovesByType(class OH_Orbiter *pOH_Orbiter,char cType);
		virtual void FollowMe_EnteredRoom(class OH_Orbiter *pOH_Orbiter,class Room *pRoom_Prior,class Room *pRoom_Current)=0;
		virtual void FollowMe_LeftRoom(class OH_Orbiter *pOH_Orbiter,class Room *pRoom_Prior,class Room *pRoom_Current)=0;

		FollowMe_Plugin() { m_bOneAtATime=false; }
	};
}

#endif
