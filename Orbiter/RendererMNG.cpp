#include "RendererMNG.h"

#include <zlib.h>

const char * PNGsignature = "\x89PNG\x0D\x0A\x1A\x0A";
const char * MNGsignature = "\x8AMNG\x0D\x0A\x1A\x0A";

unsigned long & ReverseEndian(unsigned long & N)
{
	ByteAccessLong X, Y;

	// TODO: Detect byte order
	X.ulValue = N;
	for (int i = 0; i <= 3; i++)
	{
		Y.ucBytes[i] = X.ucBytes[3-i];
	}

	N = Y.ulValue;

	return N;
}

void ParseMNGHeader(const char * RawData, MNGHeader * Header)
{
	const unsigned long * Data = (const unsigned long *) RawData;

	ReverseEndian(Header->frame_width = * Data++);
	ReverseEndian(Header->frame_height = * Data++);
	ReverseEndian(Header->ticks_per_second = * Data++);
	ReverseEndian(Header->nominal_layer_count = * Data++);
	ReverseEndian(Header->nominal_frame_count = * Data++);
	ReverseEndian(Header->nominal_play_time = * Data++);
	ReverseEndian(Header->simplicity_profile = * Data++);
}

bool ReadChunk(FILE * f, PNGChunk * chunk)
{
	size_t bytes;
	char buffer[1024];

	// Data length
	if (fread(&chunk->length, 1, 4, f) < 4)
		return false;

	ReverseEndian(chunk->length);

	// Chunk type code
	bytes = fread(buffer, 1, 4, f);
	if (bytes < 4)
		return false;
	buffer[bytes] = 0;
	chunk->type = buffer;

	// Chunk binary data
	chunk->data = new char[chunk->length];
	if (fread(chunk->data, 1, chunk->length, f) < chunk->length)
		return false;

	// Chunk CRC
	if (fread(&chunk->crc32, 1, 4, f) < 4)
		return false;

	return true;
}

size_t MNGHeader::BinaryForm(char * & MemoryZonePointer) const
{
	const chunk_size = 40;

	MemoryZonePointer = new char[chunk_size];
	char * Position = MemoryZonePointer;
	
	unsigned long length = 28;
	ReverseEndian(length);

	unsigned long W, H, T, L, F, P, S;
	W = frame_width;
	H = frame_height;
	T = ticks_per_second;
	L = nominal_layer_count;
	F = nominal_frame_count;
	P = nominal_play_time;
	S = simplicity_profile;

	ReverseEndian(W);
	ReverseEndian(H);
	ReverseEndian(T);
	ReverseEndian(L);
	ReverseEndian(F);
	ReverseEndian(P);
	ReverseEndian(S);

	memcpy(Position, &length, 4); Position += 4;
	memcpy(Position, "MHDR", 4); Position += 4;
	memcpy(Position, &W, 4); Position += 4;
	memcpy(Position, &H, 4); Position += 4;
	memcpy(Position, &T, 4); Position += 4;
	memcpy(Position, &L, 4); Position += 4;
	memcpy(Position, &F, 4); Position += 4;
	memcpy(Position, &P, 4); Position += 4;
	memcpy(Position, &S, 4); Position += 4;

	unsigned long CRC32;
	CRC32 = crc32(0, Z_NULL, 0);
	CRC32 = crc32(CRC32, (const Bytef *) "MHDR", 4);
	CRC32 = crc32(CRC32, (const Bytef *) &W, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &H, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &T, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &L, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &F, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &P, 4);
	CRC32 = crc32(CRC32, (const Bytef *) &S, 4);

	memcpy(Position, &CRC32, 4); Position += 4;

	return chunk_size;
}

bool PNGCatChunks::AddChunk(const PNGChunk & Chunk)
{
	if (count() == 0 && Chunk.type != "IHDR")
		return false; // given chunk is not the first one

	if (m_Chunks[count() - 1].type == "IEND")
		return false; // PNG already complete

	m_Chunks.insert(m_Chunks.end(), Chunk);
	return true;
}

size_t PNGCatChunks::CatChunks(char * & MemoryZonePointer) const // it is the user's responsibility deleting this pointer
{
	size_t Size = 0;
	const int Chunk_Extra = 3 * 4;

	for (size_t i = 0; i < count(); i++)
	{
		Size += m_Chunks[i].length + Chunk_Extra; // data length plus 3 32bit chunk values
	}

	MemoryZonePointer = new char[Size];
	char * Position = MemoryZonePointer;

	for (size_t i = 0; i < count(); i++)
	{
		unsigned long length = m_Chunks[i].length;
		unsigned long crc32 = m_Chunks[i].crc32;

		ReverseEndian(length);
		ReverseEndian(crc32);

		memcpy(Position, &length, 4); Position += 4;
		memcpy(Position, m_Chunks[i].type.c_str(), 4); Position += 4;
		memcpy(Position, m_Chunks[i].data, m_Chunks[i].length); Position += m_Chunks[i].length;
		memcpy(Position, &crc32, 4); Position += 4;
	}

	return Size;
}

size_t PNGCatChunks::count() const
{
	return m_Chunks.size();
}

void PNGCatChunks::clear()
{
	m_Chunks.clear();
}

RendererMNG::RendererMNG()
{
}

RendererMNG::~RendererMNG()
{
	m_vectMNGframes.clear();
}

size_t RendererMNG::count() const
{
	return m_vectMNGframes.size();
}

RendererImage RendererMNG::GetFrame(size_t number) const
{
	if (number >= count())
		return RendererImage();

	return m_vectMNGframes[number];
}

void RendererMNG::ReplaceFrame(size_t number, const RendererImage & frame)
{
	if (number >= count())
		return;

	m_vectMNGframes[number] = frame;
}

void RendererMNG::InsertFrame(size_t number, const RendererImage & frame)
{
	if (number >= count())
		return;
	m_vectMNGframes.insert(&m_vectMNGframes[number], frame);
}

void RendererMNG::AppendFrame(const RendererImage & frame)
{
	m_vectMNGframes.insert(m_vectMNGframes.end(), frame);
}

void RendererMNG::DeleteFrame(size_t number)
{
	if (number >= count())
		return;

	m_vectMNGframes.erase(&m_vectMNGframes[number]);
}

const MNGHeader & RendererMNG::GetHeader() const
{
	return m_MNGHeader;
}

void RendererMNG::SetHeader(const MNGHeader & Header)
{
	m_MNGHeader = Header;
}
