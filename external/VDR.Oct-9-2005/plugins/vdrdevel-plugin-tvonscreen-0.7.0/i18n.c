/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c,v 1.15 2004/11/30 20:08:24 schmitzj Exp $
 *
 */

#include "i18n.h"

const tI18nPhrase tvoPhrases[] = {
  { "TV-OnScreen", // English
    "TV-OnScreen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "TV-ohjelmat", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Shows the EPG info in form of a typical TV magazine", // English
    "", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "TV-ohjelmatiedot (OSD)", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "o'clock", // English
    "Uhr", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    " ", // suomi (empty space)
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "%d-%m", // English
    "%d.%m.", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "%d.%m.", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Press 1 for help", // English
    "Für Hilfe die 1 drücken", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Paina '1' nähdäksesi opasteen", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Timer conflict!", // English
    "Timer Konflikt!", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    " ", // suomi (empty space)
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },

// Prefs
  { "show channel logos", // English
    "zeige Kanal-Logos", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Näytä kanavien logot", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "use XL fonts", // English
    "benutze XL Schrift", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Käytä XL-kirjasinta", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "hide info line", // English
    "verstecke Info Zeile", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Piilota inforivi", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "show channel names", // English
    "zeige Kanal-Namen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Näytä kanavien nimet", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "show logos in black&white", // English
    "zeige Kanal-Logos in Schwarz/Weiß", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Näytä logot mustavalkoisina", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "enable color problem work around", // English
    "Farbproblem Umgehung aktivieren", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Saksalainen värikorjaus", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "user point in time 1 (Key 4)", // English
    "Anwenderzeitpunkt 1 (Taste 4)", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Ajankohta #1 (näppäin '4')", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "user point in time 2 (Key 5)", // English
    "Anwenderzeitpunkt 2 (Taste 5)", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Ajankohta #2 (näppäin '5')", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "user point in time 3 (Key 6)", // English
    "Anwenderzeitpunkt 3 (Taste 6)", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Ajankohta #3 (näppäin '6')", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "jump to next day point if ago", // English
    "Gehe zum nächsten Tag wenn vorbei", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Siirry tarvittaessa seur. päivään", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Show search item itself", // English
    "Zeige auch Suchbegriff", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Näytä hakuehto", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },

// Help
  { "NORMAL MODE:", // English
    "NORMALER MODUS:", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "NORMAALITILA:\n", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "arrows\n\tmove view", // English
    "Pfeile\n\tAnsicht bewegen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Nuolinäppäimet'\n\tohjaa näkymää", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "back\n\tclose TV OnScreen", // English
    "back\n\tschließe TV OnScreen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Takaisin'\n\tsulje laajennos", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "red/blue\n\t-/+ one day", // English
    "rot/blau\n\t-/+ ein Tag", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Punainen'/'Sininen'\n\tyksi päivä eteen/taaksepäin", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "green/yellow\n\tone page left/right", // English
    "grün/gelb\n\teine Seite links/rechts", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Vihreä'/'Keltainen'\n\tseuraava/edellinen sivu", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "7/9\n\tone page left/right", // English
    "7/9\n\teine Seite links/rechts", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'7'/'9'\n\tseuraava/edellinen sivu", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "8\n\tgoto current channel", // English
    "8\n\tgehe zum aktuellen Kanal", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'8'\n\tsiirry nykyiselle kanavalle", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "0\n\tgoto now", // English
    "0\n\tgehe zur aktuellen Zeit", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'0'\n\tsiirry nykyhetkeen", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "4/5/6\n\tgoto configured time", // English
    "4/5/6\n\tgehe zu eingestellten Zeitpunkten", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'4'/'5'/'6'\n\tsiirry määriteltyyn ajankohtaan", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "ok\n\tswitch to edit mode\n", // English
    "ok\n\taktiviere Editier-Modus\n", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'OK'\n\tvaihda muokkaustilaan\n", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "EDIT MODE:", // English
    "EDITIER MODUS:", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "MUOKKAUSTILA:\n", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "back\n\tback to normal mode", // English
    "back\n\tzurück zum normalen Modus", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Takaisin'\n\tvaihda normaalitilaan", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "arrows\n\tmove selected schedule", // English
    "Pfeile\n\tSendung auswählen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Nuolinäppäimet'\n\tvalitse ohjelma", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "record\n\tcreate timer", // English
    "record\n\terzeuge Timer", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'Tallenna'\n\tluo ajastin", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "ok\n\tshow details", // English
    "ok\n\tzeige Sendungsdetails", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "'OK'\n\tnäytä lisätiedot", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
// End Help

// Details
  { "Record", // English
    "Aufnehmen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Tallenna", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Search", // English
    "Suchen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Etsi", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "AutoTimer", // English
    "AutoTimer", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "AutoTimer", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Added AutoTimer to vdradmin.", // English
    "AutoTimer in vdradmin aufgenommen.", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "AutoTimer lisätty VDRAdminiin.", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Startoption 'vdradminfile' not set!", // English
    "Startoption 'vdradminfile' nicht gesetzt!", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Käynnistysoptio 'vdradminfile' puuttuu!", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },

// Search
  { "Search for", // English
    "Suche nach", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Etsitään termiä", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Searching...", // English
    "Suchen...", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Etsitään...", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Nothing found!", // English
    "Nichts gefunden!", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "", // suomi
    "Ei löydetty!", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Back", // English
    "Zurück", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Takaisin", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "Details", // English
    "Details", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Lisätiedot", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },
  { "in all", // English
    "in allen", // Deutsch
    "", // Slovenski
    "", // Italiano
    "", // Nederlands
    "", // Português
    "", // Français
    "", // Norsk
    "Etsi kaikki", // suomi
    "", // Polski
    "", // Español
    "", // ÅëëçíéêÜ
    "", // Svenska
    "", // Romaneste
    "", // Magyar
    "", // Català
#if VDRVERSNUM >= 10302
    ""  // ÀãááÚØÙ (Russian)
#if VDRVERSNUM >= 10307
    "", // Hrvatski (Croatian)
#if VDRVERSNUM >= 10313
    "", // Eesti
#if VDRVERSNUM >= 10316
    "", // Dansk
#endif
#endif
#endif
#endif
  },

  { NULL }
  };
