#ifndef EmailLogger_H
#define EmailLogger_H

namespace DCE
{

	class EMailLogger : public Logger
	{
	public:

		EMailLogger(string to, string from, string subject);
		virtual ~EMailLogger();

		static EMailLogger* Parse(TokenStream& ts, StringStringMap& vars);

		virtual void WriteEntry(class Logger::Entry& entry);

	protected:
		string m_To;
		string m_From;
		string m_Subject;
		string m_Server;
	};
}

#endif
