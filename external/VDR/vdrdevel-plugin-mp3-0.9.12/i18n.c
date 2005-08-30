/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include "common.h"
#include "i18n.h"

const char *i18n_name = 0;

const tI18nPhrase Phrases[] = {
/*
 * French     translations provided by Maxime Guilbot <maxime.guilbot@orange.fr>
 *               additions provided by Pierre-Henri Beguin <ph.beguin@free.fr>
 * Slovenian  translations provided by Matjaz Thaler <matjaz.thaler@guest.arnes.si>
 * Greek      translations provided by Dimitrios Dimitrakos <mail@dimitrios.de>
 * Swedish    translations provided by Jan Ekholm <chakie@infa.abo.fi>
 * Spanish & 
 * Catalan    translations provided by Ramon Roca <ramon.roca@xcombo.com>
 * Finnish    translations provided by Rolf Ahrenberg <rahrenbe@cc.hut.fi>
 *               additions provided by Ville Skyttä <ville.skytta@iki.fi>
 * Russian    translations provided by Vyacheslav Dikonov <sdiconov@mail.ru>
*/
  { "Parent",
    "Zurück",
    "Nazaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parent",
    "", // TODO
    "Takaisin",
    "", // TODO
    "Predecesor", 
    "Piso",
    "Tillbaka",
    "", // TODO
    "", // TODO
    "Anterior",
#if VDRVERSNUM >= 10302
    "²ÒÕàå",
#endif
  },
  { "Select",
    "Auswählen",
    "Izberi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Sélectionner",
    "", // TODO
    "Valitse",
    "", // TODO
    "Seleccionar", 
    "Epilogi",
    "Välj",
    "", // TODO
    "", // TODO
    "Escollir",
#if VDRVERSNUM >= 10302
    "²ëÑàÐâì",
#endif
  },
  { "Error scanning directory!",
    "Fehler beim Lesen des Verzeichnisses!",
    "Napaka pri pregledovanju direktorija!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours du répertoire!",
    "", // TODO
    "Hakemiston avaaminen epäonnistui!",
    "", // TODO
    "¡Error al leer una carpeta!", 
    "Lathos stin sarosi tou fakelou!",
    "Kunde inte läsa katalogen!",
    "", // TODO
    "", // TODO
    "Error al llegir una carpeta!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ áÚÐÝØàÞÒÐÝØï ÚÐâÐÛÞÓÐ!",
#endif
  },
  { "Source",
    "Datenträger",
    "Izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source",
    "", // TODO
    "Lähde",
    "", // TODO
    "Origen", 
    "Pigi",
    "Källa",
    "", // TODO
    "", // TODO
    "Orígen",
#if VDRVERSNUM >= 10302
    "´ØáÚ",
#endif
  },
  { "Mount",
    "Einbinden",
    "Priklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Monter",
    "", // TODO
    "Kiinnitä",
    "", // TODO
    "Mount", 
    "Sindesi",
    "Montera",
    "", // TODO
    "", // TODO
    "Mount",
#if VDRVERSNUM >= 10302
    "¿ÞÔÚÛîçØâì",
#endif
  },
  { "Unmount",
    "Aushängen",
    "Izklopi",
    "", // TODO
    "", // TODO
    "", // TODO
    "Démonter",
    "", // TODO
    "Irrota",
    "", // TODO
    "Unmount",
    "Aposindesi",
    "Avmontera",
    "", // TODO
    "", // TODO
    "Unmount",
#if VDRVERSNUM >= 10302
    "¾âÚÛîçØâì",
#endif
  },
  { "Selected source is not mounted!",
    "Ausgewählte Datenträger ist nicht eingebunden!",
    "Izbran izvor ni prikljucen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source sélectionnée non montée!",
    "", // TODO
    "Valittu lähde ei ole kiinnitetty!",
    "", // TODO
    "¡El origen deseado no està montado!", 
    "Epilegmeni Pigi den ine sindemeni!",
    "Den valda källan är inte monterad!",
    "", // TODO
    "", // TODO
    "l'Origen sel.leccionat no està muntat!",
#if VDRVERSNUM >= 10302
    "²ëÑàÐÝÝëÙ ÔØáÚ ÝÕ ßÞÔÚÛîçÕÝ!",
#endif
  },
  { "Mount succeeded",
    "Einbinden erfolgreich",
    "Priklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "Montage réussi",
    "", // TODO
    "Kiinnittäminen onnistui",
    "", // TODO
    "mount correcto", 
    "I sindesi petixe",
    "Monteringen lyckades",
    "", // TODO
    "", // TODO
    "mount correcte",
#if VDRVERSNUM >= 10302
    "´ØáÚ ßÞÔÚÛîçÕÝ",
#endif
  },
  { "Mount failed!",
    "Einbinden fehlgeschlagen!",
    "Napaka pri priklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du montage!",
    "", // TODO
    "Kiinnittäminen epäonnistui!",
    "", // TODO
    "¡No he podido montar!", 
    "I sindesi apetixe!",
    "Monteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No he pogut muntar!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ßÞÔÚÛîçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Unmount succeeded",
    "Aushängen erfolgreich",
    "Izklop izveden",
    "", // TODO
    "", // TODO
    "", // TODO
    "Démontage réussi",
    "", // TODO
    "Irrottaminen onnistui",
    "", // TODO
    "Éxito al unmount", 
    "I aposindesi itan epitixisi",
    "Avmonteringen lyckades",
    "", // TODO
    "", // TODO
    "unmount amb èxit",
#if VDRVERSNUM >= 10302
    "´ØáÚ ÞâÚÛîçÕÝ",
#endif
  },
  { "Unmount failed!",
    "Aushängen fehlgeschlagen!",
    "Napaka pri izklopu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec du démontage!",
    "", // TODO
    "Irrottaminen epäonnistui!",
    "", // TODO
    "¡No puedo desmontar!", 
    "I aposindesi den itan epitixis!",
    "Avmonteringen misslyckades!",
    "", // TODO
    "", // TODO
    "No puc desmontar!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ÞâÚÛîçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Eject failed!",
    "Auswerfen fehlgeschlagen!",
    "Napaka pri izmetu!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Echec de l'éjection!",
    "", // TODO
    "Aseman avaaminen epäonnistui!",
    "", // TODO
    "¡No puedo expulsar!", 
    "I apovoli apetixe!",
    "Mata ut!",
    "", // TODO
    "", // TODO
    "No puc expulsar!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ Ø×ÒÛÕçÕÝØï ÔØáÚÐ!",
#endif
  },
  { "Hide mainmenu entry",
    "Hauptmenüeintrag verstecken",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Piilota valinta päävalikosta",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "ÁÚàëâì ÚÞÜÐÝÔã Ò ÜÕÝî",
#endif
  },

// start of MP3 specific phrases

  { "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3-soitin",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
    "MP3",
#if VDRVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì MP3",
#endif
  },
  { "A versatile audio player",
    "Ein vielseitiger Audio-Player",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Monipuolinen audiosoitin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì ÐãÔØÞäÐÙÛÞÒ",
#endif
  },
  { "Browse",
    "Blättern",
    "Isci",
    "", // TODO
    "", // TODO
    "", // TODO
    "Parcourir",
    "", // TODO
    "Selaa",
    "", // TODO
    "Explorar",
    "Selida",
    "Bläddra",
    "", // TODO
    "", // TODO
    "Explorar",
#if VDRVERSNUM >= 10302
    "ºÐâÐÛÞÓØ",
#endif
  },
  { "MP3 source",
    "MP3 Datenträger",
    "MP3 izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MP3",
    "", // TODO
    "MP3-lähde",
    "", // TODO
    "Origen MP3", 
    "Pigi MP3",
    "MP3 källa",
    "", // TODO
    "", // TODO
    "Orígen MP3",
#if VDRVERSNUM >= 10302
    "´ØáÚ MP3",
#endif
  },
  { "Rename",
    "Umbenennen",
    "Preimenuj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer",
    "", // TODO
    "Nimeä",
    "", // TODO
    "Renombrar", 
    "Alagi Onomatos",
    "Döp om",
    "", // TODO
    "", // TODO
    "Reanomenar",
#if VDRVERSNUM >= 10302
    "¿ÕàÕØÜÕÝÞÒÐâì",
#endif
  },
  { "Add",
    "Hinzufügen",
    "Dodaj",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter",
    "", // TODO
    "Lisää",
    "", // TODO
    "Añadir", 
    "Prosthesi",
    "Lägg till",
    "", // TODO
    "", // TODO
    "Afegir",
#if VDRVERSNUM >= 10302
    "´ÞÑÐÒØâì",
#endif
  },
  { "Remove",
    "Entfernen",
    "Odstrani",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever",
    "", // TODO
    "Poista",
    "", // TODO
    "Borrar", 
    "Aferesi",
    "Radera",
    "", // TODO
    "", // TODO
    "Esborrar",
#if VDRVERSNUM >= 10302
    "ÃÔÐÛØâì",
#endif
  },
  { "Play all",
    "Alle abspielen",
    "Predvajaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Jouer tous",
    "", // TODO
    "Soita kaikki",
    "", // TODO
    "Reproducirlo todo", 
    "Peksimo olon",
    "Spela alla",
    "", // TODO
    "", // TODO
    "Escoltar-ho tot",
#if VDRVERSNUM >= 10302
    "²ÞáßàÞØ×ÒÕáâØ Òáñ",
#endif
  },
  { "Add all",
    "Alle hinzufügen",
    "Dodaj vse",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter tous",
    "", // TODO
    "Lisää kaikki",
    "", // TODO
    "Añadir todo", 
    "Prosthesi olon",
    "Lägg till alla",
    "", // TODO
    "", // TODO
    "Afegir-ho tot",
#if VDRVERSNUM >= 10302
    "´ÞÑÐÒØâì Òáñ",
#endif
  },
  { "Empty directory!",
    "Leeres Verzeichnis!",
    "Direktorij prazen!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Répertoire vide!",
    "", // TODO
    "Hakemisto on tyhjä!",
    "", // TODO
    "¡Carpeta vacía!", 
    "Adios fakelos!",
    "Tom katalog!",
    "", // TODO
    "", // TODO
    "Carpeta buida!",
#if VDRVERSNUM >= 10302
    "¿ãáâÞÙ ÚÐâÐÛÞÓ!",
#endif
  },
  { "Add recursivly?",
    "Recursiv hinzufügen?",
    "Rekurzivno dodaj?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ajouter récursivement?",
    "", // TODO
    "Lisää rekursiivisesti?",
    "", // TODO
    "¿Añadir recursivamente?", 
    "Prosthesi recursiv?",
    "Lägg till rekursivt?",
    "", // TODO
    "", // TODO
    "Afegir recursivament?",
#if VDRVERSNUM >= 10302
    "´ÞÑÐÒØâì àÕÚãàáØÒÝÞ?",
#endif
  },
  { "Remove entry?",
    "Eintrag entfernen?",
    "Odstrani izbrano?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Enlever titre?",
    "", // TODO
    "Poistetaanko merkintä?",
    "", // TODO
    "¿Borrar la entrada?", 
    "Aferesi simiou?",
    "Radera post?",
    "", // TODO
    "", // TODO
    "Esborrar la entrada?",
#if VDRVERSNUM >= 10302
    "ÃÔÐÛØâì ×ÐßØáì?",
#endif
  },
  { "Playlist editor",
    "Abspielliste bearbeiten",
    "Urejevalnik liste",
    "", // TODO
    "", // TODO
    "", // TODO
    "Editeur de liste de lecture",
    "", // TODO
    "Soittolistan muokkaus",
    "", // TODO
    "Editor de listas de reproducción", 
    "Metatropes stin Playlista",
    "Spellisteditor",
    "", // TODO
    "", // TODO
    "Editor de llistes de reproducció",
#if VDRVERSNUM >= 10302
    "ÀÕÔÐÚâÞà áßØáÚÐ ßÕáÕÝ",
#endif
  },
  { "Directory browser",
    "Verzeichnisanzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "Hakemistoselain",
    "", // TODO
    "Explorar carpetes", 
    "Endiski fakelou",
    "Katalogbläddrare",
    "", // TODO
    "", // TODO
    "Navegar per les carpetes",
#if VDRVERSNUM >= 10302
    "¿àÞáÜÞâà ÚÐâÐÛÞÓÞÒ",
#endif
  },
  { "Delete playlist?",
    "Abspielliste löschen?",
    "Odstrani listo?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Effacer liste?",
    "", // TODO
    "Poistetaanko soittolista?",
    "", // TODO
    "¿Borrar la lista?", 
    "Svisimo listas?",
    "Radera spellista?",
    "", // TODO
    "", // TODO
    "Esborrar la Llista?",
#if VDRVERSNUM >= 10302
    "ÃÔÐÛØâì áßØáÞÚ",
#endif
  },
  { "Are you sure?",
    "Wirklich löschen?",
    "Ste prepricani?",
    "", // TODO
    "", // TODO
    "", // TODO
    "Etes-vous sûr?",
    "", // TODO
    "Oletko varma?",
    "", // TODO
    "¿Está seguro?", 
    "Ise sigouros?",
    "Är du säker?",
    "", // TODO
    "", // TODO
    "N'esteu segur?",
#if VDRVERSNUM >= 10302
    "²ë ãÒÕàÕÝë?",
#endif
  },
  { "unnamed",
    "unbenannt",
    "neimenovan",
    "", // TODO
    "", // TODO
    "", // TODO
    "sansnom",
    "", // TODO
    "nimetön",
    "", // TODO
    "sin nombre", 
    "xoris onoma",
    "namnlös",
    "", // TODO
    "", // TODO
    "sense nom",
#if VDRVERSNUM >= 10302
    "ÑÕ×ëÜïÝÝëÙ",
#endif
  },
  { "unknown",
    "unbekannt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "inconnu",
    "", // TODO
    "tuntematon",
    "", // TODO
    "desconocido", 
    "", // TODO
    "okänd",
    "", // TODO
    "", // TODO
    "desconegut",
#if VDRVERSNUM >= 10302
    "ÝÕØ×ÒÕáâÝëÙ",
#endif
  },
  { "Error scanning playlists!",
    "Fehler beim Einlesen der Abspiellisten!",
    "Napaka pri pregledovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de parcours de la liste!",
    "", // TODO
    "Soittolistojen lukeminen epäonnistui!",
    "", // TODO
    "¡Error al leer las listas!", 
    "Lathos stin sarosi tis Playlistas!",
    "Fel uppstod då spellistorna lästes!",
    "", // TODO
    "", // TODO
    "Error al llegir les llistes!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ßÞØáÚÐ áßØáÚÞÒ ßÕáÕÝ!",
#endif
  },
  { "Error deleting playlist!",
    "Fehler beim Löschen der Abspielliste!",
    "Napaka pri odstranjevanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur d'effacement de la liste!",
    "", // TODO
    "Soittolistan poistaminen epäonnistui!",
    "", // TODO
    "¡Error al borrar una lista!",
    "Lathos stin akirosi tis Playlistas!",
    "Fel uppstod då spellistan raderades!",
    "", // TODO
    "", // TODO
    "Error al esborrar una llista!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ãÔÐÛÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Error creating playlist!",
    "Fehler beim Erstellen der Abspielliste!",
    "Napaka pri kreiranju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de création de la liste!",
    "", // TODO
    "Soittolistan luominen epäonnistui!",
    "", // TODO
    "¡Error al crear una lista!",
    "Lathos stin dimiourgia tis Playlistas!",
    "Fel uppstod då spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al crear una llista!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ áÞ×ÔÐÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Can't edit a WinAmp playlist!",
    "Editieren von WinAmp Abspiellisten nicht möglich!",
    "Ne morem vnesti liste od WinAmp-a!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ne peut pas éditer une liste WinAmp!",
    "", // TODO
    "WinAmp-soittolistaa ei voida muokata!",
    "", // TODO
    "¡No puedo editar una lista del WinAmp!", 
    "Den mporo na kano metatropes se WinAmp Playlista!",
    "Kan inte redigera en WinAmp-spellista!",
    "", // TODO
    "", // TODO
    "No puc editar una llista del WinAmp!",
#if VDRVERSNUM >= 10302
    "½ÕÒÞ×ÜÞÖÝÞ àÕÔÐÚâØàÞÒÐâì áßØáÞÚ WinAmp!",
#endif
  },
  { "Error renaming playlist!",
    "Fehler beim Umbenennen der Abspielliste!",
    "Napaka pri preimenovanju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur pour renommer la liste!",
    "", // TODO
    "Soittolistan uudelleennimeäminen epäonnistui!",
    "", // TODO
    "¡Errir al renombrar la lista!", 
    "Latsos stin metonomasi tis Playlistas!",
    "Fel uppstod då spellistan döptes om!",
    "", // TODO
    "", // TODO
    "Error al reanomenar la llista!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ßÕàÕØÜÕÝÞÒÐÝØï áßØáÚÐ!",
#endif
  },
  { "Loading playlist...",
    "Lade Abspielliste...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Chargement playlist...",
    "", // TODO
    "Avataan soittolistaa...",
    "", // TODO
    "Cargando la lista de reproducción...", 
    "", // TODO
    "Läser in spellista...",
    "", // TODO
    "", // TODO
    "Carregant la llista de reproducció...",
#if VDRVERSNUM >= 10302
    "ÇâÕÝØÕ áßØáÚÐ ßÕáÕÝ...",
#endif
  },
  { "Error loading playlist!",
    "Fehler beim Lesen der Abspielliste!",
    "Napaka pri nalaganju liste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur de chargement de la liste!",
    "", // TODO
    "Soittolistan avaaminen epäonnistui!",
    "", // TODO
    "¡Error al cargar la lista de reproducción!", 
    "Lathos sto fortoma tis Playlistas!",
    "Fel uppstod då spellistan lästes in!",
    "", // TODO
    "", // TODO
    "Error al carregar la llista de reproducció!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ çâÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Scanning playlists...",
    "Durchsuche Abspiellisten...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne playlists...",
    "", // TODO
    "Selataan soittolistoja...", 
    "", // TODO
    "Leyendo las listas de reproducción...",
    "", // TODO
    "Söker spellistor...",
    "", // TODO
    "", // TODO
    "Llegint les llistes de reproducció...",
#if VDRVERSNUM >= 10302
    "¿ÞØáÚ áßØáÚÞÒ ßÕáÕÝ...",
#endif
  },
  { "Scanning directory...",
    "Durchsuche Verzeichnis...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Scanne le repertoire...",
    "", // TODO
    "Selataan hakemistoa...",
    "", // TODO
    "Leyendo las carpetas..",
    "", // TODO
    "Söker igenom katalog...",
    "", // TODO
    "", // TODO
    "Revisant les carpetes..",
#if VDRVERSNUM >= 10302
    "ÁÚÐÝØàÞÒÐÝØÕ ÚÐâÐÛÞÓÞÒ...",
#endif
  },
  { "Building playlist...",
    "Baue Abspielliste auf...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Construit la playlist...",
    "", // TODO
    "Muodostetaan soittolistaa...",
    "", // TODO
    "Construyendo la lista de reproducción...",
    "", // TODO
    "Skapar en spellista...",
    "", // TODO
    "", // TODO
    "Construïnt la llista de reproducció...",
#if VDRVERSNUM >= 10302
    "¿ÞáâàÞÕÝØÕ áßØáÚÐ ßÕáÕÝ...",
#endif
  },
  { "Error building playlist!",
    "Fehler beim Aufbau der Abspielliste!",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Erreur construction playlist!",
    "", // TODO
    "Soittolistan muodostaminen epäonnistui!",
    "", // TODO
    "¡Error al construir la lista de reproducción!",
    "", // TODO
    "Fel uppstod då spellistan skapades!",
    "", // TODO
    "", // TODO
    "Error al consstruir la llista de reproducció!",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ßÞáâàÞÕÝØï áßØáÚÐ ßÕáÕÝ!",
#endif
  },
  { "Remote CDDB lookup...",
    "Remote CDDB Abfrage...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Consultation CDDB...",
    "", // TODO
    "Kysytään CDDB-tietoja...",
    "", // TODO
    "Consultando CDDB...",
    "", // TODO
    "Fjärrförfrågan till CDDB...",
    "", // TODO
    "", // TODO
    "Consultant CDDB...",
#if VDRVERSNUM >= 10302
    "·ÐßàÞá Ú ãÔÐÛñÝÝÞÙ ÑÐ×Õ CDDB...",
#endif
  },
  { "Connecting to stream server ...",
    "Verbinde mit Stream Server...",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Connexion au Serveur Stream...",
    "", // TODO
    "Yhdistetään palvelimeen...",
    "", // TODO
    "Conectando con el servidor...", 
    "", // TODO
    "Kontaktar stream-server...",
    "", // TODO
    "", // TODO
    "Connectant al servidor...",
#if VDRVERSNUM >= 10302
    "¿ÞÔÚÛîçÕÝØÕ Ú áÕàÒÕàã ßÞâÞÚÞÒ...",
#endif
  },
  { "Rename playlist",
    "Abspielliste umbenennen",
    "Preimenuj listo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Renommer la liste",
    "", // TODO
    "Nimeä soittolista",
    "", // TODO
    "Renombrar la lista de reproducción",
    "Metonomasi tis Playlistas",
    "Döp om spellistan",
    "", // TODO
    "", // TODO
    "Reanomenar la llista de repr.",
#if VDRVERSNUM >= 10302
    "¿ÕàÕØÜÕÝÞÒÐâì áßØáÞÚ ßÕáÕÝ",
#endif
  },
  { "Old name:",
    "Alter Name:",
    "Staro ime:",
    "", // TODO
    "", // TODO
    "", // TODO
    "Ancien nom:",
    "", // TODO
    "Vanha nimi",
    "", // TODO
    "Nombre anterior:", 
    "Palio onoma:",
    "Gammalt namn:",
    "", // TODO
    "", // TODO
    "Nom anterior:",
#if VDRVERSNUM >= 10302
    "ÁâÐàÞÕ ØÜï:",
#endif
  },
  { "New name",
    "Neuer Name",
    "Novo ime",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nouveau nom",
    "", // TODO
    "Uusi nimi",
    "", // TODO
    "Nombre nuevo", 
    "Neo onoma",
    "Nytt namn",
    "", // TODO
    "", // TODO
    "Nou nom", 
#if VDRVERSNUM >= 10302
    "½ÞÒÞÕ ØÜï:",
#endif
  },
  { "Filenames",
    "Dateinamen",
    "Imena datotek",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noms de fichiers",
    "", // TODO
    "tiedostonimet",
    "", // TODO
    "Nombre de los archivos", 
    "Onomata arxeion",
    "Filnamn",
    "", // TODO
    "", 
    "Nom dels arxius",
#if VDRVERSNUM >= 10302
    "ØÜÕÝÐ äÐÙÛÞÒ",
#endif
  },
  { "ID3 names",
    "ID3 Namen",
    "ID3 imena",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom ID3",
    "", // TODO
    "ID3-nimet",
    "", // TODO
    "Nombre ID3", 
    "ID3 onomata",
    "ID3 namn",
    "", // TODO
    "", // TODO
    "Nom ID3",
#if VDRVERSNUM >= 10302
    "ID3-ØÜÕÝÐ",
#endif
  },
  { "ID3 information",
    "ID3 Information",
    "ID3 informacija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Information ID3",
    "", // TODO
    "ID3-informaatio",
    "", // TODO
    "Información ID3",
    "Plirofories ID3",
    "ID3 information",
    "", // TODO
    "", // TODO
    "Informació ID3",
#if VDRVERSNUM >= 10302
    "ID3-ÜÕâÚØ",
#endif
  },
  { "ID3 info",
    "ID3 Info",
    "ID3 info",
    "", // TODO
    "", // TODO
    "", // TODO
    "Info ID3",
    "", // TODO
    "ID3-info",
    "", // TODO
    "ID3 info",
    "ID3 pliroforia",
    "ID3 info",
    "", // TODO
    "", // TODO
    "ID3 info",
#if VDRVERSNUM >= 10302
    "ÔÐÝÝëÕ ID3",
#endif
  },
  { "Filename",
    "Dateiname",
    "Ime datoteke",
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du fichier",
    "", // TODO
    "Tiedostonimi",
    "", // TODO
    "Nombre del archivo", 
    "Onoma arxeiou",
    "Filnamn",
    "", // TODO
    "", // TODO
    "Nom de l'arxiu",
#if VDRVERSNUM >= 10302
    "¸Üï äÐÙÛÐ",
#endif
  },
  { "Length",
    "Länge",
    "Dolzina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Longueur",
    "", // TODO
    "Pituus",
    "", // TODO
    "Duración", 
    "Megethos",
    "Längd",
    "", // TODO
    "", // TODO
    "Durada",
#if VDRVERSNUM >= 10302
    "´ÛØÝÐ",
#endif
  },
  { "Title",
    "Titel",
    "Naslov",
    "", // TODO
    "", // TODO
    "", // TODO
    "Titre",
    "", // TODO
    "Kappale",
    "", // TODO
    "Título", 
    "Titlos",
    "Tittel",
    "", // TODO
    "", // TODO
    "Títol",
#if VDRVERSNUM >= 10302
    "½Ð×ÒÐÝØÕ",
#endif
  },
  { "Artist",
    "Interpret",
    "Avtor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Artiste",
    "", // TODO
    "Esittäjä",
    "", // TODO
    "Artista",
    "Ermineftis",
    "Artist",
    "", // TODO
    "", // TODO
    "Intèrpret",
#if VDRVERSNUM >= 10302
    "¸áßÞÛÝØâÕÛì",
#endif
  },
  { "Album",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "", // TODO
    "Album",
    "", // TODO
    "Albumi",
    "", // TODO
    "Album",
    "Album",
    "Album",
    "", // TODO
    "", // TODO
    "Àlbum",
#if VDRVERSNUM >= 10302
    "°ÛìÑÞÜ",
#endif
  },
  { "Year",
    "Jahr",
    "Leto",
    "", // TODO
    "", // TODO
    "", // TODO
    "Année",
    "", // TODO
    "Vuosi",
    "", // TODO
    "", // TODO
    "Etos",
    "År",
    "", // TODO
    "", // TODO
    "Any",
#if VDRVERSNUM >= 10302
    "³ÞÔ",
#endif
  },
  { "Samplerate",
    "Sample Rate",
    "Vzorcenje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Fréquence",
    "", // TODO
    "Näytteenottotaajuus",
    "", // TODO
    "Frecuencia", 
    "Sixnotita",
    "Samplingshastighet",
    "", // TODO
    "", // TODO
    "Freqüència",
#if VDRVERSNUM >= 10302
    "ÇÐáâÞâÐ",
#endif
  },
  { "Bitrate",
    "Bit Rate",
    "Bitna hitrost",
    "", // TODO
    "", // TODO
    "", // TODO
    "Précision",
    "", // TODO
    "Bittinopeus",
    "", // TODO
    "Bitrate",
    "Bitrate",
    "Bithastighet",
    "", // TODO
    "", // TODO
    "Bitrate",
#if VDRVERSNUM >= 10302
    "¿ÞâÞÚ",
#endif
  },
  { "Setup.MP3$Initial loop mode",
    "Default Loop Modus",
    "Osnovni nacin ponavljanje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de répétition initial",
    "", // TODO
    "Jatkuva soitto oletuksena",
    "", // TODO
    "Modo de repetición inicial", 
    "", // TODO
    "Normalt upprepningsläge",
    "", // TODO
    "", // TODO
    "Mode de repetició inicial",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ßÞÒâÞàÐ ßÞ ãÜÞÛçÐÝØî",
#endif
  },
  { "Setup.MP3$Initial shuffle mode",
    "Default Shuffle Modus",
    "Osnovni nacin mesano",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de lecture aléatoire initial",
    "", // TODO
    "Satunnaissoitto oletuksena",
    "", // TODO
    "Lectura aleatória inicial", 
    "", // TODO
    "Normalt blandläge",
    "", // TODO
    "", // TODO
    "Lectura aleatòria inicial",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ áÛãçÐÙÝ. ÒëÑÞàÐ ßÞ ãÜÞÛçÐÝØî",
#endif
  },
  { "Setup.MP3$Audio output mode",
    "Audio Ausgabe Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Äänen ulostulomoodi",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ÒëÒÞÔÐ ×ÒãÚÐ",
#endif
  },
  { "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
    "OSS",
#if VDRVERSNUM >= 10302
    "OSS",
#endif
  },
  { "Setup.MP3$Audio mode",
    "Audio Modus",
    "Avdio nacin",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode audio",
    "", // TODO
    "Äänimoodi",
    "", // TODO
    "Modo de audio", 
    "Katastasi audio",
    "Audioläge",
    "", // TODO
    "", // TODO
    "Mode d'audio",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ßÕàÕÞæØäàÞÒÚØ ×ÒãÚÐ",
#endif
  },
  { "Round",
    "Runden",
    "Zaokrozeni",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arrondir",
    "", // TODO
    "pyöristetty",
    "", // TODO
    "Redondear", 
    "Kikli",
    "Avrunda",
    "", // TODO
    "", // TODO
    "Arrodonir",
#if VDRVERSNUM >= 10302
    "ßàØÑÛØÖÕÝØÕ",
#endif
  },
  { "Dither",
    "Streuen",
    "Raztrosni",
    "", // TODO
    "", // TODO
    "", // TODO
    "Choisir",
    "", // TODO
    "ditteroitu",
    "", // TODO
    "Compactar",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Compactar",
#if VDRVERSNUM >= 10302
    "áÓÛÐÖØÒÐÝØÕ",
#endif
  },
  { "Setup.MP3$Background scan",
    "Hintergrund Scan",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Taustaskannaus",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "ÄÞÝÞÒÞÕ áÚÐÝØàÞÒÐÝØÕ",
#endif
  },
  { "ID3 only",
    "nur ID3",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "vain ID3",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "ÂÞÛìÚÞ ID3",
#endif
  },
  { "ID3 & Level",
    "ID3 & Pegel",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "ID3 & taso",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "ID3 Ø ãàÞÒÕÝì",
#endif
  },
  { "Setup.MP3$Editor display mode",
    "Editor Anzeige Modus",
    "Nacin prikaza urejevalnika",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage de l'éditeur",
    "", // TODO
    "Muokkaustilan näyttömoodi",
    "", // TODO
    "Modo de visualización del editor",
    "", // TODO
    "Redigerarens visuella läge",
    "", // TODO
    "", // TODO
    "Mode de visualització del editor",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ àÕÔÐÚâÞàÐ áßØáÚÞÒ",
#endif
  },
  { "Setup.MP3$Display mode",
    "Anzeige Modus",
    "Nacin prikaza",
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode d'affichage",
    "", // TODO
    "Näyttömoodi",
    "", // TODO
    "Modo de Visualización", 
    "", // TODO
    "Visuellt läge",
    "", // TODO
    "", // TODO
    "Mode de visualització",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ØÝâÕàäÕÙáÐ",
#endif
  },
  { "Setup.MP3$Background mode",
    "Hintergrund Modus",
    "Ozadje",
    "", // TODO
    "", // TODO
    "", // TODO
    "Arrière plan",
    "", // TODO
    "Taustamoodi",
    "", // TODO
    "Ejecutar al fondo", 
    "", // TODO
    "Bakgrundsläge",
    "", // TODO
    "", // TODO
    "Execució de fons",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ äÞÝÐ",
#endif
  },
  { "Setup.MP3$CDDB for CD-Audio",
    "CDDB für CD-Audio",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "CDDB pour CD-Audio",
    "", // TODO
    "CDDB-tietokanta",
    "", // TODO
    "CDDB para el Aidio-CD", 
    "", // TODO
    "CDDB för CD-audio",
    "", // TODO
    "", // TODO
    "CDDB per al Audio-CD",
#if VDRVERSNUM >= 10302
    "CDDB ÔÛï ÐãÔØÞ-CD",
#endif
  },
  { "disabled",
    "aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "deactiver",
    "", // TODO
    "pois",
    "", // TODO
    "desactivado", 
    "", // TODO
    "avstängd",
    "", // TODO
    "", // TODO
    "desactivat",
#if VDRVERSNUM >= 10302
    "ÒëÚÛ",
#endif
  },
  { "local only",
    "nur lokal",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local uniquement",
    "", // TODO
    "vain paikallinen",
    "", // TODO
    "solo local", 
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "solsament local",
#if VDRVERSNUM >= 10302
    "âÞÛìÚÞ ÛÞÚÐÛìÝ.",
#endif
  },
  { "local&remote",
    "lokal und entfernt",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local et distant",
    "", // TODO
    "paikallinen & etä",
    "", // TODO
    "local y remoto",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "local i remot",
#if VDRVERSNUM >= 10302
    "ÛÞÚÐÛìÝ. Ø ãÔÐÛñÝ.",
#endif
  },
  { "Setup.MP3$Use 48kHz mode only",
    "Nur 48kHz Modus benutzen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode 48kHz uniquement",
    "", // TODO
    "Käytä vain 48kHz moodia",
    "", // TODO
    "Solo modo 48kHz",
    "", // TODO
    "Använd endast 48Khz-läge",
    "", // TODO
    "", // TODO
    "Només mode 48kHz",
#if VDRVERSNUM >= 10302
    "ÂÞÛìÚÞ 48 Ú³æ",
#endif
  },
  { "Setup.MP3$Use HTTP proxy",
    "HTTP Proxy benutzen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Utiliser un Proxy HTTP",
    "", // TODO
    "Käytä HTTP-välityspalvelinta",
    "", // TODO
    "Utilizar proxy HTTP", 
    "", // TODO
    "Använd en HTTP-proxy",
    "", // TODO
    "", // TODO
    "Util·litzar proxy HTTP",
#if VDRVERSNUM >= 10302
    "¸áßÞÛì×ÞÒÐâì HTTP ßàÞÚáØ",
#endif
  },
  { "Setup.MP3$HTTP proxy host",
    "HTTP Proxy Name",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Nom du Proxy HTTP",
    "", // TODO
    "HTTP-välityspalvelimen osoite",
    "", // TODO
    "Nombre del puerto HTTP", 
    "", // TODO
    "HTTP-proxyns namn",
    "", // TODO
    "", // TODO
    "Nom del proxy HTTP",
#if VDRVERSNUM >= 10302
    "ÁÕàÒÕà HTTP ßàÞÚáØ",
#endif
  },
  { "Setup.MP3$HTTP proxy port",
    "HTTP Proxy Port",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Port du proxy HTTP",
    "", // TODO
    "HTTP-välityspalvelimen portti",
    "", // TODO
    "Puerto del Proxy HTTP", 
    "", // TODO
    "HTTP-proxyns port",
    "", // TODO
    "", // TODO
    "Port del proxy HTTP",
#if VDRVERSNUM >= 10302
    "¿Þàâ HTTP ßàÞÚáØ",
#endif
  },
  { "Setup.MP3$CDDB server",
    "CDDB Server",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Serveur CDDB",
    "", // TODO
    "CDDB-palvelin",
    "", // TODO
    "Servidor CDDB", 
    "", // TODO
    "CDDB server",
    "", // TODO
    "", // TODO
    "Servidor CDDB",
#if VDRVERSNUM >= 10302
    "ÁÕàÒÕà CDDB",
#endif
  },
  { "Setup.MP3$CDDB port",
    "CDDB Port",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Port CDDB",
    "", // TODO
    "CDDB-palvelimen portti",
    "", // TODO
    "Puerto CDDB",
    "", // TODO
    "CDDB port",
    "", // TODO
    "", // TODO
    "Port CDDB",
#if VDRVERSNUM >= 10302
    "¿Þàâ CDDB",
#endif
  },
  { "Black",
    "Schwarz",
    "Crnina",
    "", // TODO
    "", // TODO
    "", // TODO
    "Noir",
    "", // TODO
    "musta",
    "", // TODO
    "Negra", 
    "Mavro",
    "Svart",
    "", // TODO
    "", // TODO
    "Negre",
#if VDRVERSNUM >= 10302
    "çñàÝëÙ íÚàÐÝ",
#endif
  },
  { "Live",
    "Live",
    "Slika",
    "", // TODO
    "", // TODO
    "", // TODO
    "Imagen",
    "", // TODO
    "live",
    "", // TODO
    "En vivo", 
    "Zontana",
    "Live",
    "", // TODO
    "", // TODO
    "Imatge",
#if VDRVERSNUM >= 10302
    "âÕÛÕÒØÔÕÝØÕ",
#endif
  },
  { "Images",
    "Bilder",
    "",
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "kansikuva",
    "", // TODO
    "", 
    "",
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },
  { "Setup.MP3$Mainmenu mode",
    "Hauptmenu Modus",
    "Nacin glavnega menija",
    "", // TODO
    "", // TODO
    "", // TODO
    "Menu principal",
    "", // TODO
    "Päävalikon tila",
    "", // TODO
    "Menú Principal", 
    "", // TODO
    "Huvudmenyläge",
    "", // TODO
    "", // TODO
    "Menú Principal",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ÓÛÐÒÝÞÓÞ ÜÕÝî",
#endif
  },
  { "Playlists",
    "Abspiellisten",
    "Lista",
    "", // TODO
    "", // TODO
    "", // TODO
    "Listes de lecture",
    "", // TODO
    "soittolistat",
    "", // TODO
    "Lista de reproducción", 
    "Listes peksimatos",
    "Spellistor",
    "", // TODO
    "", // TODO
    "Llista a reproduïr",
#if VDRVERSNUM >= 10302
    "áßØáÚØ ßÕáÕÝ",
#endif
  },
  { "Browser",
    "Verz.anzeige",
    "Navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur",
    "", // TODO
    "selain",
    "", // TODO
    "Navegador",
    "", // TODO
    "Bläddra",
    "", // TODO
    "", // TODO
    "Navegador",
#if VDRVERSNUM >= 10302
    "ÚÐâÐÛÞÓØ",
#endif
  },
  { "Setup.MP3$Normalizer level",
    "Normalisierer Pegel",
    "Normalni nivo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Niveau de normalisation",
    "", // TODO
    "Normalisoinnin taso",
    "", // TODO
    "Nivel de normalización", 
    "", // TODO
    "Normaliseringsnivå",
    "", // TODO
    "", // TODO
    "Nivell de normalització",
#if VDRVERSNUM >= 10302
    "ÃàÞÒÕÝì ÝÞàÜÐÛØ×ÐæØØ",
#endif
  },
  { "Setup.MP3$Limiter level",
    "Begrenzer Pegel",
    "Limitni nivo",
    "", // TODO
    "", // TODO
    "", // TODO
    "Niveau limite",
    "", // TODO
    "Rajoittimen taso",
    "", // TODO
    "Limitador del nivel", 
    "", // TODO
    "Begränsningsnivå",
    "", // TODO
    "", // TODO
    "Limitador del nivell",
#if VDRVERSNUM >= 10302
    "¿ÞàÞÓ ÞÓàÐÝØçØâÕÛï ßØÚÞÒ",
#endif
  },
  { "Setup.MP3$Abort player at end of list",
    "Abspieler am Listenende beenden",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Keskeytä soittolistan loputtua",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "²ëåÞÔ Ò ÚÞÝæÕ áßØáÚÐ ßÕáÕÝ",
#endif
  },
  { "Setup.MP3$Replay display",
    "Wiedergabe Anzeige",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Toistotilan näyttömoodi",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "¸ÝâÕàäÕÙá Ò àÕÖØÜÕ ÒÞáßàÞØ×ÒÕÔÕÝØï",
#endif
  },
  { "classic",
    "klassisch",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "klassinen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "ÚÛÐááØçÕáÚØÙ",
#endif

  },
  { "via skin",
    "über Skin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "ulkoasun mukaan",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "áÞÓÛÐáÝÞ âÕÜÕ",
#endif

  },

// start of MPlayer specific phrases

  { "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer-mediasoitin",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
    "MPlayer",
#if VDRVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì ÒØÔÕÞ",
#endif
  },
  { "Media replay via MPlayer",
    "Medien Wiedergabe über MPlayer",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "MPlayeriin perustuva mediasoitin",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
#if VDRVERSNUM >= 10302
    "¿àÞØÓàëÒÐâÕÛì ÒØÔÕÞäÐÙÛÞÒ",
#endif
  },
  { "MPlayer browser",
    "MPlayer Verzeichnisanzeige",
    "MPlayer navigator",
    "", // TODO
    "", // TODO
    "", // TODO
    "Navigateur MPlayer",
    "", // TODO
    "MPlayer-hakemistoselain",
    "", // TODO
    "Navegador del MPlayer",
    "Mplayer endiksi fakelon",
    "MPlayer-bläddrare",
    "", // TODO
    "", // TODO
    "Navegador del MPlayer",
#if VDRVERSNUM >= 10302
    "¿àÞáÜÞâà ÚÐâÐÛÞÓÞÒ MPlayer",
#endif
  },
  { "MPlayer source",
    "MPlayer Datenträger",
    "MPlayer izvor",
    "", // TODO
    "", // TODO
    "", // TODO
    "Source MPlayer",
    "", // TODO
    "MPlayer-lähde",
    "Orígen del Mplayer", 
    "", // TODO
    "Pigi MPlayer",
    "MPlayer källa",
    "", // TODO
    "", // TODO
    "Orígen del MPlayer",
#if VDRVERSNUM >= 10302
    "¸áâÞçÝØÚ ÒØÔÕÞ MPlayer",
#endif
  },
  { "Setup.MPlayer$Control mode",
    "Kontroll Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Mode de controle",
    "", // TODO
    "Komentotila",
    "", // TODO
    "Modo de control", 
    "", // TODO
    "Kontrolläge",
    "", // TODO
    "", // TODO
    "Mode de control",
#if VDRVERSNUM >= 10302
    "ÀÕÖØÜ ãßàÐÒÛÕÝØï",
#endif
  },
  { "Traditional",
    "Traditionell",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Traditionnel",
    "", // TODO
    "perinteinen",
    "", // TODO
    "Tradicional", 
    "", // TODO
    "Traditionell",
    "", // TODO
    "", // TODO
    "Tradicional",
#if VDRVERSNUM >= 10302
    "ÞÑëçÝëÙ",
#endif
  },
  { "Slave",
    "Slave",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "Esclave",
    "", // TODO
    "orja",
    "", // TODO
    "Esclavo", 
    "", // TODO
    "Slav",
    "", // TODO
    "", // TODO
    "Esclau",
#if VDRVERSNUM >= 10302
    "àÐáèØàÕÝÝëÙ",
#endif

  },
  { "Setup.MPlayer$Resume mode",
    "Modus für Wiederaufnahme",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Resume-toiminto",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },
  { "local first",
    "zuerst local",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "paikallinen",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },
  { "global only",
    "nur global",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "globaali",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },
  { "MPlayer Audio ID",
    "MPlayer Audio ID",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "MPlayerin ääniraidan valinta",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },
  { "Audiostream ID",
    "Tonspur ID",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "",
    "", // TODO
    "Ääniraidan ID",
    "", // TODO
    "", 
    "", // TODO
    "",
    "", // TODO
    "", // TODO
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
  },

  { NULL }
  };
