<?php

/***************************************************************************
 *                            lang_admin.php [Uighur_latin]
 *                              -------------------
 *     begin                : Sat Dec 16 2000
 *     copyright            : (C) 2001 The phpBB Group
 *     email                : support@phpbb.com
 *
 *     $Id: lang_admin.php,v 1.35.2.9 2003/06/10 00:31:19 psotfx Exp $
 *
 ****************************************************************************/




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

//
// Modules, this replaces the keys used
// in the modules[][] arrays in each module file
//
$lang['General'] = 'Uniwérsal';
$lang['Users'] = ' Qollan\'ghuchilar';
$lang['Groups'] = 'Guruppa';
$lang['Forums'] = 'Sehipe';
$lang['Styles'] = 'Uslub/Téma';

$lang['Configuration'] = 'Seplimiler';
$lang['Permissions'] = 'Hoquqlar';
$lang['Manage'] = 'Bashqurush';
$lang['Disallow'] = 'Isimlarni Cheklesh';
$lang['Prune'] = 'Aptomatik Qirqish';
$lang['Mass_Email'] = 'Top E-mail Yollash';
$lang['Ranks'] = 'Derije';
$lang['Smilies'] = 'Chiraylar';
$lang['Ban_Management'] = 'Cheklesh Kontroli';
$lang['Word_Censor'] = 'Söz Sénzorluqi';
$lang['Export'] = 'Éksport Qilish';
$lang['Create_new'] = 'Qurush';
$lang['Add_new'] = 'Qoshush';
$lang['Backup_DB'] = 'Zapaslash';
$lang['Restore_DB'] = 'Sanliqambar Qachilash';




//


// Index
//
$lang['Admin'] = 'Bashqurush';
$lang['Not_admin'] = 'Sizning bu munberni bashqurush hoquqingiz yoq';
$lang['Welcome_phpBB'] = 'phpBB ge xush keldingiz';
$lang['Admin_intro'] = 'Munbéringizge phpBB ni tallighanliqingiz üchün teshekkür éytimiz. Bu yerde munbéringizdiki istatistikilargha nezer salalaysiz. Bu betning sol teripidiki <u>Bashqurush Bashbéti</u> ulunushni chékip ayrilalaysiz. Bu yerdin munber bashbétige ötüsh üchün soldiki phpBB tughini chékisiz. Betning sol teripidiki bashqa ulanmilar munberni her türlük kontrol qilishingizgha ijazet bergen bolup, her bir tür ichide uni ishlitish körsetmisi bérilgen.';
$lang['Main_index'] = 'Munber Bashbéti';
$lang['Forum_stats'] = 'Munber Istatistikiliri';
$lang['Admin_Index'] = 'Bashqurush Bashbéti';
$lang['Preview_forum'] = 'Sehipe Sinash';

$lang['Click_return_admin_index'] = 'Bashqurush bashbétige ötüsh üchün %sbu yerni%s chéking.';

$lang['Statistic'] = 'Istatistika';
$lang['Value'] = 'Qimmiti';
$lang['Number_posts'] = 'Yézilmilar Sani';

$lang['Posts_per_day'] = 'Kündilik Yézilma';
$lang['Number_topics'] = 'Témilar Sani';
$lang['Topics_per_day'] = 'Kündilik Téma';
$lang['Number_users'] = 'Qollan\'ghuchilar Sani';
$lang['Users_per_day'] = 'Kündilik Qollan\'ghuchilar';
$lang['Board_started'] = 'Munber échilghan waqit';
$lang['Avatar_dir_size'] = 'Awatar munderije sighimi';
$lang['Database_size'] = 'Sanliqambar Sighimi';
$lang['Gzip_compression'] ='Gzip Préslash';
$lang['Not_available'] = 'Mewjut emes';

$lang['ON'] = 'Ochuq'; // This is for GZip compression
$lang['OFF'] = 'Étik'; 


//
// DB Utils
//
$lang['Database_Utilities'] = 'Sanliqambar Iqtidarliri';

$lang['Restore'] = 'Eslige Keltürüsh';
$lang['Backup'] = 'Zapaslash';
$lang['Restore_explain'] = 'Bu iqtidar arqiliq phpBB jedwellirini <B>eslige keltürüshke</B> bolidu. Eger sérwér qollisa, u siz yükligen GZip préslan\'ghan höjjitini aptomatik préssizlaydu. <B>Diqqet:</B> Bu arqiliq barliq kona höjjetlerning ornigha yéngisi yézilidu. Eslige keltürüsh xéli uzun waqit dawamlishidu, tamamlinishtin burun bu bettini yapmang.';
$lang['Backup_explain'] = 'Bu yerde phpBB ning barliq sanliq melumatlirini zapasliyalaysiz. Eger phpBB ning sanliqambirigha özingiz jedwel qoshqan bolsingiz hem uni zapaslimaqchi bolsingiz, uning namini \'Qoshumche Jedweller\' katekchisige kirgüzüng(birdin artuq kirgüzgende, pesh bilen ayrip yézing). Eger sérwér qollisa, höjjetliringizni chüshürüshtin burun ularni GZip bilen présliyalaysiz.';

$lang['Backup_options'] = 'Zapaslash Tallimiliri';

$lang['Start_backup'] = 'Zapaslashni bashlash';
$lang['Full_backup'] = 'Toluq zapaslash';
$lang['Structure_backup'] = 'Qurulmisinila zapaslash';
$lang['Data_backup'] = 'Datanila zapaslash';
$lang['Additional_tables'] = 'Qoshumche Jedweller';
$lang['Gzip_compress'] = 'Gzip préslanma höjjiti';
$lang['Select_file'] = 'Bir höjjet tallash';
$lang['Start_Restore'] = 'Eslige keltürüshni bashlash';

$lang['Restore_success'] = 'Sanliqambar muweppeqiyetlik eslige keltürüldi. <br /><br />Munbéringiz zapaslighan waqittiki haletke keltürüldi.';
$lang['Backup_download'] = 'Download hazirla bashlinidu; selpel texir qiling.';
$lang['Backups_not_supported'] = 'Kechürüng, sanliqambar sistémingiz zapas höjjetlerni qollimidi.';

$lang['Restore_Error_uploading'] = 'Zapas höjjetlerni yükleshta xataliq bar';
$lang['Restore_Error_filename'] = 'Höjjet namida mesile bar; bashqa birsini sinang';
$lang['Restore_Error_decompress'] = 'Gzip höjjiti yéyilmidi; sap tékst höjjiti yükleng';
$lang['Restore_Error_no_file'] = 'Héchqandaq höjjet yüklenmidi';


//
// Auth pages
//
$lang['Select_a_User'] = 'Bir qollan\'ghuchi tallang';
$lang['Select_a_Group'] = 'Bir guruppa tallang';
$lang['Select_a_Forum'] = 'Bir sehipe tallang';
$lang['Auth_Control_User'] = 'Qollan\'ghuchi hoquqi kontroli'; 
$lang['Auth_Control_Group'] = 'Guruppa hoquqi kontroli'; 
$lang['Auth_Control_Forum'] = 'Sehipe ishlitish hoquqi kontroli'; 
$lang['Look_up_User'] = 'Qollan\'ghuchi tallash'; 
$lang['Look_up_Group'] = 'Guruppa tallash'; 
$lang['Look_up_Forum'] = 'Sehipe tallash'; 

$lang['Group_auth_explain'] = 'Bu yerde siz guruppining hoquqliri we nazaretchi halitini özgerteleysiz. Untup qalmangki, guruppining hoquqlirini özgertkendin kéyinmu musteqil qollan\'ghuchilarning hoquqi yenila küchke ige. Eger bundaq ehwal bopqalsa, siz xewerlendürilisiz.';
$lang['User_auth_explain'] = 'Bu yerde siz qollan\'ghuchilarning hoquqi we nazaretchilik haletlirini özgerteleysiz. Untup qalmangki, qollan\'ghuchilarning hoquqini özgertkendin kéyinmu guruppining hoquqi yenila küchke ige. Eger bundaq ehwal bopqalsa, siz xewerlendürilisiz.';
$lang['Forum_auth_explain'] = 'Bu yerde sehipini ishlitish hoquqini özgerteleysiz. Addiy we aliydin ibaret ikki xil usul bolup, aliy usul arqiliq sehipilerni nahayti yaxshi kontrolliyalaysiz. Yeni qaysi xildiki qollanchilarning qandaq meshghulatlarni élip baralaydighanliqini belgiliyeleysiz.  Bu özgürüshlerning qollan\'ghuchilarning sehipilerni ishlitishige tesir bolidighanliqini untup qalmang. <B>Eskertish: </B>\'Aliy Usul\' üchün qisqartilghan sözler, Barl.--Barliq, Qoll.--Qollan\'ghuchi, Xusu.--Xususiy, Naza.—Nazaretchi, Bash.--Bashqurghuchi.';

$lang['Simple_mode'] = 'Addiy Usul';
$lang['Advanced_mode'] = 'Aliy Usul';
$lang['Moderator_status'] = 'Nazaretchi haliti';

$lang['Allowed_Access'] = 'Yol Ochuq';
$lang['Disallowed_Access'] = 'Yol Tosaq';
$lang['Is_Moderator'] = 'Nazaretchilik Bar';
$lang['Not_Moderator'] = 'Nazaretchilik Yoq';

$lang['Conflict_warning'] = 'Diqqet: Hoquq toqunushi körüldi';
$lang['Conflict_access_userauth'] = 'Bu qollan\'ghuchi guruppa ezasi bolghanliq seweblik, yenila bu sehipige kirish hoquqi bar. Guruppining hoquqini özgertish yaki bu qollan\'ghuchining guruppa ezaliq salahitini bikar qilish arqiliq uni bu sehipige kirishtin tosalaysiz. Guruppining hoquqliri (öz ichige alghan sehipiler) töwendikidek.';
$lang['Conflict_mod_userauth'] = 'Bu qollan\'ghuchi guruppa ezasi bolghanliq seweblik, yenila bu sehipige nazaretchilik qilalaydu. Guruppining hoquqini özgertish yaki bu qollan\'ghuchining hoquqini élip tashlash arqiliq uni bu sehipige nazaretchililik qilishtin tosalaysiz. Guruppining hoquqliri (öz ichige alghan sehipiler) töwendikidek.';


$lang['Conflict_access_groupauth'] = 'Töwendiki qollan\'ghuchilarning bashqa qollan\'ghuchilargha hoquq bérish salahiti bolghanliq seweblik, yenila bu sehipige kireleydu. Ularning salahitini özgertish arqiliq, ularning bu sehipige kirishini tosalaysiz. Qollan\'ghuchilarning hoquqliri (öz ichige alghan sehipiler) töwendikidek.';
$lang['Conflict_mod_groupauth'] = ' Töwendiki qollan\'ghuchilarning bashqa qollan\'ghuchilargha hoquq bérish salahiti bolghanliq seweblik, yenila bu sehipige nazaretchilik qilalaydu. Ularning salahitini özgertish arqiliq, ularning bu sehipige nazaretchilik qilishini tosalaysiz. Qollan\'ghuchilarning hoquqliri (öz ichige alghan sehipiler) töwendikidek.';

$lang['Public'] = 'Ashkare';
$lang['Private'] = 'Xususiy';
$lang['Registered'] = 'Tizimlatqan';
$lang['Administrators'] = 'Bashqurghuchilar';
$lang['Hidden'] = 'Yoshurun';

// These are displayed in the drop down boxes for advanced
// mode forum auth, try and keep them short!
$lang['Forum_ALL'] = 'Barl.';
$lang['Forum_REG'] = 'Qoll.';
$lang['Forum_PRIVATE'] = 'Xusu.';
$lang['Forum_MOD'] = 'Naza.';
$lang['Forum_ADMIN'] = 'Bash.';

$lang['View'] = 'Körüsh';
$lang['Read'] = 'Oqush';
$lang['Post'] = 'Yollash';
$lang['Reply'] = 'Jawab Qayturush';
$lang['Edit'] = 'Tehrirlesh';

$lang['Delete'] = 'Öchürüsh';
$lang['Sticky'] = 'Muhim';

$lang['Announce'] = 'Uqturush'; 
$lang['Vote'] = 'Bélet Tashlash';
$lang['Pollcreate'] = 'Ray-sinash Qurush';

$lang['Permissions'] = 'Hoquqlar';
$lang['Simple_Permission'] = 'Adettiki Hoquq';


$lang['User_Level'] = 'Qollan\'ghuchi Derjisi'; 
$lang['Auth_User'] = 'Qollan\'ghuchi';
$lang['Auth_Admin'] = 'Bashqurghuchi';
$lang['Group_memberships'] = 'Guruppa Ezaliri';
$lang['Usergroup_members'] = 'Bu guruppining ezaliri';

$lang['Forum_auth_updated'] = 'Sehipe ishlitish hoquqliri yéngilandi';
$lang['User_auth_updated'] = 'Qollan\'ghuchilar hoquqi yéngilandi';
$lang['Group_auth_updated'] = 'Guruppa hoquqliri yéngilandi';

$lang['Auth_updated'] = 'Hoquqlar yéngilandi';
$lang['Click_return_userauth'] = 'Qollan\'ghuchilar Hoquqigha qaytish üchün %sbu yerni%s chéking.';
$lang['Click_return_groupauth'] = 'Guruppilar Hoquqigha qaytish üchün %sbu yerni%s chéking.';
$lang['Click_return_forumauth'] = 'Sehipiler Hoquqigha qaytish üchün %sbu yerni%s chéking.';


//
// Banning
//
$lang['Ban_control'] = 'Cheklesh Kontroli';
$lang['Ban_explain'] = 'Bu yerde qollan\'ghuchilarni chekliyeleysiz. Belgilen\'gen bir qollan\'ghuchini chekliyeleysiz weyaki melum bir dairidiki IP adrésni yaki host naminimu chekliyeleysiz. Bu usul arqiliq cheklen\'gen qollan\'ghuchilarning munber bashbétige kirishini tosqili bolidu. Belgilen\'gen e-mail adrésini cheklesh arqiliq, u qollan\'ghuchining bashqa isim bilen tizimlitishini tosalaysiz. E-mail adrésini cheklesh arqiliq peqet qayta tizimlitishini tosqili bolidu, biraq yézilmilarni yollishini tosqili bolmaydu. Shunga aldinqi ikki usulning birsini ishlitip chekleng.';
$lang['Ban_explain_warn'] = 'Diqqet qilingki, birnechche IP adrésni kirgüzginingizde, bu IP adréslar arisidiki barliq IP adréslar cheklesh dairisige kirip kétidu. Adrésning bek köp qoshulup kétishining aldini élish üchün, maslashturush belgisi(*) ni ishliting. Eng yaxshisi konkrét bir IP adrési kirgüzüng.';

$lang['Select_username'] = 'Qollanchi isimini tallang';
$lang['Select_ip'] = 'IP adrésni tallang';
$lang['Select_email'] = 'E-mail adrésini tallang';

$lang['Ban_username'] = 'Bir yaki birnechche qollan\'ghuchi isimini cheklesh';
$lang['Ban_username_explain'] = 'Birleshme konupka we maus arqiliq birnechche qollan\'ghuchini biraqla chekliyeleysiz.';

$lang['Ban_IP'] = 'Bir yaki birnechche IP adrési yaki hostnamini cheklesh';
$lang['IP_hostname'] = 'IP adrésliri yaki hostnamliri';
$lang['Ban_IP_explain'] = 'Oxshimighan birnechche IP adrésini yaki hostnamini kirgüzügende, ularni pesh bilen ayring. Bir dairidiki IP adrésini kirgüzgende, bashlinishi we axiridikini siziqche(-) bilen ayring; (*) belgisini maslashturushta ishliting.';

$lang['Ban_email'] = 'Bir yaki birnechche e-mail adrésini cheklesh';
$lang['Ban_email_explain'] = ' Oxshimighan birnechche e-mail kirgüzügende, ularni pesh bilen ayring. (*) belgisini maslashturushta ishliting (meslen: *@hotmail.com).';

$lang['Unban_username'] = 'Qollanchi chekleshni qayturush';
$lang['Unban_username_explain'] = 'Birleshme konupka we maus arqiliq birnechche qollan\'ghuchini biraqla qayturalaysiz.';

$lang['Unban_IP'] = 'IP adrésini chekleshni qayturush';
$lang['Unban_IP_explain'] = 'Birleshme konupka we maus arqiliq birnechche IP adrésni biraqla qayturalaysiz.';

$lang['Unban_email'] = 'E-mail adrési chekleshni qayturush';
$lang['Unban_email_explain'] = 'Birleshme konupka we maus arqiliq birnechche e-mail adrésini biraqla chekliyeleysiz.';

$lang['No_banned_users'] = 'Cheklen\'gen qollan\'ghuchi isimi yoq';
$lang['No_banned_ip'] = 'Cheklen\'gen IP adrési yoq';
$lang['No_banned_email'] = 'Cheklen\'gen e-mail adrési yoq';

$lang['Ban_update_sucessful'] = 'Cheklesh tizimi muweppeqiyetlik yéngilandi';
$lang['Click_return_banadmin'] = 'Cheklesh Kontroligha qaytish üchün %sbu yerni%s chéking.';


//
// Configuration
//
$lang['General_Config'] = 'Omumiy Seplimiler';

$lang['Config_explain'] = 'Bu yerde munbéringizdiki asasliq tallashlarni özgerteleysiz. Qollan\'ghuchi we sehipe seplimiliri üchün sol tereptiki bashqurush ulunushlirini ishliting.';

$lang['Click_return_config'] = 'Omumiy Seplimilerge qaytish üchün %sbu yerni%s chéking. ';

$lang['General_settings'] = 'Omumiy Tengshekler';
$lang['Server_name'] = 'Domain Nami';
$lang['Server_name_explain'] = 'Munber programmisi ijra bolidighan domain nami';
$lang['Script_path'] = 'Programma munderijisi';
$lang['Script_path_explain'] = 'phpBB2 programmisining domain adrésidiki mas munderijisi';
$lang['Server_port'] = 'Sérwér Porti';
$lang['Server_port_explain'] = 'Sérwér ijra bolidighan port, u adette 80 bolidu. Mushundaq bolmighanda uni özgerting';
$lang['Site_name'] = 'Munber Nami';
$lang['Site_desc'] = 'Munber Chüshendürlishi';

$lang['Board_disable'] = 'Munberni taqash';
$lang['Board_disable_explain'] = 'Bu munber barliq qollan\'ghuchilargha taqilidu. Bu iqtidarni ijra qilghandin kéyin munberdin chiqip kétmeng, bolmisa qayta kirelmeysiz.';
$lang['Acct_activation'] = 'Hésab qozghutush';
$lang['Acc_None'] = 'Hajetsiz'; // These three entries are the type of activation
$lang['Acc_User'] = 'E-mail arqiliq';
$lang['Acc_Admin'] = 'Bashqurghuchi arqiliq';

$lang['Abilities_settings'] = 'Qollan\'ghuchi we Sehipe Tengshekliri';
$lang['Max_poll_options'] = 'Ray-sinash türlirining maksimum sani';
$lang['Flood_Interval'] = 'Tashqin Ariliqi';
$lang['Flood_Interval_explain'] = 'Qollan\'ghuchining ikki yézilma ariliqidiki
saqlash waqit chéki[sékont]'; 
$lang['Board_email_form'] = 'Qollan\'ghuchilar ara e-mail';
$lang['Board_email_form_explain'] = 'Qollan\'ghuchilar bu munber arqiliq öz ara
e-mail yézishalaydu.';
$lang['Topics_per_page'] = 'Her bettiki témilar sani';
$lang['Posts_per_page'] = 'Her bettiki yézilmilar sani';
$lang['Hot_threshold'] = 'Qizziq témigha aylinishtiki yézilmilar sani';
$lang['Default_style'] = 'Asasiy Uslub';
$lang['Override_style'] = 'Qollan\'ghuchilar uslubini özgertish';
$lang['Override_style_explain'] = 'Qollan\'ghuchilar tallighan uslubni asasiy
uslubqa özgertidu.';
$lang['Default_language'] = 'Asasiy Til';

$lang['Date_format'] = 'Waqit Formati';
$lang['System_timezone'] = 'Sistéma Waqit Rayoni';
$lang['Enable_gzip'] = 'GZip préslashqa ijazet bérish';
$lang['Enable_prune'] = 'Qirqishqa ijazet bérish';
$lang['Allow_HTML'] = 'HTML ge ijazet bérish';


$lang['Allow_BBCode'] = 'BBCode ge ijazet bérish';
$lang['Allowed_tags'] = 'HTML belgisige ijazet bérish';
$lang['Allowed_tags_explain'] = 'Belgilerni pesh bilen ayring';
$lang['Allow_smilies'] = 'Chiraylargha ijazet bérish';
$lang['Smilies_path'] = 'Chiraylar saqlan\'ghan munderije';
$lang['Smilies_path_explain'] = 'phpBB bash munderijisining astidiki chiraylar
saqlan\'ghan munderije, mesilen: images/smiles';
$lang['Allow_sig'] = 'Imzagha ijazet bérish';
$lang['Max_sig_length'] = 'Imzaning maksimum uzunluqi';
$lang['Max_sig_length_explain'] = 'Qollan\'ghuchining imzasidiki eng köp hérpler
sanining chéki.';
$lang['Allow_name_change'] = 'Isimini özgertishke ijazet bérish';

$lang['Avatar_settings'] = 'Awatar Tengshekliri';
$lang['Allow_local'] = 'Awatar galériyesige ijazet bérish';
$lang['Allow_remote'] = 'Yiraqtiki awatargha ijazet bérish';
$lang['Allow_remote_explain'] = 'Bashqa torbétidiki awatarni ulunush qilip
ishlitish';
$lang['Allow_upload'] = 'Awatar yükleshke ijazet bérish';
$lang['Max_filesize'] = 'Awatarning maksimum  sighimi';
$lang['Max_filesize_explain'] = 'Yüklimekchi bolghan awatar resimi üchün';
$lang['Max_avatar_size'] = 'Awatarning maksimum rezmisi';
$lang['Max_avatar_size_explain'] = '(Piksél boyiche Igizlik x Kenglik)';
$lang['Avatar_storage_path'] = 'Awatar saqlan\'ghan munderije';
$lang['Avatar_storage_path_explain'] = ' phpBB bash munderijining astidiki
awatar saqlan\'ghan munderije, mesilen: images/avatars';
$lang['Avatar_gallery_path'] = 'Awatar galériyisi munderijisi';
$lang['Avatar_gallery_path_explain'] = ' phpBB bash munderijining astidiki
awatar-galériyisi saqlan\'ghan munderije, mesilen: images/avatars/gallery';

$lang['COPPA_settings'] = 'COPPA Tengshekliri';
$lang['COPPA_fax'] = 'COPPA Faks Nomuri';
$lang['COPPA_mail'] = 'COPPA E-mail Adrési';
$lang['COPPA_mail_explain'] = 'Bu e-mail adrésigha ata-analar COPPA gha
tizimlitish iltimaslirini yollaydu';

$lang['Email_settings'] = 'E-mail Tengshekliri';


$lang['Admin_email'] = 'Bashqurghuchi E-mail Adrési';
$lang['Email_sig'] = 'E-mail Imzasi';
$lang['Email_sig_explain'] = 'Bu imza bashqurghuchi yollighan barliq
e-maillargha qoshuludu';
$lang['Use_SMTP'] = 'SMTP Sérwér arqiliq e-mail yollash';
$lang['Use_SMTP_explain'] = 'SMTP Sérwér arqiliq e-mail yollimaqchi bolsingiz,
\'Hee\' ni tallang';
$lang['SMTP_server'] = 'SMTP Sérwér Adrési';
$lang['SMTP_username'] = 'SMTP qollan\'ghuchi isimi';
$lang['SMTP_username_explain'] = 'SMTP sérwér isimni telep qilghanda, andin uni 
kirgüzüng';
$lang['SMTP_password'] = 'SMTP Paroli';
$lang['SMTP_password_explain'] = ' SMTP sérwér parolni telep qilghanda, andin
uni  kirgüzüng';

$lang['Disable_privmsg'] = 'Xususiy Meséj';
$lang['Inbox_limits'] = 'Xet sanduqining maksimum sighimi';
$lang['Sentbox_limits'] = 'Yollash sanduqining maksimum sighimi';
$lang['Savebox_limits'] = 'Saqlash sanduqining maksimum sighimi';

$lang['Cookie_settings'] = 'Cookie Tengshekliri'; 
$lang['Cookie_settings_explain'] = 'Bu yerde cookie üchün browsérgha néme
yollinidginaliqini belgiliyeleysiz. Köp ehwalda mushu esli qimmiti boyiche
belgilinidu, eger choqum özgertish kérek bolghanda éhtiyatchanliq bilen ish
qiling. Xata békitilip qalsa héchkim bu munberge kirelmesliki mumkin.';
$lang['Cookie_domain'] = 'Cookie domaini';
$lang['Cookie_name'] = 'Cookie nami';
$lang['Cookie_path'] = 'Cookie munderijisi';
$lang['Cookie_secure'] = 'Cookie bixeterliki';
$lang['Cookie_secure_explain'] = 'Eger sérwéringiz SSL da ijra bolsa, bu
iqtidarni qozghutung, bolmisa taqaq tursun';
$lang['Session_length'] = 'Séssiye uzunluqi [sékont]';

// Visual Confirmation
$lang['Visual_confirm'] = 'Körüp Éniqlashni Qozghutush';
$lang['Visual_confirm_explain'] = 'Tizimlatqanda qollan\'ghuchidin resim(teswir)
arqilik békitilgen bir kodni kirgüzüshi telep qilinidu.';


//
// Forum Management
//
$lang['Forum_admin'] = 'Sehipe Bashqurush';
$lang['Forum_admin_explain'] = 'Bu yerde katégoriye we sehipe qoshush, öchürüsh,
tehrirlesh hem qaytidin retlesh élip baralaysiz.';
$lang['Edit_forum'] = 'Sehipe tehrirlesh';
$lang['Create_forum'] = 'Yéngi sehipe qurush';
$lang['Create_category'] = 'Yéngi katégoriye qurush';

$lang['Remove'] = 'Chiqiriwétish';
$lang['Action'] = 'Meshghulat';
$lang['Update_order'] = 'Tertipni özgertish';
$lang['Config_updated'] = 'Sehipe seplimiliri muweppiqiyetlik özgertildi';
$lang['Edit'] = 'Tehrirlesh';
$lang['Delete'] = 'Öchürüsh';

$lang['Move_up'] = 'Üstige';
$lang['Move_down'] = 'Astigha';
$lang['Resync'] = 'Resync';
$lang['No_mode'] = 'Héchqandaq usul tallanmidi';
$lang['Forum_edit_delete_explain'] = 'Bu yerde siz munberge ait barliq
tengsheklerni belgiliyeleysiz. Qollan\'ghuchilar we sehipe tengsheklirini
belgilesh üchün sol tereptiki mas ulunushlarni ishliting';

$lang['Move_contents'] = 'Barliq mezmunlarni yötkesh';
$lang['Forum_delete'] = 'Sehipe öchürüsh';
$lang['Forum_delete_explain'] = 'Bu yerde sehipe yaki katégoriye öchüreleysiz,
hem sehipidiki barliq mezmunlarni öz ichige alghan témilarni yaki sehipilerni
yötkiyeleysiz.';

$lang['Status_locked'] = 'Quluplandi';
$lang['Status_unlocked'] = 'Échildi';
$lang['Forum_settings'] = 'Sehipe Tengshekliri';
$lang['Forum_name'] = 'Sehipe nami';
$lang['Forum_desc'] = 'Sehipe chüshendürlishi';
$lang['Forum_status'] = 'Sehipe haliti';
$lang['Forum_pruning'] = 'Aptomatik Qirqish';

$lang['prune_freq'] = 'Qererlik tekshürüsh dewri';
$lang['prune_days'] = 'Xélidin béri jawab yézilmighan témilarni qirqish ';
$lang['Set_prune_data'] = 'Aptomatik qirqish iqtidarini qozghattingiz, emma
dewrilinidighan künning sanini bermidingiz. Qaytip kün sanini belgileng.';


$lang['Move_and_Delete'] = 'Yötkesh we Öchürüsh';

$lang['Delete_all_posts'] = 'Barliq yézilmilarni öchürüsh';
$lang['Nowhere_to_move'] = 'Yötkeydighan yer yoq';

$lang['Edit_Category'] = 'Katégoriye tehrirlesh';
$lang['Edit_Category_explain'] = 'Bu yerde bir katégoriyening namini
özgerteleysiz.';

$lang['Forums_updated'] = 'Sehipe we katégoriye uchurliri muweppeqiyetlik
özgertildi.';

$lang['Must_delete_forums'] = 'Bu katégoriyeni öchürüshtin burun uningdiki
barliq sehipilerni öchürüshingiz kérek.';

$lang['Click_return_forumadmin'] = 'Sehipe Bashqurushqa qaytish üchün %sbu
yerni%s chéking.';


//
// Smiley Management
//
$lang['smiley_title'] = 'Chiraylar Tehrirlesh';
$lang['smile_desc'] = 'Bu yerde siz bu chiraylarni, yeni qollan\'ghuchilar
yazmiliri we xususiy meséjlirida ishlitidighan chiraylarni qoshalaysiz,
öchüreleysiz weyaki tehrirliyeleysiz.';

$lang['smiley_config'] = 'Chiray Seplimiliri';
$lang['smiley_code'] = 'Chiray Kodi';
$lang['smiley_url'] = 'Chiray Resim Höjjiti';
$lang['smiley_emot'] = 'Chiray Resim Keypiyati';
$lang['smile_add'] = 'Yéngi Chiray Qoshush';

$lang['Smile'] = 'Chiray';
$lang['Emotion'] = 'Keypiyati';

$lang['Select_pak'] = 'Pak (Pack) höjjitini tallash';
$lang['replace_existing'] = 'Hazirqi chirayni almashturush';
$lang['keep_existing'] = 'Hazirqi chirayni saqlap qélish';

$lang['smiley_import_inst'] = 'Siz chiray Pack (.pak) höjjitini yéying hem
barliq höjjetlerni muwapiq munderijige yükleng. Andin toghra buyruqni tallap
ularni qachilang.';

$lang['smiley_import'] = 'Chiraylar Paki Kirgüzüsh';
$lang['choose_smile_pak'] = 'Chiray pak höjjitini tallash';
$lang['import'] = 'Chiraylar kirgüzüsh';
$lang['smile_conflicts'] = 'Toqunush bolghanda néme qilish kérek?';
$lang['del_existing_smileys'] = 'Chiraylarni import qilishtin burun, bar bolghan
chiraylarni öchürüng';
$lang['import_smile_pack'] = 'Chiraylar Paki Kirgüzüsh';
$lang['export_smile_pack'] = 'Chiraylar Paki qurush';
$lang['export_smiles'] = 'Hazirqi chiraylardin bir pak qurush üchün, %sbu
yerni%s chékip smiles.pak höjjitini chüshüreleysiz. Höjjet kéngeytilgen namining
.pak bolishigha diqqet qiling. Andin  barliq chiraylar resimliridin bir
préslan\'ghan .pak höjjiti qurudu.';

$lang['smiley_add_success'] = 'Chiray muweppeqiyetlik qoshuldi!';
$lang['smiley_edit_success'] = 'Chiray muweppeqiyetlik özgertildi!';
$lang['smiley_import_success'] = 'Chiraylar muweppeqiyetlik kirgüzüldi!';
$lang['smiley_del_success'] = 'Chiraylar muweppeqiyetlik öchürüldi!';
$lang['Click_return_smileadmin'] = 'Chiraylar Bashqurushqa qaytish üchün %sbu
yerni%s chéking.';



//
// User Management
//
$lang['User_admin'] = 'Qollan\'ghuchi Bashqurush';
$lang['User_admin_explain'] = 'Bu yerde qollan\'ghuchilarning uchurini we bezi
tallashlarni özgerteleysiz. Eger qollan\'ghuchining hoquqini özgertmekchi
bolsingiz, qollan\'ghuchi hoquqi we guruppa hoquqi iqtidarini ishliting.';

$lang['Look_up_user'] = 'Qollan\'ghuchi tallash';

$lang['Admin_user_fail'] = 'Qollan\'ghuchi profili özgertilelmidi.';
$lang['Admin_user_updated'] = 'Qollan\'ghuchi profile muweppeqiyetlik
özgertildi.';
$lang['Click_return_useradmin'] = 'Qollan\'ghuchi Bashqurushqa qaytish üchün 
%sbu yerni%s chéking.';

$lang['User_delete'] = 'Bu qollan\'ghuchini öchürüsh';
$lang['User_delete_explain'] = 'Qollanchini öchürüsh üchün bu yerni chéking.
Öchürgendin kéyin eslige keltürülelmeydu.';
$lang['User_deleted'] = 'Qollan\'ghuchi muweppeqiyetlik öchürüldi.';

$lang['User_status'] = 'Bu qollan\'ghuchi qozghutulghan halette';
$lang['User_allowpm'] = 'Xususiy meséj iberteleydu';
$lang['User_allowavatar'] = 'Awatar ishliteleydu';

$lang['Admin_avatar_explain'] = 'Bu yerde qollan\'ghuchining hazirqi awatarini
köreleysiz hem öchüreleysiz.';

$lang['User_special'] = 'Alahide Tallashlar(bashqurghuchi üchün)';
$lang['User_special_explain'] = 'Bu tallashni qollan\'ghuchilar özgertelmeydu. 
Siz bu yerde qollan\'ghuchilarning tallishigha bérilmigen haletlerni we bashqa
tallashlirini özgerteleysiz.';


//
// Group Management

//
$lang['Group_administration'] = 'Guruppa Bashqurush';
$lang['Group_admin_explain'] = 'Bu yerde siz barliq guruppilarni bashquralaysiz.
Siz hazir bar bolghan guruppilarni öchüreleysiz, tehrirliyeleysiz yaki yéngi
guruppa quralaysiz. Siz guruppa bashqurghuchini tallap, guruppining
haletlirini(ochuq, taqaq, yoshurun) özgerteleysiz hem guruppining nami we
chüshendürlishini özgerteleysiz.';

$lang['Error_updating_groups'] = 'Guruppa yéngilighanda xataliq körüldi';
$lang['Updated_group'] = 'Guruppa muweppeqiyetlik özgertildi';
$lang['Added_new_group'] = 'Yéngi guruppa muweppeqiyetlik quruldi';
$lang['Deleted_group'] = 'Guruppa muweppeqiyetlik öchürüldi';
$lang['New_group'] = 'Yéngi guruppa qurush';
$lang['Edit_group'] = 'Guruppa tehrirlesh';

$lang['group_name'] = 'Guruppa nami';
$lang['group_description'] = 'Guruppa chüshendürlishi';
$lang['group_moderator'] = 'Guruppa bashqurghuchi';
$lang['group_status'] = 'Guruppa haliti';
$lang['group_open'] = 'Ochuq guruppa';
$lang['group_closed'] = 'Taqaq guruppa';

$lang['group_hidden'] = 'Yoshurun guruppa';
$lang['group_delete'] = 'Öchürülgen guruppa';
$lang['group_delete_check'] = 'Bu guruppini öchürüsh';

$lang['submit_group_changes'] = 'Özgürüshlerni yollash';
$lang['reset_group_changes'] = 'Qaytilash';
$lang['No_group_name'] = 'Guruppigha bir nam béring';
$lang['No_group_moderator'] = 'Guruppa bashqurghuchi belgileng';
$lang['No_group_mode'] = 'Guruppining halitini (ochuq/taqaq) belgilishingz
kérek';
$lang['No_group_action'] = 'Héchnerse tallanmidi';
$lang['delete_group_moderator'] = 'Kona guruppa bashliqini öchüremsiz?';
$lang['delete_moderator_explain'] = 'Eger guruppa bashqurghuchini
almashturmaqchi bolsingiz, tallashni tallap kona bashqurghuchini öchürüng. 
Uningdin bashqa tallashni tallimisingiz u qollan\'ghuchi mushu guruppining
ezaliqida turiwéridu.';
$lang['Click_return_groupsadmin'] = 'Guruppa Bashqurushqa qaytish üchün %sbu
yerni%s chéking.';
$lang['Select_group'] = 'Guruppa tallash';
$lang['Look_up_group'] = 'Guruppa izdesh';



//
// Prune Administration
//
$lang['Forum_Prune'] = 'Aptomatik Qirqish';
$lang['Forum_Prune_explain'] = 'Bu iqtidarda, belgilen\'gen melum waqit ichide
jawab yézilmighan témilarning hemmisi aptomatik öchürülidu. Eger waqitni
belgilimisingiz barliq témilar öchürülidu. Ray sinash élip bériliwatqan téma we
uqturush öchörülmeydu. Ularni qol bilen öchürüsh kérek.';
$lang['Do_Prune'] = 'Aptomatik qirqish';
$lang['All_Forums'] = 'Barliq Sehipe';

$lang['Prune_topics_not_posted'] = 'Birnechche küngiche jawab yézilmighan
témilar qirqilidu';
$lang['Topics_pruned'] = 'Qirqilidighan Témilar';
$lang['Posts_pruned'] = 'Qirqilidighan Yézilmilar';
$lang['Prune_success'] = 'Sehipini qirqish muweppeqiyetlik boldi';


//
// Word censor

//
$lang['Words_title'] = 'Söz Süzüsh';
$lang['Words_explain'] = 'Bu yerde yézilmilardiki aptomatik süzülidighan
sözlerni belgiliyeleysiz. Uningdin bashqa qollan\'ghuchilar bu sözlerni öz ichige
alghan isimlarnimu ishlitelmeydu. Siz bu sözlerde maslashturush belgisi (*) ni
ishletsingizmu bolidu. Mesilen: til* - tillash, tillidim tillaymen;  *til* -
alwastilar, ajritildi; *üzüm - tüzüm, kündüzüm qatarliqalarni öz ichige alidu.';
$lang['Word'] = 'Söz';
$lang['Edit_word_censor'] = 'Soz sénzor Tehrirlesh';
$lang['Replacement'] = 'Almisdhidighan Söz';
$lang['Add_new_word'] = 'Yéngi söz qoshush';
$lang['Update_word'] = 'Sözni yéngilash';

$lang['Must_enter_word'] = 'Sénzorlinidighan söz we uning ornigha almishidighan
sözni kirghüzüng';
$lang['No_word_selected'] = 'Tehrirleshke süz tallanmidi';

$lang['Word_updated'] = 'Sénzorlinidighan söz muweppeqiyetlik yéngilandi';
$lang['Word_added'] = 'Sénzorlinidighan söz muweppeqiyetlik qoshuldi';
$lang['Word_removed'] = 'Tallighan söz muweppeqiyetlik öchürüldi';

$lang['Click_return_wordadmin'] = 'Söz Chekleshni Bashqurushqa qaytish üchün
%sbu yerni%s chéking.';


//
// Mass Email
//
$lang['Mass_email_explain'] = 'Bu yerde siz,  tizimlatqan barliq
qollan\'ghuchilargha meséj yollapla qalmastin yene melum bir guruppidiki barliq
ezalargha meséj yolliyalaysiz. Guruppqa yollighan meséj  guruppa
bashqurghuchining e-mail adrésigha yollinidu hem uning (karbon) kopiyesi
qarighularche barliq tapshurwalghuchi ezalargha yollinidu. Eger ezaliri köp
bolghan chong bir guruppigha yollimaqchi bolsingiz, sewr-tawet bilen saqlang,
yollashni yérim yolda toxtitiwetmeng. Toplap email ibertishke waqit kop kétishi
normal hadise, programma ijra bolghan chaghda siz eskertishni bayqaysiz.';

$lang['Compose'] = 'Xet yézish'; 

$lang['Recipients'] = 'Tapshurwalghuchi'; 

$lang['All_users'] = 'Barliq Qollan\'ghuchi';


$lang['Email_successfull'] = 'Meséjingiz yollunup boldi';
$lang['Click_return_massemail'] = 'Top E-mail Yollashqa qaytish üchün %sbu
yerni%s chéking. ';


//
// Ranks admin
//
$lang['Ranks_title'] = 'Derije Bashqurush';
$lang['Ranks_explain'] = 'Bu yerde derijilerni qoshush, tehrirlesh, körüsh we
öchürüsh élip baralaysiz. Siz oxshashla özingiz derijilerni turghuzup
qollan\'ghuchi bashqurghuchisi arqiliq qollan\'ghuchilargha qollinalaysiz.';

$lang['Add_new_rank'] = 'Yéngi derije qoshush';

$lang['Rank_title'] = 'Derije Nami';

$lang['Rank_special'] = 'Alahide Derije';


$lang['Rank_minimum'] = 'Minimum yézilma sani';
$lang['Rank_maximum'] = 'Maksimum yézilam sani';
$lang['Rank_image'] = 'Derije resimi(phpBB2 ning nisbiy munderijisi)';
$lang['Rank_image_explain'] = 'Herbir derijige mas bolghan resimni
belgiliyeleysiz';


$lang['Must_select_rank'] = 'Bir derije tallang';
$lang['No_assigned_rank'] = 'Héchqandaq alahide derije belgilenmidi';

$lang['Rank_updated'] = 'Derije muweppeqiyetlik yéngilandi';
$lang['Rank_added'] = 'Derije muweppeqiyetlik qoshuldi';
$lang['Rank_removed'] = 'Derije muweppeqiyetlik öchürüldi';
$lang['No_update_ranks'] = 'Bu derije muweppeqiyetlik öchürüldi. Emma bu
derijidiki qollan\'ghuchilarning derijisi özgermid. Bularning derijisini qol
arqiliq özgertishingiz kérek.';

$lang['Click_return_rankadmin'] = 'Derije Bashqurushqa qaytish üchün %sbu
yerni%s chéking.';


//

// Disallow Username Admin
//
$lang['Disallow_control'] = 'Qollanchi Isimni Cheklesh';
$lang['Disallow_explain'] = 'Bu yerde siz qollan\'ghuchilar ishlitishke
bolmaydighan isimlarni chekliyeleysiz. Maslashturush belgisini(*) ishletsingiz
bolidu. Diqqet qilingki, tizimlitip bolghan qollan\'ghuchining isimini cheklesh
üchün, awwal u qollan\'ghuchini öchürüwétishingiz kérek.';

$lang['Delete_disallow'] = 'Öchürüsh';
$lang['Delete_disallow_title'] = 'Chekleydighan bir qollanchi isimini öchürüsh';
$lang['Delete_disallow_explain'] = 'Bu yerde cheklimekchi bolghan bir qollanchi
isimini tallap, öchürüshni chéksingiz u isim öchürülidu.';

$lang['Add_disallow'] = 'Qoshush';
$lang['Add_disallow_title'] = 'Cheklinidighan qollanchi isimini qoshush';
$lang['Add_disallow_explain'] = 'Melum dairidiki qollanchi isimlirini cheklesh
üchün, maslashturush belgisini(*) ishletsingiz bolidu.';

$lang['No_disallowed'] = 'Cheklinidighan qollanchi isimi yoq';

$lang['Disallowed_deleted'] = 'Cheklinidighan qollanchi isimi muweppeqiyetlik
öchürüldi';
$lang['Disallow_successful'] = 'Cheklinidighan qollanchi isimi muweppeqiyetlik
qoshuldi';

$lang['Disallowed_already'] = 'Kirgüzgen bu isim cheklenmidi. U cheklesh
tizimlikide bolishi mumkin, yaki u isim hazir ishlitiliwatqan bolishi mumkin.';

$lang['Click_return_disallowadmin'] = 'Qollanchi Isimini Chekleshke qaytish
üchün %sbu yerni%s chéking. ';


//
// Styles Admin

//
$lang['Styles_admin'] = 'Uslub Bashqurush';
$lang['Styles_explain'] = 'Bu yerde uslublarni(endize we téma) qoshalaysiz,
öchüreleysiz we bashquralaysiz';
$lang['Styles_addnew_explain'] = 'Töwendiki tizimliktikisi hazirqi barliq
témilardur. Bular téxi phpBB ning sanliqambirigha qachilanmidi. Témidin birni
qachilash üchün, qachilash ulunushini chéking.';

$lang['Select_template'] = 'Bir endize tallash';

$lang['Style'] = 'Uslub';
$lang['Template'] = 'Endize';
$lang['Install'] = 'Qachilash';
$lang['Download'] = 'Yük Chüshürüsh';

$lang['Edit_theme'] = 'Téma Tehrirlesh';
$lang['Edit_theme_explain'] = 'Siz bu yerde, tallan\'ghan téma tengsheklirini
özgerteleysiz';

$lang['Create_theme'] = 'Téma Qurush';
$lang['Create_theme_explain'] = ' Bu yerde tallighan endizingizge bir téma
quralaysiz. Réngini belgiligende(16lik reqem ishlitisiz), # belgisini
ishlitishke bolmaydu. Mesilen: CCCCCC ishlitish toghra, #CCCCCC bundaq ishlitish
xata.';


$lang['Export_themes'] = 'Téma Éksport Qilish';
$lang['Export_explain'] = 'Bu yerde tallighan endizingiz üchün téma sanliq
melumatlirini chiqiralaysiz. Jedweldin bir endize tallighandin kéyin sistéma
témining seplime uchurlirini qurup uni endize munderijisige saqlaydu. Eger
saqliyalmisa, bu höjjetni chüshürshingizge tallash béridu. Egre sistéming bu
höjjetlerni biwasite saqlishini ümid qilsingiz, yazghili bolidighan bir endize
munderijisi békitishingiz kérek. Tepsili uchurgha érishish üchün phpBB2
qollanmisini körüng.';

$lang['Theme_installed'] = 'Tallan\'ghan téma muweppeqiyetlik qachilandi';
$lang['Style_removed'] = ' Tallan\'ghan uslub sanliqambardin öchürüwétildi. Uni
sistémidin pütünley öchürüwétish üchun, endize munderjisini öchürüshingiz
kérek.';
$lang['Theme_info_saved'] = ' Tallan\'ghan téma üchürliri muweppeqiyetlik
saqlandi. Siz hazir  theme_info.cfg(eger tallan\'ghan endize munderijisige uyghun
bolsa) ning sheklini  peqet-oquludighan haletke özgertishingiz kérek.';

$lang['Theme_updated'] = 'Tallan\'ghan téma yéngilandi. Siz hazir yéngi téma
tengsheklirini éksport qilishingiz kérek.';

$lang['Theme_created'] = ' Téma quruldi. Bixeterlikni saqlash we xataliq
bolmasliq üchün, hazir téma seplimilirini éksport qilishingiz kérek.';

$lang['Confirm_delete_style'] = 'Bu uslubni choqum öchüremsiz';

$lang['Download_theme_cfg'] = ' Bu éksportér téma uchurliri höjjitini yazalmidi.
Astidiki konupkini chékip, browsér arqili bu höjjetni chüshürüng. Chüshürgendin
kéyin uni endize höjjiti bar munderijige köchürsingiz bolidu. Uni tarqitish yaki
bashqa yerde ishlitip qélish üchün, pak höjjiti qilip qoysingiz bolidu.';
$lang['No_themes'] = ' Tallan\'ghan endizide héchqandaq téma yoq. Yéngi téma
qurush üchün, sol tereptiki Téma Qurush ulunushini chéking.';
$lang['No_template_dir'] = ' Endize munderijisi échilalmidi. U belkim
oqulmaydighan tengshilip qalghan yaki mewjut emes.';
$lang['Cannot_remove_style'] = ' Tallan\'ghan bu uslubni öchürelmeysiz, chünki u
asasiy sehipige  tewe. Bashqa bir asasiy sehipini tallap, qayta sinang.';
$lang['Style_exists'] = 'Tallan\'ghan uslub nami mewjut, qaytip bashqa bir namni
tallang. ';

$lang['Click_return_styleadmin'] = 'Uslub Bashqurushqa qaytish üchün %sbu
yerni%s chéking.';


$lang['Theme_settings'] = 'Téma Tengshekliri';

$lang['Theme_element'] = 'Téma Éléménti';
$lang['Simple_name'] = 'Addiy Nami';
$lang['Value'] = 'Qimmiti';
$lang['Save_Settings'] = 'Tengsheklerni Saqlash';

$lang['Stylesheet'] = 'CSS Uslubi Jedwili';
$lang['Background_image'] = 'Teglik Resimi';
$lang['Background_color'] = 'Teglik Renggi';


$lang['Theme_name'] = 'Téma Nami';
$lang['Link_color'] = 'Ulunush Renggi';
$lang['Text_color'] = 'Tékst Renggi';
$lang['VLink_color'] = 'Ziyarettin Kéyinki Ulunush Renggi';
$lang['ALink_color'] = 'Qozghutulghandiki Renggi';
$lang['HLink_color'] = 'Tallighandiki Ulunush Renggi';
$lang['Tr_color1'] = 'Jedwel Istoni Renggi 1';
$lang['Tr_color2'] = 'Jedwel Istoni Renggi 2';
$lang['Tr_color3'] = 'Jedwel Istoni Renggi 3';

$lang['Tr_class1'] = 'Jedwel Istoni Sinipi 1';
$lang['Tr_class2'] = 'Jedwel Istoni Sinipi 2';
$lang['Tr_class3'] = 'Jedwel Istoni Sinipi 3';
$lang['Th_color1'] = 'Jedwel Béshi Renggi 1';
$lang['Th_color2'] = 'Jedwel Béshi Renggi 2';
$lang['Th_color3'] = 'Jedwel Béshi Renggi 3';
$lang['Th_class1'] = 'Jedwel Béshi Sinipi 1';
$lang['Th_class2'] = 'Jedwel Béshi Sinipi 2';
$lang['Th_class3'] = 'Jedwel Béshi Sinipi 3';
$lang['Td_color1'] = 'Jedwel Katekchisi Renggi 1';
$lang['Td_color2'] = 'Jedwel Katekchisi Renggi 2';
$lang['Td_color3'] = 'Jedwel Katekchisi Renggi 3';
$lang['Td_class1'] = 'Jedwel Katekchisi Sinipi 1';
$lang['Td_class2'] = 'Jedwel Katekchisi Sinipi 2';
$lang['Td_class3'] = 'Jedwel Katekchisi Sinipi 3';
$lang['fontface1'] = 'Font Tipi 1';
$lang['fontface2'] = 'Font Tipi 2';
$lang['fontface3'] = 'Font Tipi 3';
$lang['fontsize1'] = 'Font Rezmisi 1';
$lang['fontsize2'] = 'Font Rezmisi 2';
$lang['fontsize3'] = 'Font Rezmisi 3';
$lang['fontcolor1'] = 'Font Renggi 1';
$lang['fontcolor2'] = 'Font Renggi 2';
$lang['fontcolor3'] = 'Font Renggi 3';

$lang['span_class1'] = 'Span Sinipi 1';
$lang['span_class2'] = 'Span Sinipi 2';
$lang['span_class3'] = 'Span Sinipi 3';
$lang['img_poll_size'] = 'Ray-sinash resimi rezmisi [px]';
$lang['img_pm_size'] = 'Xususiy meséjlar halite resimi rezmisi [px]';


//
// Install Process
//
$lang['Welcome_install'] = 'PhpBB 2 ni Qachilighanliqingizni Qarshi Alimiz';
$lang['Initial_config'] = 'Asasiy Seplime';
$lang['DB_config'] = 'Sanliqambar Seplimisi';
$lang['Admin_config'] = 'Bashqurush Seplimisi';
$lang['continue_upgrade'] = 'Seplime höjjiti Config.php ni kompyutéringizgha
chüshürüp bolghandin kéyin \'Yéngilashni Dawamlash\' konupkisini bésip
dawamlashtursingiz bolidu.Yéngilash tamamlan\'ghandin kéyin andin seplime
höjjitini yükleng. ';
$lang['upgrade_submit'] = 'Yéngilashni Dawamlash';


$lang['Installer_Error'] = 'Qachilash dawamida bir xataliq körüldi';
$lang['Previous_Install'] = 'Burun qachilighan bir phpBB2 bayqaldi';
$lang['Install_db_error'] = 'Sanliqambar yéngiliniwatqanda xataliq körüldi';


$lang['Re_install'] = 'Siz burun qachilighan munber qozghutughluq.<br /><br
/>Eger siz phpBB 2 ni qayta qachilimaqchi bolsingiz Hee konupkisini astidiki
chéking. Diqqet qilingki, bundaq qilsingiz hazir bar bolghan barliq sanliq
melumatlar öchürlüp kétidu hem héchqandaqi zapaslanmaydu! Bashqurghuchining
hazir ishlitiwatqan hésabi we paroli yéngidin quruludu hem héchqandaq
tengshekler saqlinip qalmaydu. <br /><br />Hee konupkisini bésishtin burun obdan
oylunung!';



$lang['Inst_Step_0'] = 'phpBB 2 ni tallighiningiz üchücn teshekkür. Qachilashni
tamamlash üchün, töwendiki melumatlarni toldurung. Qachilashtin ilgiri,
sanliqambarning teyyarlan\'ghan bolishigha diqqet qiling. Eger ODBC ishlitidighan
sanliq ambar qachilimaqchi bolsingiz, mesilen: MS Access, dawamlashtin burun
ular üchün awwal bir  DSN qurushingiz kérek.';

$lang['Start_Install'] = 'Qachilashni Bashlash';
$lang['Finish_Install'] = 'Qachilashn Tamamlandi';

$lang['Default_lang'] = 'Munber tili';
$lang['DB_Host'] = 'Sanliqambar sérwér adrési';
$lang['DB_Name'] = 'Sanliqambar nami';
$lang['DB_Username'] = 'Sanliqambar Ishletküchi nami';
$lang['DB_Password'] = 'Sanliqambar Paroli';
$lang['Database'] = 'Sanliqambiringiz';
$lang['Install_lang'] = 'Qachilash tilini tallash';
$lang['dbms'] = 'Sanliqambar tipi';
$lang['Table_Prefix'] = 'Sanliqambardiki jedwellerning aldi qoshulghuchisi';
$lang['Admin_Username'] = 'Bashqurghuchi Isimi';
$lang['Admin_Password'] = 'Bashqurghuchi Paroli';
$lang['Admin_Password_confirm'] = 'Bashqurghuchi Paroli [ Jezimlesh ]';

$lang['Inst_Step_2'] = 'Bashqurghuchingizning qollanchi isimi quruldi. Mushu
peytte asasiy qachilashmu tamamlandi. Siz hazir bir betke, yeni sizning yéngi
qachilimini bashqurushingizgha ijazet bérilgen bir betke keltürilisiz. Omumiy
seplimilerni hem zörür bolghan özgürüshler bolghanliqini tekshürüp béqing. phpBB
2 ni tallighiningizge teshekkür éytimiz.';

$lang['Unwriteable_config'] = 'Shu esnada seplime höjjiti yézilalmidi. Seplime
höjjitini kompyutéringizgha chüshürüsh üchün astidiki konupkini chéking. Andin
özingiz bu höjjetni phpBB 2 munderijisige yükleng. Andin aldida teminligen
bashqurghuchi isimi we parolingiz bilen sistéma bashqurush kontroligha(kirgendin
kéyin betning astida körünidu) kirip omumiy seplimiliringizni tekshürüng. phpBB
2 ni tallighiningizge teshekkür éytimiz.';

$lang['Download_config'] = 'Seplime Höjjitini Chüshürüsh';

$lang['ftp_choose'] = 'Chüshürüsh Usulini Tallash';
$lang['ftp_option'] = '<br />PHP ning bu nusxisida FTP ning kéngeytilgen
iqtidari qozghutulghan bolup, seplime höjjitini FTP arqiliq ornigha yollash
mumkinchilikingiz bar.';
$lang['ftp_instructs'] = 'Seplime höjjitini FTP arqiliq phpbb2 diki ornigha
aptomatik yollashni tallidingiz. Töwendiki uchurlarni kirgüzüp bu jeryanni
addiylashturung. Diqqet qilingki, FTP munderjisi bilen phpBB2 qachilinidighan
munderije oxshash bolishi kérek.';
$lang['ftp_info'] = 'FTP uchurliringizni kirgüzüng';
$lang['Attempt_ftp'] = 'FTP arqiliq seplime höjjitini yollash';
$lang['Send_file'] = 'Bu höjjetni özemge ibertip, FTP arqiliq ornigha
yollaymen.';
$lang['ftp_path'] = 'phpBB2 ning FTP munderijisi';
$lang['ftp_username'] = 'FTP Qollanchi nami';
$lang['ftp_password'] = 'FTP Paroli';
$lang['Transfer_config'] = 'Yollashni bashlash';
$lang['NoFTP_config'] = 'Seplime höjjitini ornigha yollash meghlup boldi. Uni
chüshürüp qol arqiliq ornigha yollang.';

$lang['Install'] = 'Qachilash';
$lang['Upgrade'] = 'Yéngilash';


$lang['Install_Method'] = 'Qachilash usulini tallang';

$lang['Install_No_Ext'] = 'Sérwérdiki PHP seplimiliri siz tallighan sanliqambar
türige mas kelmidi.';

$lang['Install_No_PCRE'] = 'phpBB2 üchün kéreklik bolghan Perl ning
uyghunlashturush qaide modéli PHP seplimilirige mas kelmidi!';

//
// That's all Folks!
// -------------------------------------------------

?>