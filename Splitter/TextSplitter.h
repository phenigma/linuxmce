#ifndef TEXT_SPLITTER
#define TEXT_SPLITTER

#include <string>
#include <list>
using namespace std;

typedef string Word;
typedef list<Word> Row;

class TextLineExtract
{
	protected:
		bool lineSplitter(char c);
		bool wordSplitter(char c);
		void AddRow(Row line);

	public:
		list<Row> lines;
		list<Row> & Extract(string text);
		void Clear();
		void AddWord(Row & line, Word word);
};

#endif
