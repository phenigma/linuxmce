/*
 * $Id: i18n.c,v 1.4 2005/05/30 09:23:41 lordjaxom Exp $
 */

#include "i18n.h"
#include <vdr/config.h>

const tI18nPhrase Phrases[] = {
	/*
  { "English",
    "Deutsch",
    "Slovenski",
    "Italiano",
    "Nederlands",
    "Português",
    "Français",
    "Norsk",
    "suomi", // this is not a typo - it's really lowercase!
    "Polski",
    "Español",
    "ÅëëçíéêÜ", // Greek
    "Svenska",
    "Românã",
    "Magyar",
    "Català",
    "ÀãááÚØÙ", // Russian
    "Hrvatski",
    "Eesti",
    "Dansk",
  },
	*/
  { "Loader for text-based skins",
    "Lader für textbasierte Skins",
    "",
    "",
    "",
    "",
    "",
    "",
    "Helposti muokattavat ulkoasut",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Flush image cache",
    "Bildspeicher leeren",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tyhjennä kuvat välimuistista",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Flushing image cache...",
    "Bildspeicher wird geleert...",
    "",
    "",
    "",
    "",
    "",
    "",
    "Tyhjennetään välimuistia...",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Max. image cache size",
    "Max. Größe des Bildspeichers",
    "",
    "",
    "",
    "",
    "",
    "",
    "Välimuistin maksimikoko",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
  { "Skin too large or incorrectly aligned",
    "Skin zu groß oder nicht korrekt ausgerichtet",
    "",
    "",
    "",
    "",
    "",
    "",
    "Ulkoasu on liian suuri tai väärin asemoitu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
  },
	{ NULL }
};

cText2SkinI18n::cText2SkinI18n(const char *Skin): cText2SkinFile(Skin) {
	mIdentity   = (std::string)"text2skin_" + Skin;
	mNumPhrases = 0;
	mPhrases    = (tI18nPhrase*)malloc(sizeof(tI18nPhrase));
	memset(mPhrases[mNumPhrases], 0, sizeof(tI18nPhrase));
}

cText2SkinI18n::~cText2SkinI18n() {
	for (int i = 0; mPhrases[i][0]; ++i) {
		for (int j = 0; j < I18nNumLanguages; ++j)
			free((void*)mPhrases[i][j]);
	}
	free(mPhrases);
}

bool cText2SkinI18n::Parse(const char *Text) {
	int l = strlen(Text);
	bool result = false;
	if (l) {
		if (strncmp(Text, "Item=Translation,", 17) == 0) {
			int i;
			tI18nPhrase p;
			memset(&p, 0, sizeof(tI18nPhrase));
			Text += 17;

			for (i = 0; i < I18nNumLanguages; ++i) {
				char *langs = strdup(I18nLanguageCode(i));
				char *ptr = langs, *ep;
				std::string text;
				p[i] = strdup("");
				do {
					if ((ep = strchr(ptr, ',')) != NULL)
						*ep = '\0';
					if (ParseVar(Text, ptr, text)) {
						free((void*)p[i]);
						p[i] = strdup(text.c_str());
					}
					ptr = ep + 1;
				} while (ep != NULL);
				free(langs);
			}

			int idx = mNumPhrases++;
			for (i = 0; i < I18nNumLanguages; ++i)
				if (!p[i]) p[i] = "";
			mPhrases = (tI18nPhrase*)realloc(mPhrases, (mNumPhrases + 1) * sizeof(tI18nPhrase));
			memcpy(mPhrases[idx], p, sizeof(tI18nPhrase));
			memset(mPhrases[mNumPhrases], 0, sizeof(tI18nPhrase));
			result = true;
		} else
			esyslog("ERROR: text2skin: syntax error");
	}
	return result;
}

bool cText2SkinI18n::Load(const std::string &Filename) {
	if (cText2SkinFile::Load(Filename)) {
		I18nRegister(mPhrases, mIdentity.c_str());
		return true;
	}
	return false;
}
