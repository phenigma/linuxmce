#ifndef FILETRANSFER_H
#define FILETRANSFER_H

struct FileInfo
{
	int Length;
	int M, D, Y, h, m, s;
};

typedef enum { FU_UPDATED, FU_SKIPPED, FU_NOFILE, FU_NOSERVER } FileUpdateResponses;

#endif

