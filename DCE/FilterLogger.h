#ifndef FilterLogger_H
#define FilterLogger_H

class FilterLogger : public MultiLogger
{
public:
	FilterLogger();
	virtual ~FilterLogger();

	virtual void WriteEntry(class Logger::Entry& entry);

	static FilterLogger* Parse(TokenStream& ts, NameLoggerMap& targets, StringStringMap& vars);

protected:
	bool LevelMatch(int Level);
	bool ParseLevels(::std::string str);
	
	::std::set<int> m_Levels;
	int m_GreaterThan;
	int m_LessThan;
	bool m_Invert;
};

#endif
