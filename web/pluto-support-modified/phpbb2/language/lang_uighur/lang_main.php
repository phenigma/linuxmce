<?php
/***************************************************************************
 *                            lang_main.php [Uighur_latin]
 *                              -------------------
 *     begin                : Sat Dec 16 2000
 *     copyright            : (C) 2001 The phpBB Group
 *     email                : support@phpbb.com
 *
 *     $Id: lang_main.php,v 1.85.2.15 2003/06/10 00:31:19 psotfx Exp $
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
// Muhemmed Erdem (M.Erdem) :: webmaster@misran.com  :: http://www.misran.com
// Abdireyim (abdireyim)    :: abdireyim@yahoo.com   :: http://freeud.tripod.com
//
// Pikirler bolsa bu adrésni ishliting/For questions and comments use: misran_erdem@hotmail.com
//
// Diqqet: Bu emgekning hormiti uchun bolsimu tekstlerni, chekitlik herplerni we 
// atalghularni xalighanche ozgertmeng. Pikirliringiz bolsa 
// terjime qilghuchilar bilen alaqe qiling.
//
// Release date: 2003-09-10
//
//
//
// The format of this file is ---> $lang['message'] = 'text';
//
// You should also try to set a locale and a character encoding (plus direction). The encoding and direction
// will be sent to the template. The locale may or may not work, it's dependent on OS support and the syntax
// varies ... give it your best guess!
//

$lang['ENCODING'] = 'iso-8859-1';
$lang['DIRECTION'] = 'ltr';
$lang['LEFT'] = 'left';
$lang['RIGHT'] = 'right';
$lang['DATE_FORMAT'] =  'Y.d.M'; // This should be changed to the default date format for your language, php date() format

// This is optional, if you would like a _SHORT_ message output
// along with our copyright message indicating you are the translator
// please add it here.
// $lang['TRANSLATION'] = '';

//
// Common, these terms are used
// extensively on several pages
//
$lang['Forum'] = 'Sehipe';
$lang['Category'] = 'Katégoriye';
$lang['Topic'] = 'Téma';
$lang['Topics'] = 'Témilar';
$lang['Replies'] = 'Jawablar';
$lang['Views'] = 'Qétimi';
$lang['Post'] = 'Yézilma';
$lang['Posts'] = 'Yézilmilar';


$lang['Posted'] = 'Arxip';
$lang['Username'] = 'Qollanchi Isimi';
$lang['Password'] = 'Parol';
$lang['Email'] = 'E-mail';
$lang['Poster'] = 'Yazghuchi';
$lang['Author'] = 'Aptor';
$lang['Time'] = 'Waqit';
$lang['Hours'] = 'Saet';
$lang['Message'] = 'Meséj';

$lang['1_Day'] = '1 Künlük';
$lang['7_Days'] = '7 Künlük';
$lang['2_Weeks'] = '2 Heptilik';
$lang['1_Month'] = '1 Ayliq';
$lang['3_Months'] = '3 Ayliq';
$lang['6_Months'] = '6 Ayliq';
$lang['1_Year'] = '1 Yilliq';

$lang['Go'] = 'Bashlash';
$lang['Jump_to'] = 'Buninggha ötüsh';
$lang['Submit'] = 'Yollash';
$lang['Reset'] = 'Qaytilash';

$lang['Cancel'] = 'Inawetsiz';
$lang['Preview'] = 'Sinash';
$lang['Confirm'] = 'Békitish';
$lang['Spellcheck'] = 'Tekshürüsh';
$lang['Yes'] = 'Hee';
$lang['No'] = 'Yaq';
$lang['Enabled'] = 'Qozghutuldi';
$lang['Disabled'] = 'Taqaldi';
$lang['Error'] = 'Xata';

$lang['Next'] = 'Kéyinki';
$lang['Previous'] = 'Aldinqi';
$lang['Goto_page'] = 'Baridighan Bet';
$lang['Joined'] = 'Tizimlatqan Waqit';
$lang['IP_Address'] = 'IP Adrési';

$lang['Select_forum'] = 'Sehipe Tallash';
$lang['View_latest_post'] = 'Axirqi Yézilmini Körüsh';
$lang['View_newest_post'] = 'Yéngi Yézilmini Körüsh';
$lang['Page_of'] =  '<b>%d</b>. bet  (jem\'iy <b>%d</b> bet)'; // Replaces with: Page 1 of 2 for example

$lang['ICQ'] = 'ICQ Nomuri';
$lang['AIM'] = 'AIM Adrési';
$lang['MSNM'] = 'MSN Messenger';
$lang['YIM'] = 'Yahoo Messenger';

$lang['Forum_Index'] = '%s Bashbet';  // eg. sitename Forum Index, %s can be removed if you prefer


$lang['Post_new_topic'] = 'Yéngi Téma Yézish';
$lang['Reply_to_topic'] = 'Jawab Yézish';
$lang['Reply_with_quote'] = 'Neqillep Jawab Yézish';

$lang['Click_return_topic'] = 'Témigha qaytish üchün %sbu yerni%s bésing'; // %s's here are for uris, do not remove!
$lang['Click_return_login'] = 'Yene bir sinash üchün %sbu yerni%s bésing';
$lang['Click_return_forum'] = 'Sehipige qaytish üchün %sbu yerni%s bésing';
$lang['Click_view_message'] = 'Meséjingizni körüsh üchün %sbu yerni%s bésing';
$lang['Click_return_modcp'] = 'Nazaretchi kontroligha  qaytish üchün %sbu yerni%s bésing';
$lang['Click_return_group'] = 'Guruppa uchurigha qaytish üchün %sbu yerni%s bésing';

$lang['Admin_panel'] = 'Bashqurghuchi Kontroli';

$lang['Board_disable'] = 'Kechürüng, sehipini hazirche ishletkili bolmaydu. Sel turup qayta sinap körüng.';


//
// Global Header strings
//
$lang['Registered_users'] = 'Tizimlatqan Qollan\'ghuchilar:';
$lang['Browsing_forum'] = 'Bu sehipidiki qollan\'ghuchilar:';
$lang['Online_users_zero_total'] = 'Jem\'iy <b>0</b> qollan\'ghuchilar torda :: ';
$lang['Online_users_total'] = 'Jem\'iy <b>%d</b> qollan\'ghuchilar torda :: ';
$lang['Online_user_total'] = 'Jem\'iy <b>%d</b> qollan\'ghuchi torda :: ';
$lang['Reg_users_zero_total'] = 'Héchkim tizimlatmidi, ';
$lang['Reg_users_total'] = '%d Qollan\'ghuchi tizimlatti, ';
$lang['Reg_user_total'] = '%d Qollan\'ghuchi tizimlatti, ';
$lang['Hidden_users_zero_total'] = 'yoshurunlar yoq we ';
$lang['Hidden_user_total'] = '%d yoshurun we ';
$lang['Hidden_users_total'] = '%d yoshurun we ';
$lang['Guest_users_zero_total'] = 'méhmanmu yoq.';
$lang['Guest_users_total'] = '%d méhman bar.';
$lang['Guest_user_total'] = '%d méhman bar.';
$lang['Record_online_users'] = 'Bügün\'ge qeder eng köp bolghanda <b>%s</b> kishi %s de torda boldi.'; // first %s = number of users, second %s is the date.

$lang['Admin_online_color'] = '%sBashqurghuchi%s';
$lang['Mod_online_color'] = '%sNazaretchi%s';

$lang['You_last_visit'] = 'Axirqi ziyaritingiz: %s'; // %s replaced by date/time
$lang['Current_time'] = 'Hazirqi waqit: %s'; // %s replaced by time

$lang['Search_new'] = 'Axirqi ziyarettin kéyinki yézilmilar';
$lang['Search_your_posts'] = 'Özining yazmiliri';
$lang['Search_unanswered'] = 'Jawab qayturulmighan yézilmilar';

$lang['Register'] = 'Tizimlitish';
$lang['Profile'] = 'Profil';
$lang['Edit_profile'] = 'Profilni Özgertish';
$lang['Search'] = 'Izdesh';
$lang['Memberlist'] = 'Ezalar Tizimi';
$lang['FAQ'] = 'FAQ (Soal-Jawablar)';
$lang['BBCode_guide'] = 'BBCode Qollanmisi';
$lang['Usergroups'] = 'Guruppilar';
$lang['Last_Post'] = 'Axirqi Yézilma';
$lang['Moderator'] = 'Nazaretchi';
$lang['Moderators'] = 'Nazaretchiler';



//
// Stats block text
//
$lang['Posted_articles_zero_total'] = 'Qollan\'ghuchilirimiz téxi <b>héchnerse</b> yazmidi.'; // Number of posts
$lang['Posted_articles_total'] = 'Qollan\'ghuchilirimiz jem\'iy <b>%d</b> parche maqale yazdi.'; // Number of posts
$lang['Posted_article_total'] = 'Qollan\'ghuchilirimiz jem\'iy <b>%d</b> parche maqale yazdi.'; // Number of posts
$lang['Registered_users_zero_total'] = 'Bizge téxi <b>héchkim</b> tizimlatmidi.'; // # registered users
$lang['Registered_users_total'] = 'Munberde <b>%d</b> neper tizimlatqan qollan\'ghuchilar bar.'; // # registered users
$lang['Registered_user_total'] = 'Munberde <b>%d</b> neper tzimlatqan qollan\'ghuchilar bar.'; // # registered users
$lang['Newest_user'] = 'Eng yéngi tizimlatqan qollan\'ghuchi: <b>%s%s%s</b>'; // a href, username, /a 


$lang['No_new_posts_last_visit'] = 'Axirqi ziyaritingizdin buyan yéngi yézilma yoq ';
$lang['No_new_posts'] = 'Yéngi yézilma yoq';
$lang['New_posts'] = 'Yéngi yézilmilar';
$lang['New_post'] = 'Yéngi yézilma';
$lang['No_new_posts_hot'] = 'Yéngi yézilma yoq [ Qizziq ]';
$lang['New_posts_hot'] = 'Yéngi yézilmilar [Qizziq ]';
$lang['No_new_posts_locked'] = 'Yéngi yézilma yoq [ Quluplan\'ghan ]';
$lang['New_posts_locked'] = 'Yéngi yézilmilar [ Quluplan\'ghan ]';
$lang['Forum_is_locked'] = 'Sehipe quluplandi';


//
// Login
//
$lang['Enter_password'] = 'Ismingiz we Parolingizni Kiqgüzüng! ';
$lang['Login'] = 'Kirish';
$lang['Logout'] = 'Qaytip Chiqish';

$lang['Forgotten_password'] = 'Parolimni unutup qaptimen!';

$lang['Log_me_in'] = 'Her kelgende aptomatik kirish';

$lang['Error_login'] = 'Siz isimni yaki parolni xata kirgüzüp qoydingiz. ';


//
// Index page
//
$lang['Index'] = 'Bashbet';
$lang['No_Posts'] = 'Yézilma Yoq';
$lang['No_forums'] = 'Bu bette sehipe yoq';

$lang['Private_Message'] = 'Xususiy Meséj';
$lang['Private_Messages'] = 'Xususiy Meséjlar';
$lang['Who_is_Online'] = 'Kim Torda?';

$lang['Mark_all_forums'] = 'Hemme sehipini oqup bolghan qilish';
$lang['Forums_marked_read'] = 'Hemme sehipe oqup bolunghan qilindi';



//
// Viewforum
//
$lang['View_forum'] = 'Sehipini Körüsh';

$lang['Forum_not_exist'] = 'Siz tallighan sehipe mewjut emes.';
$lang['Reached_on_error'] = 'Bu bettka  xata kélip qaldingiz';

$lang['Display_topics'] = 'Ilgiriki yézilmilarni körsütüsh';
$lang['All_Topics'] = 'Barliq Témilar';

$lang['Topic_Announcement'] = '<b>Uqturush:</b>';
$lang['Topic_Sticky'] = '<b>Muhim:</b>';
$lang['Topic_Moved'] = '<b>Yötkelgen:</b>';

$lang['Topic_Poll'] = '<b>[ Ray-Sinash ]</b>';

$lang['Mark_all_topics'] = 'Barliq yézilmilarni oqup bolghan qilish';
$lang['Topics_marked_read'] = 'Barliq yézilmilar oqulghan qilindi.';

$lang['Rules_post_can'] = 'Bu sehipige yéngi yézilmilar <b>yazalaysiz.</b>';
$lang['Rules_post_cannot'] = 'Bu sehipige yéngi yézilmilar <b>yazalmaysiz.</b>';
$lang['Rules_reply_can'] = 'Bu sehipidiki yézilmilargha jawab <b>yazalaysiz.</b>';
$lang['Rules_reply_cannot'] = 'Bu sehipidiki yézilmilargha jawab <b>yazalmaysiz.</b>';
$lang['Rules_edit_can'] = 'Bu sehipidiki yazmiliringizni <b>tehrirliyeleysiz.</b>';
$lang['Rules_edit_cannot'] = 'Bu sehipidiki yazmiliringizni <b>tehrirliyelmeysiz.</b>';
$lang['Rules_delete_can'] = 'Bu sehipidiki yazmiliringizni <b>öchüreleysiz.</b>';
$lang['Rules_delete_cannot'] = ' Bu sehipidiki yazmiliringizni <b>öchürelmeysiz.</b>';
$lang['Rules_vote_can'] = 'Bu sehipidiki ray sinashqa awaz <b>béreleysiz.</b>';
$lang['Rules_vote_cannot'] = 'Bu sehipidiki ray sinashqa awaz <b>bérelmeysiz.</b>';
$lang['Rules_moderate'] = 'Bu sehipige %snazaretchilik%s <b>qilalaysiz.</b>'; // %s replaced by a href links, do not remove!

$lang['No_topics_post_one'] = 'Bu sehipide yézilma yoq. <br />Yéngi téma yézish üchün <b>Yéngi Téma</b> ni bésing.';


//
// Viewtopic
//
$lang['View_topic'] = 'Téma Körüsh';

$lang['Guest'] = 'Méhman';
$lang['Post_subject'] = 'Mawzu';
$lang['View_next_topic'] = 'Kéyinki Téma';
$lang['View_previous_topic'] = 'Aldinqi Téma';
$lang['Submit_vote'] = 'Awaz Bérish';
$lang['View_results'] = 'Netijilerni Körüsh';

$lang['No_newer_topics'] = 'Bu sehipide téxi yéngi téma yoq.';
$lang['No_older_topics'] = 'Bu sehipide téxi kona téma yoq.';
$lang['Topic_post_not_exist'] = 'Tallighan téma bu menberde emes.';
$lang['No_posts_topic'] = 'Bu témigha jawab yézilmighan.';

$lang['Display_posts'] = 'Ilgiriki yézilmilarni körsütüsh';
$lang['All_Posts'] = 'Barliq Yézilmilar';
$lang['Newest_First'] = 'Yéngisini awwal';
$lang['Oldest_First'] = 'Konisini awwal';

$lang['Back_to_top'] = 'Choqqigha qaytish';

$lang['Read_profile'] = 'Qollan\'ghuchi profilini körüsh'; 
$lang['Send_email'] = 'Qollan\'ghuchigha e-mail yollash';
$lang['Visit_website'] = 'Qollan\'ghuchining torbétini ziyaret qilish';
$lang['ICQ_status'] = 'ICQ Haliti';
$lang['Edit_delete_post'] = 'Yézilmini Tehrirlesh/Öchürüsh';
$lang['View_IP'] = 'Bu qollan\'ghuchining IP adrésini körsütüsh';
$lang['Delete_post'] = 'Bu yézilmini öchürüsh';

$lang['wrote'] = 'töwendikidek yazghan'; // proceeds the username and is followed by the quoted text
$lang['Quote'] = 'Neqil'; // comes before bbcode quote output.
$lang['Code'] = 'Kod'; // comes before bbcode code output.

$lang['Edited_time_total'] = 'Eng axirqi qétim %s teripidin %s da özgertildi, jem\'iy %d qétim özgertildi.'; // Last edited by me on 12 Oct 2001, edited 1 time in total
$lang['Edited_times_total'] = 'Eng axirqi qétim %s teripidin %s da özgertildi, jem\'iy %d qétim özgertildi.'; // Last edited by me on 12 Oct 2001; edited 2 times in total

$lang['Lock_topic'] = 'Bu témini quluplash';
$lang['Unlock_topic'] = 'Bu témini échish';
$lang['Move_topic'] = 'Bu témini yötkesh';
$lang['Delete_topic'] = 'Bu témini öchürüsh';
$lang['Split_topic'] = 'Bu témini ayrish';

$lang['Stop_watching_topic'] = 'Bu témidiki jawablarni körüshni toxtitish';
$lang['Start_watching_topic'] = 'Bu témidiki jawablarni dawamliq körüsh';
$lang['No_longer_watching'] = ' Siz bu témini emdi körmeysiz.';
$lang['You_are_watching'] = 'Siz hazir bu témini köriwatisiz.';

$lang['Total_votes'] = 'Jem\'iy Awaz';

//
// Posting/Replying (Not private messaging!)
//
$lang['Message_body'] = 'Meséj Téksti';
$lang['Topic_review'] = 'Téma Tekshürüsh';

$lang['No_post_mode'] = 'Héchqandaq yézilma shekli tallanmidi'; // If posting.php is called without a mode (newtopic/reply/delete/etc, shouldn't be shown normaly)

$lang['Post_a_new_topic'] = 'Yéngi téma yézish';
$lang['Post_a_reply'] = 'Jawab Yézish';
$lang['Post_topic_as'] = 'Téma Türi';
$lang['Edit_Post'] = 'Yézilma Özgertish';
$lang['Options'] = 'Tallash';

$lang['Post_Announcement'] = 'Uqturush';
$lang['Post_Sticky'] = 'Muhim';
$lang['Post_Normal'] = 'Normal';

$lang['Confirm_delete'] = 'Bu yézilmini öchürüshke jezim qilamsiz?';
$lang['Confirm_delete_poll'] = 'Bu ray-sinashni öchürüshke jezim qilamsiz?';

$lang['Flood_Error'] = 'Bayiqi yézilmingizdin kéyin bundaq qisqa waqitta yéngi bir yézilma yolliyalmaysiz, biraz saqlang.';
$lang['Empty_subject'] = 'Yéngi téma achqanda uningha bir mawzu bérishingiz zörürdur. ';
$lang['Empty_message'] = 'Téksti yoq yézilmilarni yolliyalmaysiz.';
$lang['Forum_locked'] = 'Bu sehipe quluplanghan; siz yézilma yollash, tehrirlesh yaki jawab yézish élip baralmaysiz.';
$lang['Topic_locked'] = 'Bu téma quluplanghan; siz yézilma tehrirlesh yaki jawab bérish élip baralmaysiz.';
$lang['No_post_id'] = 'Tehrirlesh üchün yézilmidin birni tallang.';
$lang['No_topic_id'] = 'Jawab yézishingiz üchün témidin birni tallang. ';
$lang['No_valid_mode'] = 'Siz peqet yézilma yazalaysiz, jawab qayturalaysiz, uni tehrirliyeleysiz yaki neqil qilalaysiz. Yene bir qétim sinap béqing.';
$lang['No_such_post'] = 'Bundaq yézilma yoq, yene bir qétim sinap béqing.';
$lang['Edit_own_posts'] = 'Kechürüng, peqet özingizning yazmilirinila tehrirliyeleysiz.';
$lang['Delete_own_posts'] = 'Kechürüng, peqet özingizning yazmilirinila öchüreleysiz.';
$lang['Cannot_delete_replied'] = 'Kechürüng, jawab qayturulghan yézilmilarni öchürelmeysiz.';
$lang['Cannot_delete_poll'] = 'Kechürüng, aktip haldiki ray-sinashni öchürelmeysiz. ';
$lang['Empty_poll_title'] = 'Ray-sinash üchün bir mawzu béring.';
$lang['To_few_poll_options'] = 'Ray-sinash üchün az dégende ikki tallash béring.';
$lang['To_many_poll_options'] = 'Ray-sinash üchün bek köp tallash bériwettingiz. ';
$lang['Post_has_no_poll'] = 'Bu yézilmida ray-sinash yoq.';
$lang['Already_voted'] = 'Bu ray-sinashqa awaz qoshup boldingiz.';
$lang['No_vote_option'] = 'Awaz qoshush üchün birni tallishingiz zörür.';

$lang['Add_poll'] = 'Ray-Sinash Qoshush';
$lang['Add_poll_explain'] = 'Eger témingizgha ray-sinash qoshushni xalimisingiz, bu tallashni bosh qaldurung.';
$lang['Poll_question'] = 'Soal';
$lang['Poll_option'] = 'Ray-sinash Türi';
$lang['Add_option'] = 'Ray-sinash Türi Qoshush';
$lang['Update'] = 'Yéngilash';
$lang['Delete'] = 'Öchürüsh';
$lang['Poll_for'] = 'Ray-Sinash Mudditi';



$lang['Days'] = 'Kün'; // This is used for the Run poll for ... Days + in admin_forums for pruning
$lang['Poll_for_explain'] = '[ Muddetsiz ray sinash üchün 0 ni kirgüzüng yaki bosh qaldurung ]';
$lang['Delete_poll'] = 'Ray-Sinashni Öchürüsh';

$lang['Disable_HTML_post'] = 'Bu yézilmida HTML ishlitishni cheklesh';
$lang['Disable_BBCode_post'] = 'Bu yézilmida BBCode ishlitishni cheklesh';
$lang['Disable_Smilies_post'] = 'Bu yézilmida chiraylar ishlitishni cheklesh';

$lang['HTML_is_ON'] = 'HTML <u>Ochuq</u>';
$lang['HTML_is_OFF'] = 'HTML <u>Étik</u>';
$lang['BBCode_is_ON'] = '%sBBCode%s <u>Ochuq</u>'; // %s are replaced with URI pointing to FAQ
$lang['BBCode_is_OFF'] = '%sBBCode%s <u>Étik</u>';
$lang['Smilies_are_ON'] = 'Chiraylar <u>Ochuq</u>';
$lang['Smilies_are_OFF'] = 'Chiraylar <u>Étik</u>';

$lang['Attach_signature'] = 'Imza qoshush (imza profil ichide özgertilidu)';
$lang['Notify'] = 'Jawab yézilghanda manga xewer qilinsun';
$lang['Delete_post'] = 'Bu yézilmini öchürüsh';

$lang['Stored'] = 'Meséjingiz muweppeqiyetlik saqlandi.';
$lang['Deleted'] = ' Meséjingiz muweppeqiyetlik öchürildi.';
$lang['Poll_delete'] = 'Ray-Sinash muweppeqiyetlik öchürildi.';
$lang['Vote_cast'] = 'Bélitingiz muwappiqiyetlik tashlandi.';


$lang['Topic_reply_notification'] = 'Jawab Uqturushi';

$lang['bbcode_b_help'] = 'Tom yézish: [b]tékst[/b]  (alt+b)';
$lang['bbcode_i_help'] = 'Chong xet: [i]tékst[/i]  (alt+i)';
$lang['bbcode_u_help'] = 'Asti siziqliq: [u]tékst[/u]  (alt+u)';
$lang['bbcode_q_help'] = 'Neqil keltürüsh: [quote]tékst[/quote]  (alt+q)';
$lang['bbcode_c_help'] = 'Kod körsütüsh: [code]Kod[/code]  (alt+c)';
$lang['bbcode_l_help'] = 'Tertipsiz Ret: [list]tékst[/list] (alt+l)';
$lang['bbcode_o_help'] = 'Tertiplik Ret: [list=]tékst[/list]  (alt+o)';
$lang['bbcode_p_help'] = 'Resim kirgüzüsh: [img]http://resim_url[/img]  (alt+p)';
$lang['bbcode_w_help'] = 'URL kirgüzüsh: [url]http://url[/url] yaki [url=http://url]URL tékst[/url]  (alt+w)';
$lang['bbcode_a_help'] = 'Échilghan barliq BBCode lirini taqash';
$lang['bbcode_s_help'] = 'Xet renggi: [color=red]tékst[/color]  Tewisiye: color=#FF0000 sheklide ishletsingizmu bolidu';
$lang['bbcode_f_help'] = 'Xet chongluqi: [size=x-small]kichik xet[/size]';

$lang['Emoticons'] = 'Chiraylar';
$lang['More_emoticons'] = 'Téximu köp';

$lang['Font_color'] = 'Xet renggi';
$lang['color_default'] = 'Esli';
$lang['color_dark_red'] = 'Toq Qizi';
$lang['color_red'] = 'Qizil';
$lang['color_orange'] = 'Eplisin';
$lang['color_brown'] = 'Qongur';
$lang['color_yellow'] = 'Sériq';
$lang['color_green'] = 'Yéshil';
$lang['color_olive'] = 'Zeytun';
$lang['color_cyan'] = 'Köküsh yéshil';
$lang['color_blue'] = 'Kök';
$lang['color_dark_blue'] = 'Toq Kök';
$lang['color_indigo'] = 'Köküsh';

$lang['color_violet'] = 'Sösün';
$lang['color_white'] = 'Aq';
$lang['color_black'] = 'Qara';
$lang['Font_size'] = 'Xet Rezmisi';
$lang['font_tiny'] = 'Bek kichik';
$lang['font_small'] = 'Kichik';
$lang['font_normal'] = 'Normal';
$lang['font_large'] = 'Chong';
$lang['font_huge'] = 'Zor';

$lang['Close_Tags'] = 'Taglarni Yépish';
$lang['Styles_tip'] = 'Eskertish: Uslublarni tallan\'ghan tékstke tzézla qoshulalaydu.';



//
// Private Messaging
//
$lang['Private_Messaging'] = 'Xususiy Meséj';

$lang['Login_check_pm'] = 'Xususiy Meséjni oqush üchün kirish';
$lang['New_pms'] = '%d parche yéngi meséjingiz bar'; // You have 2 new messages
$lang['New_pm'] = '%d parche yéngi meséjingiz bar'; // You have 1 new message
$lang['No_new_pm'] = 'Yéngi meséjingiz yoq';
$lang['Unread_pms'] = '%d parche oqumighan meséjingiz bar';
$lang['Unread_pm'] = '%d parche oqumighan meséjingiz bar';
$lang['No_unread_pm'] = 'Oqumighan meséjingiz yoq';
$lang['You_new_pm'] = 'Yéngi bir xususiy meséj sizni saqlawatidu';
$lang['You_new_pms'] = 'Yéngi xususiy meséjlar sizni saqlawatidu';
$lang['You_no_new_pm'] = 'Saqlawatqan yéngi meséjingiz yoq';

$lang['Unread_message'] = 'Oqulmighan Meséj';
$lang['Read_message'] = 'Oqulghan Meséj';

$lang['Read_pm'] = 'Meséj oqush';
$lang['Post_new_pm'] = 'Meséj yézish';
$lang['Post_reply_pm'] = 'Meséjge jawab bérish';
$lang['Post_quote_pm'] = 'Meséjni neqillesh';
$lang['Edit_pm'] = 'Meséjni tehrirlesh';

$lang['Inbox'] = 'Xet Sanduqi';
$lang['Outbox'] = 'Yollash Sanduqi';
$lang['Savebox'] = 'Arxip Sanduqi';
$lang['Sentbox'] = 'Yollanghan Xet';
$lang['Flag'] = 'Belge';
$lang['Subject'] = 'Mawzu';
$lang['From'] = 'Ibertküchi';
$lang['To'] = 'Tapshuriwalghuchi';
$lang['Date'] = 'Chisla';
$lang['Mark'] = 'Tallandi';
$lang['Sent'] = 'Yollandi';
$lang['Saved'] = 'Saqlandi';
$lang['Delete_marked'] = 'Tallan\'ghanni Öchürüsh';
$lang['Delete_all'] = 'Hemmini Öchürüsh';
$lang['Save_marked'] = 'Tallan\'ghanni Saqlash'; 
$lang['Save_message'] = 'Meséjni Saqlash';
$lang['Delete_message'] = 'Meséjni Öchürüsh';

$lang['Display_messages'] = 'Ilgirki meséjlarni körsütüsh'; // Followed by number of days/weeks/months
$lang['All_Messages'] = 'Barliq Meséjlar';

$lang['No_messages_folder'] = 'Bu munderijide meséj yoq.';


$lang['PM_disabled'] = 'Bu sehipide xususiy meséj taqiwétildi. ';
$lang['Cannot_send_privmsg'] = 'Kechürüng, xususiy meséj yollash bashqurghuchi teripidin taqiwétildi.';
$lang['No_to_user'] = 'Bu meséjni yollash üchün, yollighuchi ismini kirgüzüng';
$lang['No_such_user'] = 'Kechürüng, bu isimdiki qollan\'ghuchi mewjut emes.';

$lang['Disable_HTML_pm'] = 'Bu meséjda HTML ishlitishni cheklesh';
$lang['Disable_BBCode_pm'] = 'Bu meséjda BBCode ishlitishni cheklesh';
$lang['Disable_Smilies_pm'] = 'Bu meséjda chiraylar ishlitishni cheklesh';

$lang['Message_sent'] = 'Meséjingiz yollandi.';

$lang['Click_return_inbox'] = 'Xet Sanduqigha qaytish üchün %sbu yerni%s chéking.';
$lang['Click_return_index'] = 'Bashbetke qaytish üchün %sbu yerni%s chéking.';

$lang['Send_a_new_message'] = 'Yéngi xususiy meséj yollash';
$lang['Send_a_reply'] = 'Xususiy meséjge jawab qayturush';
$lang['Edit_message'] = 'Xususiy meséjni tehrirlesh';

$lang['Notification_subject'] = 'Yéngi bir xususiy meséj keldi!';

$lang['Find_username'] = 'Izdesh';
$lang['Find'] = 'Tépildi';
$lang['No_match'] = 'Qollanchi isimi tépilmidi.';

$lang['No_post_id'] = 'Yézilma ID si bérilmidi.';
$lang['No_such_folder'] = 'Bundaq munderije yoq.';
$lang['No_folder'] = 'Munderije tallanmidi ';


$lang['Mark_all'] = 'Hemmisini tallash';
$lang['Unmark_all'] = 'Hemmisini bikar qilish';

$lang['Confirm_delete_pm'] = 'Bu meséjni choqum öchüremsiz?';
$lang['Confirm_delete_pms'] = 'Bu meséjlarni choqum öchüremsiz?';

$lang['Inbox_size'] = 'Xet sanduqi %d%% toldi'; // eg. Your Inbox is 50% full
$lang['Sentbox_size'] = 'Yollanghan xet %d%% boldi'; 
$lang['Savebox_size'] = 'Arxip sanduqi %d%% toldi'; 

$lang['Click_view_privmsg'] = 'Xet Sanduqigha bérish üchün %sbu yerni%s chéking.';



//
// Profiles/Registration
//
$lang['Viewing_user_profile'] = 'Profil körsütüsh :: %s'; // %s is username 
$lang['About_user'] = '%s heqqide'; // %s is username

$lang['Preferences'] = 'Tallashlar';

$lang['Items_required'] = '* belgisi barlirini toldurush zörürdur.';
$lang['Registration_info'] = 'Tizimlitish Uchurliri';
$lang['Profile_info'] = 'Profil Uchurliri';
$lang['Profile_info_warn'] = 'Töwendikiliri ashkare körünidighan uchurlar';
$lang['Avatar_panel'] = 'Awatar Kontroli';
$lang['Avatar_gallery'] = 'Awatar Galériyisi';

$lang['Website'] = 'Torbet';
$lang['Location'] = 'Orni';
$lang['Contact'] = 'Alaqe';
$lang['Email_address'] = 'E-mail adrési';
$lang['Email'] = 'E-mail';
$lang['Send_private_message'] = 'Xususiy meséj yollash';
$lang['Hidden_email'] = '[ Yoshurnush ]';
$lang['Search_user_posts'] = 'Bu qollan\'ghuchining yézilmilirini izdesh';
$lang['Interests'] = 'Qiziqishi';
$lang['Occupation'] = 'Kespi'; 
$lang['Poster_rank'] = 'Qollan\'ghuchi Derijisi';

$lang['Total_posts'] = 'Jem\'iy Yézilma';
$lang['User_post_pct_stats'] = 'Jem\'iy yézilmining %.2f%% '; // 1.25% of total
$lang['User_post_day_stats'] = 'Künlük yézilmilar %.2f '; // 1.5 posts per day
$lang['Search_user_posts'] = '%s ning barliq yézilmilirini izdesh'; // Find all posts by username

$lang['No_user_id_specified'] = 'Kechürüng, bundaq qollan\'ghuchi yoq.';
$lang['Wrong_Profile'] = 'Özingizning bolmighan profilni özgertelmeysiz.';

$lang['Only_one_avatar'] = 'Peqet birla awatar tallinidu';
$lang['File_no_data'] = 'Teminligen torbétingizde data yoq';

$lang['No_connection_URL'] = 'Teminligen ulinish adrési(URL) gha ulanmidi';
$lang['Incomplete_URL'] = 'Teminligen ulinish adrési(URL) toluq emes';
$lang['Wrong_remote_avatar_format'] = 'Teminligen awatar ulinish adrési(URL)toghra formatta emes';
$lang['No_send_account_inactive'] = 'Kechürüng, hazirche parol ibertilmidi, chünki sizning hésabingiz téxi qozghutulmighan. Sehipe bashqurghuchi bilen alaqilishing.';

$lang['Always_smile'] = 'Chiraylar daim ochuq';
$lang['Always_html'] = 'HTML daim ochuq';
$lang['Always_bbcode'] = 'BBCode daim ochuq';
$lang['Always_add_sig'] = 'Daim imzayimni qoshimen';
$lang['Always_notify'] = 'Daim méni jawablardin xewerlendür';
$lang['Always_notify_explain'] = 'Sizning yazmiliringizgha jawab qayturulghanda e-mail arqiliq sizni xewerlendüridu. Buni yézilma yollighanda özgerteleysiz.';

$lang['Board_style'] = 'Sehipe Uslubi';
$lang['Board_lang'] = 'Sehipe Tili';
$lang['No_themes'] = 'Sanliq Ambarda Téma Yoq';
$lang['Timezone'] = 'Waqit Rayoni';

$lang['Date_format'] = 'Chisla formati';
$lang['Date_format_explain'] = 'Chisla formati ipadilesh tili PHP diki <a href=\'http://www.php.net/date\' target=\'_other\'>date()</a> bilen oxshash';
$lang['Signature'] = 'Imza';
$lang['Signature_explain'] = 'Bu imza yazmiliringizning axirigha qoshulup körsütilidu. Imzaning %d herp cheklimisi bar.';
$lang['Public_view_email'] = 'E-mail adrésim daim körünsun';

$lang['Current_password'] = 'Hazirqi Parol';
$lang['New_password'] = 'Yéngi Parol';
$lang['Confirm_password'] = 'Parolni Jezimlesh';
$lang['Confirm_password_explain'] = 'Parolingizni yaki e-mail adrésingizni özgertish üchün hazirqi parolingizni kirgüzisiz.';
$lang['password_if_changed'] = 'Parolni özgertmekchi bolsingiz, peqet yéngi parolni kirgüzüng.';
$lang['password_confirm_if_changed'] = 'Parolni özgertmekchi bolsingiz, peqet yéngi parolni kirgüzüp jezimleshtürüng.';

$lang['Avatar'] = 'Awatar';
$lang['Avatar_explain'] = 'Yézilmingizning yénigha kichik bir resim körsütidu. Daim peqet birla resim körsütilidu, resimning kengliki %d din, igizliki %d din we chongluqi %dKB din chong bolmisun.';
$lang['Upload_Avatar_file'] = 'Kompyutéringizdin bir awatar yollash';
$lang['Upload_Avatar_URL'] = 'Bir URL din awatar köchürüsh';
$lang['Upload_Avatar_URL_explain'] = 'Awatarning ulunush ornining URL ni kirgüzsingiz, awatar resimi bu yerge köchürülidu.';
$lang['Pick_local_Avatar'] = 'Galériyedin awatar tallash';
$lang['Link_remote_Avatar'] = 'Bashqa bettin awatar tallash';
$lang['Link_remote_Avatar_explain'] = ' Ulimaqchi bolghan awatar resimi ornining URL ni kirgüzüng';
$lang['Avatar_URL'] = 'Awatar resimining URLsi';
$lang['Select_from_gallery'] = 'Galériyedin awatar tallash';
$lang['View_avatar_gallery'] = 'Galériye körsütüsh';

$lang['Select_avatar'] = 'Awatar tallash';
$lang['Return_profile'] = 'Awatarni inawetsiz qilish';
$lang['Select_category'] = 'Katégoriye tallash';

$lang['Delete_Image'] = 'Resim öchürüsh';
$lang['Current_Image'] = 'Hazirqi Resim';

$lang['Notify_on_privmsg'] = 'Xususiy meséj kelgende méni xewerlendursun';
$lang['Popup_on_privmsg'] = 'Xususiy meséj kelgende yene bir köznek échilsun'; 
$lang['Popup_on_privmsg_explain'] = 'Yéngi meséj kelgende, yéngi bir köznek échilip xewerlendürsun.';
$lang['Hide_user'] = 'Torda halitimni yoshursun';

$lang['Profile_updated'] = 'Profilingiz yéngilandi';
$lang['Profile_updated_inactive'] = 'Profilingiz yéngilandi. Emmam bezi haletliringizni özgertiwettingiz, hem hésabingiz hazir étik halette. Uni qandaq qozghutushni bilish üchün e-mailngizni körüng, yaki bashqurghuchining qozghutup bérishini saqlang.';

$lang['Password_mismatch'] = 'Ikki qétimqda kirgüzgen parolingiz oxshash bolmidi.';
$lang['Current_password_mismatch'] = 'Hazirqi parolingiz sanliq ambardiki parolgha mas kelmidi.';
$lang['Password_long'] = 'Parolingiz 32 herptin éship ketmesliki kérek.';
$lang['Too_many_registers'] = 'Tizimlitishni bek tola sinidingiz. Sel turup qayta sinang.';
$lang['Username_taken'] = 'Kechürüng, bu isim ishlitip bolunghan.';
$lang['Username_invalid'] = 'Kechürüng, bu isim arisida inawetsiz belge( \') bar iken.';
$lang['Username_disallowed'] = 'Kechürüng, bu isim cheklengen.';
$lang['Email_taken'] = 'Kechürüng, bu e-mail adrési bashqa bir qollan\'ghuchi teripidin ishlitilgen.';
$lang['Email_banned'] = 'Kechürüng, bu e-mail adrési cheklengen. ';
$lang['Email_invalid'] = 'Kechürüng, bu e-mail adrési inawetsiz.';
$lang['Signature_too_long'] = 'Imzayingiz bek uzun.';
$lang['Fields_empty'] = 'Zörür bolghan katekchilerni toldurushingiz kérek.';
$lang['Avatar_filetype'] = 'Awatar höjjitining formati .jpg, .gif or .png bolishi kérek';
$lang['Avatar_filesize'] = 'Awatarning sighimi %d KB din kichik bolishi kérek.'; // The avatar image file size must be less than 6 KB
$lang['Avatar_imagesize'] = 'Awatar kengliki %d din, igizliki %d din kichik bolishi kérek.';

$lang['Welcome_subject'] = ' %s ge Xush Keldingiz'; // Welcome to my.com forums

$lang['New_account_subject'] = 'Yéngi Qollan\'ghuchi Hésabi';
$lang['Account_activated_subject'] = 'Hésab Qozghutush';

$lang['Account_added'] = 'Tizimlatqiningizge teshekkür, hésabingiz quruldi. Isim we parolingiz bilen kirsingiz bolidu.';

$lang['Account_inactive'] = 'Hésabingiz quruldi. Bu sehipe hésab qozghutushni telep qilidu, shunga e-mail adrésingizge qozghutush achquchi ibertildi. E-mailingizdin tepsiliy uchurgha érisheleysiz. ';
$lang['Account_inactive_admin'] = ' Hésabingiz quruldi. Bu sehipe bashqurghuchining hésab qozghutushni telep qilidu, hésabingiz qozghutulghnda, sizge e-mail ibertilip hewerlendürlisiz. ';
$lang['Account_active'] = 'Hésabingiz qozghutuldi. Tizimlatqiningiz üchün teshekkür éytimiz. ';
$lang['Account_active_admin'] = 'Hésab hazir qozghutuldi';
$lang['Reactivate'] = 'Hésabingizni qaytidin qozghutung!';
$lang['Already_activated'] = 'Hésabingizni qozghutup boldingiz';
$lang['COPPA'] = 'Hésabingiz quruldi, emma tekshürüshtin ötüsh kérek. Tepsilatini e-mail arqiliq biling. ';

$lang['Registration'] = 'Tizimlitish Kélishimi';
$lang['Reg_agreement'] = 'Bashqurghuchilar we sehipe nazretchiliri bezi bolmughur yézilmilarni imkan qeder qisqa waqit ichide birterep qilishqa yaki pütünley öchüriwétishke térishidu, emma ular barliq meséjlarni tekshürüp kételishi natayin. Shunga bu sehipidiki barliq yézilmilarning  qanuniy mesuliyiti uning yollighuchiliri ige bolidu, lékin sehipe bashqurghuchilar mes\'ul bomaydu .<br /><br />Siz haqaret qilish, pitne-pasat tarqitish, qopal bolush, töhmet qilish, öchmenlik qilish, tehdit sélish xarektiridiki, shehwaniy bolghan we qanungha xilap materiallarni choqum yollimasliqqa qoshulishingiz kérek. Bulargha diqqet qilmisingiz siz derhal we menggülük yasaqqa (qoghlunisiz) uchraysiz (we sizning tor mulazimet wakaletchingizmu xewerdar qilinidu). Yuqarqi prinsiplarning yürgüzülishge yardimi bolishi üchün, yollanmilarning barliq IP adrésliri xatirlinidu. Siz choqum nazaretchi, bashqurghuchi weyaki bet igisining herqandaq waqitta yézilmilarni özgertishi, yötkishi we öchurishige qoshulishingiz kérek. Qollan\'ghuchichi bolush süpitingiz bilen, siz teminligen matériyallarning sanliq ambarda saqlinishigha qoshulishingiz kérek. Bu uchurlar sizning ruxsitingizsiz üchinchi bir shexiske ashkarlanmaydu, emma webmaster, bashqurghuchi we nazaretchiler hakérler (hacker)ning hujum qilishi sewebidin matériyallarning ashkarlinip qélishini tosushqa kapalet bérelmeydu.<br /><br />Bu munazire sistémisi uchurlarni kompyutéringizda cookie arqiliq saqlaydu, bu cookielar siz yuqurida kirgüzgen uchurlarning héchqaysisini öz ichige almaydu, cookie diki uchurlar peqet sizge munberni qolayliq körüshingiz üchünla. E-mail adrésingiz peqet sizning hésabingizni we parolingizni ewetip bérish hem parolni unutup qalghanda yéngi parol ewetip bérish üchün ishlitilidu. <br /><br />Töwendiki ulanmini cheksingiz bu kélishimge qoshulghanliqingizni bildüridu.';

$lang['Agree_under_13'] = 'Yuqirdiki kélishimge qoshulimen hem men 13 yashtin <b>kichik</b> .';
$lang['Agree_over_13'] = 'Yuqirdiki kélishimge qoshulimen hem men 13 yashtin <b>chong</b> .';
$lang['Agree_not'] = 'Bu kélishimge qoshulmaymen.';

$lang['Wrong_activation'] = 'Kirgüzgen qozghutush kodi sanliq ambardikisi bilen mas kelmidi.';
$lang['Send_password'] = 'Manga bir yéngi parol ibertsun'; 
$lang['Password_updated'] = 'Yéngi parolngiz quruldi; bu toghrida bir tepsili körsetme e-mailingizgha ibertildi.';
$lang['No_email_match'] = 'Kirgüzgen e-mail adrési sanliq ambardikisige maslashmidi. ';
$lang['New_password_activation'] = 'Yéngi parolning qozghutulishi';

$lang['Password_activated'] = 'Hésabingiz qayta qozghutuldi. Kirish üchün e-mail arqiliq tapshurup alghan parolni ishliting.';

$lang['Send_email_msg'] = 'E-mail yollash';
$lang['No_user_specified'] = 'Qollanchi isimi tallanmidi';
$lang['User_prevent_email'] = 'Bu qollan\'ghuchi e-mail qobul qilishni xalimaydu. Xususiy meséj yollap sinap béqing.';
$lang['User_not_exist'] = 'Bundaq qollan\'ghuchi yoq';
$lang['CC_email'] = 'Bu e-mailning bir kopiyisini özige yollash';

$lang['Email_message_desc'] = 'Mu meséj tékst höjjiti péti yollandi, HTML yaki BBCode ni ishletmeng. Jawab qayturidighan adrésqa sizning e-mail adrésingiz bérildi. ';
$lang['Flood_email_limit'] = 'Mushu peytte bashqa e-mail yolliyalmaysiz, sel turup qayta sinang. ';
$lang['Recipient'] = 'Tapshurwalghuchi';
$lang['Email_sent'] = 'Bu e-mail yollandi.';
$lang['Send_email'] = 'E-mail yollash';
$lang['Empty_subject_email'] = 'E-mailgha bir mawzu qoyushingiz kérek.';

$lang['Empty_message_email'] = 'E-mailning bir téksti bolishi kérek.';


//
// Visual confirmation system strings
//
$lang['Confirm_code_wrong'] = 'Kirgüzgen jezimlesh kodi toghra bolmidi';
$lang['Too_many_registers'] = 'Sizning tizimlitishni sinash qétim saningiz cheklimidin ésip ketti, sel turup qayta sinang.';
$lang['Confirm_code_impaired'] = 'Eger siz daim awarchilikke yoluqsingiz yaki bashqa sewebler tüpeyli kodni oquyalmisingiz, %sBashqurghuchi%s din yardem sorang.';
$lang['Confirm_code'] = 'Jezimlesh Kodi';
$lang['Confirm_code_explain'] = 'Körgen kodni toghra kirgüzüng. Bu kodning chong-kichik yézilishda perq bar, nölning ichide bir diagonal siziq bar.';




//
// Memberslist
//
$lang['Select_sort_method'] = 'Sortlash usulini tallang';
$lang['Sort'] = 'Sortlash';
$lang['Sort_Top_Ten'] = 'Aldinqi 10 Qollan\'ghuchi';
$lang['Sort_Joined'] = 'Tizimlatqan Waqit';
$lang['Sort_Username'] = 'Qollanchi Isimi';
$lang['Sort_Location'] = 'Orni';
$lang['Sort_Posts'] = 'Yézilma Sani';
$lang['Sort_Email'] = 'E-mail Adrési';
$lang['Sort_Website'] = 'Torbéti';
$lang['Sort_Ascending'] = 'Artma';
$lang['Sort_Descending'] = 'Azaytma';
$lang['Order'] = 'Tertipi';


//
// Group control panel

//
$lang['Group_Control_Panel'] = 'Guruppa Kontroli';
$lang['Group_member_details'] = 'Guruppa Ezaliri Tepsilati';
$lang['Group_member_join'] = 'Bir Guruppqa Qétilish';

$lang['Group_Information'] = 'Guruppa Uchuri';
$lang['Group_name'] = 'Guruppa Nami';

$lang['Group_description'] = 'Guruppa Chüshendürlishi';
$lang['Group_membership'] = 'Guruppa Ezaliri';
$lang['Group_Members'] = 'Guruppa Ezaliri';

$lang['Group_Moderator'] = 'Guruppa Nazaretchisi';
$lang['Pending_members'] = 'Kandidat Ezalar';

$lang['Group_type'] = 'Guruppa türi';
$lang['Group_open'] = 'Ochuq Guruppa';
$lang['Group_closed'] = 'Étik Guruppa';
$lang['Group_hidden'] = 'Yoshurun Guruppa';

$lang['Current_memberships'] = 'Hazirqi Ezalar';
$lang['Non_member_groups'] = 'Ezasiz Guruppilar';
$lang['Memberships_pending'] = 'Kandidat Ezalar';


$lang['No_groups_exist'] = 'Hazirche guruppa yoq';
$lang['Group_not_exist'] = 'Bundaq guruppa yoq';

$lang['Join_group'] = 'Guruppqa Qétilish';
$lang['No_group_members'] = 'Bu guruppta eza yoq';
$lang['Group_hidden_members'] = 'Bu guruppa yoshurunghan, uning ezalirini körelmeysiz.';
$lang['No_pending_group_members'] = 'Bu guruppta kandidat eza yoq';
$lang['Group_joined'] = 'Muweppeqiyetlik tizimlandingiz.<br />Nazaretchining tekshürishidin ötkendin keyin sizni xewerlendüridu.';
$lang['Group_request'] = 'Guruppqa qétilish iltimasingiz quruldi.';
$lang['Group_approved'] = 'Iltimasingiz tapshuruldi.';
$lang['Group_added'] = 'Guruppqa qétildingiz.'; 
$lang['Already_member_group'] = 'Siz bu gruppining ezasi boldingiz.';
$lang['User_is_member_group'] = 'Bu eza ezeldin mushu guppining ezasi.';
$lang['Group_type_updated'] = 'Guruppa türi muweppeqiyetlik yéngilandi.';

$lang['Could_not_add_user'] = 'Siz tallighan qollan\'ghuchi mewjut emes.';
$lang['Could_not_anon_user'] = 'Méhman guruppa ezasi bolalmaydu.';

$lang['Confirm_unsub'] = 'Bu guruppa ezalirini emeldin qaldurushqa jezim qilamsiz?';
$lang['Confirm_unsub_pending'] = 'Iltimasingiz téxi tekshürüshtin ötmidi, uni qayturiwalamsiz?';

$lang['Unsub_success'] = 'Iltimasingizni qayturiwaldingiz.';

$lang['Approve_selected'] = 'Qobullash tallandi';
$lang['Deny_selected'] = 'Ret qilish tallandi';
$lang['Not_logged_in'] = 'Bu guruppqa qétilish üchün kirgen bolishingiz kérek.';
$lang['Remove_selected'] = 'Yötkesh tallandi';
$lang['Add_member'] = 'Eza Qoshush';
$lang['Not_group_moderator'] = 'Siz bu guruppining nazaretchisi emes, shunga bu guruppini bashquralmaysiz.';

$lang['Login_to_join'] = 'Guruppqa qétilish yaki bashqurush üchün, awwal kiring.';
$lang['This_open_group'] = 'Bu bir ochuq guruppa, ezaliqqa iltimas qilish üchün chéking.';
$lang['This_closed_group'] = 'Bu bir étik guruppa, yéngi eza qobul qilinmaydu.';
$lang['This_hidden_group'] = 'Bu bir yoshurun guruppa, aptomatik tizimlitish qobul qilinmaydu.';
$lang['Member_this_group'] = 'Siz bu guruppining ezasi.';
$lang['Pending_this_group'] = 'Siz bu guruppining kandidat ezasi.';
$lang['Are_group_moderator'] = 'Siz bu gurppining nazaretchisi.';
$lang['None'] = 'Yoq';

$lang['Subscribe'] = 'Iltimas Qilish';
$lang['Unsubscribe'] = 'Qayturiwélish';
$lang['View_Information'] = 'Guruppa Heqqide';


//
// Search
//
$lang['Search_query'] = 'Izdesh';
$lang['Search_options'] = 'Izdesh Türliri';

$lang['Search_keywords'] = 'Achquchluq söz boyiche';
$lang['Search_keywords_explain'] = '<u>AND</u> ni zörür bolghan sözlerni izdeshte, <u>OR</u> ni tépilsa bolidighan sözlerni izdeshte we <u>NOT</u> ni  netijide körüshni xalimaydighan sözlerde ishlitisiz. * belgisi bolsa izdeshte qismen mas keltürüshke ishlitilidu.';

$lang['Search_author'] = 'Aptor boyiche';
$lang['Search_author_explain'] = '* belgisi bolsa izdeshte qismen mas keltürüshke ishlitilidu.';

$lang['Search_for_any'] = 'Xalighan bir söz boyiche izdesh';
$lang['Search_for_all'] = 'Barliq söz boyiche izdesh';
$lang['Search_title_msg'] = 'Mawzu we tékst boyiche izdesh';
$lang['Search_msg_only'] = 'Peqet tékst boyiche izdesh';

$lang['Return_first'] = 'Eng aldidiki '; // followed by xxx characters in a select box
$lang['characters_posts'] = 'herplerni körsütüsh';

$lang['Search_previous'] = 'Waqit dairisi'; // followed by days, weeks, months, year, all in a select box

$lang['Sort_by'] = 'Sortlash usuli';
$lang['Sort_Time'] = 'Waqit boyiche';
$lang['Sort_Post_Subject'] = 'Téma boyiche';
$lang['Sort_Topic_Title'] = 'Mawzu boyiche';
$lang['Sort_Author'] = 'Aptor boyiche';
$lang['Sort_Forum'] = 'Sehipe boyiche';

$lang['Display_results'] = 'Netijisini körüsh';
$lang['All_available'] = 'Hemmisi';
$lang['No_searchable_forums'] = 'Sizning hemme sehipini izdesh hoquqingiz yoq.';

$lang['No_search_match'] = 'Shertingizge mas kélidighan yézilma yaki téma yoq iken. ';
$lang['Found_search_match'] = '%d Parche tékst tépildi.'; // eg. Search found 1 match
$lang['Found_search_matches'] = '%d Parche tékst tépildi. '; // eg. Search found 24 matches

$lang['Close_window'] = 'Köznekni yépish';


//
// Auth related entries
//
// Note the %s will be replaced with one of the following 'user' arrays
$lang['Sorry_auth_announce'] = 'Kechürüng, peqet %s bu sehipide uqturush yolliyalaydu.';
$lang['Sorry_auth_sticky'] = 'Kechürüng, peqet %s bu sehipide muhim meséj yolliyalaydu.'; 
$lang['Sorry_auth_read'] = 'Kechürüng, peqet %s bu sehipidiki yézilmilarni ouyalaydu.'; 
$lang['Sorry_auth_post'] = 'Kechürüng, peqet %s bu sehipide yézilma yolliyalaydu.'; 
$lang['Sorry_auth_reply'] = 'Kechürüng, peqet %s bu sehipidiki yézilmilirgha jawab qayturalaydu.';
$lang['Sorry_auth_edit'] = 'Kechürüng, peqet %s bu sehipidiki yézilmilarni tehrirliyeleydu.'; 
$lang['Sorry_auth_delete'] = 'Kechürüng, peqet %s bu sehipidiki yézilmilarni öchüreleydu.';
$lang['Sorry_auth_vote'] = 'Kechürüng, peqet %s bu sehipidiki ray-sinashqa awaz béreleydu.';

// These replace the %s in the above strings
$lang['Auth_Anonymous_Users'] = '<b>Méhmanlar</b>';
$lang['Auth_Registered_Users'] = '<b>Tizimlatqan Qollan\'ghuchilar</b>';
$lang['Auth_Users_granted_access'] = '<b>Alahide imtiyazliq qollan\'ghuchilar </b>';
$lang['Auth_Moderators'] = '<b>Nazaretchiler</b>';
$lang['Auth_Administrators'] = '<b>Bashqurghuchilar</b>';

$lang['Not_Moderator'] = 'Siz bu sehipining nazaretchisi emes.';
$lang['Not_Authorised'] = 'Hoquq bérilmigen';

$lang['You_been_banned'] = 'Bu sehipidin qoghlandingiz.<br />Sewebini bet bashqurghuchi yaki nazaretchidin sürüshtürüng.';


//
// Viewonline
//
$lang['Reg_users_zero_online'] = '0 Neper tizimlatqan qollan\'ghuchi we '; // There are 5 Registered and
$lang['Reg_users_online'] = '%d Neper tizimlatqan qollan\'ghuchil we '; // There are 5 Registered and
$lang['Reg_user_online'] = '%d Neper tizimlatqan qollan\'ghuchi we '; // There is 1 Registered and
$lang['Hidden_users_zero_online'] = '0 Neper yoshurn qollan\'ghuchi torda.'; // 6 Hidden users online
$lang['Hidden_users_online'] = '%d Neper yoshurun qollan\'ghuchi torda.'; // 6 Hidden users online
$lang['Hidden_user_online'] = '%d Neper yoshurun qollan\'ghuchi torda.'; // 6 Hidden users online
$lang['Guest_users_online'] = '%d Neper méhmanlar torda.'; // There are 10 Guest users online
$lang['Guest_users_zero_online'] = '0 Neper méhmanlar torda.'; // There are 10 Guest users online
$lang['Guest_user_online'] = '%d Neper méhman torda.'; // There is 1 Guest user online
$lang['No_users_browsing'] = 'Mushu peytte munberde qollan\'ghuchilar yoq.';

$lang['Online_explain'] = 'Bu yerde körsütilgini, mushu axirqi 5 minutta torda bolghan qollan\'ghuchilarning ehwali';

$lang['Forum_Location'] = 'Munberdiki Orni';
$lang['Last_updated'] = 'Yéqinda yéngélanghan';

$lang['Forum_index'] = 'Munber Bashbéti';
$lang['Logging_on'] = 'Kirish';
$lang['Posting_message'] = 'Meséj Yézish';
$lang['Searching_forums'] = 'Sehipe Izdesh';
$lang['Viewing_profile'] = 'Profil Körsütüsh';
$lang['Viewing_online'] = 'Tordikilarni körsütüsh';
$lang['Viewing_member_list'] = 'Ezalar tizimini körsütüsh';
$lang['Viewing_priv_msgs'] = 'Xususiy meséjni körsütüsh';
$lang['Viewing_FAQ'] = 'FAQ (Qollanma)ni körsütüsh';


//
// Moderator Control Panel
//
$lang['Mod_CP'] = 'Nazaretchi Kontroli';
$lang['Mod_CP_explain'] = 'Töwendiki iqtidarlar arqiliq bu sehipige nisbeten meshghulat qilalaysiz. Xalighan yézilmilar üstide quluplash, échish, yötkesh yaki öchürüsh qilalaysiz.';

$lang['Select'] = 'Tallash';
$lang['Delete'] = 'Öchürüsh';
$lang['Move'] = 'Yötkesh';
$lang['Lock'] = 'Quluplash';
$lang['Unlock'] = 'Échish';

$lang['Topics_Removed'] = 'Tallighan témilar muweppeqiyetlik öchürüldi.';
$lang['Topics_Locked'] = 'Tallighan témilar muweppeqiyetlik quluplandi.';
$lang['Topics_Moved'] = 'Tallighan témilar muweppeqiyetlik yötkeldi.';
$lang['Topics_Unlocked'] = 'Tallighan témilar muweppeqiyetlik échildi.';
$lang['No_Topics_Moved'] = 'Héchqandaq téma yötkelmidi.';

$lang['Confirm_delete_topic'] = 'Tallighan témini/témilarni rastinla öchüremsiz?';
$lang['Confirm_lock_topic'] = 'Tallighan témini/témilarni rastinla quluplamsiz?';
$lang['Confirm_unlock_topic'] = 'Tallighan témini/témilarni rastinla achamsiz?';
$lang['Confirm_move_topic'] = 'Tallighan témini/témilarni rastinla yötkemsiz?';

$lang['Move_to_forum'] = 'Yötkeydighan sehipe';
$lang['Leave_shadow_topic'] = 'Kona sehipide kopiyesini qaldurush';

$lang['Split_Topic'] = 'Téma Bölüsh Kontroli';
$lang['Split_Topic_explain'] = 'Töwendiki usul arqiliq bir témini ikkige ayriyalaysiz, uningdiki yézilmilarni bir birlep tallisingiz yaki tallan\'ghan yézilmini ayrisingiz bolidu.';
$lang['Split_title'] = 'Yéngi témining mawzisi';

$lang['Split_forum'] = 'Yéngi témining sehipisi';
$lang['Split_posts'] = 'Tallighan yézilmilarni ayrish';
$lang['Split_after'] = 'Tallighan yézilmdin ayrish';
$lang['Topic_split'] = 'Tallighan téma muweppeqiyetlik ayrildi.';

$lang['Too_many_error'] = 'Bek köp yézilma talliwettingiz. Bölünmekchi bolghan témidin kéyin peqet birla yézilma talliyalaysiz!';

$lang['None_selected'] = 'Ayrish üchün héchqandaq téma tallimidingiz, qaytip az dégendimu birni tallang.';
$lang['New_forum'] = 'New forum';

$lang['This_posts_IP'] = 'Bu yézilmini yollighan IP adrés';
$lang['Other_IP_this_user'] = 'Bu qollan\'ghuchining bashqa IP adrési';
$lang['Users_this_IP'] = 'Bu IP adrésni ishletken bashqa qollan\'ghuchilar';
$lang['IP_info'] = 'IP Uchurliri';
$lang['Lookup_IP'] = 'IP adrésini izdesh';


//
// Timezones ... for display on each page
//
$lang['All_times'] = 'Barliq waqitlar: %s'; // eg. All times are GMT - 12 Hours (times from next block)

$lang['-12'] = 'GMT - 12 Saet';
$lang['-11'] = 'GMT - 11 Saet';
$lang['-10'] = 'HST (Hawaii)';
$lang['-9'] = 'GMT - 9 Saet';

$lang['-8'] = 'PST (U.S./Canada)';
$lang['-7'] = 'MST (U.S./Canada)';
$lang['-6'] = 'CST (U.S./Canada)';
$lang['-5'] = 'EST (U.S./Canada)';
$lang['-4'] = 'GMT - 4 Saet';
$lang['-3.5'] = 'GMT - 3.5 Saet';
$lang['-3'] = 'GMT - 3 Saet';
$lang['-2'] = 'Mid-Atlantic';
$lang['-1'] = 'GMT - 1 Saet';
$lang['0'] = 'GMT';
$lang['1'] = 'CET (Europe)';
$lang['2'] = 'EET (Europe)';
$lang['3'] = 'GMT + 3 Saet';
$lang['3.5'] = 'GMT + 3.5 Saet';
$lang['4'] = 'GMT + 4 Saet';
$lang['4.5'] = 'GMT + 4.5 Saet';

$lang['5'] = 'GMT + 5 Saet';
$lang['5.5'] = 'GMT + 5.5 Saet';
$lang['6'] = 'GMT + 6 Saet (Ürümchi Waqti)';
$lang['6.5'] = 'GMT + 6.5 Saet';
$lang['7'] = 'GMT + 7 Saet';
$lang['8'] = 'GMT + 8 Saet (Beijing Waqti)';
$lang['9'] = 'GMT + 9 Saet';
$lang['9.5'] = 'CST (Australia)';
$lang['10'] = 'EST (Australia)';
$lang['11'] = 'GMT + 11 Saet';
$lang['12'] = 'GMT + 12 Saet';
$lang['13'] = 'GMT + 13 Saet';

// These are displayed in the timezone select box
$lang['tz']['-12'] = '(GMT -12:00 Saet) Eniwetok, Kwajalein';
$lang['tz']['-11'] = '(GMT -11:00 Saet) Midway Island, Samoa';
$lang['tz']['-10'] = '(GMT -10:00 Saet) Haway';
$lang['tz']['-9'] = '(GMT -9:00 Saet) Alyaska';
$lang['tz']['-8'] = '(GMT -8:00 Saet) Ténch Okyan Waqti(Amérika, Kanada), Tijuana';
$lang['tz']['-7'] = '(GMT -7:00 Saet) Taghliq Waqti(Amérika, Kanada), Arizona';
$lang['tz']['-6'] = '(GMT -6:00 Saet) Ottura Rayon Waqti(Amérika, Kanada), Méksika Shehri';
$lang['tz']['-5'] = '(GMT -5:00 Saet) Sherqiy Qisim Waqti(Amérika, Kanada), Bogota, Lima, Quito';
$lang['tz']['-4'] = '(GMT -4:00 Saet) Atlantik Okyan Waqti(Kanada), Karakas, La Paz';
$lang['tz']['-3.5'] = '(GMT -3:30 Saet) Newfoundland';
$lang['tz']['-3'] = '(GMT -3:00 Saet) Brazliye, Buenos Aires, Georgetown, Falkland Is';
$lang['tz']['-2'] = '(GMT -2:00 Saet) Ottura Atlantik Okyan, Ascension Is., St. Helena';
$lang['tz']['-1'] = '(GMT -1:00 Saet) Azores, Kape Verde Islands';
$lang['tz']['0'] = '(GMT) Casablanca, Dublin, Edinburgh, London, Lisbon, Monrovia';
$lang['tz']['1'] = '(GMT +1:00 Saet) Amsterdam, Bérlin, Brussel, Madrid, Parij, Rim';
$lang['tz']['2'] = '(GMT +2:00 Saet) Qahire, Helsinki, Kaliningrad, Jenubiy Afriqa';
$lang['tz']['3'] = '(GMT +3:00 Saet) Baghdad, Riyadh, Moskowa, Nairobi';
$lang['tz']['3.5'] = '(GMT +3:30 Saet) Téhran';
$lang['tz']['4'] = '(GMT +4:00 Saet) Abu Dhabi, Baku, Muscat, Tbilisi';
$lang['tz']['4.5'] = '(GMT +4:30 Saet) Kabul';
$lang['tz']['5'] = '(GMT +5:00 Saet) Ekaterinburg, Islamabad, Karachi, Tashkent';
$lang['tz']['5.5'] = '(GMT +5:30 Saet) Bombay, Kalkutta, Madras, Yéngi Delhi';
$lang['tz']['6'] = '(GMT +6:00 Saet) Ürümchi, Almata, Kolombo, Dhaka, Yéngi Sibériye';
$lang['tz']['6.5'] = '(GMT +6:30 Saet) Rangoon';
$lang['tz']['7'] = '(GMT +7:00 Saet) Bangkok, Hanoi, Jakarta';
$lang['tz']['8'] = '(GMT +8:00 Saet) Beijing, Hong Kong, Perth, Singapor, Taipei';
$lang['tz']['9'] = '(GMT +9:00 Saet) Osaka, Sapporo, Seoul, Tokyo, Yakutsk';
$lang['tz']['9.5'] = '(GMT +9:30 Saet) Adelaide, Darwin';
$lang['tz']['10'] = '(GMT +10:00 Saet) Kanberra, Guam, Melbourne, Sidnéy, Vladivostok';
$lang['tz']['11'] = '(GMT +11:00 Saet) Magadan, Yéngi Kaledonia, Solomon Islands';
$lang['tz']['12'] = '(GMT +12:00 Saet) Auckland, Wellington, Fiji, Marshall Island';
$lang['tz']['13'] = 'GMT + 13 Saet';

$lang['datetime']['Sunday'] = 'Yekshenbe';
$lang['datetime']['Monday'] = 'Düshenbe';
$lang['datetime']['Tuesday'] = 'Seyshenbe';
$lang['datetime']['Wednesday'] = 'Charshenbe';
$lang['datetime']['Thursday'] = 'Peyshenbe';

$lang['datetime']['Friday'] = 'Jüme';
$lang['datetime']['Saturday'] = 'Shenbe';
$lang['datetime']['Sun'] = 'Yek';
$lang['datetime']['Mon'] = 'Düs';
$lang['datetime']['Tue'] = 'Sey';
$lang['datetime']['Wed'] = 'Cha';
$lang['datetime']['Thu'] = 'Pey';
$lang['datetime']['Fri'] = 'Jüm';
$lang['datetime']['Sat'] = 'She';
$lang['datetime']['January'] = 'Yanwar';
$lang['datetime']['February'] = 'Féwral';
$lang['datetime']['March'] = 'Mart';
$lang['datetime']['April'] = 'April';
$lang['datetime']['May'] = 'May';
$lang['datetime']['June'] = 'Iyun';
$lang['datetime']['July'] = 'Iyul';
$lang['datetime']['August'] = 'Awghust';
$lang['datetime']['September'] = 'Séntebér';
$lang['datetime']['October'] = 'Oktebér';
$lang['datetime']['November'] = 'Noyabér';
$lang['datetime']['December'] = 'Dékabér';
$lang['datetime']['Jan'] = 'Yan';
$lang['datetime']['Feb'] = 'Féw';
$lang['datetime']['Mar'] = 'Mart';
$lang['datetime']['Apr'] = 'Apr';
$lang['datetime']['May'] = 'May';
$lang['datetime']['Jun'] = 'Iyun';
$lang['datetime']['Jul'] = 'Iyul';
$lang['datetime']['Aug'] = 'Aug';
$lang['datetime']['Sep'] = 'Sén';
$lang['datetime']['Oct'] = 'Ökt';
$lang['datetime']['Nov'] = 'Noy';
$lang['datetime']['Dec'] = 'Dék';

//
// Errors (not related to a
// specific failure on a page)
//
$lang['Information'] = 'Uchur';
$lang['Critical_Information'] = 'Hel qilghuch uchur';

$lang['General_Error'] = 'Adettiki xataliq';

$lang['Critical_Error'] = 'Hel qilghuch xataliq';
$lang['An_error_occured'] = 'Bir xataliq körüldi.';
$lang['A_critical_error'] = 'Bir hel qilghuch xataliq körüldi.';

// Translator credit
$lang['TRANSLATION_INFO'] = "Uyghurchigha terjime qilghuchilar: M.Erdem & Abdireyim";
//
// That's all, Folks!
// -------------------------------------------------

?>