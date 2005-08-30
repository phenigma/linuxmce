/*
 * $Id: common.c,v 1.6 2005/01/07 21:46:51 lordjaxom Exp $
 */

#include "common.h"
#include <vdr/plugin.h>

const std::string &SkinPath(void) 
{
	// should never change
	static std::string path = cPlugin::ConfigDirectory(PLUGIN_NAME_I18N);
	return path;
}

const char *ChannelNumber(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
  if (Channel) {
     if (!Channel->GroupSep())
        snprintf(buffer, sizeof(buffer), "%d%s", Channel->Number(), Number ? "-" : "");
     }
  else if (Number)
     snprintf(buffer, sizeof(buffer), "%d-", Number);
  return buffer;
}

const char *ChannelName(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel)
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL
			|| (ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	return buffer;
}

const char *ChannelShortName(const cChannel *Channel, int Number) 
{
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->ShortName(true));
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ',')) != NULL) {
		char *start = ptr + 1;
		if ((ptr = strchr(start, ';')) != NULL)
			*ptr = '\0';
		return start;
	} else if ((ptr = strchr(buffer, ';')) != NULL)
		*ptr = '\0';
#endif
	
	return buffer;
}
/*
const char *ChannelBouquet(const cChannel *Channel, int Number) {
	static char buffer[256];
	buffer[0] = '\0';
	if (Channel) 
#if VDRVERSNUM < 10315
		snprintf(buffer, sizeof(buffer), "%s", Channel->Name());
#else
		snprintf(buffer, sizeof(buffer), "%s", Channel->Provider());
#endif
	else if (!Number)
		snprintf(buffer, sizeof(buffer), "%s", tr("*** Invalid Channel ***"));

#if VDRVERSNUM < 10315
	char *ptr;
	if ((ptr = strchr(buffer, ';')) != NULL)
		return ptr + 1;
	else
		return "";
#else
	return buffer;
#endif
}
*/

bool StoppedTimer(const char *Name) 
{
	cTimer *timer = Timers.First();
	while (timer) {
		if (strcmp(Name, timer->File()) == 0)
			break;
		timer = Timers.Next(timer);
	}
	return timer == NULL || !timer->Recording();
}

const cRecording *GetRecordingByName(const char *Name)
{
	const cRecording *rec = Recordings.First();
	for (; rec != NULL; rec = Recordings.Next(rec)) {
		if (strcmp(rec->Name(), Name) == 0)
			return rec;
	}
	return NULL;
}

cxType TimeType(time_t Time, const std::string &Format) 
{
	static char result[1000];
	struct tm tm_r, *tm;
	tm = localtime_r(&Time, &tm_r);

	if (Time > 0) {
		if (Format.length() > 0) {
			strftime(result, sizeof(result), Format.c_str(), tm);
			
			cxType r = result;
			/*if      (Format.find("%s") != -1 || Format.find("%S") != -1
			      || Format.find("%Es") != -1 || Format.find("%ES") != -1
			      || Format.find("%Os") != -1 || Format.find("%OS") != -1)
				r.SetUpdate(1000);
			else if (Format.find("%M") != -1 || Format.find("%EM") != -1
			      || Format.find("%OM") != -1)
				r.SetUpdate(1000*60);*/

			return r;
		} else
			return Time;
	}
	return false;
}

cxType DurationType(uint Index, const std::string &Format) 
{
	static char result[1000];
	if (Index > 0) {
		if (Format.length() > 0) {
			uint update = 0;
			const char *ptr = Format.c_str(); 
			char *res = result;
			enum { normal, format } state = normal;
			int n = 0;
			int f = (Index % FRAMESPERSEC) + 1;
			int s = (Index / FRAMESPERSEC);
			int m = s / 60 % 60;
			int h = s / 3600;
			s %= 60;
			while (*ptr && res < result + sizeof(result)) {
				switch (state) {
				case normal:
					if (*ptr == '%')
						state = format;
					else
						*(res++) = *ptr;
					break;

				case format:
					switch (*ptr) {
					case 'H':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", h);
						break;

					case 'k':
						n = snprintf(res, sizeof(result) - (res - result), "% 2d", h);
						break;

					case 'M':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", m);
						update = 1000*60;
						break;

					case 'm':
						n = snprintf(res, sizeof(result) - (res - result), "%d", m + (h * 60));
						update = 1000*60;
						break;

					case 'S':
						n = snprintf(res, sizeof(result) - (res - result), "%02d", s);
						update = 1000;
						break;
					
					case 'f':
						n = snprintf(res, sizeof(result) - (res - result), "%d", f);
						update = 1000;
						break;

					case '%':
						n = 1;
						*res = '%';
						break;
					}
					res += n;
					state = normal;
					break;
				}
				++ptr;
			}

			cxType r = result;
			r.SetUpdate(update);
			return r;
		} else
			return (int)Index;
	}
	return false;
}

bool ParseVar(const char *Text, const char *Name, std::string &Value) 
{
	const char *ptr1, *ptr2;
	char *str;
	bool res = false;
	asprintf(&str, ",%s=", Name);
	if ((ptr1 = strstr(Text, str)) || (strncmp(ptr1 = Text, str + 1, strlen(str) - 1) == 0)) {
		if (ptr1 == Text)
			--ptr1;
		ptr1 += strlen(str);
		if ((ptr2 = strchr(ptr1, ',')) || (ptr2 = strchr(ptr1, ';'))) {
			int pos;
			Value = ptr1;
			Value.erase(ptr2 - ptr1);
			if (Value[0] == '"')
				SkipQuotes(Value);
			while ((pos = Value.find('$')) != -1)
				Value.replace(pos, 1, "{*}");
			res = true;
		}
	}
	free(str);
	return res;
}

bool ParseVar(const char *Text, const char *Name, tColor *Value) {
	std::string value;
	if (ParseVar(Text, Name, value) && value[0] == '#') {
		*Value = (tColor)strtoul(value.c_str() + 1, NULL, 16);
		return true;
	}
	return false;
}

void SkipQuotes(std::string &Value) {
	char quote = Value[0];
	int i;
	Value.erase(0, 1);
	for (i = 0; i < (int)Value.length() && Value[i] != quote; ++i) {
		if (Value[i] == '\\')
			Value.erase(i, 1);
	}
	if (Value[i] == quote)
		Value.erase(i, 1);
	else
		esyslog("ERROR: text2skin: missing closing %c", quote);
}
