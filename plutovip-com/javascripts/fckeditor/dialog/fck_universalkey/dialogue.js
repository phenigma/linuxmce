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
 * Authors:
 *   Abdul-Aziz Al-Oraij (top7up@hotmail.com)
 *   Michel STAELENS (michel.staelens@wanadoo.fr)
 *   Bernadette CIERZNIAK
 */
 
if (parent.location==window.location){
	document.write('<table><tr><td><input style="WIDTH: 100%" type="button" fckLang="DlgBtnOK" value="OK" onclick="ok();"><br> <input type="button" fckLang="DlgBtnCancel" value="Cancel" onclick="cancel();"> </td><form><td width=100%><textarea name="uni_area" cols="40" rows="4" style="width:100%"></textarea></td></form></tr></table>')
	}
function afficher(txt){
	if (parent.location!=window.location&&parent.formulaire.QuestCourante!=null){
		parent.formulaire.QuestCourante.value=txt
	}else if (parent.location==window.location){
		document.forms[0].elements[0].value=txt
	}
}
function rechercher(){
	if (parent.location!=window.location&&parent.formulaire.QuestCourante!=null){
		return(parent.formulaire.QuestCourante.value)
	}else if (parent.location==window.location){
		return(document.forms[0].elements[0].value)
	}else {
		return("")
	}
}