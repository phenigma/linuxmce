#ifndef RENDERERMNG_H
#define RENDERERMNG_H

#include "RendererImage.h"

#include <vector>
using namespace std;

extern const char * PNGsignature;
extern const char * MNGsignature;

class MNGHeader;
class PNGChunk;

unsigned long & ReverseEndian(unsigned long & N);
void ParseMNGHeader(const char * RawData, MNGHeader * Header);
bool ReadChunk(FILE * f, PNGChunk * chunk);

union ByteAccessLong
{
	unsigned long ulValue;
	unsigned char ucBytes[4];
};

class PNGChunk
{
	public:
		string type;
		unsigned long length;
		char * data;
		unsigned long crc32;

		PNGChunk() : type(""), length(0), data(NULL), crc32(0) {}
		~PNGChunk() { clear(); }

		void clear() { if (data) delete [] data; length = 0; }
};

class MNGHeader
{
	public:
		unsigned long frame_width, frame_height, ticks_per_second;
		unsigned long nominal_layer_count, nominal_frame_count, nominal_play_time, simplicity_profile;

		MNGHeader() : frame_width(0), frame_height(0), ticks_per_second(0),
			nominal_layer_count(0), nominal_frame_count(0), nominal_play_time(0), simplicity_profile(0) {}

		MNGHeader(const char * RawData)
		{
			ParseMNGHeader(RawData, this);
		}

		size_t BinaryForm(char * & MemoryZonePointer) const;
};

class PNGCatChunks
{
	private:
		vector<const PNGChunk *> m_Chunks;

	public:
		PNGCatChunks() {}
		~PNGCatChunks();

		bool AddChunk(const PNGChunk * Chunk);
		size_t CatChunks(char * & MemoryZonePointer) const;
		const PNGChunk *GetChunk(int i) const { return (i < count() ? m_Chunks[i] : NULL); };
		void clear();
		size_t count() const;
};

class InMemoryMNG
{
public:
	vector<PNGCatChunks *> m_vectMNGframes;
	MNGHeader *m_pMNGHeader;

	InMemoryMNG();
	~InMemoryMNG();
	size_t GetFrame(int iIndex, char *&pData);
};

class RendererMNG
{
	private:
		vector<RendererImage *> m_vectMNGframes;
		MNGHeader m_MNGHeader;

	public:
		RendererMNG();
		RendererMNG(int Width, int Height);
		~RendererMNG();

		size_t count() const;
		RendererImage *GetFrame(size_t number) const;

		void ReplaceFrame(size_t number, RendererImage * frame);
		void InsertFrame(size_t number, RendererImage * frame);
		void AppendFrame(RendererImage * frame);
		void DeleteFrame(size_t number);

		const MNGHeader & GetHeader() const;
		void SetHeader(const MNGHeader & Header);
};

#endif
