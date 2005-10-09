/*
 * $Id: common.h,v 1.4 2005/01/27 17:31:12 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_COMMON_H
#define VDR_TEXT2SKIN_COMMON_H

#include "xml/string.h"
#include <string>
#include <vdr/osd.h>
#include <vdr/config.h>

#if defined(DEBUG) || defined(BENCH)
#	ifdef DEBUG
#		define Dprintf(x...) fprintf(stderr, x)
#	else
#		define Dprintf(x...)
#	endif
#	define Dbench(x) uint64 bench_##x = time_ms()
#	define Ddiff(t,x) fprintf(stderr, "%s took %llu ms\n", t, time_ms() - bench_##x)
#else
#	define Dprintf(x...)
#	define Dbench(x)
#	define Ddiff(t,x)
#endif

#if VDRVERSNUM >= 10318
#	define time_ms() cTimeMs::Now()
#	define Apid1() Apid(0)
#	define Apid2() Apid(1)
#	define Dpid1() Dpid(0)
#	define Dpid2() Dpid(1)
#endif

/* Check if a channel is a radio station. */
#define ISRADIO(x) ((x)->Vpid()==0||(x)->Vpid()==1||(x)->Vpid()==0x1fff)

// class forwards

class cMarks;
class cChannel;
class cRecording;

// helper functions

const std::string &SkinPath(void);
const char *ChannelNumber(const cChannel *Channel, int Number);
const char *ChannelName(const cChannel *Channel, int Number);
const char *ChannelShortName(const cChannel *Channel, int Number);
//const char *ChannelBouquet(const cChannel *Channel, int Number);

bool StoppedTimer(const char *Name);
const cRecording *GetRecordingByName(const char *Name);

cxType TimeType(time_t Time, const std::string &Format);
cxType DurationType(uint Index, const std::string &Format);

bool ParseVar(const char *Text, const char *Name, std::string &Value);
bool ParseVar(const char *Text, const char *Name, tColor *Value);
	
void SkipQuotes(std::string &Value);

#endif // VDR_TEXT2SKIN_COMMON_H
