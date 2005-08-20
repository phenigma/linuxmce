#ifndef VFD_LCD_Base_h
#define VFD_LCD_Base_h

#define VL_MSGTYPE_STARTUP					1
#define VL_MSGTYPE_STATUS					2
#define VL_MSGTYPE_NOW_PLAYING_MAIN			3
#define VL_MSGTYPE_NOW_PLAYING_SECTION		4
#define VL_MSGTYPE_NOW_PLAYING_TIME_CODE	5
#define VL_MSGTYPE_NOW_PLAYING_SPEED		6
#define VL_MSGTYPE_RIPPING_NAME				7
#define VL_MSGTYPE_RIPPING_PROGRESS			8
#define VL_MSGTYPE_RUNTIME_ERRORS			9
#define VL_MSGTYPE_RUNTIME_NOTICES			10

class VFD_LCD_Message
{
public:
	string m_sMessage;
	time_t m_tExpireTime,m_tDisplayTime,m_tCreateTime;

	VFD_LCD_Message(string sMessage,int ExpireTime)
	{
		m_sMessage=sMessage;
		m_tCreateTime=time(NULL);
		m_tExpireTime = m_tCreateTime+ExpireTime;
		m_tDisplayTime=0;
	}
};

typedef map<string,VFD_LCD_Message *> MapMessages;

namespace DCE
{
	class VFD_LCD_Base
	{
	public:
		bool m_bVL_ThreadRunning;
		void RunThread();

	protected:
		pluto_pthread_mutex_t m_VL_MessageMutex;  // This will also protect the callback map
		pthread_cond_t m_VL_MessageCond;
		pthread_t m_ptVL_Thread;
		bool m_bQuit_VL;
		int m_iNumColumns,m_iNumLines;

		map<int,MapMessages *> m_mapMessages;
		MapMessages *m_mapMessages_Find(int iMessageType) { map<int,MapMessages *>::iterator it = m_mapMessages.find(iMessageType); return it==m_mapMessages.end() ? NULL : (*it).second; }
		MapMessages *m_mapMessages_Get(int iMessageType) { 
			MapMessages *pMapMessages = m_mapMessages_Find(iMessageType); 
			if( !pMapMessages )
			{
				pMapMessages = new MapMessages();
				m_mapMessages[iMessageType]=pMapMessages;
			}
			return pMapMessages;
		}
		bool ContainsMessagesOfType(int iMessageType) { MapMessages *p = m_mapMessages_Find(iMessageType); return p ? p->size()>0 : false; }

		VFD_LCD_Base(int iNumColumns,int iNumLines);
		~VFD_LCD_Base();

		virtual void NewMessage(int iMessageType,string sName,string sMessage,int ExpiresSeconds);
		virtual int UpdateDisplay();

		int DisplayErrorMessages();
		int DisplayNoticesMessages();
		int DisplayNowPlayingRippingMessages();
		int DisplayStatusMessages();
		void DisplayDate();

		virtual void DoUpdateDisplay(string sMessage) {} // A derived class is expected to implement this
	};

}
#endif
