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
 * cz.js: Czech support.
 *
 * Authors:
 *   Plachow (plachow@atlas.cz)
 */

// Toolbar Items and Context Menu

lang["Cut"]					= "Vyjmout" ;
lang["Copy"]				= "Kopírovat" ;
lang["Paste"]				= "Vložit" ;
lang["PasteText"]			= "Vložit bez formátování" ;
lang["PasteWord"]			= "Vložit z Wordu" ;
lang["Find"]				= "Najít" ;
lang["SelectAll"]			= "Vybrat vše" ;
lang["RemoveFormat"]		= "Odstranit formátování" ;
lang["InsertLink"]			= "Vložit/zmenit odkaz" ;
lang["RemoveLink"]			= "Odstranit odkaz" ;
lang["InsertImage"]			= "Vložit/zmenit obrázek" ;
lang["InsertTable"]			= "Vložit/zmenit tabulku" ;
lang["InsertLine"]			= "Vložit horizontální linku" ;
lang["InsertSpecialChar"]	= "Vložit speciální znak" ;
lang["InsertSmiley"]		= "Vložit smajlík" ;
lang["About"]				= "O aplikaci FCKeditor" ;

lang["Bold"]				= "Tucne" ;
lang["Italic"]				= "Kurzíva" ;
lang["Underline"]			= "Podtržení" ;
lang["StrikeThrough"]		= "Preškrtnutí" ;
lang["Subscript"]			= "Spodní index" ;
lang["Superscript"]			= "Horní index" ;
lang["LeftJustify"]			= "Zarovnat vlevo" ;
lang["CenterJustify"]		= "Zarovnat na stred" ;
lang["RightJustify"]		= "Zarovnat vpravo" ;
lang["BlockJustify"]		= "Zarovnat do bloku" ;
lang["DecreaseIndent"]		= "Zmenšit odsazení" ;
lang["IncreaseIndent"]		= "Zvetšit odsazení" ;
lang["Undo"]				= "Zpet" ;
lang["Redo"]				= "Znovu" ;
lang["NumberedList"]		= "Císlovaný seznam" ;
lang["BulletedList"]		= "Seznam s odrážkami" ;

lang["ShowTableBorders"]	= "Zobrazit ohranicení tabulek" ;
lang["ShowDetails"]			= "Zobrazit podrobnosti" ;

lang["FontStyle"]			= "Styl" ;
lang["FontFormat"]			= "Formát" ;
lang["Font"]				= "Písmo" ;
lang["FontSize"]			= "Velikost" ;
lang["TextColor"]			= "Barva textu" ;
lang["BGColor"]				= "Barva pozadí" ;
lang["Source"]				= "Zdroj" ;

// Context Menu

lang["EditLink"]			= "Vlastnosti odkazu" ;
lang["InsertRow"]			= "Vložit rádek" ;
lang["DeleteRows"]			= "Smazat rádek" ;
lang["InsertColumn"]		= "Vložit sloupec" ;
lang["DeleteColumns"]		= "Smazat sloupec" ;
lang["InsertCell"]			= "Vložit bunku" ;
lang["DeleteCells"]			= "Smazat bunky" ;
lang["MergeCells"]			= "Sloucit bunky" ;
lang["SplitCell"]			= "Rozdelit bunku" ;
lang["CellProperties"]		= "Vlastnosti bunky" ;
lang["TableProperties"]		= "Vlastnosti tabulky" ;
lang["ImageProperties"]		= "Vlastnosti obrázku" ;

// Alerts and Messages

lang["ProcessingXHTML"]		= "Zpracovávám XHTML. Moment, prosím..." ;
lang["Done"]				= "Hotovo" ;
lang["PasteWordConfirm"]	= "Text, který práve vkládáte do dokumentu, pochází pravdepodobne z aplikace MS Word.\nChcete provést vycištení zdojového kódu?" ;
lang["NotCompatiblePaste"]	= "Tento príkaz je funkcní pouze v IE verze 5.5 a vyšší. Chcete vložit text bez vycištení?" ;

// Dialogs
lang["DlgBtnOK"]			= "OK" ;
lang["DlgBtnCancel"]		= "Zrušit" ;
lang["DlgBtnClose"]			= "Zavrít" ;

// Image Dialog
lang["DlgImgTitleInsert"]	= "Vložit obrázek" ;
lang["DlgImgTitleEdit"]		= "Zmenit obrázek" ;
lang["DlgImgBtnUpload"]		= "Poslat na server" ;
lang["DlgImgURL"]			= "URL" ;
lang["DlgImgUpload"]		= "Upload" ;
lang["DlgImgBtnBrowse"]		= "Procházet server" ;
lang["DlgImgAlt"]			= "Alternativní text" ;
lang["DlgImgWidth"]			= "Šírka" ;
lang["DlgImgHeight"]		= "Výška" ;
lang["DlgImgLockRatio"]		= "Zachovat pomer stran" ;
lang["DlgBtnResetSize"]		= "Puvodní velikost" ;
lang["DlgImgBorder"]		= "Okraj" ;
lang["DlgImgHSpace"]		= "HSpace" ;
lang["DlgImgVSpace"]		= "VSpace" ;
lang["DlgImgAlign"]			= "Zarovnání" ;
lang["DlgImgAlignLeft"]		= "Left" ;
lang["DlgImgAlignAbsBottom"]	= "Abs Bottom" ;
lang["DlgImgAlignAbsMiddle"]	= "Abs Middle" ;
lang["DlgImgAlignBaseline"]	= "Baseline" ;
lang["DlgImgAlignBottom"]	= "Bottom" ;
lang["DlgImgAlignMiddle"]	= "Middle" ;
lang["DlgImgAlignRight"]	= "Right" ;
lang["DlgImgAlignTextTop"]	= "Text Top" ;
lang["DlgImgAlignTop"]		= "Top" ;
lang["DlgImgPreview"]		= "Náhled" ;
lang["DlgImgMsgWrongExt"]	= "Jsou povoleny pouze následující datové typy:\n\n" + config.ImageUploadAllowedExtensions + "\n\nOperace zrušena." ;
lang["DlgImgAlertSelect"]	= "Please select an image to upload." ;		// TODO

// Link Dialog
lang["DlgLnkWindowTitle"]	= "Link" ;		// TODO
lang["DlgLnkURL"]			= "URL" ;
lang["DlgLnkUpload"]		= "Upload" ;
lang["DlgLnkTarget"]		= "Cíl" ;
lang["DlgLnkTargetNotSet"]	= "<nenastaveno>" ;
lang["DlgLnkTargetBlank"]	= "Nové okno (_blank)" ;
lang["DlgLnkTargetParent"]	= "Rodicovské okno (_parent)" ;
lang["DlgLnkTargetSelf"]	= "Stejné okno (_self)" ;
lang["DlgLnkTargetTop"]		= "Hlavní okno (_top)" ;
lang["DlgLnkTitle"]			= "Nadpis" ;
lang["DlgLnkBtnUpload"]		= "Poslat na server" ;
lang["DlgLnkBtnBrowse"]		= "Procházet server" ;
lang["DlgLnkMsgWrongExtA"]	= "Pro upload jsou povoleny pouze následující datové typy:\n\n" + config.LinkUploadAllowedExtensions + "\n\nOperace zrušena." ;
lang["DlgLnkMsgWrongExtD"]	= "Tyto datové typy nejsou povoleny pro upload:\n\n" + config.LinkUploadDeniedExtensions + "\n\nOperace zrušena." ;

// Color Dialog
lang["DlgColorTitle"]		= "Vyberte barvu" ;
lang["DlgColorBtnClear"]	= "Vymazat" ;
lang["DlgColorHighlight"]	= "Zvýraznit" ;
lang["DlgColorSelected"]	= "Vybraná" ;

// Smiley Dialog
lang["DlgSmileyTitle"]		= "Vložení smajlíku" ;

// Special Character Dialog
lang["DlgSpecialCharTitle"]	= "Vložení speciálního znaku" ;

// Table Dialog
lang["DlgTableTitleInsert"]	= "Vložení tabulky" ;
lang["DlgTableTitleEdit"]	= "Editace tabulky" ;
lang["DlgTableRows"]		= "Rádky" ;
lang["DlgTableColumns"]		= "Sloupce" ;
lang["DlgTableBorder"]		= "Tlouštka okraju" ;
lang["DlgTableAlign"]		= "Zarovnání" ;
lang["DlgTableAlignNotSet"]	= "<nenastaveno>" ;
lang["DlgTableAlignLeft"]	= "Vlevo" ;
lang["DlgTableAlignCenter"]	= "Na stred" ;
lang["DlgTableAlignRight"]	= "Vpravo" ;
lang["DlgTableWidth"]		= "Šírka" ;
lang["DlgTableWidthPx"]		= "pixelu" ;
lang["DlgTableWidthPc"]		= "procent" ;
lang["DlgTableHeight"]		= "Výška" ;
lang["DlgTableCellSpace"]	= "Mezera mezi bunkami" ;
lang["DlgTableCellPad"]		= "Odsazení v bunce" ;
lang["DlgTableCaption"]		= "Titulek" ;

// Table Cell Dialog
lang["DlgCellTitle"]		= "Vlastnosti bunky" ;
lang["DlgCellWidth"]		= "Šírka" ;
lang["DlgCellWidthPx"]		= "pixelu" ;
lang["DlgCellWidthPc"]		= "procent" ;
lang["DlgCellHeight"]		= "Výška" ;
lang["DlgCellWordWrap"]		= "Zalomení textu" ;
lang["DlgCellWordWrapNotSet"]	= "<nenastaveno>" ;
lang["DlgCellWordWrapYes"]		= "Ano" ;
lang["DlgCellWordWrapNo"]		= "ne" ;
lang["DlgCellHorAlign"]		= "Horizontální zarovnání" ;
lang["DlgCellHorAlignNotSet"]	= "<nenastaveno>" ;
lang["DlgCellHorAlignLeft"]		= "Vlevo" ;
lang["DlgCellHorAlignCenter"]	= "Na stred" ;
lang["DlgCellHorAlignRight"]	= "Vpravo" ;
lang["DlgCellVerAlign"]		= "Vertikální zarovnání" ;
lang["DlgCellVerAlignNotSet"]	= "<nenastaveno>" ;
lang["DlgCellVerAlignTop"]		= "Top" ;
lang["DlgCellVerAlignMiddle"]	= "Middle" ;
lang["DlgCellVerAlignBottom"]	= "Bottom" ;
lang["DlgCellVerAlignBaseline"]	= "Baseline" ;
lang["DlgCellRowSpan"]		= "Sprežení rádku" ;
lang["DlgCellCollSpan"]		= "Sprežení sloupcu" ;
lang["DlgCellBackColor"]	= "Barva pozadí" ;
lang["DlgCellBorderColor"]	= "Barva okraju" ;
lang["DlgCellBtnSelect"]	= "Vybrat..." ;

// About Dialog
lang["DlgAboutVersion"]		= "verze" ;
lang["DlgAboutLicense"]		= "Licensed under the terms of the GNU Lesser General Public License" ;
lang["DlgAboutInfo"]		= "For further information go to" ;
