#ifndef BufferedLogger_H
#define BufferedLogger_H

namespace DCE
{

	class BufferedLogger : public MultiLogger, public AlarmEvent
	{
	public:
		BufferedLogger();
		virtual ~BufferedLogger();

		virtual void WriteEntry(class Logger::Entry& entry);

		virtual void Flush();
		virtual void Start();

		static BufferedLogger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

	protected:
		virtual void AlarmCallback(int id, void* param);
		static int ParseTimeSpec(::std::string str);

		typedef ::std::queue<Logger::Entry> LogEntryQueue;

		size_t m_MaxEntries;
		LogEntryQueue m_Queue;
		int m_FlushDelay;

		AlarmManager* m_AlarmManager;
	};
}

#endif
