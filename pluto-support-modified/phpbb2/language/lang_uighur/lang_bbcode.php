<?php
/***************************************************************************
 *                         lang_bbcode.php [Uighur_latin]
 *                            -------------------
 *   begin                : Wednesday Oct 3, 2001
 *   copyright            : (C) 2001 The phpBB Group
 *   email                : support@phpbb.com
 *
 *   $Id: lang_bbcode.php,v 1.3.2.2 2002/12/18 15:40:20 psotfx Exp $
 *
 *
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/
//
// Uyghurchigha Terjime qilghuchilar/Translation by:
//
// Muhemmed Erdem (M.Erdem)  :: webmaster@misran.com    :: http://www.misran.com
// Abdireyim (abdireyim)     :: abdireyim@yahoo.com     :: http://freeud.tripod.com
//
// Pikirler bolsa bu adresni ishliting/For questions and comments use: misran_erdem@hotmail.com
//
// Diqqet: Bu emgekning hormiti uchun bolsimu tekstlerni, chekitlik herplerni we 
// atalghularni xalighanche ozgertmeng. Pikirliringiz bolsa 
// terjime qilghuchilar bilen alaqe qiling.
//
// Release date: 2003-09-10
//
//
// Format is same as lang_main
// 
// To add an entry to your BBCode guide simply add a line to this file in this format:
// $faq[] = array("question", "answer");
// If you want to separate a section enter $faq[] = array("--","Block heading goes here if wanted");
// Links will be created automatically
//
// DO NOT forget the ; at the end of the line.
// Do NOT put double quotes (") in your BBCode guide entries, if you absolutely must then escape them ie. \"something\";
//
// The BBCode guide items will appear on the BBCode guide page in the same order they are listed in this file
//
// If just translating this file please do not alter the actual HTML unless absolutely necessary, thanks :)
//
// In addition please do not translate the colours referenced in relation to BBCode any section, if you do
// users browsing in your language may be confused to find they're BBCode doesn't work :D You can change
// references which are 'in-line' within the text though.
//
  
$faq[] = array("--","Tonushturush");
$faq[] = array("BBCode dégen néme?", "BBCode bolsa HTML ning alahide bir xil
shekli, sizning BBCode ni ishlitelishingiz sistéma bashqurghuchisigha baghliq.
Undin bashqa siz ayrim yézilmilar üchün bu iqtidarni toxtutup qoyalaysiz. BBCode
ning shekli HTML ge oxshash, nishan [ we ]ning ichige élindu. Uning bilen
némining qandaq körsütülidighanliqini téximu obdan kontrol qilghili bolidu. Siz
maqale yazmaqchi bolghan ramkining üstide BBCode ni qoshush üchün qoyulghan
konupkilar bar. Töwende téximu tepsili chüshenche bérimiz.");

$faq[] = array("--","Tékstni Formatlash");
$faq[] = array("Tom, yantu, asti sizilghan tékstlerni qandaq chiqirimiz?",
"BBCode bezi tékst belgilirini teminligen bolup, siz tékstning sheklini asanla
özgerteleysiz. Mesilen: <ul><li>Tom <b>[b][/b]</b>, yene alayluq: <br /><br
/><b>[b]</b>Yaxshimu siz?<b>[/b]</b><br /><br /><b>Yaxshimu siz?</b> bolup
körünidu<br /><br /></li><li>Astigha sizish toghra kelse <b>[u][/u]</b>,
mesilen:<br /><br /><b>[u]</b>Yaxshimu siz?<b>[/u]</b><br /><br /><u>Yaxshimu
siz?</u> bolup körünidu<br /><br /></li><li> Tékstni yantu qilmaqchi
bolsaq<b>[i][/i]</b>, mesilen: <br /><br />Bu bek <b>[i]</b>yayshi
boldi!<b>[/i]</b><br /><br />yézilghanda, Bu bek <i>yaxshi boldi!</i> bolup
körünidu.</li></ul>");

$faq[] = array("Tékstning renggini yaki rezmisini qandaq özgertimiz?",
"Yézilmidiki tékstning renggini yaki rezmisini özgertish üchün, töwendiki
belgilerni ishlitisiz. Diqqet, körsütish ünümi sizning browséringiz hem
sistémingizgha baghliq: <ul><li>Tékst renggini özgertishte,
<b>[color=][/color]</b> ni ishlitimiz. Özingiz perq etkidek reng ismini
qoysingizmu bolidu (mesilen: red, blue, yellow...) yaki reng kodini
ishletsingizmu bolidu(misal: #FFFFFF, #000000). Misal alsaq, qizil tékst
ishlitish üchün choqum mundaq bolshi kérek:<br /><br
/><b>[color=red]</b>Yaxshimu siz?<b>[/color]</b><br /><br />yaki<br /><br
/><b>[color=#FF0000]</b>Yaxshimu siz?<b>[/color]</b><br /><br />hemmiside mundaq
körünidu:<span style=\"color:red\">Yaxshimu siz?</span><br /><br
/></li><li>Tékst rezmisini özgertishtimu xuddiy yuqurqigha oxshash, tagning
yézilishi: <b>[size=][/size]</b>. Bu tag bolsa sizning ishletken endizingiz
bilen munasiwetlik, lékin tewsiye qilin'ghan format bolsa tékstning rezmisini
piksél bilen ipadileydighan sanliq qimmet, yeni 1 (bek kichik, korelmeysiz) din
29 (alahide chong) ghiche. Misal alsaq:<br /><br
/><b>[size=9]</b>KICHIK<b>[/size]</b> bolghanda <br /><br /> <span
style=\"font-size:9px\">KICHIK</span> bolup körünidu<br /><br /> <br /><br
/><b>[size=24]</b>BÜYÜK<b>[/size]</b> bolghanda<br /><br /> <span
style=\"font-size:24px\">BÜYÜK</span> bolup körünidu</li></ul>");
$faq[] = array("Oxshashmighan taglerni birlikte ishletsem bolamdu?", "Elbette
bolidu. Jelip qilarliq bolsun üchün, mundaq ishletsingiz bolidu. mesilen:<br
/><br /><b>[size=18][color=red][b]</b>DIQQET!<b>[/b][/color][/size]</b><br /><br
/> buningda mundaq körünidu: <span
style=\"color:red;font-size:18px\"><b>DIQQET!</b></span><br /><br />Bundaq
tékstni köp ishlitiwetmeng! Bularni özingiz belgileysiz. BBCode ni ishletkende,
imkan bar toghra belge ishliting. Töwendikisi xata missal:<br /><br
/><b>[b][u]</b>Bu xata missal<b>[/b][/u]</b>");

$faq[] = array("--","Neqil Keltürüshte Ishlitilidighan Belgiler");
$faq[] = array("Neqil keltürüshte ishlitilidighan belgiler", "Ikki xil usulda
maqalidiki pütün tékstni yaki qismen tékstni neqil qilip ishlitishke bolidu.
Kelgen orunni körsütüsh we biwasite neqil qilip ishlitish.<ul><li>Siz sehipide
neqil keltürgende, yézilma  tékstning   <b>[quote=\"\"][/quote]</b> tagining
ichige élip kirilgenlikini körisiz. Bu usul arqiliq siz tallighan melum
matériyallarni, yeni ular melum kishige qaritilamdu yaki bashqa nersilerge
qaritilamdu, neqil keltüreleysiz. Mesilen: Alimning maqalisini neqil
keltürmekchi bolsingiz, mundaq yézishingiz kérek:<br /><br
/><b>[quote=\"Alim\"]</b>Alimning yazmiliri... <b>[/quote]</b><br /><br />
Buning netijisi yeni, <b>Alim mundaq yazghan:(yézilma)</b>, aptomatik halda
neqilning aldigha qoshulup qalidu. Ésingizde bolsunki,<b> choqum</b> \"\"esli
aptorning isimi \"we\" ning ichige élip yézilishi kérek.<br /><br
/></li><li>Ikkinchi xil usulda, siz qarighularche neqil keltürisiz. Buningda
neqil keltürmekchi bolghan tékst  choqum <b>[quote][/quote]</b> ning ichige
élinishi kerek. Bu xil neqil keltürüshte, tékstning aldida 'Neqil:' digen söz
körünidu. </li></ul>");
$faq[] = array("Programma kodi we turaqliq kengliktiki tékst korsitish", "Eger bir
bölek programma kodi we turaqliq kengliktiki tékst körsetmekchi bolsingiz,
<b>[code][/code]</b> tagini ishlitishingiz kérek. Mesilen:<br /><br
/><b>[code]</b>echo \"kod mezmuni\";<b>[/code]</b><br /><br />Kéyin ijra
bolghanda, barliq <b>[code][/code]</b> ning ichige élin'ghan tékstler öz péti
körünidu.");

$faq[] = array("--","Tizimlik Turghuzush");
$faq[] = array("Tertipsiz tizimlik turghuzush", "BBCode ikki xil tizimlik
sheklini qollaydu, tertiplik we tertipsiz. Tertipsiz tizimlikte türler birining
keynidin biri ulinip körünidu, tizimlik turghuzushta <b>[list][/list]</b> ni, tür
ipadileshte <b>[*]</b> ni ishlitimiz. Eger yaxshi köridighan renglerni tizip
chiqmaqhi bolsingiz, mundaq yazisiz. mesilen:<br /><br /><b>[list]</b><br
/><b>[*]</b>Qizil<br /><b>[*]</b>Kök<br /><b>[*]</b>Sériq<br /><b>[/list]</b><br
/><br />Buningda mundaq tizimlik barliqqa
kélidu:<ul><li>Qizil</li><li>Kök</li><li>Sériq</li></ul>");
$faq[] = array("Tertiplik tizimlik turghuzush", "Ikkinchi xil tizimlik shekli,
tertiplik tizimlik turghuzush sizge her bir türning tizimlik nomurini
türghuzushni buyruydu,  <b>[list=1][/list]</b> bilen san tertiplik tizimlik
turghuzushqa, yaki <b>[list=a][/list]</b> bilen élipbe tertipidiki tizimlik
turghuzushqa bolidu. Tertipsiz tizimlikke oxshash biz <b>[*]</b> bilen
tizimliktiki türlerni ipadileymiz. Mesilen:<br /><br /><b>[list=1]</b><br
/><b>[*]</b>Dukan'gha bérish<br /><b>[*]</b>Bir kompyutér sétiwélish<br
/><b>[*]</b>Kompyutér buzulghanda uni tillash<br /><b>[/list]</b><br /><br
/>Buningda mundaq tizimlik barliqqa kélidu:<ol type=\"1\"><li>Dukan'gha
bérish</li><li>Bir kompyutér sétiwélish</li><li>Kompyutér buzulghanda uni
tillash</li></ol>Eger élipbe tertipni ishletkende mundaq bolidu:<br /><br
/><b>[list=a]</b><br /><b>[*]</b>Birinchi mumkinchilik<br /><b>[*]</b>Ikkinchi
mumkinchilik<br /><b>[*]</b>Üchünchi mumkinchilik<br /><b>[/list]</b><br /><br
/>Netije bolsa:<ol type=\"a\"><li>Brinchi mumkinchilik</li><li>Ikkinchi
mumkinchilik </li><li>Üchünchi mumkinchilik</li></ol>");

$faq[] = array("--", "Ulanma Qurush");
$faq[] = array("Bashqa bir betlerge ulash", "phpBB BBCode da bir qanche xil URL
lar, Uniform Resource Indicators, qurush usulliri bar.<ul><li>Ularning ichide,
birinchisi bolsa <b>[url=][/url]</b> tagi arqiliq ulunush hasil qilish, yeni
tenglik belgisi ( = ) din kéyin, némila kirgüzmeng, tag ichidiki tékstler URL
gha oxshash bolup körünidu (yeni ulunush hasil qilidu). Mesilen, phpBB.com gha
ulunush qilishta, mundaq qilsingiz bolidu:<br /><br
/><b>[url=http://www.phpbb.com/]</b>phpBB ni ziyaret qiling!<b>[/url]</b><br
/><br />Buning netijisi bolsa, <a href=\"http://www.phpbb.com/\";
target=\"_blank\">phpBB ni ziyaret qiling!</a> Ulanmini chekkende yéngi bir
köznek échilghanliqini körüsiz, shuning bilen bundaq ulunush
qollan'ghuchining esli betni dawamliq körüshi üchün qolayliq.<br /><br
/></li><li>Eger URL özlikidin ulanma bolup körunsun désingiz, mundaq addiy
belgilesh élip béring:<br /><br
/><b>[url]</b>http://www.phpbb.com/<b>[/url]</b><br /><br />Buningda mundaq
ulanma qurulidu, <a href=\"http://www.phpbb.com/\";
target=\"_blank\">http://www.phpbb.com/</a><br /><br /></li><li> phpBB ning 
qoshumche iqtidarliri ichide, bir <i>Séhirlik Ulanma</i>dégen iqtidar bar bolup
herqandaq sintastikisi toghra bolghan URL larni ulanma qiliwetidu. Siz
héchqandaq belge qoshmisingizmu hetta URL ni bashlaydighan http://ni
yazmisingizmu bolidu. Mesilen, maqalighizde www.phpbb.com yézilsa ,bu,
özlikidinla <a href=\"http://www.phpbb.com/\";
target=\"_blank\">www.phpbb.com</a> ge aylinidu.<br /><br /></li><li>Bu xil
iqtidar oxshashla e-mail adrésiqimu ishleydu. Siz melum adrésni toluq
yazsingizmu bolidu, mesilen:<br /><br
/><b>[email]</b>no.one@domain.adr<b>[/email]</b><br /><br />bu mundaq körünidu:
<a href=\"emailto:no.one@domain.adr\">no.one@domain.adr</a> yaki addiyla
no.one@domain.adr dep kirgüzsingiz, sistéma özlikidin uni ulunushqa
aylanduridu.<br /><br /></li></ul>Siz BBCode we URL belgisini bashqa belgiler
bilen qoshup ishletsingizmu bolidu. Alayluq, <b>[img][/img]</b> (töwendiki
chüshendürüshke qarang), <b>[b][/b]</b>...qatarliqlar, herqandaq belgini
arlashturup ishletsingiz bolidu, emma toghra bolsun yeni belgilerning échilishi
we étilishi tertipi toghra bolushi kérek, mesilen:<br /><br
/><b>[url=http://www.phpbb.com/][img]</b>http://www.phpbb.com/images/phplogo.gif<b>[/url][/img]</b><br
/><br />bu toghra emes, namuwapiq ishlitishler maqalingizning yoqulup kétishige
seweb bolishi mumkin, éhtiyat qilarsiz.");

$faq[] = array("--", "Yézilmilarda Resim Körsütish");
$faq[] = array("Yézilmigha resim kirgüzüsh", "phpBB BBCode yézilma ichige resim
qisturush belgisini teminligen. Ikki muhim ishqa diqqet qiling;  biri, nurghun
qollan'ghuchilar maqalirida bek köp resimler bolushini yaxshi körmeydu, yene
biri, resimler choqum torda bolishi kerek (mesilen: kompyutérdiki höjjet
bolmaydu, kompyutéringiz sérwér bolmisila). Nöwette phpBB bolsa yerlik halda
resim saqlash iqtidarini hazirlimidi  (kéler qetimqi phpBB nusxisida belkim bu
iqtidar qoshup qalar). Resim körsetmekchi bolsingiz choqum <b>[img][/img]</b>
belgisini hem resim ulan'ghan tor adrésini yézing.  mesilen:<br /><br
/><b>[img]</b>http://www.phpbb.com/images/phplogo.gif<b>[/img]</b><br /><br
/>Yuquridiki ulanma qurushta sözlep ötken boyiche, <b>[url][/url]</b> tagi bilen
resimge ulanma qursingizmu bolidu. mesilen:<br /><br
/><b>[url=http://www.phpbb.com/][img]</b>http://www.phpbb.com/images/phplogo.gif<b>[/img][/url]</b><br
/><br />Buning netijisi bolsa:<br /><br /><a href=\"http://www.phpbb.com/\";
target=\"_blank\"><img src=\"templates/subSilver/images/logo_phpBB_med.gif\";
border=\"0\" alt=\"\" /></a><br />");

$faq[] = array("--", "Bashqa Iqtidarlar");
$faq[] = array("Men özem tüzgen tagni ishletsem bolamdu?", "Yaq, nöwette phpBB
2.0 nusxisida bu iqtidar yoq. Biz ashundaq bir mumkinchilikni tépip uni phpBB
ning kéyinki nusxisida teminlesh üchün tirishiwatimiz.");

//
// This ends the BBCode guide entries
//

?>