#ifndef XMLTV_H
#define XMLTV_H

extern int XMLTVcounter_PK_Schedule,XMLTVcounter_PK_Program,XMLTVcounter_PK_Station;

class XMLTVStation
{
public:
	int m_Channel,m_iPK_Station;
	class XMLTVSchedule *m_pFirstSchedule;
	class XMLTVStation *m_pNextStation;
	class XMLTVProvider *m_pProvider;
	class EPGChannel *m_pEPGChannel;
	string m_sDescription;
	string m_sID;
	int m_PK_Provider_Station;

	XMLTVStation(class XMLTVProvider *pProvider);
};

// An instance of a show
class XMLTVSchedule
{
public:
	class XMLTVStation *m_pStation;
	class XMLTVSchedule *m_pNextSchedule;
	class XMLTVProgram *m_pProgram;
	time_t tStartDate,tEndDate;
	string m_sDescription,m_sSubtitle;
	int m_iPK_Schedule;
	class EPGProvider_Station__Schedule *m_pProvider_Station__Schedule;

	XMLTVSchedule() : m_pStation(NULL), m_pNextSchedule(NULL), m_pProgram(NULL), m_iPK_Schedule(++XMLTVcounter_PK_Schedule),
		m_pProvider_Station__Schedule(NULL) {}
};

// A show
class XMLTVProgram
{
public:
	string m_sTitle;
	list<XMLTVSchedule *> m_listSchedule;
	int m_iPK_Program;

	XMLTVProgram() : m_iPK_Program(++XMLTVcounter_PK_Program) {}
	~XMLTVProgram();
};

class XMLTVProvider
{
private:
	class XMLTVStation *pTempStation;  // Just for temporary use

public:
	map<int,XMLTVStation *> mapXMLTVStationsByNumber;
	map<string,XMLTVStation *> mapXMLTVStationsByID;
	map<string,XMLTVProgram *> mapXMLTVPrograms;
	class XMLTVStation *m_pFirstStation;
	bool m_bValid;
	int m_iPK_Provider;
	class PlutoServerCore *m_pCore;

	XMLTVProvider(int PK_Provider,string Filename,class PlutoServerCore *pCore);
	~XMLTVProvider();
	void ProcessStation(string *s,list<string *> &listChannel,list<string *>::iterator &itLines);
	void ProcessProgram(string *s,list<string *> &listChannel,list<string *>::iterator &itLines);
	void CalculateDates(XMLTVSchedule *pSchedule,string Start,string Stop);
	time_t CalculateDate(string Date);
};

#endif

