/*
 * $Id: loader.c,v 1.3 2005/01/02 19:56:05 lordjaxom Exp $
 */

#include "loader.h"
#include "i18n.h"
#include "theme.h"
#include "display.h"
#include "text2skin.h"
#include "xml/parser.h"
#include "xml/skin.h"
#include <vdr/plugin.h>
#include <sys/types.h>
#include <dirent.h>

void cText2SkinLoader::Start(void) {
	DIR *d = opendir(SkinPath().c_str());
	if (d) {
		struct dirent ent;
		struct dirent *result;
		while ((readdir_r(d, &ent, &result)) == 0 && result != NULL) {
			char *path;
			struct stat buf;
			if (strcmp(result->d_name, ".") == 0 || strcmp(result->d_name, "..") == 0)
				continue;
			asprintf(&path, "%s/%s", SkinPath().c_str(), result->d_name);
			if (stat((SkinPath() + "/" + result->d_name).c_str(), &buf) == 0 
					&& S_ISDIR(buf.st_mode))
				Load(result->d_name);
			free(path);
		}
		closedir(d);
	}
}

void cText2SkinLoader::Load(const char *Skin) {
	cText2SkinI18n *translations = NULL;
	std::string transfile = SkinPath() + "/" + Skin + "/" + Skin + ".trans";
	if (access(transfile.c_str(), F_OK) == 0) {
		translations = new cText2SkinI18n(Skin);
		if (!translations->Load(transfile))
			DELETENULL(translations);
	}

	cText2SkinTheme *theme = new cText2SkinTheme(Skin);
	std::string themefile = SkinPath() + "/" + Skin + "/" + Skin + ".colors";
	theme->Load(themefile);
	
	std::string skinfile = SkinPath() + "/" + Skin + "/" + Skin + ".skin";
	if (access(skinfile.c_str(), F_OK) == 0) {
		isyslog("parsing %s", skinfile.c_str());

		cxSkin *skin = xmlParse(Skin, skinfile, translations, theme);
		if (skin) {
			if (skin->Version() == cText2SkinPlugin::SkinVersion()) {
				new cText2SkinLoader(skin, translations, theme, Skin, skin->Title());
				return;
			} else
				esyslog("ERROR: text2skin: Skin is version %s, expecting %s", skin->Version().c_str(), 
				        cText2SkinPlugin::SkinVersion());
		} else
			esyslog("ERROR: error in skin file");
		delete skin;
	}
}

cText2SkinLoader::cText2SkinLoader(cxSkin *Data, cText2SkinI18n *I18n, cText2SkinTheme *Theme, 
                                   const std::string &Skin, const std::string &Description): 
		cSkin(Skin.c_str(), Theme->Theme()),
		mData(Data),
		mI18n(I18n),
		mTheme(Theme),
		mDescription(Description)
{
}

cText2SkinLoader::~cText2SkinLoader() 
{
	delete mData;
	delete mI18n;
	delete mTheme;
}

cSkinDisplayChannel *cText2SkinLoader::DisplayChannel(bool WithInfo) 
{
	return new cText2SkinDisplayChannel(this, WithInfo);
}

cSkinDisplayMenu *cText2SkinLoader::DisplayMenu(void) 
{
	return new cText2SkinDisplayMenu(this);
}

cSkinDisplayVolume *cText2SkinLoader::DisplayVolume(void) 
{
	return new cText2SkinDisplayVolume(this);
}

cSkinDisplayReplay *cText2SkinLoader::DisplayReplay(bool ModeOnly) 
{
	return new cText2SkinDisplayReplay(this, ModeOnly);
}

cSkinDisplayMessage *cText2SkinLoader::DisplayMessage(void) 
{
	return new cText2SkinDisplayMessage(this);
}

#if VDRVERSNUM >= 10318
cSkinDisplayTracks *cText2SkinLoader::DisplayTracks(const char *Title, int NumTracks, 
                                                    const char * const *Tracks)
{
	return new cText2SkinDisplayTracks(this, Title, NumTracks, Tracks);
}
#endif
