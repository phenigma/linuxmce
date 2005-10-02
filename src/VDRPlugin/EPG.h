#ifndef EPG_h
#define EPG_h

namespace VDREPG
{
	class Channel;  // A channel, like CNN
	class Event;  // An episode of a program, like 'The Simpsons' on 2005-09-01 at 11:00am.  'The Simpsons' is a class Program
	class Program; // A program, like 'The Simpsons'.  All events with the same Title are assumed to be the same program
	class Stream; // A data stream, such as 'dolby digital 5.1' or 'English'
	class EPG; // The master class containing the whole program guide

	class Channel
	{
	public:
		int m_ChannelID;
		string m_sChannelName,m_sFrequency;
		Event *m_pEvent_First,*m_pEvent_Last;

		Channel(char *szLine);
		Event *GetCurrentEvent();
	};

	// An instance of a show
	class Event
	{
	public:
		Channel *m_pChannel;
		Program *m_pProgram;
		Event *m_pEvent_Next,*m_pEvent_Prior;
		string m_sDescription_Short,m_sDescription_Long;
		int m_EventID;
		time_t m_tStartTime,m_tStopTime;
		int m_VPSTime;
		list<Stream *> m_listStream;

		Event(char *szLine,Channel *pChannel);
		string GetProgram();
	};

	// A show
	class Program
	{
	public:
		string m_sTitle;
		list<Event *> m_listEvent;

		Program(char *szLine) { m_sTitle=szLine; }
	};

	class Stream
	{
	public:
		string m_sFormat,m_sType,m_sLanguage,m_sDescription;
		Stream(string sLine) {} ;
	};

	class EPG
	{
	private:
		// These variables are used while reading in an epg file only
		Channel *m_pChannel_Reading;
		Event *m_pEvent_Reading;
		Program *m_pProgram_Reading;

	public:
		time_t m_tTime_First,m_tTime_Last;
		vector<Channel *> m_vectChannel;
		map<int,Event *> m_mapEvent;
		Event *m_mapEvent_Find(int EventID) { map<int,Event *>::iterator it = m_mapEvent.find(EventID); return it==m_mapEvent.end() ? NULL : (*it).second; }
		map<string,Program *> m_mapProgram;
		Program *m_mapProgram_Find(string sTitle) { map<string,Program *>::iterator it = m_mapProgram.find(sTitle); return it==m_mapProgram.end() ? NULL : (*it).second; }

		EPG() { m_tTime_First=m_tTime_Last=0; };
		~EPG();
		bool ReadFromFile(string sFile); // Read the epg data from a file
		void ProcessLine(char *szLine);  // Process a line within that file
	};
}


#endif

