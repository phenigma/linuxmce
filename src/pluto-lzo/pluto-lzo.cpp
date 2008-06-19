#include "PlutoUtils/LZO.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/PlutoDefs.h"

#include <iostream>
using namespace std;

int Usage(char *prg)
{
	cout << "Usage: " << prg << " {c|d} source_file dest_file" << endl;
	return 1;
}

int main(int argc, char *argv[])
{
	if (argc != 4)
		return Usage(argv[0]);

	if (argv[1][0] == 'c') // compress
	{
		string sData;
		FileUtils::ReadTextFile(argv[2], sData);

		long iSize = 0;
		char *pCompressedData = LZO::Compress((lzo_byte *)sData.c_str(), (lzo_uint)sData.size(), (lzo_uint *)&iSize);
		FileUtils::WriteBufferIntoFile(argv[3], pCompressedData, iSize);
		delete [] pCompressedData;
	}
	else if (argv[1][0] == 'd') // decompress
	{
		size_t nCompressedDataSize = 0;
		char *pCompressedData = FileUtils::ReadFileIntoBuffer(argv[2], nCompressedDataSize);
		long size;
		char *pUncompressedData = LZO::Decompress((lzo_byte *)pCompressedData, (lzo_uint)nCompressedDataSize, (lzo_uint *)&size);
		FileUtils::WriteBufferIntoFile(argv[3], pUncompressedData, size);
		delete [] pUncompressedData;
		PLUTO_SAFE_DELETE_ARRAY(pCompressedData);
	}
	else
		return Usage(argv[0]);

	return 0;
}
