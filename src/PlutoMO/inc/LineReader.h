#ifndef __LINEREADER_H
#define __LINEREADER_H

#include <f32file.h>

class CAsciiLineReader : public CBase
	{
public:
	void OpenL(TFileName aFileName);
	TBool ReadL(TPtrC8& aDes, TInt& aFilePos);
	void Close();
private:
	TBuf8<256> iLineBuffer;
	TPtrC8 iPtr;
	TBool iEof;
	RFile iFile;
	RFs iSession;
	TInt iFilePos;
	};

#endif