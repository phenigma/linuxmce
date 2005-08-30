/*
 * $Id: status.h,v 1.7 2005/06/03 08:53:13 lordjaxom Exp $
 */
 
#ifndef VDR_TEXT2SKIN_STATUS_H
#define VDR_TEXT2SKIN_STATUS_H

#include "common.h"
#include <vdr/status.h>

class cText2SkinRender;

class cText2SkinStatus: public cStatus {
public:
	enum eReplayMode {
		replayNone,
		replayNormal,
		replayMP3,
		replayMPlayer,
		replayDVD,
		replayVCD,
		replayImage,

#define __REPLAY_COUNT__ (cText2SkinStatus::replayImage+1)
	};

	typedef std::string tRecordingInfo;
	typedef std::vector<tRecordingInfo> tRecordings;

private:
	cText2SkinRender *mRender;
	eReplayMode       mReplayMode;
	bool              mReplayIsLoop;
	bool              mReplayIsShuffle;
	tRecordings       mRecordings;
	cMutex            mRecordingsLock;
	uint              mCurrentRecording;
	uint              mNextRecording;
	int               mLastLanguage;

protected:
	virtual void Replaying(const cControl *Control, const char *Name);
	virtual void Recording(const cDevice *Device, const char *Name);
	virtual void OsdClear(void);

public:
	cText2SkinStatus(void);

	void SetLanguage(int Language) { mLastLanguage = Language; }
	void SetRender(cText2SkinRender *Render);

	cxType GetTokenData(const txToken &Token);

	eReplayMode ReplayMode(void) const { return mReplayMode; }
};

extern cText2SkinStatus Text2SkinStatus;

#endif // VDR_TEXT2SKIN_STATUS_H
