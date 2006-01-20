#ifndef MultiLogger_H
#define MultiLogger_H

class MultiLogger : public Logger
{
public:
	MultiLogger();
	virtual ~MultiLogger();
	
	virtual void WriteEntry(class Logger::Entry& entry);
	virtual void Start();
	static Logger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

	void AddOutput(Logger* logger);
	
protected:
	typedef ::std::vector<Logger*> LoggerVector;
	LoggerVector m_Loggers;
};

#endif
