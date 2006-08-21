class SMPTEGen
{
	int m_SampleSize, m_BufferCount, m_BufferSize;
	char *m_pBuffer, *m_pCurBuffer;
	bool m_bFlipFlop;

	void AddPulse(bool b);
	void AddBit(bool b);
	void AddByte(int Byte);

	void ClearBuffer();

public:
	SMPTEGen(int BufferCount=1 , int SampleSize=1); // Buffer 
	virtual ~SMPTEGen();

	bool AddSMPTE(long SMPTE); // Returns TRUE when buffer is full
	char *GetAndClearBuffer(int &length);  // Returns the current buffer and begins a new one.
	char *GetSMPTE(long SMTPE, int &length); // Most useful with single-slice buffer, just gets and returns one smpte frame

	static long FromTimecode(const char *s);
};

