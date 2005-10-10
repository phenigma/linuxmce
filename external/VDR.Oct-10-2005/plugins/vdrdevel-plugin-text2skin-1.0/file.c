/*
 * $Id: file.c,v 1.1.1.1 2004/12/19 22:03:13 lordjaxom Exp $
 */

#include "file.h"
#include <unistd.h>

cText2SkinFile::cText2SkinFile(const char *Skin) {
	mSkin = Skin;
}

cText2SkinFile::~cText2SkinFile() {
}

bool cText2SkinFile::Load(const std::string &Filename) {
	bool result = true;
	if (access(Filename.c_str(), F_OK) == 0) {
		isyslog("text2skin: loading %s", Filename.c_str());
		FILE *f = fopen(Filename.c_str(), "r");
		if (f) {
			int line = 0;
			char buffer[MAXPARSEBUFFER];
			result = true;
			while (fgets(buffer, sizeof(buffer), f) > 0) {
				line++;
				char *ptr = skipspace(stripspace(buffer));
				if (!isempty(ptr) && ptr[0] != '#') {
					if (!Parse(ptr)) {
						esyslog("ERROR: error in %s, line %d\n", Filename.c_str(), line);
						result = false;
						break;
					}
				}
			}
			fclose(f);
		} else {
			LOG_ERROR_STR(Filename.c_str());
			result = false;
		}
	}
	return result;
}


