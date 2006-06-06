#include "TextSplitter.h"

#include <iostream>
using namespace std;

bool TextLineExtract::lineSplitter(char c)
{
	static string splitters = "lcrtmbW";

	for (string::iterator i = splitters.begin(); i != splitters.end() ; i++)
		if (* i == c)
			return true;
	return false;
}

/**
 * iulian.d: This function is locale-dependent, it won't work with
 * French characters, for example. It's also not Unicode-aware;
 * it should work with std::wstring and wchar_t
 * 
 */ 
bool TextLineExtract::wordSplitter(char c)
{
	if (
		(c >= 'a' && c <='z')	  || 
		(c >= 'A' && c <= 'Z')	  || 
		(c >= '0' && c <= '9')    ||
		(c == '\'') || (c == '"') ||
		(c == '(')  || (c == ')') ||  
		(c == '[')  || (c == ']')  
		//|| c == ',' || c == '\'' || c == '(' || c == ')' || c == '/' 
		//|| c == '?' || c == '!'  || c == ';' || c == '"' || c == '.'
		//|| c == '[' || c == ']'  || c == '-' || c == '=' || c == '_'
	)
	{
		return false;
	}
	return true;
}

void TextLineExtract::AddRow(Row line)
{
	if (line.size() > 0)
	{
		lines.insert(lines.end(), line);
	}
}

void TextLineExtract::AddWord(Row & line, Word word)
{
	if (word.length() > 0)
	{
		line.insert(line.end(), word);
	}
}

list<Row> & TextLineExtract::Extract(string text)
{
	string flags = "";
	Word word = "";
	Row line;
	char c;
	bool inflags = false, splitter = true;

	for (string::iterator i = text.begin(); i != text.end(); i++)
	{
		c = * i;
		if (c == '\r')
			continue;

		if (c == '~')
		{
			inflags = !inflags;
			if (!inflags) //just existed a flags section
				word += '~';
			AddWord(line, word);
			word = "";
		}
		
		if (inflags)
		{
			if (lineSplitter(c))
			{
				AddRow(line);
				line.clear();
			}
			word += c;
		}
		else if (c >= ' ' && c != '~')
		{
			if( word.size() > 0 || !isspace(c) ) /* Avoid adding space characters in front of words */
			{
				word += c;
			}
		}
		else if (c == '\n')
		{
			AddWord(line, word);
			word = "";
			AddRow(line);
			line.clear();
		}
		
		if (!inflags && wordSplitter(c))
		{
			//if (! splitter)
			{
				AddWord(line, word);
				//splitter = true;
				word = "";
			}
		}
		else
		{
			//splitter = false;
		}
	}
	AddWord(line, word);
	AddRow(line);

	return lines;
}

void TextLineExtract::Clear()
{
	lines.clear();
}
