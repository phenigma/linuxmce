/*
 * FCKeditor - The text editor for internet
 * Copyright (C) 2003-2004 Frederico Caldeira Knabben
 *
 * Licensed under the terms of the GNU Lesser General Public License
 * (http://www.opensource.org/licenses/lgpl-license.php)
 *
 * For further information go to http://www.fredck.com/FCKeditor/ 
 * or contact fckeditor@fredck.com.
 *
 * pl.js: Polish support.
 *
 * Authors:
 *   Jakub Boesche (jboesche@gazeta.pl)
 */

// Toolbar Items and Context Menu

lang["Cut"]				= "Wytnij" ;
lang["Copy"]			= "Kopiuj" ;
lang["Paste"]			= "Wklej" ;
lang["PasteText"]		= "Wklej jako czysty tekst" ;
lang["PasteWord"]		= "Wklej z Worda" ;
lang["Find"]			= "Znajdz" ;
lang["SelectAll"]		= "Zaznacz wszystko" ;
lang["RemoveFormat"]	= "Usun formatowanie" ;
lang["InsertLink"]		= "Wstaw/edytuj hiperlšcze" ;
lang["RemoveLink"]		= "Usun hiperlacze" ;
lang["InsertImage"]		= "Wstaw/edytuj obrazek" ;
lang["InsertTable"]		= "Wstaw/edytuj tabele" ;
lang["InsertLine"]		= "Wstaw linie pozioma" ;
lang["InsertSpecialChar"]	= "Wstaw znak specjalny" ;
lang["InsertSmiley"]		= "Wstaw emotikone" ;
lang["About"]			= "FCKeditor - informacje" ;

lang["Bold"]			= "Pogrubienie" ;
lang["Italic"]			= "Kursywa" ;
lang["Underline"]		= "Podkreslenie" ;
lang["StrikeThrough"]	= "Przekreslenie" ;
lang["Subscript"]		= "Indeks dolny" ;
lang["Superscript"]		= "Indeks górny" ;
lang["LeftJustify"]		= "Wyrównanie do lewej" ;
lang["CenterJustify"]	= "Wyrównanie do srodka" ;
lang["RightJustify"]	= "Wyrównaj do prawej" ;
lang["BlockJustify"]	= "Wyjustuj" ;
lang["DecreaseIndent"]	= "Zmniejsz wciecie" ;
lang["IncreaseIndent"]	= "Zwieksz wciecie" ;
lang["Undo"]			= "Cofnij" ;
lang["Redo"]			= "Ponów" ;
lang["NumberedList"]	= "Lista numerowana" ;
lang["BulletedList"]	= "Lista wypunktowana" ;

lang["ShowTableBorders"]= "Pokazuj ramke tabeli" ;
lang["ShowDetails"]		= "Pokaz szczególy" ;

lang["FontStyle"]		= "Styl" ;
lang["FontFormat"]		= "Format" ;
lang["Font"]			= "Czcionka" ;
lang["FontSize"]		= "Rozmiar" ;
lang["TextColor"]		= "Kolor tekstu" ;
lang["BGColor"]			= "Kolor tla" ;
lang["Source"]			= "Zródlo dokumentu" ;

// Context Menu

lang["EditLink"]		= "Edytuj hiperlacze" ;
lang["InsertRow"]		= "Wstaw wiersz" ;
lang["DeleteRows"]		= "Usun wiersze" ;
lang["InsertColumn"]	= "Wstaw kolumne" ;
lang["DeleteColumns"]	= "Usun kolumny" ;
lang["InsertCell"]		= "Wstaw komórke" ;
lang["DeleteCells"]		= "Usun komórki" ;
lang["MergeCells"]		= "Polacz komórki" ;
lang["SplitCell"]		= "Podziel komórke" ;
lang["CellProperties"]	= "Wlasciwosci komórki" ;
lang["TableProperties"]	= "Wlasciwosci tabeli" ;
lang["ImageProperties"]	= "Wlasciwosci obrazka" ;

// Alerts and Messages

lang["ProcessingXHTML"]	= "Przetwarzanie XHTML. Prosze czekac..." ;
lang["Done"]			= "Gotowe" ;
lang["PasteWordConfirm"]= "Tekst, który chcesz wkleic, prawdopodobnie pochodzi z programu Word. Czy chcesz go wyczyscic przed wklejeniem?" ;
lang["NotCompatiblePaste"]	= "Ta funkcja jest dostepna w programie Internet Explorer w wersji 5.5 lub wyzszej. Czy chcesz wkleic bez czyszczenia?" ;

// Dialogs
lang["DlgBtnOK"]		= "OK" ;
lang["DlgBtnCancel"]	= "Anuluj" ;
lang["DlgBtnClose"]		= "Zamknij" ;

// Image Dialog
lang["DlgImgTitleInsert"]	= "Wstaw obrazek" ;
lang["DlgImgTitleEdit"]	= "Edytuj obrazek" ;
lang["DlgImgBtnUpload"]	= "Wyslij" ;
lang["DlgImgURL"]		= "URL" ;
lang["DlgImgUpload"]	= "Wyslij" ;
lang["DlgImgBtnBrowse"]	= "Przegladaj" ;
lang["DlgImgAlt"]		= "Tekst zastepczy" ;
lang["DlgImgWidth"]		= "Szerokosc" ;
lang["DlgImgHeight"]	= "Wysokosc" ;
lang["DlgImgLockRatio"]	= "Zachowaj proporcje" ;
lang["DlgBtnResetSize"]	= "Skasuj rozmiar" ;
lang["DlgImgBorder"]	= "Ramka" ;
lang["DlgImgHSpace"]	= "Odstep poziomy" ;
lang["DlgImgVSpace"]	= "Odstep pionowy" ;
lang["DlgImgAlign"]		= "Wyrównanie" ;
lang["DlgImgAlignLeft"]	= "Do lewej" ;
lang["DlgImgAlignAbsBottom"]	= "Do dolu" ;
lang["DlgImgAlignAbsMiddle"]	= "Do srodka w pionie" ;
lang["DlgImgAlignBaseline"]	= "Do linii bazowej" ;
lang["DlgImgAlignBottom"]	= "Do dolu" ;
lang["DlgImgAlignMiddle"]	= "Do srodka" ;
lang["DlgImgAlignRight"]	= "Do prawej" ;
lang["DlgImgAlignTextTop"]	= "Do góry tekstu" ;
lang["DlgImgAlignTop"]		= "Do góry" ;
lang["DlgImgPreview"]		= "Podglad" ;
lang["DlgImgMsgWrongExt"]	= "Niestety dozwolone sa tylko nastepujace typy plików:\n\n" + config.ImageUploadAllowedExtensions + "\n\nOperacja nie powiodla sie." ;
lang["DlgImgAlertSelect"]	= "Wybierz obrazek do wyslania." ;		// NEW


// Link Dialog
lang["DlgLnkWindowTitle"]	= "Hiperlacze" ;		// NEW
lang["DlgLnkURL"]			= "Adres URL" ;
lang["DlgLnkUpload"]		= "Wyslij" ;
lang["DlgLnkTarget"]		= "Okno docelowe" ;
lang["DlgLnkTargetNotSet"]	= "<brak ustawien>" ;
lang["DlgLnkTargetBlank"]	= "Nowe okno (_blank)" ;
lang["DlgLnkTargetParent"]	= "Okno nadrzedne (_parent)" ;
lang["DlgLnkTargetSelf"]	= "Okno biezace (_self)" ;
lang["DlgLnkTargetTop"]		= "Okno najwyzsze w hierarchii (_top)" ;
lang["DlgLnkTitle"]			= "Tytul" ;
lang["DlgLnkBtnUpload"]		= "Wyslij" ;
lang["DlgLnkBtnBrowse"]		= "Przegladaj" ;
lang["DlgLnkMsgWrongExtA"]	= "Dozwolone sa tylko nastepujace typy plików:\n\n" + config.LinkUploadAllowedExtensions + "\n\nOperacja nie powiodla sie." ;
lang["DlgLnkMsgWrongExtD"]	= "Niedozwolone typy plików:\n\n" + config.LinkUploadDeniedExtensions + "\n\nOperacja nie powiodla sie." ;

// Color Dialog
lang["DlgColorTitle"]		= "Wybierz kolor" ;
lang["DlgColorBtnClear"]	= "Wyczysc" ;
lang["DlgColorHighlight"]	= "Podglad" ;
lang["DlgColorSelected"]	= "Wybrane" ;

// Smiley Dialog
lang["DlgSmileyTitle"]		= "Wstaw emotikone" ;

// Special Character Dialog
lang["DlgSpecialCharTitle"]	= "Wstaw znak specjalny" ;

// Table Dialog
lang["DlgTableTitleInsert"]	= "Wstaw tabele" ;
lang["DlgTableTitleEdit"]	= "Edytuj tabele" ;
lang["DlgTableRows"]		= "Liczba wierszy" ;
lang["DlgTableColumns"]		= "Liczba kolumn" ;
lang["DlgTableBorder"]		= "Grubosc ramki" ;
lang["DlgTableAlign"]		= "Wyrównanie" ;
lang["DlgTableAlignNotSet"]	= "<brak ustawien>" ;
lang["DlgTableAlignLeft"]	= "Do lewej" ;
lang["DlgTableAlignCenter"]	= "Do srodka" ;
lang["DlgTableAlignRight"]	= "Do prawej" ;
lang["DlgTableWidth"]		= "Szerokosc" ;
lang["DlgTableWidthPx"]		= "piksele" ;
lang["DlgTableWidthPc"]		= "%" ;
lang["DlgTableHeight"]		= "Wysokosc" ;
lang["DlgTableCellSpace"]	= "Odstep pomiedzy komórkami" ;
lang["DlgTableCellPad"]		= "Margines wewnetrzny komórek" ;
lang["DlgTableCaption"]		= "Tytul" ;

// Table Cell Dialog
lang["DlgCellTitle"]		= "Wlasciwosci komórki" ;
lang["DlgCellWidth"]		= "Szerokosc" ;
lang["DlgCellWidthPx"]		= "piksele" ;
lang["DlgCellWidthPc"]		= "percent" ;
lang["DlgCellHeight"]		= "Wysokosc" ;
lang["DlgCellWordWrap"]		= "Zawijanie tekstu" ;
lang["DlgCellWordWrapNotSet"]	= "<brak ustawien>" ;
lang["DlgCellWordWrapYes"]	= "Tak" ;
lang["DlgCellWordWrapNo"]	= "Nie" ;
lang["DlgCellHorAlign"]		= "Wyrównanie poziome" ;
lang["DlgCellHorAlignNotSet"]	= "<brak ustawien>" ;
lang["DlgCellHorAlignLeft"]	= "Do lewej" ;
lang["DlgCellHorAlignCenter"]	= "Do srodka" ;
lang["DlgCellHorAlignRight"]	= "Do prawej" ;
lang["DlgCellVerAlign"]		= "Wyrównanie pionowe" ;
lang["DlgCellVerAlignNotSet"]	= "<brak ustawien>" ;
lang["DlgCellVerAlignTop"]		= "Do góry" ;
lang["DlgCellVerAlignMiddle"]	= "Do srodka" ;
lang["DlgCellVerAlignBottom"]	= "Do dolu" ;
lang["DlgCellVerAlignBaseline"]	= "Do linii bazowej" ;
lang["DlgCellRowSpan"]		= "Zajetosc wierszy" ;
lang["DlgCellCollSpan"]		= "Zajetosc kolumn" ;
lang["DlgCellBackColor"]	= "Kolor tla" ;
lang["DlgCellBorderColor"]	= "Kolor ramki" ;
lang["DlgCellBtnSelect"]	= "Wybierz..." ;

// About Dialog
lang["DlgAboutVersion"]		= "wersja" ;
lang["DlgAboutLicense"]		= "na licencji GNU Lesser General Public License" ;
lang["DlgAboutInfo"]		= "Wiecej informacji uzyskasz w" ;