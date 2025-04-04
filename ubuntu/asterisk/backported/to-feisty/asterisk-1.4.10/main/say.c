/*
 * Asterisk -- An open source telephony toolkit.
 *
 * Copyright (C) 1999 - 2005, Digium, Inc.
 *
 * Mark Spencer <markster@digium.com>
 * George Konstantoulakis <gkon@inaccessnetworks.com>
 *
 * See http://www.asterisk.org for more information about
 * the Asterisk project. Please do not directly contact
 * any of the maintainers of this project for assistance;
 * the project provides a web site, mailing lists and IRC
 * channels for your use.
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

/*! \file
 *
 * \brief Say numbers and dates (maybe words one day too)
 *
 * \author Mark Spencer <markster@digium.com>
 * 
 * \note 12-16-2004 : Support for Greek added by InAccess Networks (work funded by HOL, www.hol.gr) George Konstantoulakis <gkon@inaccessnetworks.com>
 *  						
 * \note 2007-02-08 : Support for Georgian added by Alexander Shaduri <ashaduri@gmail.com>,
 *  						Next Generation Networks (NGN).
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision: 77795 $")

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#ifdef SOLARIS
#include <iso/limits_iso.h>
#endif

#include "asterisk/file.h"
#include "asterisk/channel.h"
#include "asterisk/logger.h"
#include "asterisk/options.h"
#include "asterisk/say.h"
#include "asterisk/lock.h"
#include "asterisk/localtime.h"
#include "asterisk/utils.h"

/* Forward declaration */
static int wait_file(struct ast_channel *chan, const char *ints, const char *file, const char *lang);


static int say_character_str_full(struct ast_channel *chan, const char *str, const char *ints, const char *lang, int audiofd, int ctrlfd)
{
	const char *fn;
	char fnbuf[256];
	char ltr;
	int num = 0;
	int res = 0;

	while (str[num] && !res) {
		fn = NULL;
		switch (str[num]) {
		case ('*'):
			fn = "digits/star";
			break;
		case ('#'):
			fn = "digits/pound";
			break;
		case ('!'):
			fn = "letters/exclaimation-point";
			break;
		case ('@'):
			fn = "letters/at";
			break;
		case ('$'):
			fn = "letters/dollar";
			break;
		case ('-'):
			fn = "letters/dash";
			break;
		case ('.'):
			fn = "letters/dot";
			break;
		case ('='):
			fn = "letters/equals";
			break;
		case ('+'):
			fn = "letters/plus";
			break;
		case ('/'):
			fn = "letters/slash";
			break;
		case (' '):
			fn = "letters/space";
			break;
		case ('0'):
		case ('1'):
		case ('2'):
		case ('3'):
		case ('4'):
		case ('5'):
		case ('6'):
		case ('7'):
		case ('8'):
		case ('9'):
			strcpy(fnbuf, "digits/X");
			fnbuf[7] = str[num];
			fn = fnbuf;
			break;
		default:
			ltr = str[num];
			if ('A' <= ltr && ltr <= 'Z') ltr += 'a' - 'A';		/* file names are all lower-case */
			strcpy(fnbuf, "letters/X");
			fnbuf[8] = ltr;
			fn = fnbuf;
		}
		if (fn && ast_fileexists(fn, NULL, NULL) > 0) {
			res = ast_streamfile(chan, fn, lang);
			if (!res) {
				if ((audiofd  > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
		num++;
	}

	return res;
}

static int say_phonetic_str_full(struct ast_channel *chan, const char *str, const char *ints, const char *lang, int audiofd, int ctrlfd)
{
	const char *fn;
	char fnbuf[256];
	char ltr;
	int num = 0;
	int res = 0;

	while (str[num] && !res) {
		fn = NULL;
		switch (str[num]) {
		case ('*'):
			fn = "digits/star";
			break;
		case ('#'):
			fn = "digits/pound";
			break;
		case ('!'):
			fn = "letters/exclaimation-point";
			break;
		case ('@'):
			fn = "letters/at";
			break;
		case ('$'):
			fn = "letters/dollar";
			break;
		case ('-'):
			fn = "letters/dash";
			break;
		case ('.'):
			fn = "letters/dot";
			break;
		case ('='):
			fn = "letters/equals";
			break;
		case ('+'):
			fn = "letters/plus";
			break;
		case ('/'):
			fn = "letters/slash";
			break;
		case (' '):
			fn = "letters/space";
			break;
		case ('0'):
		case ('1'):
		case ('2'):
		case ('3'):
		case ('4'):
		case ('5'):
		case ('6'):
		case ('7'):
		case ('8'):
			strcpy(fnbuf, "digits/X");
			fnbuf[7] = str[num];
			fn = fnbuf;
			break;
		default:	/* '9' falls here... */
			ltr = str[num];
			if ('A' <= ltr && ltr <= 'Z') ltr += 'a' - 'A';		/* file names are all lower-case */
			strcpy(fnbuf, "phonetic/X_p");
			fnbuf[9] = ltr;
			fn = fnbuf;
		}
		if (fn && ast_fileexists(fn, NULL, NULL) > 0) {
			res = ast_streamfile(chan, fn, lang);
			if (!res) {
				if ((audiofd  > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
		num++;
	}

	return res;
}

static int say_digit_str_full(struct ast_channel *chan, const char *str, const char *ints, const char *lang, int audiofd, int ctrlfd)
{
	const char *fn;
	char fnbuf[256];
	int num = 0;
	int res = 0;

	while (str[num] && !res) {
		fn = NULL;
		switch (str[num]) {
		case ('*'):
			fn = "digits/star";
			break;
		case ('#'):
			fn = "digits/pound";
			break;
		case ('-'):
			fn = "digits/minus";
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			strcpy(fnbuf, "digits/X");
			fnbuf[7] = str[num];
			fn = fnbuf;
			break;
		}
		if (fn && ast_fileexists(fn, NULL, NULL) > 0) {
			res = ast_streamfile(chan, fn, lang);
			if (!res) {
				if ((audiofd  > -1) && (ctrlfd > -1))
                                        res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
                                else
                                        res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
		num++;
	}

	return res;
}

/* Forward declarations */
/*! \page Def_syntaxlang Asterisk Language Syntaxes supported
    \note Not really language codes.
	For these language codes, Asterisk will change the syntax when
	saying numbers (and in some cases dates and voicemail messages
	as well)
      \arg \b da    - Danish
      \arg \b de    - German
      \arg \b en    - English (US)
      \arg \b en_GB - English (British)
      \arg \b es    - Spanish, Mexican
      \arg \b fr    - French
      \arg \b he    - Hebrew
      \arg \b it    - Italian
      \arg \b nl    - Dutch
      \arg \b no    - Norwegian
      \arg \b pl    - Polish       
      \arg \b pt    - Portuguese
      \arg \b pt_BR - Portuguese (Brazil)
      \arg \b se    - Swedish
      \arg \b tw    - Taiwanese / Chinese
      \arg \b ru    - Russian
      \arg \b ge    - Georgian

 \par Gender:
 For Some languages the numbers differ for gender and plural.
 \arg Use the option argument 'f' for female, 'm' for male and 'n' for neuter in languages like Portuguese, French, Spanish and German.
 \arg use the option argument 'c' is for commune and 'n' for neuter gender in nordic languages like Danish, Swedish and Norwegian.
 use the option argument 'p' for plural enumerations like in German
 
 Date/Time functions currently have less languages supported than saynumber().

 \todo Note that in future, we need to move to a model where we can differentiate further - e.g. between en_US & en_UK

 See contrib/i18n.testsuite.conf for some examples of the different syntaxes

 \par Portuguese
 Portuguese sound files needed for Time/Date functions:
 pt-ah
 pt-ao
 pt-de
 pt-e
 pt-ora
 pt-meianoite
 pt-meiodia
 pt-sss

 \par Spanish
 Spanish sound files needed for Time/Date functions:
 es-de
 es-el

 \par Italian
 Italian sound files needed for Time/Date functions:
 ore-una
 ore-mezzanotte

*/

/* Forward declarations of language specific variants of ast_say_number_full */
static int ast_say_number_full_en(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_cz(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_da(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_de(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_en_GB(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_es(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_fr(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_he(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_it(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_nl(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_no(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_pl(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_pt(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_se(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_tw(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_gr(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_number_full_ru(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_number_full_ge(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);

/* Forward declarations of language specific variants of ast_say_enumeration_full */
static int ast_say_enumeration_full_en(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd);
static int ast_say_enumeration_full_da(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);
static int ast_say_enumeration_full_de(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd);

/* Forward declarations of ast_say_date, ast_say_datetime and ast_say_time functions */
static int ast_say_date_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_da(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_date_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang);

static int ast_say_date_with_format_en(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_da(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_de(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_es(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_he(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_fr(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_it(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_nl(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_pl(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_pt(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_tw(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);
static int ast_say_date_with_format_gr(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone);

static int ast_say_time_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_pt_BR(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_tw(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_time_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang);

static int ast_say_datetime_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_pt_BR(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_tw(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang);

static int ast_say_datetime_from_now_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_from_now_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_from_now_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang);
static int ast_say_datetime_from_now_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang);

static int wait_file(struct ast_channel *chan, const char *ints, const char *file, const char *lang) 
{
	int res;
	if ((res = ast_streamfile(chan, file, lang)))
		ast_log(LOG_WARNING, "Unable to play message %s\n", file);
	if (!res)
		res = ast_waitstream(chan, ints);
	return res;
}

/*! \brief  ast_say_number_full: call language-specific functions */
/* Called from AGI */
static int say_number_full(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	if (!strcasecmp(language,"en") ) {	/* English syntax */
	   return(ast_say_number_full_en(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "cz") ) {	/* Czech syntax */
	   return(ast_say_number_full_cz(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "da") ) {	/* Danish syntax */
	   return(ast_say_number_full_da(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "de") ) {	/* German syntax */
	   return(ast_say_number_full_de(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "en_GB") ) {	/* British syntax */
	   return(ast_say_number_full_en_GB(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "no") ) {	/* Norwegian syntax */
	   return(ast_say_number_full_no(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "es") || !strcasecmp(language, "mx")) {	/* Spanish syntax */
	   return(ast_say_number_full_es(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "fr") ) {	/* French syntax */
	   return(ast_say_number_full_fr(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "he") ) {	/* Hebrew syntax */
	   return(ast_say_number_full_he(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "it") ) {	/* Italian syntax */
	   return(ast_say_number_full_it(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "nl") ) {	/* Dutch syntax */
	   return(ast_say_number_full_nl(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "pl") ) {	/* Polish syntax */
	   return(ast_say_number_full_pl(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "pt") || !strcasecmp(language, "pt_BR")) {	/* Portuguese syntax */
	   return(ast_say_number_full_pt(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "se") ) {	/* Swedish syntax */
	   return(ast_say_number_full_se(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "tw") || !strcasecmp(language, "zh") ) {	/* Taiwanese / Chinese syntax */
	   return(ast_say_number_full_tw(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "gr") ) {	/* Greek syntax */
	   return(ast_say_number_full_gr(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "ru") ) {	/* Russian syntax */
	   return(ast_say_number_full_ru(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "ge") ) {	/* Georgian syntax */
	   return(ast_say_number_full_ge(chan, num, ints, language, options, audiofd, ctrlfd));
	}

	/* Default to english */
	return(ast_say_number_full_en(chan, num, ints, language, audiofd, ctrlfd));
}

/*! \brief  ast_say_number_full_en: English syntax */
/* This is the default syntax, if no other syntax defined in this file is used */
static int ast_say_number_full_en(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && (num || playh)) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else	if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else	if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10);
		} else {
			if (num < 1000){
				snprintf(fn, sizeof(fn), "digits/%d", (num/100));
				playh++;
				num -= ((num / 100) * 100);
			} else {
				if (num < 1000000) { /* 1,000,000 */
					res = ast_say_number_full_en(chan, num / 1000, ints, language, audiofd, ctrlfd);
					if (res)
						return res;
					num = num % 1000;
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					if (num < 1000000000) {	/* 1,000,000,000 */
						res = ast_say_number_full_en(chan, num / 1000000, ints, language, audiofd, ctrlfd);
						if (res)
							return res;
						num = num % 1000000;
						snprintf(fn, sizeof(fn), "digits/million");
					} else {
						ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
						res = -1;
					}
				}
			}
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd  > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

static int exp10_int(int power)
{
	int x, res= 1;
	for (x=0;x<power;x++)
		res *= 10;
	return res;
}

/*! \brief  ast_say_number_full_cz: Czech syntax */
/* files needed:
 * 1m,2m - gender male
 * 1w,2w - gender female
 * 3,4,...,20
 * 30,40,...,90
 * 
 * hundereds - 100 - sto, 200 - 2ste, 300,400 3,4sta, 500,600,...,900 5,6,...9set 
 * 
 * for each number 10^(3n + 3) exist 3 files represented as:
 * 		1 tousand = jeden tisic = 1_E3
 * 		2,3,4 tousands = dva,tri,ctyri tisice = 2-3_E3
 * 		5,6,... tousands = pet,sest,... tisic = 5_E3
 *
 * 		million = _E6
 * 		miliard = _E9
 * 		etc...
 *
 * tousand, milion are  gender male, so 1 and 2 is 1m 2m
 * miliard is gender female, so 1 and 2 is 1w 2w
 */
static int ast_say_number_full_cz(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	char fn[256] = "";
	
	int hundered = 0;
	int left = 0;
	int length = 0;
	
	/* options - w = woman, m = man, n = neutral. Defaultl is woman */
	if (!options)
		options = "w";
	
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	
	while(!res && (num || playh)) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 3 ) {
			snprintf(fn, sizeof(fn), "digits/%d%c",num,options[0]);
			playh = 0;
			num = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d",num);
			playh = 0;
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10);
		} else if (num < 1000) {
			hundered = num / 100;
			if ( hundered == 1 ) {
				snprintf(fn, sizeof(fn), "digits/1sto");
			} else if ( hundered == 2 ) {
				snprintf(fn, sizeof(fn), "digits/2ste");
			} else {
					res = ast_say_number_full_cz(chan,hundered,ints,language,options,audiofd,ctrlfd);
				if (res)
					return res;
				if (hundered == 3 || hundered == 4) {	
					snprintf(fn, sizeof(fn), "digits/sta");
				} else if ( hundered > 4 ) {
					snprintf(fn, sizeof(fn), "digits/set");
				}
			}
			num -= (hundered * 100);
		} else { /* num > 1000 */
			length = (int)log10(num)+1;  
			while ( (length % 3 ) != 1 ) {
				length--;		
			}
			left = num / (exp10_int(length-1));
			if ( left == 2 ) {  
				switch (length-1) {
					case 9: options = "w";  /* 1,000,000,000 gender female */
						break;
					default : options = "m"; /* others are male */
				}
			}
			if ( left > 1 )	{ /* we dont say "one thousand" but only thousand */
				res = ast_say_number_full_cz(chan,left,ints,language,options,audiofd,ctrlfd);
				if (res) 
					return res;
			}
			if ( left >= 5 ) { /* >= 5 have the same declesion */
				snprintf(fn, sizeof(fn), "digits/5_E%d",length-1);	
			} else if ( left >= 2 && left <= 4 ) {
				snprintf(fn, sizeof(fn), "digits/2-4_E%d",length-1);
			} else { /* left == 1 */
				snprintf(fn, sizeof(fn), "digits/1_E%d",length-1);
			}
			num -= left * (exp10_int(length-1));
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) {
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				} else {
					res = ast_waitstream(chan, ints);
				}
			}
			ast_stopstream(chan);
		}
	}
	return res; 
}

/*! \brief  ast_say_number_full_da: Danish syntax */
/* New files:
 In addition to English, the following sounds are required: "1N", "millions", "and" and "1-and" through "9-and" 
 */
static int ast_say_number_full_da(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int playa = 0;
	int cn = 1;		/* +1 = commune; -1 = neuter */
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	if (options && !strncasecmp(options, "n",1)) cn = -1;

	while(!res && (num || playh || playa )) {
		/* The grammar for Danish numbers is the same as for English except
		* for the following:
		* - 1 exists in both commune ("en", file "1N") and neuter ("et", file "1")
		* - numbers 20 through 99 are said in reverse order, i.e. 21 is
		*   "one-and twenty" and 68 is "eight-and sixty".
		* - "million" is different in singular and plural form
		* - numbers > 1000 with zero as the third digit from last have an
		*   "and" before the last two digits, i.e. 2034 is "two thousand and
		*   four-and thirty" and 1000012 is "one million and twelve".
		*/
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (playa) {
			snprintf(fn, sizeof(fn), "digits/and");
			playa = 0;
		} else if (num == 1 && cn == -1) {
			snprintf(fn, sizeof(fn), "digits/1N");
			num = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			int ones = num % 10;
			if (ones) {
				snprintf(fn, sizeof(fn), "digits/%d-and", ones);
				num -= ones;
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			}
		} else {
			if (num < 1000) {
				int hundreds = num / 100;
				if (hundreds == 1)
					snprintf(fn, sizeof(fn), "digits/1N");
				else
					snprintf(fn, sizeof(fn), "digits/%d", (num / 100));

				playh++;
				num -= 100 * hundreds;
				if (num)
					playa++;

			} else {
				if (num < 1000000) {
					res = ast_say_number_full_da(chan, num / 1000, ints, language, "n", audiofd, ctrlfd);
					if (res)
						return res;
					num = num % 1000;
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					if (num < 1000000000) {
						int millions = num / 1000000;
						res = ast_say_number_full_da(chan, millions, ints, language, "c", audiofd, ctrlfd);
						if (res)
							return res;
						if (millions == 1)
							snprintf(fn, sizeof(fn), "digits/million");
						else
							snprintf(fn, sizeof(fn), "digits/millions");
						num = num % 1000000;
					} else {
						ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
						res = -1;
					}
				}
				if (num && num < 100)
					playa++;
			}
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_de: German syntax */
/* New files:
 In addition to English, the following sounds are required:
 "millions"
 "1-and" through "9-and" 
 "1F" (eine)
 "1N" (ein)
 NB "1" is recorded as 'eins'
 */
static int ast_say_number_full_de(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0, t = 0;
	int mf = 1;                            /* +1 = male and neuter; -1 = female */
	char fn[256] = "";
	char fna[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	if (options && (!strncasecmp(options, "f",1)))
		mf = -1;

	while(!res && num) {
		/* The grammar for German numbers is the same as for English except
		* for the following:
		* - numbers 20 through 99 are said in reverse order, i.e. 21 is
		*   "one-and twenty" and 68 is "eight-and sixty".
		* - "one" varies according to gender
		* - 100 is 'hundert', however all other instances are 'ein hundert'
		* - 1000 is 'tausend', however all other instances are 'ein tausend'
		* - 1000000 is always 'eine million'
		* - "million" is different in singular and plural form
		*/
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 100 && t) {
			snprintf(fn, sizeof(fn), "digits/and");
			t = 0;
		} else if (num == 1 && mf == -1) {
			snprintf(fn, sizeof(fn), "digits/%dF", num);
			num = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			int ones = num % 10;
			if (ones) {
				snprintf(fn, sizeof(fn), "digits/%d-and", ones);
				num -= ones;
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			}
		} else if (num == 100 && t == 0) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			num = 0;
		} else if (num < 1000) {
			int hundreds = num / 100;
			num = num % 100;
			if (hundreds == 1) {
				snprintf(fn, sizeof(fn), "digits/1N");
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", hundreds);
			}
			snprintf(fna, sizeof(fna), "digits/hundred");
			t = 1;
		} else if (num == 1000 && t == 0) {
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = 0;
		} else 	if (num < 1000000) {
			int thousands = num / 1000;
			num = num % 1000;
			t = 1;
			if (thousands == 1) {
				snprintf(fn, sizeof(fn), "digits/1N");
				snprintf(fna, sizeof(fna), "digits/thousand");
			} else {
				res = ast_say_number_full_de(chan, thousands, ints, language, options, audiofd, ctrlfd);
				if (res)
					return res;
				snprintf(fn, sizeof(fn), "digits/thousand");
			}
		} else if (num < 1000000000) {
			int millions = num / 1000000;
			num = num % 1000000;
			t = 1;
			if (millions == 1) {
				snprintf(fn, sizeof(fn), "digits/1F");
				snprintf(fna, sizeof(fna), "digits/million");
			} else {
				res = ast_say_number_full_de(chan, millions, ints, language, options, audiofd, ctrlfd);
				if (res)
					return res;
				snprintf(fn, sizeof(fn), "digits/millions");
			}
		} else if (num <= INT_MAX) {
			int billions = num / 1000000000;
			num = num % 1000000000;
			t = 1;
			if (billions == 1) {
				snprintf(fn, sizeof(fn), "digits/1F");
				snprintf(fna, sizeof(fna), "digits/milliard");
			} else {
				res = ast_say_number_full_de(chan, billions, ints, language, options, audiofd, ctrlfd);
				if (res) {
					return res;
				}
				snprintf(fn, sizeof(fn), "digits/milliards");
			}
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
			if (!res) {
				if (strlen(fna) != 0 && !ast_streamfile(chan, fna, language)) {
					if ((audiofd > -1) && (ctrlfd > -1))
						res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
					else
						res = ast_waitstream(chan, ints);
				}
				ast_stopstream(chan);
				strcpy(fna, "");
			}
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_en_GB: British and Norwegian syntax */
/* New files:
 In addition to American English, the following sounds are required:  "and"
 */
static int ast_say_number_full_en_GB(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int playa = 0;
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && (num || playh || playa )) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (playa) {
			snprintf(fn, sizeof(fn), "digits/and");
			playa = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10);
		} else if (num < 1000) {
			int hundreds = num / 100;
			snprintf(fn, sizeof(fn), "digits/%d", (num / 100));

			playh++;
			num -= 100 * hundreds;
			if (num)
				playa++;
		} else 	if (num < 1000000) {
			res = ast_say_number_full_en_GB(chan, num / 1000, ints, language, audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num % 1000;
			if (num && num < 100)
				playa++;
		} else 	if (num < 1000000000) {
				int millions = num / 1000000;
				res = ast_say_number_full_en_GB(chan, millions, ints, language, audiofd, ctrlfd);
				if (res)
					return res;
				snprintf(fn, sizeof(fn), "digits/million");
				num = num % 1000000;
				if (num && num < 100)
					playa++;
		} else {
				ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
				res = -1;
		}
		
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_es: Spanish syntax */
/* New files:
 Requires a few new audios:
   1F.gsm: feminine 'una'
   21.gsm thru 29.gsm, cien.gsm, mil.gsm, millon.gsm, millones.gsm, 100.gsm, 200.gsm, 300.gsm, 400.gsm, 500.gsm, 600.gsm, 700.gsm, 800.gsm, 900.gsm, y.gsm 
 */
static int ast_say_number_full_es(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playa = 0;
	int mf = 0;                            /* +1 = male; -1 = female */
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	if (options) {
		if (!strncasecmp(options, "f",1))
			mf = -1;
		else if (!strncasecmp(options, "m", 1))
			mf = 1;
	}

	while (!res && num) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playa) {
			snprintf(fn, sizeof(fn), "digits/and");
			playa = 0;
		} else if (num == 1) {
			if (mf < 0)
				snprintf(fn, sizeof(fn), "digits/%dF", num);
			else if (mf > 0)
				snprintf(fn, sizeof(fn), "digits/%dM", num);
			else 
				snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 31) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num/10)*10);
			num -= ((num/10)*10);
			if (num)
				playa++;
		} else if (num == 100) {
			snprintf(fn, sizeof(fn), "digits/100");
			num = 0;
		} else if (num < 200) {
			snprintf(fn, sizeof(fn), "digits/100-and");
			num -= 100;
		} else {
			if (num < 1000) {
				snprintf(fn, sizeof(fn), "digits/%d", (num/100)*100);
				num -= ((num/100)*100);
			} else if (num < 2000) {
				num = num % 1000;
				snprintf(fn, sizeof(fn), "digits/thousand");
			} else {
				if (num < 1000000) {
					res = ast_say_number_full_es(chan, num / 1000, ints, language, options, audiofd, ctrlfd);
					if (res)
						return res;
					num = num % 1000;
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					if (num < 2147483640) {
						if ((num/1000000) == 1) {
							res = ast_say_number_full_es(chan, num / 1000000, ints, language, "M", audiofd, ctrlfd);
							if (res)
								return res;
							snprintf(fn, sizeof(fn), "digits/million");
						} else {
							res = ast_say_number_full_es(chan, num / 1000000, ints, language, options, audiofd, ctrlfd);
							if (res)
								return res;
							snprintf(fn, sizeof(fn), "digits/millions");
						}
						num = num % 1000000;
					} else {
						ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
						res = -1;
					}
				}
			}
		}

		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);

		}
			
	}
	return res;
}

/*! \brief  ast_say_number_full_fr: French syntax */
/* 	Extra sounds needed:
 	1F: feminin 'une'
 	et: 'and' */
static int ast_say_number_full_fr(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int playa = 0;
	int mf = 1;                            /* +1 = male; -1 = female */
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	
	if (options && !strncasecmp(options, "f",1))
		mf = -1;

	while(!res && (num || playh || playa)) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (playa) {
			snprintf(fn, sizeof(fn), "digits/et");
			playa = 0;
		} else if (num == 1) {
			if (mf < 0)
				snprintf(fn, sizeof(fn), "digits/%dF", num);
			else
				snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 21) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 70) {
			snprintf(fn, sizeof(fn), "digits/%d", (num/10)*10);
			if ((num % 10) == 1) playa++;
			num = num % 10;
		} else if (num < 80) {
			snprintf(fn, sizeof(fn), "digits/60");
			if ((num % 10) == 1) playa++;
			num = num - 60;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/80");
			num = num - 80;
		} else if (num < 200) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			num = num - 100;
		} else if (num < 1000) {
			snprintf(fn, sizeof(fn), "digits/%d", (num/100));
			playh++;
			num = num % 100;
		} else if (num < 2000) {
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num - 1000;
		} else if (num < 1000000) {
			res = ast_say_number_full_fr(chan, num / 1000, ints, language, options, audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num % 1000;
		} else	if (num < 1000000000) {
			res = ast_say_number_full_fr(chan, num / 1000000, ints, language, options, audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/million");
			num = num % 1000000;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}



/*! \brief  ast_say_number_full_he: Hebrew syntax */
/* 	Extra sounds needed:
 	1F: feminin 'one'
	ve: 'and'
	1hundred: 1 hundred
	2hundred: 2 hundreds
	2thousands: 2 thousand 
	thousands: plural of 'thousand'
	3sF 'Smichut forms (female)
	4sF
	5sF
	6sF
	7sF
	8sF
	9sF
	3s 'Smichut' forms (male)
	4s
	5s
	6s
	7s
	9s
	10s
	11s
	12s
	13s
	14s
	15s
	16s
	17s
	18s
	19s

TODO: 've' should sometimed be 'hu':
* before 'shtaym' (2, F)
* before 'shnaym' (2, M)
* before 'shlosha' (3, M)
* before 'shmone' (8, M)
* before 'shlosim' (30)
* before 'shmonim' (80)

What about:
'sheva' (7, F)?
'tesha' (9, F)?
*/
#define SAY_NUM_BUF_SIZE 256
static int ast_say_number_full_he(struct ast_channel *chan, int num, 
    const char *ints, const char *language, const char *options, 
    int audiofd, int ctrlfd)
{
	int res = 0;
	int state = 0; /* no need to save anything */
	int mf = 1;    /* +1 = Masculin; -1 = Feminin */
	char fn[SAY_NUM_BUF_SIZE] = "";
	ast_verbose(VERBOSE_PREFIX_3 "ast_say_digits_full: started. "
		"num: %d, options=\"%s\"\n",
		num, options
	);
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	
	if (options && !strncasecmp(options, "f",1))
		mf = -1;

	/* Do we have work to do? */
	while(!res && (num || (state>0) ))  {
		/* first type of work: play a second sound. In this loop
		 * we can only play one sound file at a time. Thus playing 
		 * a second one requires repeating the loop just for the 
		 * second file. The variable 'state' remembers where we were.
		 * state==0 is the normal mode and it means that we continue
		 * to check if the number num has yet anything left.
		 */
		ast_verbose(VERBOSE_PREFIX_3 "ast_say_digits_full: num: %d, "
			"state=%d, options=\"%s\", mf=%d\n",
			num, state, options, mf
		);
		if (state==1) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			state = 0;
		} else if (state==2) {
			snprintf(fn, sizeof(fn), "digits/ve");
			state = 0;
		} else if (state==3) {
			snprintf(fn, sizeof(fn), "digits/thousands");
			state=0;
		} else if (num <21) {
			if (mf < 0)
				snprintf(fn, sizeof(fn), "digits/%dF", num);
			else
				snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num/10)*10);
			num = num % 10;
			if (num>0) state=2;
		} else if (num < 200) {
			snprintf(fn, sizeof(fn), "digits/1hundred");
			num = num - 100;
			state=2;
		} else if (num < 300) {
			snprintf(fn, sizeof(fn), "digits/2hundred");
			num = num - 200;
			state=2;
		} else if (num < 1000) {
			snprintf(fn, sizeof(fn), "digits/%d", (num/100));
			state=1;
			num = num % 100;
		} else if (num < 2000) {
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num - 1000;
		} else if (num < 3000) {
			snprintf(fn, sizeof(fn), "digits/2thousand");
			num = num - 2000;
                        if (num>0) state=2;
		} else if (num < 20000) {
			snprintf(fn, sizeof(fn), "digits/%ds",(num/1000));
			num = num % 1000;
			state=3;
		} else if (num < 1000000) {
			res = ast_say_number_full_he(chan, num / 1000, ints, language, options, audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num % 1000;
		} else	if (num < 1000000000) {
			res = ast_say_number_full_he(chan, num / 1000000, ints, language, options, audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/million");
			num = num % 1000000;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_it:  Italian */
static int ast_say_number_full_it(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int tempnum = 0;
	char fn[256] = "";

	if (!num)
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

		/*
		Italian support

		Like english, numbers up to 20 are a single 'word', and others
 		compound, but with exceptions.
		For example 21 is not twenty-one, but there is a single word in 'it'.
		Idem for 28 (ie when a the 2nd part of a compund number
		starts with a vowel)

		There are exceptions also for hundred, thousand and million.
		In english 100 = one hundred, 200 is two hundred.
		In italian 100 = cento , like to say hundred (without one),
		200 and more are like english.
		
		Same applies for thousand:
		1000 is one thousand in en, 2000 is two thousand.
		In it we have 1000 = mille , 2000 = 2 mila 

		For million(s) we use the plural, if more than one
		Also, one million is abbreviated in it, like on-million,
		or 'un milione', not 'uno milione'.
		So the right file is provided.
		*/

		while(!res && (num || playh)) {
			if (num < 0) {
				snprintf(fn, sizeof(fn), "digits/minus");
				if ( num > INT_MIN ) {
					num = -num;
				} else {
					num = 0;
				}	
			} else if (playh) {
				snprintf(fn, sizeof(fn), "digits/hundred");
				playh = 0;
			} else if (num < 20) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 21) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 28) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 31) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 38) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 41) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 48) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 51) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 58) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 61) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 68) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 71) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 78) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 81) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 88) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 91) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num == 98) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else if (num < 100) {
				snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
				num -= ((num / 10) * 10);
			} else {
				if (num < 1000) {
					if ((num / 100) > 1) {
						snprintf(fn, sizeof(fn), "digits/%d", (num/100));
						playh++;
					} else {
						snprintf(fn, sizeof(fn), "digits/hundred");
					}
					num -= ((num / 100) * 100);
				} else {
					if (num < 1000000) { /* 1,000,000 */
						if ((num/1000) > 1)
							res = ast_say_number_full_it(chan, num / 1000, ints, language, audiofd, ctrlfd);
						if (res)
							return res;
						tempnum = num;
						num = num % 1000;
						if ((tempnum / 1000) < 2)
							snprintf(fn, sizeof(fn), "digits/thousand");
						else /* for 1000 it says mille, for >1000 (eg 2000) says mila */
							snprintf(fn, sizeof(fn), "digits/thousands");
					} else {
						if (num < 1000000000) { /* 1,000,000,000 */
							if ((num / 1000000) > 1)
								res = ast_say_number_full_it(chan, num / 1000000, ints, language, audiofd, ctrlfd);
							if (res)
								return res;
							tempnum = num;
							num = num % 1000000;
							if ((tempnum / 1000000) < 2)
								snprintf(fn, sizeof(fn), "digits/million");
							else
								snprintf(fn, sizeof(fn), "digits/millions");
						} else {
							ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
							res = -1;
						}
					}
				}
			}
			if (!res) {
				if(!ast_streamfile(chan, fn, language)) {
					if ((audiofd > -1) && (ctrlfd > -1))
						res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
					else
						res = ast_waitstream(chan, ints);
				}
				ast_stopstream(chan);
			}
		}
	return res;
}

/*! \brief  ast_say_number_full_nl: dutch syntax */
/* New files: digits/nl-en
 */
static int ast_say_number_full_nl(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int units = 0;
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	while (!res && (num || playh )) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			units = num % 10;
			if (units > 0) {
				res = ast_say_number_full_nl(chan, units, ints, language, audiofd, ctrlfd);
				if (res)
					return res;
				num = num - units;
				snprintf(fn, sizeof(fn), "digits/nl-en");
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", num - units);
				num = 0;
			}
		} else {
			if (num < 1000) {
				snprintf(fn, sizeof(fn), "digits/%d", (num/100));
				playh++;
				num -= ((num / 100) * 100);
			} else {
				if (num < 1000000) { /* 1,000,000 */
					res = ast_say_number_full_en(chan, num / 1000, ints, language, audiofd, ctrlfd);
					if (res)
						return res;
					num = num % 1000;
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					if (num < 1000000000) { /* 1,000,000,000 */
						res = ast_say_number_full_en(chan, num / 1000000, ints, language, audiofd, ctrlfd);
						if (res)
							return res;
						num = num % 1000000;
						snprintf(fn, sizeof(fn), "digits/million");
					} else {
						ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
						res = -1;
					}
				}
			}
		}

		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_no: Norwegian syntax */
/* New files:
 In addition to American English, the following sounds are required:  "and", "1N"
 */
static int ast_say_number_full_no(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int playa = 0;
	int cn = 1;		/* +1 = commune; -1 = neuter */
	char fn[256] = "";
	
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	
	if (options && !strncasecmp(options, "n",1)) cn = -1;

	while(!res && (num || playh || playa )) {
		/* The grammar for Norwegian numbers is the same as for English except
		* for the following:
		* - 1 exists in both commune ("en", file "1") and neuter ("ett", file "1N")
		*   "and" before the last two digits, i.e. 2034 is "two thousand and
		*   thirty-four" and 1000012 is "one million and twelve".
		*/
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (playa) {
			snprintf(fn, sizeof(fn), "digits/and");
			playa = 0;
		} else if (num == 1 && cn == -1) {
			snprintf(fn, sizeof(fn), "digits/1N");
			num = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10);
		} else if (num < 1000) {
			int hundreds = num / 100;
			if (hundreds == 1)
				snprintf(fn, sizeof(fn), "digits/1N");
			else
				snprintf(fn, sizeof(fn), "digits/%d", (num / 100));

			playh++;
			num -= 100 * hundreds;
			if (num)
				playa++;
		} else 	if (num < 1000000) {
			res = ast_say_number_full_no(chan, num / 1000, ints, language, "n", audiofd, ctrlfd);
			if (res)
				return res;
			snprintf(fn, sizeof(fn), "digits/thousand");
			num = num % 1000;
			if (num && num < 100)
				playa++;
		} else 	if (num < 1000000000) {
				int millions = num / 1000000;
				res = ast_say_number_full_no(chan, millions, ints, language, "c", audiofd, ctrlfd);
				if (res)
					return res;
				snprintf(fn, sizeof(fn), "digits/million");
				num = num % 1000000;
				if (num && num < 100)
					playa++;
		} else {
				ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
				res = -1;
		}
		
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

typedef struct {  
	char *separator_dziesiatek;
	char *cyfry[10];
	char *cyfry2[10];
	char *setki[10];
	char *dziesiatki[10];
	char *nastki[10];  
	char *rzedy[3][3];
} odmiana;

static char *pl_rzad_na_tekst(odmiana *odm, int i, int rzad)
{
	if (rzad==0)
		return "";
 
	if (i==1)
		return odm->rzedy[rzad - 1][0];
	if ((i > 21 || i < 11) &&  i%10 > 1 && i%10 < 5)
		return odm->rzedy[rzad - 1][1];
	else
		return odm->rzedy[rzad - 1][2];
}

static char* pl_append(char* buffer, char* str)
{
	strcpy(buffer, str);
	buffer += strlen(str); 
	return buffer;
}

static void pl_odtworz_plik(struct ast_channel *chan, const char *language, int audiofd, int ctrlfd, const char *ints, char *fn)
{    
	char file_name[255] = "digits/";
	strcat(file_name, fn);
	ast_log(LOG_DEBUG, "Trying to play: %s\n", file_name);
	if (!ast_streamfile(chan, file_name, language)) {
		if ((audiofd > -1) && (ctrlfd > -1))
			ast_waitstream_full(chan, ints, audiofd, ctrlfd);
		else
			ast_waitstream(chan, ints);
	}
	ast_stopstream(chan);
}

static void powiedz(struct ast_channel *chan, const char *language, int audiofd, int ctrlfd, const char *ints, odmiana *odm, int rzad, int i)
{
	/* Initialise variables to allow compilation on Debian-stable, etc */
	int m1000E6 = 0;
	int i1000E6 = 0;
	int m1000E3 = 0;
	int i1000E3 = 0;
	int m1000 = 0;
	int i1000 = 0;
	int m100 = 0;
	int i100 = 0;
	
	if (i == 0 && rzad > 0) { 
		return;
	}
	if (i == 0) {
		pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->cyfry[0]);
		return;
	}

	m1000E6 = i % 1000000000;
	i1000E6 = i / 1000000000;

	powiedz(chan, language, audiofd, ctrlfd, ints, odm, rzad+3, i1000E6);

	m1000E3 = m1000E6 % 1000000;
	i1000E3 = m1000E6 / 1000000;

	powiedz(chan, language, audiofd, ctrlfd, ints, odm, rzad+2, i1000E3);

	m1000 = m1000E3 % 1000;
	i1000 = m1000E3 / 1000;

	powiedz(chan, language, audiofd, ctrlfd, ints, odm, rzad+1, i1000);

	m100 = m1000 % 100;
	i100 = m1000 / 100;
	
	if (i100>0)
		pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->setki[i100]);

	if ( m100 > 0 && m100 <=9 ) {
		if (m1000>0)
			pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->cyfry2[m100]);
		else
			pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->cyfry[m100]);
	} else if (m100 % 10 == 0) {
		pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->dziesiatki[m100 / 10]);
	} else if (m100 <= 19 ) {
		pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->nastki[m100 % 10]);
	} else if (m100 != 0) {
		if (odm->separator_dziesiatek[0]==' ') {
			pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->dziesiatki[m100 / 10]);
			pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, odm->cyfry2[m100 % 10]);
		} else {
			char buf[10];
			char *b = buf;
			b = pl_append(b, odm->dziesiatki[m100 / 10]);  
			b = pl_append(b, odm->separator_dziesiatek);  
			b = pl_append(b, odm->cyfry2[m100 % 10]); 
			pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, buf);
		}
	} 

	if (rzad > 0) {
		pl_odtworz_plik(chan, language, audiofd, ctrlfd, ints, pl_rzad_na_tekst(odm, i, rzad));
	}
}

/* ast_say_number_full_pl: Polish syntax */
static int ast_say_number_full_pl(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
/*
Sounds needed:
0		zero
1		jeden
10		dziesiec
100		sto
1000		tysiac
1000000		milion
1000000000	miliard
1000000000.2	miliardy
1000000000.5	miliardow
1000000.2	miliony
1000000.5	milionow
1000.2		tysiace
1000.5		tysiecy
100m		stu
10m		dziesieciu
11		jedenascie
11m		jedenastu
12		dwanascie
12m		dwunastu
13		trzynascie
13m		trzynastu
14		czternascie
14m		czternastu
15		pietnascie
15m		pietnastu
16		szesnascie
16m		szesnastu
17		siedemnascie
17m		siedemnastu
18		osiemnascie
18m		osiemnastu
19		dziewietnascie
19m		dziewietnastu
1z		jedna
2		dwa
20		dwadziescia
200		dwiescie
200m		dwustu
20m		dwudziestu
2-1m		dwaj
2-2m		dwoch
2z		dwie
3		trzy
30		trzydziesci
300		trzysta
300m		trzystu
30m		trzydziestu
3-1m		trzej
3-2m		trzech
4		cztery
40		czterdziesci
400		czterysta
400m		czterystu
40m		czterdziestu
4-1m		czterej
4-2m		czterech
5		piec
50		piecdziesiat
500		piecset
500m		pieciuset
50m		piedziesieciu
5m		pieciu
6		szesc
60		szescdziesiat
600		szescset
600m		szesciuset
60m		szescdziesieciu
6m		szesciu
7		siedem
70		siedemdziesiat
700		siedemset
700m		siedmiuset
70m		siedemdziesieciu
7m		siedmiu
8		osiem
80		osiemdziesiat
800		osiemset
800m		osmiuset
80m		osiemdziesieciu
8m		osmiu
9		dziewiec
90		dziewiecdziesiat
900		dziewiecset
900m		dziewieciuset
90m		dziewiedziesieciu
9m		dziewieciu
and combinations of eg.: 20_1, 30m_3m, etc...

*/
{
	char *zenski_cyfry[] = {"0","1z", "2z", "3", "4", "5", "6", "7", "8", "9"};

	char *zenski_cyfry2[] = {"0","1", "2z", "3", "4", "5", "6", "7", "8", "9"};

	char *meski_cyfry[] = {"0","1", "2-1m", "3-1m", "4-1m", "5m",  /*"2-1mdwaj"*/ "6m", "7m", "8m", "9m"};

	char *meski_cyfry2[] = {"0","1", "2-2m", "3-2m", "4-2m", "5m", "6m", "7m", "8m", "9m"};

	char *meski_setki[] = {"", "100m", "200m", "300m", "400m", "500m", "600m", "700m", "800m", "900m"};

	char *meski_dziesiatki[] = {"", "10m", "20m", "30m", "40m", "50m", "60m", "70m", "80m", "90m"};

	char *meski_nastki[] = {"", "11m", "12m", "13m", "14m", "15m", "16m", "17m", "18m", "19m"};

	char *nijaki_cyfry[] = {"0","1", "2", "3", "4", "5", "6", "7", "8", "9"};

	char *nijaki_cyfry2[] = {"0","1", "2", "3", "4", "5", "6", "7", "8", "9"};

	char *nijaki_setki[] = {"", "100", "200", "300", "400", "500", "600", "700", "800", "900"};

	char *nijaki_dziesiatki[] = {"", "10", "20", "30", "40", "50", "60", "70", "80", "90"};

	char *nijaki_nastki[] = {"", "11", "12", "13", "14", "15", "16", "17", "18", "19"};

	char *rzedy[][3] = { {"1000", "1000.2", "1000.5"}, {"1000000", "1000000.2", "1000000.5"}, {"1000000000", "1000000000.2", "1000000000.5"}}; 

	/* Initialise variables to allow compilation on Debian-stable, etc */
	odmiana *o;

	static odmiana *odmiana_nieosobowa = NULL; 
	static odmiana *odmiana_meska = NULL; 
	static odmiana *odmiana_zenska = NULL; 

	if (odmiana_nieosobowa == NULL) {
		odmiana_nieosobowa = (odmiana *) malloc(sizeof(odmiana));

		odmiana_nieosobowa->separator_dziesiatek = " ";

		memcpy(odmiana_nieosobowa->cyfry, nijaki_cyfry, sizeof(odmiana_nieosobowa->cyfry));
		memcpy(odmiana_nieosobowa->cyfry2, nijaki_cyfry2, sizeof(odmiana_nieosobowa->cyfry));
		memcpy(odmiana_nieosobowa->setki, nijaki_setki, sizeof(odmiana_nieosobowa->setki));
		memcpy(odmiana_nieosobowa->dziesiatki, nijaki_dziesiatki, sizeof(odmiana_nieosobowa->dziesiatki));
		memcpy(odmiana_nieosobowa->nastki, nijaki_nastki, sizeof(odmiana_nieosobowa->nastki));
		memcpy(odmiana_nieosobowa->rzedy, rzedy, sizeof(odmiana_nieosobowa->rzedy));
	}

	if (odmiana_zenska == NULL) {
		odmiana_zenska = (odmiana *) malloc(sizeof(odmiana));

		odmiana_zenska->separator_dziesiatek = " ";

		memcpy(odmiana_zenska->cyfry, zenski_cyfry, sizeof(odmiana_zenska->cyfry));
		memcpy(odmiana_zenska->cyfry2, zenski_cyfry2, sizeof(odmiana_zenska->cyfry));
		memcpy(odmiana_zenska->setki, nijaki_setki, sizeof(odmiana_zenska->setki));
		memcpy(odmiana_zenska->dziesiatki, nijaki_dziesiatki, sizeof(odmiana_zenska->dziesiatki));
		memcpy(odmiana_zenska->nastki, nijaki_nastki, sizeof(odmiana_zenska->nastki));
		memcpy(odmiana_zenska->rzedy, rzedy, sizeof(odmiana_zenska->rzedy));
	}

	if (odmiana_meska == NULL) {
		odmiana_meska = (odmiana *) malloc(sizeof(odmiana));

		odmiana_meska->separator_dziesiatek = " ";

		memcpy(odmiana_meska->cyfry, meski_cyfry, sizeof(odmiana_meska->cyfry));
		memcpy(odmiana_meska->cyfry2, meski_cyfry2, sizeof(odmiana_meska->cyfry));
		memcpy(odmiana_meska->setki, meski_setki, sizeof(odmiana_meska->setki));
		memcpy(odmiana_meska->dziesiatki, meski_dziesiatki, sizeof(odmiana_meska->dziesiatki));
		memcpy(odmiana_meska->nastki, meski_nastki, sizeof(odmiana_meska->nastki));
		memcpy(odmiana_meska->rzedy, rzedy, sizeof(odmiana_meska->rzedy));
	}

	if (options) {
		if (strncasecmp(options, "f", 1) == 0)
			o = odmiana_zenska;
		else if (strncasecmp(options, "m", 1) == 0)
			o = odmiana_meska;
		else
			o = odmiana_nieosobowa;
	} else
		o = odmiana_nieosobowa;

	powiedz(chan, language, audiofd, ctrlfd, ints, o, 0, num);
	return 0;
}

/* ast_say_number_full_pt: Portuguese syntax */
/* 	Extra sounds needed: */
/* 	For feminin all sound files end with F */
/*	100E for 100+ something */
/*	1000000S for plural */
/*	pt-e for 'and' */
static int ast_say_number_full_pt(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	int mf = 1;                            /* +1 = male; -1 = female */
	char fn[256] = "";

	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	if (options && !strncasecmp(options, "f",1))
		mf = -1;

	while(!res && num ) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 20) {
			if ((num == 1 || num == 2) && (mf < 0))
				snprintf(fn, sizeof(fn), "digits/%dF", num);
			else
				snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num / 10) * 10);
			if (num % 10)
				playh = 1;
			num = num % 10;
		} else if (num < 1000) {
			if (num == 100)
				snprintf(fn, sizeof(fn), "digits/100");
			else if (num < 200)
				snprintf(fn, sizeof(fn), "digits/100E");
			else {
				if (mf < 0 && num > 199)
					snprintf(fn, sizeof(fn), "digits/%dF", (num / 100) * 100);
				else
					snprintf(fn, sizeof(fn), "digits/%d", (num / 100) * 100);
				if (num % 100)
					playh = 1;
			}
			num = num % 100;
		} else if (num < 1000000) {
			if (num > 1999) {
				res = ast_say_number_full_pt(chan, (num / 1000) * mf, ints, language, options, audiofd, ctrlfd);
				if (res)
					return res;
			}
			snprintf(fn, sizeof(fn), "digits/1000");
			if ((num % 1000) && ((num % 1000) < 100  || !(num % 100)))
				playh = 1;
			num = num % 1000;
		} else if (num < 1000000000) {
			res = ast_say_number_full_pt(chan, (num / 1000000), ints, language, options, audiofd, ctrlfd );
			if (res)
				return res;
			if (num < 2000000)
				snprintf(fn, sizeof(fn), "digits/1000000");
			else
				snprintf(fn, sizeof(fn), "digits/1000000S");
 
			if ((num % 1000000) &&
				/* no thousands */
				((!((num / 1000) % 1000) && ((num % 1000) < 100 || !(num % 100))) ||
				/* no hundreds and below */
				(!(num % 1000) && (((num / 1000) % 1000) < 100 || !((num / 1000) % 100))) ) )
				playh = 1;
			num = num % 1000000;
		} else {
			/* number is too big */
			ast_log(LOG_WARNING, "Number '%d' is too big to say.", num);
			res = -1;			
		}
		if (!res) {
			if (!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);	
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
		if (!res && playh) {
			res = wait_file(chan, ints, "digits/pt-e", language);
			ast_stopstream(chan);
			playh = 0;
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_se: Swedish syntax */
static int ast_say_number_full_se(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	char fn[256] = "";
	int cn = 1;		/* +1 = commune; -1 = neuter */
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);
	if (options && !strncasecmp(options, "n",1)) cn = -1;

	while(!res && (num || playh)) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (playh) {
			snprintf(fn, sizeof(fn), "digits/hundred");
			playh = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10);
		} else if (num == 1 && cn == -1) {	/* En eller ett? */
		 	snprintf(fn, sizeof(fn), "digits/1N");
			num = 0;
		} else {
			if (num < 1000){
				snprintf(fn, sizeof(fn), "digits/%d", (num/100));
				playh++;
				num -= ((num / 100) * 100);
			} else {
				if (num < 1000000) { /* 1,000,000 */
					res = ast_say_number_full_se(chan, num / 1000, ints, language, options, audiofd, ctrlfd);
					if (res) {
						return res;
					}
					num = num % 1000;
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					if (num < 1000000000) {	/* 1,000,000,000 */
						res = ast_say_number_full_se(chan, num / 1000000, ints, language, options, audiofd, ctrlfd);
						if (res) {
							return res;
						}
						num = num % 1000000;
						snprintf(fn, sizeof(fn), "digits/million");
					} else {
						ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
						res = -1;
					}
				}
			}
		}
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
				ast_stopstream(chan);
			}
		}
	}
	return res;
}

/*! \brief  ast_say_number_full_tw: Taiwanese / Chinese syntax */
static int ast_say_number_full_tw(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0;
	int playh = 0;
	char fn[256] = "";
	if (!num)
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && (num || playh)) {
			if (num < 0) {
				snprintf(fn, sizeof(fn), "digits/minus");
				if ( num > INT_MIN ) {
					num = -num;
				} else {
					num = 0;
				}	
			} else if (playh) {
				snprintf(fn, sizeof(fn), "digits/hundred");
				playh = 0;
			} else	if (num < 10) {
				snprintf(fn, sizeof(fn), "digits/%d", num);
				num = 0;
			} else	if (num < 100) {
				snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
				num -= ((num / 10) * 10);
			} else {
				if (num < 1000){
					snprintf(fn, sizeof(fn), "digits/%d", (num/100));
					playh++;
					num -= ((num / 100) * 100);
				} else {
					if (num < 1000000) { /* 1,000,000 */
						res = ast_say_number_full_tw(chan, num / 1000, ints, language, audiofd, ctrlfd);
						if (res)
							return res;
						num = num % 1000;
						snprintf(fn, sizeof(fn), "digits/thousand");
					} else {
						if (num < 1000000000) {	/* 1,000,000,000 */
							res = ast_say_number_full_tw(chan, num / 1000000, ints, language, audiofd, ctrlfd);
							if (res)
								return res;
							num = num % 1000000;
							snprintf(fn, sizeof(fn), "digits/million");
						} else {
							ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
							res = -1;
						}
					}
				}
			}
			if (!res) {
				if(!ast_streamfile(chan, fn, language)) {
					if ((audiofd > -1) && (ctrlfd > -1))
						res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
					else
						res = ast_waitstream(chan, ints);
				}
				ast_stopstream(chan);
			}
	}
	return res;
}


/*! \brief  determine last digits for thousands/millions (ru) */
static int get_lastdigits_ru(int num) {
	if (num < 20) {
		return num;
	} else if (num < 100) {
		return get_lastdigits_ru(num % 10);
	} else if (num < 1000) {
		return get_lastdigits_ru(num % 100);
	}
	return 0;	/* number too big */
}


/*! \brief  ast_say_number_full_ru: Russian syntax */
/*! \brief  additional files:
	n00.gsm			(one hundred, two hundred, ...)
	thousand.gsm
	million.gsm
	thousands-i.gsm		(tisyachi)
	million-a.gsm		(milliona)
	thousands.gsm
	millions.gsm
	1f.gsm			(odna)
	2f.gsm			(dve)
    
	where 'n' from 1 to 9
*/
static int ast_say_number_full_ru(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	int lastdigits = 0;
	char fn[256] = "";
	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && (num)) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus");
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else	if (num < 20) {
			if(options && strlen(options) == 1 && num < 3) {
			    snprintf(fn, sizeof(fn), "digits/%d%s", num, options);
			} else {
    			    snprintf(fn, sizeof(fn), "digits/%d", num);
			}
			num = 0;
		} else	if (num < 100) {
			snprintf(fn, sizeof(fn), "digits/%d", num - (num % 10));
			num %= 10;
		} else 	if (num < 1000){
			snprintf(fn, sizeof(fn), "digits/%d", num - (num % 100));
			num %= 100;
		} else 	if (num < 1000000) { /* 1,000,000 */
			lastdigits = get_lastdigits_ru(num / 1000);
			/* say thousands */
			if (lastdigits < 3) {
				res = ast_say_number_full_ru(chan, num / 1000, ints, language, "f", audiofd, ctrlfd);
			} else {
				res = ast_say_number_full_ru(chan, num / 1000, ints, language, NULL, audiofd, ctrlfd);
			}
			if (res)
				return res;
			if (lastdigits == 1) {
				snprintf(fn, sizeof(fn), "digits/thousand");
			} else if (lastdigits > 1 && lastdigits < 5) {
				snprintf(fn, sizeof(fn), "digits/thousands-i");
			} else {
				snprintf(fn, sizeof(fn), "digits/thousands");
			}
			num %= 1000;
		} else 	if (num < 1000000000) {	/* 1,000,000,000 */
			lastdigits = get_lastdigits_ru(num / 1000000);
			/* say millions */
			res = ast_say_number_full_ru(chan, num / 1000000, ints, language, NULL, audiofd, ctrlfd);
			if (res)
				return res;
			if (lastdigits == 1) {
				snprintf(fn, sizeof(fn), "digits/million");
			} else if (lastdigits > 1 && lastdigits < 5) {
				snprintf(fn, sizeof(fn), "digits/million-a");
			} else {
				snprintf(fn, sizeof(fn), "digits/millions");
			}
			num %= 1000000;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
				res = -1;
		}
		if (!res) {
			if (!ast_streamfile(chan, fn, language)) {
				if ((audiofd  > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}


/*! \brief  ast_say_enumeration_full: call language-specific functions */
/* Called from AGI */
static int say_enumeration_full(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	if (!strcasecmp(language,"en") ) {	/* English syntax */
	   return(ast_say_enumeration_full_en(chan, num, ints, language, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "da") ) {	/* Danish syntax */
	   return(ast_say_enumeration_full_da(chan, num, ints, language, options, audiofd, ctrlfd));
	} else if (!strcasecmp(language, "de") ) {	/* German syntax */
	   return(ast_say_enumeration_full_de(chan, num, ints, language, options, audiofd, ctrlfd));
	} 
	
	/* Default to english */
	return(ast_say_enumeration_full_en(chan, num, ints, language, audiofd, ctrlfd));
}

/*! \brief  ast_say_enumeration_full_en: English syntax */
/* This is the default syntax, if no other syntax defined in this file is used */
static int ast_say_enumeration_full_en(struct ast_channel *chan, int num, const char *ints, const char *language, int audiofd, int ctrlfd)
{
	int res = 0, t = 0;
	char fn[256] = "";
	
	while(!res && num) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus"); /* kind of senseless for enumerations, but our best effort for error checking */
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/h-%d", num);
			num = 0;
		} else if (num < 100) { 
			int tens = num / 10;
			num = num % 10;
			if (num == 0) {
				snprintf(fn, sizeof(fn), "digits/h-%d", (tens * 10));
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", (tens * 10));
			}
		} else if (num < 1000) {
			int hundreds = num / 100;
			num = num % 100;
			if (hundreds > 1 || t == 1) {
				res = ast_say_number_full_en(chan, hundreds, ints, language, audiofd, ctrlfd);
			}			
			if (res)
				return res;
			if (num) {
				snprintf(fn, sizeof(fn), "digits/hundred");
			} else {
				snprintf(fn, sizeof(fn), "digits/h-hundred");
			}
		} else if (num < 1000000) {
			int thousands = num / 1000;
			num = num % 1000;
			if (thousands > 1 || t == 1) {
				res = ast_say_number_full_en(chan, thousands, ints, language, audiofd, ctrlfd);
			}
			if (res)
				return res;
			if (num) {					
				snprintf(fn, sizeof(fn), "digits/thousand");
			} else {
				snprintf(fn, sizeof(fn), "digits/h-thousand");
			}
			t = 1;
		} else if (num < 1000000000) {
			int millions = num / 1000000;
			num = num % 1000000;
			t = 1;
			res = ast_say_number_full_en(chan, millions, ints, language, audiofd, ctrlfd);
			if (res)
				return res;
			if (num) {					
				snprintf(fn, sizeof(fn), "digits/million");
			} else {
				snprintf(fn, sizeof(fn), "digits/h-million");
			}
		} else if (num < INT_MAX) {
			int billions = num / 1000000000;
			num = num % 1000000000;
			t = 1;
			res = ast_say_number_full_en(chan, billions, ints, language, audiofd, ctrlfd);
			if (res)
				return res;
			if (num) {					
				snprintf(fn, sizeof(fn), "digits/billion");
			} else {
				snprintf(fn, sizeof(fn), "digits/h-billion");
			}
		} else if (num == INT_MAX) {
			snprintf(fn, sizeof(fn), "digits/h-last");
			num = 0;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}

		if (!res) {
			if (!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) {
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				} else {
					res = ast_waitstream(chan, ints);
				}
			}
			ast_stopstream(chan);
		}
	}
	return res;
}

/*! \brief  ast_say_enumeration_full_da: Danish syntax */
static int ast_say_enumeration_full_da(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	/* options can be: '' or 'm' male gender; 'f' female gender; 'n' neuter gender; 'p' plural */
	int res = 0, t = 0;
	char fn[256] = "", fna[256] = "";
	char *gender;

	if (options && !strncasecmp(options, "f",1)) {
		gender = "F";
	} else if (options && !strncasecmp(options, "n",1)) {
		gender = "N";
	} else {
		gender = "";
	}

	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && num) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus"); /* kind of senseless for enumerations, but our best effort for error checking */
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 100 && t) {
			snprintf(fn, sizeof(fn), "digits/and");
			t = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/h-%d%s", num, gender);
			num = 0;
		} else if (num < 100) {
			int ones = num % 10;
			if (ones) {
				snprintf(fn, sizeof(fn), "digits/%d-and", ones);
				num -= ones;
			} else {
				snprintf(fn, sizeof(fn), "digits/h-%d%s", num, gender);
				num = 0;
			}
		} else if (num == 100 && t == 0) {
			snprintf(fn, sizeof(fn), "digits/h-hundred%s", gender);
			num = 0;
		} else if (num < 1000) {
			int hundreds = num / 100;
			num = num % 100;
			if (hundreds == 1) {
				snprintf(fn, sizeof(fn), "digits/1N");
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", hundreds);
			}
			if (num) {					
				snprintf(fna, sizeof(fna), "digits/hundred");
			} else {
				snprintf(fna, sizeof(fna), "digits/h-hundred%s", gender);
			}
			t = 1;
		} else 	if (num < 1000000) {
			int thousands = num / 1000;
			num = num % 1000;
			if (thousands == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/thousand");
				} else {
					if (t) {
						snprintf(fn, sizeof(fn), "digits/1N");
						snprintf(fna, sizeof(fna), "digits/h-thousand%s", gender);
					} else {
						snprintf(fn, sizeof(fn), "digits/h-thousand%s", gender);
					}
				}
			} else {
				res = ast_say_number_full_de(chan, thousands, ints, language, options, audiofd, ctrlfd);
				if (res) {
					return res;
				}
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					snprintf(fn, sizeof(fn), "digits/h-thousand%s", gender);
				}
			}
			t = 1;
		} else if (num < 1000000000) {
			int millions = num / 1000000;
			num = num % 1000000;
			if (millions == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1F");
					snprintf(fna, sizeof(fna), "digits/million");
				} else {
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/h-million%s", gender);
				}
			} else {
				res = ast_say_number_full_de(chan, millions, ints, language, options, audiofd, ctrlfd);
				if (res) {
					return res;
				}
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/millions");
				} else {
					snprintf(fn, sizeof(fn), "digits/h-million%s", gender);
				}
			}
			t = 1;
		} else if (num < INT_MAX) {
			int billions = num / 1000000000;
			num = num % 1000000000;
			if (billions == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1F");
					snprintf(fna, sizeof(fna), "digits/milliard");
				} else {
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/h-milliard%s", gender);
				}
			} else {
				res = ast_say_number_full_de(chan, billions, ints, language, options, audiofd, ctrlfd);
				if (res)
					return res;
				if (num) {					
					snprintf(fn, sizeof(fna), "digits/milliards");
				} else {
					snprintf(fn, sizeof(fna), "digits/h-milliard%s", gender);
				}
			}
			t = 1;
		} else if (num == INT_MAX) {
			snprintf(fn, sizeof(fn), "digits/h-last%s", gender);
			num = 0;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}

		if (!res) {
			if (!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
			if (!res) {
				if (strlen(fna) != 0 && !ast_streamfile(chan, fna, language)) {
					if ((audiofd > -1) && (ctrlfd > -1)) {
						res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
					} else {
						res = ast_waitstream(chan, ints);
					}
				}
				ast_stopstream(chan);
				strcpy(fna, "");
			}
		}
	}
	return res;
}

/*! \brief  ast_say_enumeration_full_de: German syntax */
static int ast_say_enumeration_full_de(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	/* options can be: '' or 'm' male gender; 'f' female gender; 'n' neuter gender; 'p' plural */
	int res = 0, t = 0;
	char fn[256] = "", fna[256] = "";
	char *gender;

	if (options && !strncasecmp(options, "f",1)) {
		gender = "F";
	} else if (options && !strncasecmp(options, "n",1)) {
		gender = "N";
	} else {
		gender = "";
	}

	if (!num) 
		return ast_say_digits_full(chan, 0,ints, language, audiofd, ctrlfd);

	while(!res && num) {
		if (num < 0) {
			snprintf(fn, sizeof(fn), "digits/minus"); /* kind of senseless for enumerations, but our best effort for error checking */
			if ( num > INT_MIN ) {
				num = -num;
			} else {
				num = 0;
			}	
		} else if (num < 100 && t) {
			snprintf(fn, sizeof(fn), "digits/and");
			t = 0;
		} else if (num < 20) {
			snprintf(fn, sizeof(fn), "digits/h-%d%s", num, gender);
			num = 0;
		} else if (num < 100) {
			int ones = num % 10;
			if (ones) {
				snprintf(fn, sizeof(fn), "digits/%d-and", ones);
				num -= ones;
			} else {
				snprintf(fn, sizeof(fn), "digits/h-%d%s", num, gender);
				num = 0;
			}
		} else if (num == 100 && t == 0) {
			snprintf(fn, sizeof(fn), "digits/h-hundred%s", gender);
			num = 0;
		} else if (num < 1000) {
			int hundreds = num / 100;
			num = num % 100;
			if (hundreds == 1) {
				snprintf(fn, sizeof(fn), "digits/1N");
			} else {
				snprintf(fn, sizeof(fn), "digits/%d", hundreds);
			}
			if (num) {					
				snprintf(fna, sizeof(fna), "digits/hundred");
			} else {
				snprintf(fna, sizeof(fna), "digits/h-hundred%s", gender);
			}
			t = 1;
		} else 	if (num < 1000000) {
			int thousands = num / 1000;
			num = num % 1000;
			if (thousands == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/thousand");
				} else {
					if (t) {
						snprintf(fn, sizeof(fn), "digits/1N");
						snprintf(fna, sizeof(fna), "digits/h-thousand%s", gender);
					} else {
						snprintf(fn, sizeof(fn), "digits/h-thousand%s", gender);
					}
				}
			} else {
				res = ast_say_number_full_de(chan, thousands, ints, language, options, audiofd, ctrlfd);
				if (res) {
					return res;
				}
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/thousand");
				} else {
					snprintf(fn, sizeof(fn), "digits/h-thousand%s", gender);
				}
			}
			t = 1;
		} else if (num < 1000000000) {
			int millions = num / 1000000;
			num = num % 1000000;
			if (millions == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1F");
					snprintf(fna, sizeof(fna), "digits/million");
				} else {
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/h-million%s", gender);
				}
			} else {
				res = ast_say_number_full_de(chan, millions, ints, language, options, audiofd, ctrlfd);
				if (res) {
					return res;
				}
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/millions");
				} else {
					snprintf(fn, sizeof(fn), "digits/h-million%s", gender);
				}
			}
			t = 1;
		} else if (num < INT_MAX) {
			int billions = num / 1000000000;
			num = num % 1000000000;
			if (billions == 1) {
				if (num) {					
					snprintf(fn, sizeof(fn), "digits/1F");
					snprintf(fna, sizeof(fna), "digits/milliard");
				} else {
					snprintf(fn, sizeof(fn), "digits/1N");
					snprintf(fna, sizeof(fna), "digits/h-milliard%s", gender);
				}
			} else {
				res = ast_say_number_full_de(chan, billions, ints, language, options, audiofd, ctrlfd);
				if (res)
					return res;
				if (num) {					
					snprintf(fn, sizeof(fna), "digits/milliards");
				} else {
					snprintf(fn, sizeof(fna), "digits/h-milliard%s", gender);
				}
			}
			t = 1;
		} else if (num == INT_MAX) {
			snprintf(fn, sizeof(fn), "digits/h-last%s", gender);
			num = 0;
		} else {
			ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
			res = -1;
		}

		if (!res) {
			if (!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1)) 
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else  
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
			if (!res) {
				if (strlen(fna) != 0 && !ast_streamfile(chan, fna, language)) {
					if ((audiofd > -1) && (ctrlfd > -1)) {
						res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
					} else {
						res = ast_waitstream(chan, ints);
					}
				}
				ast_stopstream(chan);
				strcpy(fna, "");
			}
		}
	}
	return res;
}

static int say_date(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	if (!strcasecmp(lang, "en") ) {	/* English syntax */
		return(ast_say_date_en(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "da") ) {	/* Danish syntax */
		return(ast_say_date_da(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "de") ) {	/* German syntax */
		return(ast_say_date_de(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "fr") ) {	/* French syntax */
		return(ast_say_date_fr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "nl") ) {	/* Dutch syntax */
		return(ast_say_date_nl(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt") || !strcasecmp(lang, "pt_BR")) {	/* Portuguese syntax */
		return(ast_say_date_pt(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "gr") ) {  			/* Greek syntax */
		return(ast_say_date_gr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "ge") ) {  /* Georgian syntax */
		return(ast_say_date_ge(chan, t, ints, lang));
	}

	/* Default to English */
	return(ast_say_date_en(chan, t, ints, lang));
}

/* English syntax */
int ast_say_date_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char * ) NULL);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* Danish syntax */
int ast_say_date_da(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_enumeration(chan, tm.tm_mday, ints, lang, (char * ) NULL);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		/* Year */
		int year = tm.tm_year + 1900;
		if (year > 1999) {	/* year 2000 and later */
			res = ast_say_number(chan, year, ints, lang, (char *) NULL);	
		} else {
			if (year < 1100) {
				/* I'm not going to handle 1100 and prior */
				/* We'll just be silent on the year, instead of bombing out. */
			} else {
			    /* year 1100 to 1999. will anybody need this?!? */
				snprintf(fn,sizeof(fn), "digits/%d", (year / 100) );
				res = wait_file(chan, ints, fn, lang);
				if (!res) {
					res = wait_file(chan,ints, "digits/hundred", lang);
					if (!res && year % 100 != 0) {
						res = ast_say_number(chan, (year % 100), ints, lang, (char *) NULL);	
					}
				}
			}
		}
	}
	return res;
}

/* German syntax */
int ast_say_date_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_enumeration(chan, tm.tm_mday, ints, lang, (char * ) NULL);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		/* Year */
		int year = tm.tm_year + 1900;
		if (year > 1999) {	/* year 2000 and later */
			res = ast_say_number(chan, year, ints, lang, (char *) NULL);	
		} else {
			if (year < 1100) {
				/* I'm not going to handle 1100 and prior */
				/* We'll just be silent on the year, instead of bombing out. */
			} else {
			    /* year 1100 to 1999. will anybody need this?!? */
			    /* say 1967 as 'neunzehn hundert sieben und sechzig' */
				snprintf(fn,sizeof(fn), "digits/%d", (year / 100) );
				res = wait_file(chan, ints, fn, lang);
				if (!res) {
					res = wait_file(chan,ints, "digits/hundred", lang);
					if (!res && year % 100 != 0) {
						res = ast_say_number(chan, (year % 100), ints, lang, (char *) NULL);	
					}
				}
			}
		}
	}
	return res;
}

/* French syntax */
int ast_say_date_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char * ) NULL);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* Dutch syntax */
int ast_say_date_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char * ) NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* Portuguese syntax */
int ast_say_date_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
	if (!res)
		res = wait_file(chan, ints, fn, lang);
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
	if (!res)
		res = wait_file(chan, ints, "digits/pt-de", lang);
	snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
	if (!res)
		res = wait_file(chan, ints, fn, lang);
	if (!res)
		res = wait_file(chan, ints, "digits/pt-de", lang);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);

	return res;
}

static int say_date_with_format(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	if (!strcasecmp(lang, "en") ) {	/* English syntax */
		return(ast_say_date_with_format_en(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "da") ) {	/* Danish syntax */
		return(ast_say_date_with_format_da(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "de") ) {	/* German syntax */
		return(ast_say_date_with_format_de(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "es") || !strcasecmp(lang, "mx")) {	/* Spanish syntax */
		return(ast_say_date_with_format_es(chan, time, ints, lang, format, timezone));
 	} else if (!strcasecmp(lang, "he")) {	/* Hebrew syntax */
 		return(ast_say_date_with_format_he(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "fr") ) {	/* French syntax */
		return(ast_say_date_with_format_fr(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "it") ) {  /* Italian syntax */
		return(ast_say_date_with_format_it(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "nl") ) {	/* Dutch syntax */
		return(ast_say_date_with_format_nl(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "pl") ) {	/* Polish syntax */
		return(ast_say_date_with_format_pl(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "pt") || !strcasecmp(lang, "pt_BR")) {	/* Portuguese syntax */
		return(ast_say_date_with_format_pt(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "tw") || !strcasecmp(lang, "zh") ) {	/* Taiwanese / Chinese syntax */
		return(ast_say_date_with_format_tw(chan, time, ints, lang, format, timezone));
	} else if (!strcasecmp(lang, "gr") ) {	/* Greek syntax */
		return(ast_say_date_with_format_gr(chan, time, ints, lang, format, timezone));
	}

	/* Default to English */
	return(ast_say_date_with_format_en(chan, time, ints, lang, format, timezone));
}

/* English syntax */
int ast_say_date_with_format_en(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "ABdY 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* Month enumerated */
				res = ast_say_enumeration(chan, (tm.tm_mon + 1), ints, lang, (char *) NULL);	
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_enumeration(chan, tm.tm_mday, ints, lang, (char *) NULL);	
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 99) {
				        res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
				} else if (tm.tm_year < 1) {
					/* I'm not going to handle 1900 and prior */
					/* We'll just be silent on the year, instead of bombing out. */
				} else {
					res = wait_file(chan, ints, "digits/19", lang);
					if (!res) {
						if (tm.tm_year <= 9) {
							/* 1901 - 1909 */
							res = wait_file(chan,ints, "digits/oh", lang);
						}

						res |= ast_say_number(chan, tm.tm_year, ints, lang, (char *) NULL);
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				if (format[offset] == 'H') {
					/* e.g. oh-eight */
					if (tm.tm_hour < 10) {
						res = wait_file(chan,ints, "digits/oh",lang);
					}
				} else {
					/* e.g. eight */
					if (tm.tm_hour == 0) {
						res = wait_file(chan,ints, "digits/oh",lang);
					}
				}
				if (!res) {
					if (tm.tm_hour != 0) {
						int remainder = tm.tm_hour;
						if (tm.tm_hour > 20) {
							res = wait_file(chan,ints, "digits/20",lang);
							remainder -= 20;
						}
						if (!res) {
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", remainder);
							res = wait_file(chan,ints,nextmsg,lang);
						}
					}
				}
				break;
			case 'M':
			case 'N':
				/* Minute */
				if (tm.tm_min == 0) {
					if (format[offset] == 'M') {
						res = wait_file(chan, ints, "digits/oclock", lang);
					} else {
						res = wait_file(chan, ints, "digits/hundred", lang);
					}
				} else if (tm.tm_min < 10) {
					res = wait_file(chan,ints, "digits/oh",lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_min);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				} else {
					res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
				}
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "A", timezone);
					} else if (beg_today - 2628000 < time) {
						/* Less than a month ago - "Sunday, October third" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "ABd", timezone);
					} else if (beg_today - 15768000 < time) {
						/* Less than 6 months ago - "August seventh" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "Bd", timezone);
					} else {
						/* More than 6 months ago - "April nineteenth two thousand three" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "BdY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "A", timezone);
					} else if (beg_today - 2628000 < time) {
						/* Less than a month ago - "Sunday, October third" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "ABd", timezone);
					} else if (beg_today - 15768000 < time) {
						/* Less than 6 months ago - "August seventh" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "Bd", timezone);
					} else {
						/* More than 6 months ago - "April nineteenth two thousand three" */
						res = ast_say_date_with_format_en(chan, time, ints, lang, "BdY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_en(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				if (tm.tm_sec == 0) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else if (tm.tm_sec < 10) {
					res = wait_file(chan,ints, "digits/oh",lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				} else {
					res = ast_say_number(chan, tm.tm_sec, ints, lang, (char *) NULL);
				}
				break;
			case 'T':
				res = ast_say_date_with_format_en(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* Danish syntax */
int ast_say_date_with_format_da(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (!format)
		format = "A dBY HMS";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* Month enumerated */
				res = ast_say_enumeration(chan, (tm.tm_mon + 1), ints, lang, "m");	
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_enumeration(chan, tm.tm_mday, ints, lang, "m");	
				break;
			case 'Y':
				/* Year */
				{
					int year = tm.tm_year + 1900;
					if (year > 1999) {	/* year 2000 and later */
						res = ast_say_number(chan, year, ints, lang, (char *) NULL);	
					} else {
						if (year < 1100) {
							/* I'm not going to handle 1100 and prior */
							/* We'll just be silent on the year, instead of bombing out. */
						} else {
						    /* year 1100 to 1999. will anybody need this?!? */
						    /* say 1967 as 'nineteen hundred seven and sixty' */
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", (year / 100) );
							res = wait_file(chan,ints,nextmsg,lang);
							if (!res) {
								res = wait_file(chan,ints, "digits/hundred",lang);
								if (!res && year % 100 != 0) {
									res = ast_say_number(chan, (year % 100), ints, lang, (char *) NULL);	
								}
							}
						}
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				res = wait_file(chan,ints,"digits/oclock",lang);
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				if (!res) {
					res = wait_file(chan,ints,nextmsg,lang);
				}
				break;
			case 'H':
				/* 24-Hour, single digit hours preceeded by "oh" (0) */
				if (tm.tm_hour < 10 && tm.tm_hour > 0) {
					res = wait_file(chan,ints, "digits/0",lang);
				}
				/* FALLTRHU */
			case 'k':
				/* 24-Hour */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, (char *) NULL);	
				break;
			case 'M':
				/* Minute */
				if (tm.tm_min > 0 || format[offset+ 1 ] == 'S' ) { /* zero 'digits/0' only if seconds follow (kind of a hack) */
					res = ast_say_number(chan, tm.tm_min, ints, lang, "f");	
				}
				if ( !res && format[offset + 1] == 'S' ) { /* minutes only if seconds follow (kind of a hack) */
					if (tm.tm_min == 1) {
						res = wait_file(chan,ints,"digits/minute",lang);
					} else {
						res = wait_file(chan,ints,"digits/minutes",lang);
					}
				}
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_da(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_da(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_da(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_da(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				res = wait_file(chan,ints, "digits/and",lang);
				if (!res) {
					res = ast_say_number(chan, tm.tm_sec, ints, lang, "f");	
					if (!res) {
						res = wait_file(chan,ints, "digits/seconds",lang);
					}
				}
				break;
			case 'T':
				res = ast_say_date_with_format_da(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* German syntax */
int ast_say_date_with_format_de(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (!format)
		format = "A dBY HMS";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* Month enumerated */
				res = ast_say_enumeration(chan, (tm.tm_mon + 1), ints, lang, "m");	
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_enumeration(chan, tm.tm_mday, ints, lang, "m");	
				break;
			case 'Y':
				/* Year */
				{
					int year = tm.tm_year + 1900;
					if (year > 1999) {	/* year 2000 and later */
						res = ast_say_number(chan, year, ints, lang, (char *) NULL);	
					} else {
						if (year < 1100) {
							/* I'm not going to handle 1100 and prior */
							/* We'll just be silent on the year, instead of bombing out. */
						} else {
						    /* year 1100 to 1999. will anybody need this?!? */
						    /* say 1967 as 'neunzehn hundert sieben und sechzig' */
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", (year / 100) );
							res = wait_file(chan,ints,nextmsg,lang);
							if (!res) {
								res = wait_file(chan,ints, "digits/hundred",lang);
								if (!res && year % 100 != 0) {
									res = ast_say_number(chan, (year % 100), ints, lang, (char *) NULL);	
								}
							}
						}
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				if (!res) {
					res = wait_file(chan,ints,"digits/oclock",lang);
				}
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, (char *) NULL);	
				if (!res) {
					res = wait_file(chan,ints,"digits/oclock",lang);
				}
				break;
			case 'M':
				/* Minute */
				if (tm.tm_min > 0 || format[offset+ 1 ] == 'S' ) { /* zero 'digits/0' only if seconds follow (kind of a hack) */
					res = ast_say_number(chan, tm.tm_min, ints, lang, "f");	
				}
				if ( !res && format[offset + 1] == 'S' ) { /* minutes only if seconds follow (kind of a hack) */
					if (tm.tm_min == 1) {
						res = wait_file(chan,ints,"digits/minute",lang);
					} else {
						res = wait_file(chan,ints,"digits/minutes",lang);
					}
				}
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_de(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = now.tv_sec - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_de(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_de(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_de(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				res = wait_file(chan,ints, "digits/and",lang);
				if (!res) {
					res = ast_say_number(chan, tm.tm_sec, ints, lang, "f");	
					if (!res) {
						res = wait_file(chan,ints, "digits/seconds",lang);
					}
				}
				break;
			case 'T':
				res = ast_say_date_with_format_de(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* TODO: this probably is not the correct format for doxygen remarks */

/** ast_say_date_with_format_he Say formmated date in Hebrew
 *
 * \ref ast_say_date_with_format_en for the details of the options 
 *
 * Changes from the English version: 
 *
 * * don't replicate in here the logic of ast_say_number_full_he
 *
 * * year is always 4-digit (because it's simpler)
 *
 * * added c, x, and X. Mainly for my tests
 *
 * * The standard "long" format used in Hebrew is AdBY, rather than ABdY
 *
 * TODO: 
 * * A "ha" is missing in the standard date format, before the 'd'.
 * * The numbers of 3000--19000 are not handled well
 **/
#define IL_DATE_STR "AdBY"
#define IL_TIME_STR "IMp"
#define IL_DATE_STR_FULL IL_DATE_STR " 'digits/at' " IL_TIME_STR
int ast_say_date_with_format_he(struct ast_channel *chan, time_t time, 
    const char *ints, const char *lang, const char *format, 
    const char *timezone)
{
	/* TODO: This whole function is cut&paste from 
	 * ast_say_date_with_format_en . Is that considered acceptable?
	 **/
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (!format)
		format = IL_DATE_STR_FULL;

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e': /* Day of the month */
                                /* I'm not sure exactly what the parameters 
                                 * audiofd and ctrlfd to 
                                 * ast_say_number_full_he mean, but it seems
                                 * safe to pass -1 there. 
                                 *
                                 * At least in one of the pathes :-( 
                                 */
				res = ast_say_number_full_he(chan, tm.tm_mday,
					ints, lang, "m", -1, -1
				);
				break;
			case 'Y': /* Year */
				res = ast_say_number_full_he(chan, tm.tm_year+1900,
					ints, lang, "f", -1, -1
				);
				break;
			case 'I':
			case 'l': /* 12-Hour */
				{
					int hour = tm.tm_hour;
					hour = hour%12;
					if (hour == 0) hour=12;
				
					res = ast_say_number_full_he(chan, hour,
						ints, lang, "f", -1, -1
					);
				}
				break;
			case 'H':
			case 'k': /* 24-Hour */
				/* With 'H' there is an 'oh' after a single-
				 * digit hour */
				if ((format[offset] == 'H') && 
				    (tm.tm_hour <10)&&(tm.tm_hour>0)
				) { /* e.g. oh-eight */
					res = wait_file(chan,ints, "digits/oh",lang);
				}
				
				res = ast_say_number_full_he(chan, tm.tm_hour,
					ints, lang, "f", -1, -1
				);
				break;
			case 'M': /* Minute */
				res = ast_say_number_full_he(chan, tm.tm_min, 
					ints, lang,"f", -1, -1
				);
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or "date" */
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A 
                                 * (weekday), or "date" */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;
					char todo = format[offset]; /* The letter to format*/

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						if (todo == 'Q') {
							res = wait_file(chan,
									ints, 
									"digits/today",
									lang);
						}
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if ((todo != 'Q') &&
						(beg_today - 86400 * 6 < time))
					{
						/* Within the last week */
						res = ast_say_date_with_format_he(chan,
										  time, ints, lang, 
										  "A", timezone);
					} else {
						res = ast_say_date_with_format_he(chan,
										  time, ints, lang, 
										  IL_DATE_STR, timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_he(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S': /* Seconds */
				res = ast_say_number_full_he(chan, tm.tm_sec,
					ints, lang, "f", -1, -1
				);
				break;
			case 'T':
				res = ast_say_date_with_format_he(chan, time, ints, lang, "HMS", timezone);
				break;
			/* c, x, and X seem useful for testing. Not sure
                         * if thiey're good for the general public */
			case 'c':
				res = ast_say_date_with_format_he(chan, time, 
                                    ints, lang, IL_DATE_STR_FULL, timezone);
				break;
			case 'x':
				res = ast_say_date_with_format_he(chan, time, 
                                    ints, lang, IL_DATE_STR, timezone);
				break;
			case 'X': /* Currently not locale-dependent...*/
				res = ast_say_date_with_format_he(chan, time, 
                                    ints, lang, IL_TIME_STR, timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}


/* Spanish syntax */
int ast_say_date_with_format_es(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "'digits/es-el' Ad 'digits/es-de' B 'digits/es-de' Y 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				snprintf(nextmsg,sizeof(nextmsg), "%s", sndfile);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
				break;
			case 'Y':
				/* Year */
				res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, NULL);
				break;
			case 'M':
				/* Minute */
				res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);	
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 18)
					res = wait_file(chan, ints, "digits/p-m", lang);
				else if (tm.tm_hour > 12)
					res = wait_file(chan, ints, "digits/afternoon", lang);
				else if (tm.tm_hour)
					res = wait_file(chan, ints, "digits/a-m", lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_es(chan, time, ints, lang, "'digits/es-el' Ad 'digits/es-de' B 'digits/es-de' Y", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_es(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_es(chan, time, ints, lang, "'digits/es-el' Ad 'digits/es-de' B 'digits/es-de' Y", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_es(chan, time, ints, lang, "H 'digits/y' M", timezone);
				break;
			case 'S':
				/* Seconds */
				if (tm.tm_sec == 0) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else if (tm.tm_sec < 10) {
					res = wait_file(chan,ints, "digits/oh",lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				} else if ((tm.tm_sec < 21) || (tm.tm_sec % 10 == 0)) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					int ten, one;
					ten = (tm.tm_sec / 10) * 10;
					one = (tm.tm_sec % 10);
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", ten);
					res = wait_file(chan,ints,nextmsg,lang);
					if (!res) {
						/* Fifty, not fifty-zero */
						if (one != 0) {
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", one);
							res = wait_file(chan,ints,nextmsg,lang);
						}
					}
				}
				break;
			case 'T':
				res = ast_say_date_with_format_es(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* French syntax 
oclock = heure
*/
int ast_say_date_with_format_fr(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "AdBY 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e':
				/* First */
				if (tm.tm_mday == 1) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mday);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					res = ast_say_number(chan, tm.tm_mday, ints, lang, (char * ) NULL);
				}
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 99) {
					res = wait_file(chan,ints, "digits/2",lang);
					if (!res) {
						res = wait_file(chan,ints, "digits/thousand",lang);
					}
					if (tm.tm_year > 100) {
						if (!res) {
							res = ast_say_number(chan, tm.tm_year - 100, ints, lang, (char * ) NULL);
						}
					}
				} else {
					if (tm.tm_year < 1) {
						/* I'm not going to handle 1900 and prior */
						/* We'll just be silent on the year, instead of bombing out. */
					} else {
						res = wait_file(chan,ints, "digits/thousand",lang);
						if (!res) {
							wait_file(chan,ints, "digits/9",lang);
							wait_file(chan,ints, "digits/hundred",lang);
							res = ast_say_number(chan, tm.tm_year, ints, lang, (char * ) NULL);
						}
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				if (!res)
					res = wait_file(chan,ints, "digits/oclock",lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, (char * ) NULL);
				if (!res)
					res = wait_file(chan,ints, "digits/oclock",lang);
				break;
			case 'M':
				/* Minute */
				if (tm.tm_min == 0) {
					break;
				}
				res = ast_say_number(chan, tm.tm_min, ints, lang, (char * ) NULL);
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_fr(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or AdBY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_fr(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_fr(chan, time, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_fr(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, (char * ) NULL);
				if (!res) {
					res = wait_file(chan,ints, "digits/second",lang);
				}
				break;
			case 'T':
				res = ast_say_date_with_format_fr(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

int ast_say_date_with_format_it(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "AdB 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
				sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e':
				/* First day of the month is spelled as ordinal */
				if (tm.tm_mday == 1) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mday);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					if (!res) {
						res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
					}
				}
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 99) {
					res = wait_file(chan,ints, "digits/ore-2000",lang);
					if (tm.tm_year > 100) {
						if (!res) {
						/* This works until the end of 2021 */
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year - 100);
						res = wait_file(chan,ints,nextmsg,lang);
						}
					}
				} else {
					if (tm.tm_year < 1) {
						/* I'm not going to handle 1900 and prior */
						/* We'll just be silent on the year, instead of bombing out. */
					} else {
						res = wait_file(chan,ints, "digits/ore-1900",lang);
						if ((!res) && (tm.tm_year != 0)) {
							if (tm.tm_year <= 21) {
								/* 1910 - 1921 */
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year);
								res = wait_file(chan,ints,nextmsg,lang);
							} else {
								/* 1922 - 1999, but sounds badly in 1928, 1931, 1938, etc... */
								int ten, one;
								ten = tm.tm_year / 10;
								one = tm.tm_year % 10;
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", ten * 10);
								res = wait_file(chan,ints,nextmsg,lang);
								if (!res) {
									if (one != 0) {
										snprintf(nextmsg,sizeof(nextmsg), "digits/%d", one);
										res = wait_file(chan,ints,nextmsg,lang);
									}
								}
							}
						}
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
					res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				if (tm.tm_hour == 0) {
					res = wait_file(chan,ints, "digits/ore-mezzanotte",lang);
				} else if (tm.tm_hour == 1) {
					res = wait_file(chan,ints, "digits/ore-una",lang);
				} else {
					res = ast_say_number(chan, tm.tm_hour, ints, lang, (char *) NULL);
				}
				break;
			case 'M':
				/* Minute */
				res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
					res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;
	
					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_it(chan, time, ints, lang, "AdB", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;
	
					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_it(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_it(chan, time, ints, lang, "AdB", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_it(chan, time, ints, lang, "HM", timezone);
	        	break;
			case 'S':
				/* Seconds */
				if (tm.tm_sec == 0) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else if (tm.tm_sec < 10) {
					res = wait_file(chan,ints, "digits/oh",lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				} else if ((tm.tm_sec < 21) || (tm.tm_sec % 10 == 0)) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					int ten, one;
					ten = (tm.tm_sec / 10) * 10;
					one = (tm.tm_sec % 10);
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", ten);
					res = wait_file(chan,ints,nextmsg,lang);
					if (!res) {
						/* Fifty, not fifty-zero */
						if (one != 0) {
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", one);
							res = wait_file(chan,ints,nextmsg,lang);
						}
					}
				}
		        break;
			case 'T':
				res = ast_say_date_with_format_it(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* Dutch syntax */
int ast_say_date_with_format_nl(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "ABdY 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_number(chan, tm.tm_mday, ints, lang, NULL);
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 99) {
					res = wait_file(chan,ints, "digits/2",lang);
					if (!res) {
						res = wait_file(chan,ints, "digits/thousand",lang);
					}
					if (tm.tm_year > 100) {
						if (!res) {
							/* This works until the end of 2020 */
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year - 100);
							res = wait_file(chan,ints,nextmsg,lang);
						}
					}
				} else {
					if (tm.tm_year < 1) {
						/* I'm not going to handle 1900 and prior */
						/* We'll just be silent on the year, instead of bombing out. */
					} else {
						res = wait_file(chan,ints, "digits/19",lang);
						if (!res) {
							if (tm.tm_year <= 9) {
								/* 1901 - 1909 */
								res = wait_file(chan,ints, "digits/oh",lang);
								if (!res) {
									snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year);
									res = wait_file(chan,ints,nextmsg,lang);
								}
							} else if (tm.tm_year <= 20) {
								/* 1910 - 1920 */
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year);
								res = wait_file(chan,ints,nextmsg,lang);
							} else {
								/* 1921 - 1999 */
								int ten, one;
								ten = tm.tm_year / 10;
								one = tm.tm_year % 10;
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", ten * 10);
								res = wait_file(chan,ints,nextmsg,lang);
								if (!res) {
									if (one != 0) {
										snprintf(nextmsg,sizeof(nextmsg), "digits/%d", one);
										res = wait_file(chan,ints,nextmsg,lang);
									}
								}
							}
						}
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				res = ast_say_number(chan, tm.tm_hour, ints, lang, (char *) NULL);
				if (!res) {
					res = wait_file(chan,ints, "digits/nl-uur",lang);
				}
				break;
			case 'M':
				/* Minute */
				res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_nl(chan, time, ints, lang, "ABdY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_nl(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_nl(chan, time, ints, lang, "ABdY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_nl(chan, time, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				res = ast_say_number(chan, tm.tm_sec, ints, lang, (char *) NULL);
				break;
			case 'T':
				res = ast_say_date_with_format_nl(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* Polish syntax */
int ast_say_date_with_format_pl(struct ast_channel *chan, time_t thetime, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	ast_localtime(&thetime, &tm, timezone);

	for (offset = 0 ; format[offset] != '\0' ; offset++) {
		int remainder;
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset = 0;
				for (sndoffset = 0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan, ints, sndfile, lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg, sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan, ints, nextmsg, lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg, sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan, ints, nextmsg, lang);
				break;
			case 'm':
				/* Month enumerated */
				res = ast_say_enumeration(chan, (tm.tm_mon + 1), ints, lang, NULL);
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				remainder = tm.tm_mday;
				if (tm.tm_mday > 30) {
					res = wait_file(chan, ints, "digits/h-30", lang);
					remainder -= 30;
				}
				if (tm.tm_mday > 20 && tm.tm_mday < 30) {
					res = wait_file(chan, ints, "digits/h-20", lang);
					remainder -= 20;
				}
				if (!res) {
					snprintf(nextmsg, sizeof(nextmsg), "digits/h-%d", remainder);
					res = wait_file(chan, ints, nextmsg, lang);
				}
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 100) {
					res = wait_file(chan, ints, "digits/2", lang);
					if (!res)
						res = wait_file(chan, ints, "digits/1000.2",lang);
					if (tm.tm_year > 100) {
						if (!res)
							res = ast_say_enumeration(chan, tm.tm_year - 100, ints, lang, NULL);
					}
				} else if (tm.tm_year == 100) {
					res = wait_file(chan, ints, "digits/h-2000", lang);
				} else {
					if (tm.tm_year < 1) {
						/* I'm not going to handle 1900 and prior */
						/* We'll just be silent on the year, instead of bombing out. */
						break;
					} else {
						res = wait_file(chan, ints, "digits/1000", lang);
						if (!res) {
							wait_file(chan, ints, "digits/900", lang);
							res = ast_say_enumeration(chan, tm.tm_year, ints, lang, NULL);
						}
					}
				}
				if (!res)
					wait_file(chan, ints, "digits/year", lang);
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg, sizeof(nextmsg), "digits/t-12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg, sizeof(nextmsg), "digits/t-%d", tm.tm_hour - 12);
				else 
					snprintf(nextmsg, sizeof(nextmsg), "digits/t-%d", tm.tm_hour);

				res = wait_file(chan, ints, nextmsg, lang);
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				if (tm.tm_hour != 0) {
					snprintf(nextmsg, sizeof(nextmsg), "digits/t-%d", tm.tm_hour);
					res = wait_file(chan, ints, nextmsg, lang);
				} else 
					res = wait_file(chan, ints, "digits/t-24", lang);
				break;
			case 'M':
			case 'N':
				/* Minute */
				if (tm.tm_min == 0) {
					if (format[offset] == 'M') {
						res = wait_file(chan, ints, "digits/oclock", lang);
					} else {
						res = wait_file(chan, ints, "digits/100", lang);
					}
				} else
					res = ast_say_number(chan, tm.tm_min, ints, lang, "f"); 
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg, sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg, sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan, ints, nextmsg, lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or AdBY */
				{
					time_t tv_sec = time(NULL);
					struct tm tmnow;
					time_t beg_today;

					ast_localtime(&tv_sec,&tmnow, timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tv_sec - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < thetime) {
						/* Today */
						res = wait_file(chan, ints, "digits/today", lang);
					} else if (beg_today - 86400 < thetime) {
						/* Yesterday */
						res = wait_file(chan, ints, "digits/yesterday", lang);
					} else {
						res = ast_say_date_with_format(chan, thetime, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or AdBY */
				{
					time_t tv_sec = time(NULL);
					struct tm tmnow;
					time_t beg_today;

					ast_localtime(&tv_sec, &tmnow, timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tv_sec - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < thetime) {
						/* Today */
					} else if ((beg_today - 86400) < thetime) {
						/* Yesterday */
						res = wait_file(chan, ints, "digits/yesterday", lang);
					} else if (beg_today - 86400 * 6 < thetime) {
						/* Within the last week */
						res = ast_say_date_with_format(chan, thetime, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format(chan, thetime, ints, lang, "AdBY", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format(chan, thetime, ints, lang, "HM", timezone);
				break;
			case 'S':
				/* Seconds */
				res = wait_file(chan, ints, "digits/and", lang);
				if (!res) {
					if (tm.tm_sec == 1) {
						res = wait_file(chan, ints, "digits/1z", lang);
						if (!res)
							res = wait_file(chan, ints, "digits/second-a", lang);
					} else {
						res = ast_say_number(chan, tm.tm_min, ints, lang, "f");
						if (!res) {
							int ten, one;
							ten = tm.tm_sec / 10;
							one = tm.tm_sec % 10;
							
							if (one > 1 && one < 5 && ten != 1)
								res = wait_file(chan,ints, "digits/seconds",lang);
							else
								res = wait_file(chan,ints, "digits/second",lang);
						}
					}
				}
				break;
			case 'T':
				res = ast_say_date_with_format(chan, thetime, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res)
			break;
	}
	return res;
}

/* Portuguese syntax */
int ast_say_date_with_format_pt(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "Ad 'digits/pt-de' B 'digits/pt-de' Y 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				snprintf(nextmsg,sizeof(nextmsg), "%s", sndfile);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				if (!strcasecmp(lang, "pt_BR")) {
					res = ast_say_number(chan, tm.tm_mon+1, ints, lang, (char *) NULL);
				} else {
					snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
					res = wait_file(chan,ints,nextmsg,lang);
				}
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
				break;
			case 'Y':
				/* Year */
				res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (!strcasecmp(lang, "pt_BR")) {
					if (tm.tm_hour == 0) {
						if (format[offset] == 'I')
							res = wait_file(chan, ints, "digits/pt-a", lang);
						if (!res)
							res = wait_file(chan, ints, "digits/pt-meianoite", lang);
					} else if (tm.tm_hour == 12) {
						if (format[offset] == 'I')
							res = wait_file(chan, ints, "digits/pt-ao", lang);
						if (!res)
							res = wait_file(chan, ints, "digits/pt-meiodia", lang);
						} else {
						if (format[offset] == 'I') {
							if ((tm.tm_hour % 12) != 1)
								res = wait_file(chan, ints, "digits/pt-as", lang);
							else
								res = wait_file(chan, ints, "digits/pt-a", lang);
						}
						if (!res)
							res = ast_say_number(chan, (tm.tm_hour % 12), ints, lang, "f");
						if ((!res) && (format[offset] == 'I'))
						res = ast_say_date_with_format(chan, time, ints, lang, "P", timezone);
					}
				} else {
					if (tm.tm_hour == 0) {
						if (format[offset] == 'I')
							res = wait_file(chan, ints, "digits/pt-ah", lang);
						if (!res)
							res = wait_file(chan, ints, "digits/pt-meianoite", lang);
						}
					else if (tm.tm_hour == 12) {
						if (format[offset] == 'I')
							res = wait_file(chan, ints, "digits/pt-ao", lang);
						if (!res)
							res = wait_file(chan, ints, "digits/pt-meiodia", lang);
					}
					else {
						if (format[offset] == 'I') {
							res = wait_file(chan, ints, "digits/pt-ah", lang);
							if ((tm.tm_hour % 12) != 1)
								if (!res)
									res = wait_file(chan, ints, "digits/pt-sss", lang);
						}
						if (!res)
							res = ast_say_number(chan, (tm.tm_hour % 12), ints, lang, "f");
					}
				}
				break;
			case 'H':
			case 'k':
				/* 24-Hour */
				if (!strcasecmp(lang, "pt_BR")) {
					res = ast_say_number(chan, tm.tm_hour, ints, lang, "f");
					if ((!res) && (format[offset] == 'H')) {
						if (tm.tm_hour > 1) {
							res = wait_file(chan,ints,"digits/hours",lang);
						} else {
							res = wait_file(chan,ints,"digits/hour",lang);
						}
					}
				} else {
					res = ast_say_number(chan, -tm.tm_hour, ints, lang, NULL);
					if (!res) {
						if (tm.tm_hour != 0) {
							int remainder = tm.tm_hour;
							if (tm.tm_hour > 20) {
								res = wait_file(chan,ints, "digits/20",lang);
								remainder -= 20;
							}
							if (!res) {
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", remainder);
								res = wait_file(chan,ints,nextmsg,lang);
							}						
						}
					}
				}
				break;
			case 'M':
				/* Minute */
				if (!strcasecmp(lang, "pt_BR")) {
					res = ast_say_number(chan, tm.tm_min, ints, lang, NULL);
					if (!res) {
						if (tm.tm_min > 1) {
							res = wait_file(chan,ints,"digits/minutes",lang);
						} else {
							res = wait_file(chan,ints,"digits/minute",lang);
						}
					}
				} else {
					if (tm.tm_min == 0) {
						res = wait_file(chan, ints, "digits/pt-hora", lang);
						if (tm.tm_hour != 1)
							if (!res)
								res = wait_file(chan, ints, "digits/pt-sss", lang);			} else {
						res = wait_file(chan,ints,"digits/pt-e",lang);
						if (!res)
							res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);	
					}
				}
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (!strcasecmp(lang, "pt_BR")) {
					if ((tm.tm_hour != 0) && (tm.tm_hour != 12)) {
						res = wait_file(chan, ints, "digits/pt-da", lang);
						if (!res) {
							if ((tm.tm_hour >= 0) && (tm.tm_hour < 12))
								res = wait_file(chan, ints, "digits/morning", lang);
							else if ((tm.tm_hour >= 12) && (tm.tm_hour < 18))
								res = wait_file(chan, ints, "digits/afternoon", lang);
							else res = wait_file(chan, ints, "digits/night", lang);
						}
					}
				} else {
					if (tm.tm_hour > 12)
						res = wait_file(chan, ints, "digits/p-m", lang);
					else if (tm.tm_hour  && tm.tm_hour < 12)
						res = wait_file(chan, ints, "digits/a-m", lang);
				}
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_pt(chan, time, ints, lang, "Ad 'digits/pt-de' B 'digits/pt-de' Y", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_pt(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_pt(chan, time, ints, lang, "Ad 'digits/pt-de' B 'digits/pt-de' Y", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_pt(chan, time, ints, lang, "H 'digits/pt-e' M", timezone);
				break;
			case 'S':
				/* Seconds */
				if (!strcasecmp(lang, "pt_BR")) {
					res = ast_say_number(chan, tm.tm_sec, ints, lang, NULL);
					if (!res) {
						if (tm.tm_sec > 1) {
							res = wait_file(chan,ints,"digits/seconds",lang);
						} else {
							res = wait_file(chan,ints,"digits/second",lang);
						}
					} else if (tm.tm_sec < 10) {
						res = wait_file(chan,ints, "digits/oh",lang);
						if (!res) {
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
							res = wait_file(chan,ints,nextmsg,lang);
						}
					} else if ((tm.tm_sec < 21) || (tm.tm_sec % 10 == 0)) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
						res = wait_file(chan,ints,nextmsg,lang);
					} else {
						int ten, one;
						ten = (tm.tm_sec / 10) * 10;
						one = (tm.tm_sec % 10);
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", ten);
						res = wait_file(chan,ints,nextmsg,lang);
						if (!res) {
							/* Fifty, not fifty-zero */
							if (one != 0) {
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", one);
								res = wait_file(chan,ints,nextmsg,lang);
							}
						}						
					}
				}
				break;
			case 'T':
				res = ast_say_date_with_format_pt(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
				break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

/* Taiwanese / Chinese syntax */
int ast_say_date_with_format_tw(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (format == NULL)
		format = "YBdAkM";

	ast_localtime(&time,&tm,timezone);

	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
			case '\'':
				/* Literal name of a sound file */
				sndoffset=0;
				for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
					sndfile[sndoffset] = format[offset];
				sndfile[sndoffset] = '\0';
				res = wait_file(chan,ints,sndfile,lang);
				break;
			case 'A':
			case 'a':
				/* Sunday - Saturday */
				snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'B':
			case 'b':
			case 'h':
				/* January - December */
				snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'm':
				/* First - Twelfth */
				snprintf(nextmsg,sizeof(nextmsg), "digits/h-%d", tm.tm_mon +1);
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'd':
			case 'e':
				/* First - Thirtyfirst */
				if (!(tm.tm_mday % 10) || (tm.tm_mday < 10)) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_mday);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_mday - (tm.tm_mday % 10));
					res = wait_file(chan,ints,nextmsg,lang);
					if(!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_mday % 10);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				}
                if(!res) res = wait_file(chan,ints,"ri",lang);
				break;
			case 'Y':
				/* Year */
				if (tm.tm_year > 99) {
					res = wait_file(chan,ints, "digits/2",lang);
					if (!res) {
						res = wait_file(chan,ints, "digits/thousand",lang);
					}
					if (tm.tm_year > 100) {
						if (!res) {
							snprintf(nextmsg,sizeof(nextmsg), "digits/%d", (tm.tm_year - 100) / 10);
							res = wait_file(chan,ints,nextmsg,lang);
							if (!res) {
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", (tm.tm_year - 100) % 10);
								res = wait_file(chan,ints,nextmsg,lang);
							}
						}
					}
					if (!res) {
						res = wait_file(chan,ints, "digits/year",lang);
					}
				} else {
					if (tm.tm_year < 1) {
						/* I'm not going to handle 1900 and prior */
						/* We'll just be silent on the year, instead of bombing out. */
					} else {
						res = wait_file(chan,ints, "digits/1",lang);
						if (!res) {
							res = wait_file(chan,ints, "digits/9",lang);
						}
						if (!res) {
							if (tm.tm_year <= 9) {
								/* 1901 - 1909 */
								res = wait_file(chan,ints, "digits/0",lang);
								if (!res) {
									snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year);
									res = wait_file(chan,ints,nextmsg,lang);
								}
							} else {
								/* 1910 - 1999 */
								snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year / 10);
								res = wait_file(chan,ints,nextmsg,lang);
								if (!res) {
									snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_year % 10);
									res = wait_file(chan,ints,nextmsg,lang);
								}
							}
						}
					}
					if (!res) {
						res = wait_file(chan,ints, "digits/year",lang);
					}
				}
				break;
			case 'I':
			case 'l':
				/* 12-Hour */
				if (tm.tm_hour == 0)
					snprintf(nextmsg,sizeof(nextmsg), "digits/12");
				else if (tm.tm_hour > 12)
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - 12);
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
				res = wait_file(chan,ints,nextmsg,lang);
				if (!res) {
					res = wait_file(chan,ints, "digits/oclock",lang);
				}
				break;
			case 'H':
                if (tm.tm_hour < 10) {
                    res = wait_file(chan, ints, "digits/0", lang);
                }
			case 'k':
				/* 24-Hour */
				if (!(tm.tm_hour % 10) || tm.tm_hour < 10) {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour - (tm.tm_hour % 10));
					res = wait_file(chan,ints,nextmsg,lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_hour % 10);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				}
				if (!res) {
					res = wait_file(chan,ints, "digits/oclock",lang);
				}
				break;
			case 'M':
				/* Minute */
				if (!(tm.tm_min % 10) || tm.tm_min < 10) {
					if (tm.tm_min < 10) {
						res = wait_file(chan, ints, "digits/0", lang);
					}
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_min);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_min - (tm.tm_min % 10));
					res = wait_file(chan,ints,nextmsg,lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_min % 10);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				}
				if (!res) {
					res = wait_file(chan,ints, "digits/minute",lang);
				}
				break;
			case 'P':
			case 'p':
				/* AM/PM */
				if (tm.tm_hour > 11)
					snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
				else
					snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
				res = wait_file(chan,ints,nextmsg,lang);
				break;
			case 'Q':
				/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
						res = wait_file(chan,ints, "digits/today",lang);
					} else if (beg_today - 86400 < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else {
						res = ast_say_date_with_format_tw(chan, time, ints, lang, "YBdA", timezone);
					}
				}
				break;
			case 'q':
				/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
				{
					struct timeval now;
					struct tm tmnow;
					time_t beg_today, tt;

					gettimeofday(&now,NULL);
					tt = now.tv_sec;
					ast_localtime(&tt,&tmnow,timezone);
					/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
					/* In any case, it saves not having to do ast_mktime() */
					beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
					if (beg_today < time) {
						/* Today */
					} else if ((beg_today - 86400) < time) {
						/* Yesterday */
						res = wait_file(chan,ints, "digits/yesterday",lang);
					} else if (beg_today - 86400 * 6 < time) {
						/* Within the last week */
						res = ast_say_date_with_format_tw(chan, time, ints, lang, "A", timezone);
					} else {
						res = ast_say_date_with_format_tw(chan, time, ints, lang, "YBdA", timezone);
					}
				}
				break;
			case 'R':
				res = ast_say_date_with_format_tw(chan, time, ints, lang, "kM", timezone);
				break;
			case 'S':
				/* Seconds */
				if (!(tm.tm_sec % 10) || tm.tm_sec < 10) {
					if (tm.tm_sec < 10) {
						res = wait_file(chan, ints, "digits/0", lang);
					}
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec);
					res = wait_file(chan,ints,nextmsg,lang);
				} else {
					snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec - (tm.tm_sec % 10));
					res = wait_file(chan,ints,nextmsg,lang);
					if (!res) {
						snprintf(nextmsg,sizeof(nextmsg), "digits/%d", tm.tm_sec % 10);
						res = wait_file(chan,ints,nextmsg,lang);
					}
				}
				if (!res) {
					res = wait_file(chan,ints, "digits/second",lang);
				}
				break;
			case 'T':
				res = ast_say_date_with_format_tw(chan, time, ints, lang, "HMS", timezone);
				break;
			case ' ':
			case '	':
				/* Just ignore spaces and tabs */
			break;
			default:
				/* Unknown character */
				ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}

static int say_time(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	if (!strcasecmp(lang, "en") ) {	/* English syntax */
		return(ast_say_time_en(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "de") ) {	/* German syntax */
		return(ast_say_time_de(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "fr") ) {	/* French syntax */
		return(ast_say_time_fr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "nl") ) {	/* Dutch syntax */
		return(ast_say_time_nl(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt") ) {	/* Portuguese syntax */
		return(ast_say_time_pt(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt_BR") ) {	/* Brazilian Portuguese syntax */
		return(ast_say_time_pt_BR(chan, t, ints, lang));		
	} else if (!strcasecmp(lang, "tw") || !strcasecmp(lang, "zh") ) {	/* Taiwanese / Chinese syntax */
		return(ast_say_time_tw(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "gr") ) {  			/* Greek syntax */
		return(ast_say_time_gr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "ge") ) {  /* Georgian syntax */
		return(ast_say_time_ge(chan, t, ints, lang));
	}

	/* Default to English */
	return(ast_say_time_en(chan, t, ints, lang));
}

/* English syntax */
int ast_say_time_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	hour = tm.tm_hour;
	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);

	if (tm.tm_min > 9) {
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else if (tm.tm_min) {
		if (!res)
			res = ast_streamfile(chan, "digits/oh", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/oclock", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	return res;
}

/* German syntax */
int ast_say_time_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	if (!res)
		res = ast_say_number(chan, tm.tm_hour, ints, lang, "n");
	if (!res)
		res = ast_streamfile(chan, "digits/oclock", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
	    if (tm.tm_min > 0) 
		res = ast_say_number(chan, tm.tm_min, ints, lang, "f");
	return res;
}

/* French syntax */
int ast_say_time_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);

	res = ast_say_number(chan, tm.tm_hour, ints, lang, "f");
	if (!res)
		res = ast_streamfile(chan, "digits/oclock", lang);
	if (tm.tm_min) {
		if (!res)
		res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	}
	return res;
}

/* Dutch syntax */
int ast_say_time_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	if (!res)
		res = ast_say_number(chan, tm.tm_hour, ints, lang, (char *) NULL);
	if (!res)
		res = ast_streamfile(chan, "digits/nl-uur", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
	    if (tm.tm_min > 0) 
		res = ast_say_number(chan, tm.tm_min, ints, lang, NULL);
	return res;
}

/* Portuguese syntax */
int ast_say_time_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;
	int hour;

	ast_localtime(&t, &tm, NULL);
	hour = tm.tm_hour;
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, "f");
	if (tm.tm_min) {
		if (!res)
			res = wait_file(chan, ints, "digits/pt-e", lang);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else {
		if (!res)
			res = wait_file(chan, ints, "digits/pt-hora", lang);
		if (tm.tm_hour != 1)
			if (!res)
				res = wait_file(chan, ints, "digits/pt-sss", lang);
	}
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);
	return res;
}

/* Brazilian Portuguese syntax */
int ast_say_time_pt_BR(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);

	res = ast_say_number(chan, tm.tm_hour, ints, lang, "f");
	if (!res) {
		if (tm.tm_hour > 1)
			res = wait_file(chan, ints, "digits/hours", lang);
		else
			res = wait_file(chan, ints, "digits/hour", lang);
	}
	if ((!res) && (tm.tm_min)) {
		res = wait_file(chan, ints, "digits/pt-e", lang);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
		if (!res) {
			if (tm.tm_min > 1)
				res = wait_file(chan, ints, "digits/minutes", lang);
			else
				res = wait_file(chan, ints, "digits/minute", lang);
		}
	}
	return res;
}

/* Taiwanese / Chinese  syntax */
int ast_say_time_tw(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	hour = tm.tm_hour;
	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);
	if (!res)
		res = ast_streamfile(chan, "digits/oclock", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	if (!res)
		res = ast_streamfile(chan, "digits/minute", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	return res;
}

static int say_datetime(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	if (!strcasecmp(lang, "en") ) {	/* English syntax */
		return(ast_say_datetime_en(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "de") ) {	/* German syntax */
		return(ast_say_datetime_de(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "fr") ) {	/* French syntax */
		return(ast_say_datetime_fr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "nl") ) {	/* Dutch syntax */
		return(ast_say_datetime_nl(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt") ) {	/* Portuguese syntax */
		return(ast_say_datetime_pt(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt_BR") ) {	/* Brazilian Portuguese syntax */
		return(ast_say_datetime_pt_BR(chan, t, ints, lang));		
	} else if (!strcasecmp(lang, "tw") || !strcasecmp(lang, "zh") ) {	/* Taiwanese / Chinese syntax */
		return(ast_say_datetime_tw(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "gr") ) {  			/* Greek syntax */
		return(ast_say_datetime_gr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "ge") ) {  /* Georgian syntax */
		return(ast_say_datetime_ge(chan, t, ints, lang));
	}

	/* Default to English */
	return(ast_say_datetime_en(chan, t, ints, lang));
}

/* English syntax */
int ast_say_datetime_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);

	hour = tm.tm_hour;
	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);

	if (tm.tm_min > 9) {
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else if (tm.tm_min) {
		if (!res)
			res = ast_streamfile(chan, "digits/oh", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/oclock", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* German syntax */
int ast_say_datetime_de(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	res = ast_say_date(chan, t, ints, lang);
	if (!res) 
		ast_say_time(chan, t, ints, lang);
	return res;

}

/* French syntax */
int ast_say_datetime_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;

	ast_localtime(&t, &tm, NULL);

	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);

	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}

	if (!res)
		res = ast_say_number(chan, tm.tm_hour, ints, lang, "f");
	if (!res)
			res = ast_streamfile(chan, "digits/oclock", lang);
	if (tm.tm_min > 0) {
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} 
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* Dutch syntax */
int ast_say_datetime_nl(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	res = ast_say_date(chan, t, ints, lang);
	if (!res) {
		res = ast_streamfile(chan, "digits/nl-om", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) 
		ast_say_time(chan, t, ints, lang);
	return res;
}

/* Portuguese syntax */
int ast_say_datetime_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);

	hour = tm.tm_hour;
	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);

	if (tm.tm_min > 9) {
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else if (tm.tm_min) {
		if (!res)
			res = ast_streamfile(chan, "digits/oh", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/oclock", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res;
}

/* Brazilian Portuguese syntax */
int ast_say_datetime_pt_BR(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	res = ast_say_date(chan, t, ints, lang);
	if (!res)
		res = ast_say_time(chan, t, ints, lang);
	return res;
}

/* Taiwanese / Chinese syntax */
int ast_say_datetime_tw(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (!res)
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
 		if (!res)
			res = ast_waitstream(chan, ints);
	}

	hour = tm.tm_hour;
	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, hour, ints, lang, (char *) NULL);
	if (!res)
		res = ast_streamfile(chan, "digits/oclock", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	if (!res)
		res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	if (!res)
		res = ast_streamfile(chan, "digits/minute", lang);
	if (!res)
		res = ast_waitstream(chan, ints);
	return res;
}

static int say_datetime_from_now(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	if (!strcasecmp(lang, "en") ) {	/* English syntax */
		return(ast_say_datetime_from_now_en(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "fr") ) {	/* French syntax */
		return(ast_say_datetime_from_now_fr(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "pt") || !strcasecmp(lang, "pt_BR")) {	/* Portuguese syntax */
		return(ast_say_datetime_from_now_pt(chan, t, ints, lang));
	} else if (!strcasecmp(lang, "ge") ) {	/* Georgian syntax */
		return(ast_say_datetime_from_now_ge(chan, t, ints, lang));
	}

	/* Default to English */
	return(ast_say_datetime_from_now_en(chan, t, ints, lang));
}

/* English syntax */
int ast_say_datetime_from_now_en(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	int res=0;
	time_t nowt;
	int daydiff;
	struct tm tm;
	struct tm now;
	char fn[256];

	time(&nowt);

	ast_localtime(&t, &tm, NULL);
	ast_localtime(&nowt,&now, NULL);
	daydiff = now.tm_yday - tm.tm_yday;
	if ((daydiff < 0) || (daydiff > 6)) {
		/* Day of month and month */
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}
		if (!res)
			res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);

	} else if (daydiff) {
		/* Just what day of the week */
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}
	} /* Otherwise, it was today */
	if (!res)
		res = ast_say_time(chan, t, ints, lang);
	return res;
}

/* French syntax */
int ast_say_datetime_from_now_fr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	int res=0;
	time_t nowt;
	int daydiff;
	struct tm tm;
	struct tm now;
	char fn[256];

	time(&nowt);

	ast_localtime(&t, &tm, NULL);
	ast_localtime(&nowt, &now, NULL);
	daydiff = now.tm_yday - tm.tm_yday;
	if ((daydiff < 0) || (daydiff > 6)) {
		/* Day of month and month */
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}
		if (!res)
			res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);

	} else if (daydiff) {
		/* Just what day of the week */
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}
	} /* Otherwise, it was today */
	if (!res)
		res = ast_say_time(chan, t, ints, lang);
	return res;
}

/* Portuguese syntax */
int ast_say_datetime_from_now_pt(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	int res=0;
	time_t nowt;
	int daydiff;
	struct tm tm;
	struct tm now;
	char fn[256];

	time(&nowt);

	ast_localtime(&t, &tm, NULL);
	ast_localtime(&nowt, &now, NULL);
	daydiff = now.tm_yday - tm.tm_yday;
	if ((daydiff < 0) || (daydiff > 6)) {
		/* Day of month and month */
		if (!res)
			res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
		if (!res)
			res = wait_file(chan, ints, "digits/pt-de", lang);
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		if (!res)
			res = wait_file(chan, ints, fn, lang);
	
	} else if (daydiff) {
		/* Just what day of the week */
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		if (!res)
			res = wait_file(chan, ints, fn, lang);
	}	/* Otherwise, it was today */
	if (!strcasecmp(lang, "pt_BR")) {
		if (tm.tm_hour > 1) {
			snprintf(fn, sizeof(fn), "digits/pt-as");
		} else {
			snprintf(fn, sizeof(fn), "digits/pt-a");
		}
		if (!res)
			res = wait_file(chan, ints, fn, lang);
	} else {
		snprintf(fn, sizeof(fn), "digits/pt-ah");
		if (!res)
			res = wait_file(chan, ints, fn, lang);
		if (tm.tm_hour != 1)
		if (!res)
			res = wait_file(chan, ints, "digits/pt-sss", lang);
		if (!res)
			res = ast_say_time(chan, t, ints, lang);
	}
	return res;
}


/*********************************** GREEK SUPPORT ***************************************/


/*
 * digits/female-[1..4] : "Mia, dyo , treis, tessereis"
 */
static int gr_say_number_female(int num, struct ast_channel *chan, const char *ints, const char *lang){
	int tmp;
	int left;
	int res;
	char fn[256] = "";

	/* ast_log(LOG_DEBUG, "\n\n Saying number female %s %d \n\n",lang, num); */
	if (num < 5) {
		snprintf(fn, sizeof(fn), "digits/female-%d", num);
		res = wait_file(chan, ints, fn, lang);
	} else if (num < 13) {
		res = ast_say_number(chan, num, ints, lang, (char *) NULL);
	} else if (num <100 ) { 
		tmp = (num/10) * 10;
		left = num - tmp;
		snprintf(fn, sizeof(fn), "digits/%d", tmp);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
		if (left)
			gr_say_number_female(left, chan, ints, lang);
			
	} else {
		return -1;
	}
	return res;
}



/*
 *  	A list of the files that you need to create
 ->  	digits/xilia = "xilia"
 ->  	digits/myrio = "ekatomyrio"
 ->  	digits/thousands = "xiliades"
 ->  	digits/millions = "ektatomyria"
 ->  	digits/[1..12]   :: A pronunciation of th digits form 1 to 12 e.g. "tria"
 ->  	digits/[10..100]  :: A pronunciation of the tens from 10 to 90 
															 e,g 80 = "ogdonta" 
						 Here we must note that we use digits/tens/100 to utter "ekato"
						 and digits/hundred-100 to utter "ekaton"
 ->  	digits/hundred-[100...1000] :: A pronunciation of  hundreds from 100 to 1000 e.g 400 = 
																		 "terakosia". Here again we use hundreds/1000 for "xilia" 
						 and digits/thousnds for "xiliades"
*/

static int ast_say_number_full_gr(struct ast_channel *chan, int num, const char *ints, const char *language,int audiofd, int ctrlfd)
{
	int res = 0;
	char fn[256] = "";
	int i=0;

 
	if (!num) {
		snprintf(fn, sizeof(fn), "digits/0");
		res = ast_streamfile(chan, fn, chan->language);
		if (!res)
			return  ast_waitstream(chan, ints);
	}

	while(!res && num ) {
		i++;
		if (num < 13) {
			snprintf(fn, sizeof(fn), "digits/%d", num);
			num = 0;
		} else if (num <= 100) {
			/* 13 < num <= 100  */
			snprintf(fn, sizeof(fn), "digits/%d", (num /10) * 10);
			num -= ((num / 10) * 10); 
		} else if (num < 200) {
			/* 100 < num < 200 */
			snprintf(fn, sizeof(fn), "digits/hundred-100");
			num -= ((num / 100) * 100);
		}else if (num < 1000) {
			/* 200 < num < 1000 */
			snprintf(fn, sizeof(fn), "digits/hundred-%d", (num/100)*100);
			num -= ((num / 100) * 100);
		}else if (num < 2000){
			snprintf(fn, sizeof(fn), "digits/xilia");
			num -= ((num / 1000) * 1000);
		}
		else {
			/* num >  1000 */ 
			if (num < 1000000) {
				res = ast_say_number_full_gr(chan, (num / 1000), ints, chan->language, audiofd, ctrlfd);
				if (res)
					return res;
				num = num % 1000;
				snprintf(fn, sizeof(fn), "digits/thousands");
			}  else {
				if (num < 1000000000) { /* 1,000,000,000 */
					res = ast_say_number_full_gr(chan, (num / 1000000), ints, chan->language ,audiofd, ctrlfd);
					if (res)
						return res;
					num = num % 1000000;
					snprintf(fn, sizeof(fn), "digits/millions");
				} else {
					ast_log(LOG_DEBUG, "Number '%d' is too big for me\n", num);
					res = -1;
				}
			}
		} 
		if (!res) {
			if(!ast_streamfile(chan, fn, language)) {
				if ((audiofd > -1) && (ctrlfd > -1))
					res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
				else
					res = ast_waitstream(chan, ints);
			}
			ast_stopstream(chan);
		}
	}
	return res;
}


/*
 * The format is  weekday - day - month -year
 * 
 * A list of the files that you need to create
 * digits/day-[1..7]  : "Deytera .. Paraskeyh"
 * digits/months/1..12 : "Ianouariou .. Dekembriou"  
													Attention the months are in 
				"gekinh klhsh"
 */


static int ast_say_date_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	
	char fn[256];
	int res = 0;
	

	ast_localtime(&t,&tm,NULL);
	/* W E E K - D A Y */
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	/* D A Y */
	if (!res) {
		gr_say_number_female(tm.tm_mday, chan, ints, lang);
	}
	/* M O N T H */
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	/* Y E A R */
	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);
	return res; 
}


 
/* A list of the files that you need to create
 * digits/female/1..4 : "Mia, dyo , treis, tesseris "
 * digits/kai : "KAI"
 * didgits : "h wra"
 * digits/p-m : "meta meshmbrias" 
 * digits/a-m : "pro meshmbrias"
 */

static int ast_say_time_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{

	struct tm tm;
	int res = 0;
	int hour, pm=0;

	ast_localtime(&t, &tm, NULL);
	hour = tm.tm_hour;

	if (!hour)
		hour = 12;
	else if (hour == 12)
		pm = 1;
	else if (hour > 12) {
		hour -= 12;
		pm = 1;
	}
 
	res = gr_say_number_female(hour, chan, ints, lang);
	if (tm.tm_min) {
		if (!res)
			res = ast_streamfile(chan, "digits/kai", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
		if (!res)
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char *) NULL);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/hwra", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	if (pm) {
		if (!res)
			res = ast_streamfile(chan, "digits/p-m", lang);
	} else {
		if (!res)
			res = ast_streamfile(chan, "digits/a-m", lang);
	}
	if (!res)
		res = ast_waitstream(chan, ints);
	return res;
}



static int ast_say_datetime_gr(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;

	ast_localtime(&t, &tm, NULL);

	
	/* W E E K - D A Y */
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	/* D A Y */
	if (!res) {
		gr_say_number_female(tm.tm_mday, chan, ints, lang);
	}
	/* M O N T H */
	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}

	res = ast_say_time_gr(chan, t, ints, lang);
	return res;
}

static int ast_say_date_with_format_gr(struct ast_channel *chan, time_t time, const char *ints, const char *lang, const char *format, const char *timezone)
{
	
	struct tm tm;
	int res=0, offset, sndoffset;
	char sndfile[256], nextmsg[256];

	if (!format)
		format = "AdBY 'digits/at' IMp";

	ast_localtime(&time,&tm,timezone);
	
	for (offset=0 ; format[offset] != '\0' ; offset++) {
		ast_log(LOG_DEBUG, "Parsing %c (offset %d) in %s\n", format[offset], offset, format);
		switch (format[offset]) {
			/* NOTE:  if you add more options here, please try to be consistent with strftime(3) */
		case '\'':
			/* Literal name of a sound file */
			sndoffset=0;
			for (sndoffset=0 ; (format[++offset] != '\'') && (sndoffset < 256) ; sndoffset++)
				sndfile[sndoffset] = format[offset];
			sndfile[sndoffset] = '\0';
			res = wait_file(chan,ints,sndfile,lang);
			break;
		case 'A':
		case 'a':
			/* Sunday - Saturday */
			snprintf(nextmsg,sizeof(nextmsg), "digits/day-%d", tm.tm_wday);
			res = wait_file(chan,ints,nextmsg,lang);
			break;
		case 'B':
		case 'b':
		case 'h':
			/* January - December */
			snprintf(nextmsg,sizeof(nextmsg), "digits/mon-%d", tm.tm_mon);
			res = wait_file(chan,ints,nextmsg,lang);
			break;
		case 'd':
		case 'e':
			/* first - thirtyfirst */
			gr_say_number_female(tm.tm_mday, chan, ints, lang);
			break;
		case 'Y':
			/* Year */
			
			ast_say_number_full_gr(chan, 1900+tm.tm_year, ints, chan->language, -1, -1);
			break;
		case 'I':
		case 'l':
			/* 12-Hour */
			if (tm.tm_hour == 0)
				gr_say_number_female(12, chan, ints, lang);
			else if (tm.tm_hour > 12)
				gr_say_number_female(tm.tm_hour - 12, chan, ints, lang);
			else
				gr_say_number_female(tm.tm_hour, chan, ints, lang);
			break;
		case 'H':
		case 'k':
			/* 24-Hour */
			gr_say_number_female(tm.tm_hour, chan, ints, lang);
			break;
		case 'M':
			/* Minute */
			if (tm.tm_min) {
				if (!res)
					res = ast_streamfile(chan, "digits/kai", lang);
				if (!res)
					res = ast_waitstream(chan, ints);
				if (!res)
					res = ast_say_number_full_gr(chan, tm.tm_min, ints, lang, -1, -1);
			} else {
				if (!res)
					res = ast_streamfile(chan, "digits/oclock", lang);
				if (!res)
					res = ast_waitstream(chan, ints);
			}
			break;
		case 'P':
		case 'p':
			/* AM/PM */
			if (tm.tm_hour > 11)
				snprintf(nextmsg,sizeof(nextmsg), "digits/p-m");
			else
				snprintf(nextmsg,sizeof(nextmsg), "digits/a-m");
			res = wait_file(chan,ints,nextmsg,lang);
			break;
		case 'Q':
			/* Shorthand for "Today", "Yesterday", or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
			{
				struct timeval now;
				struct tm tmnow;
				time_t beg_today, tt;
				
				gettimeofday(&now,NULL);
				tt = now.tv_sec;
				ast_localtime(&tt,&tmnow,timezone);
				/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
				/* In any case, it saves not having to do ast_mktime() */
				beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
				if (beg_today < time) {
					/* Today */
					res = wait_file(chan,ints, "digits/today",lang);
				} else if (beg_today - 86400 < time) {
					/* Yesterday */
					res = wait_file(chan,ints, "digits/yesterday",lang);
				} else {
					res = ast_say_date_with_format_gr(chan, time, ints, lang, "AdBY", timezone);
				}
			}
			break;
		case 'q':
			/* Shorthand for "" (today), "Yesterday", A (weekday), or ABdY */
				/* XXX As emphasized elsewhere, this should the native way in your
				 * language to say the date, with changes in what you say, depending
				 * upon how recent the date is. XXX */
			{
				struct timeval now;
				struct tm tmnow;
				time_t beg_today, tt;
				
				gettimeofday(&now,NULL);
				tt = now.tv_sec;
				ast_localtime(&tt,&tmnow,timezone);
				/* This might be slightly off, if we transcend a leap second, but never more off than 1 second */
				/* In any case, it saves not having to do ast_mktime() */
				beg_today = tt - (tmnow.tm_hour * 3600) - (tmnow.tm_min * 60) - (tmnow.tm_sec);
				if (beg_today < time) {
					/* Today */
				} else if ((beg_today - 86400) < time) {
					/* Yesterday */
					res = wait_file(chan,ints, "digits/yesterday",lang);
				} else if (beg_today - 86400 * 6 < time) {
					/* Within the last week */
					res = ast_say_date_with_format_gr(chan, time, ints, lang, "A", timezone);
				} else {
					res = ast_say_date_with_format_gr(chan, time, ints, lang, "AdBY", timezone);
				}
			}
			break;
		case 'R':
			res = ast_say_date_with_format_gr(chan, time, ints, lang, "HM", timezone);
			break;
		case 'S':
			/* Seconds */
			snprintf(nextmsg,sizeof(nextmsg), "digits/kai");
			res = wait_file(chan,ints,nextmsg,lang);
			if (!res)
				res = ast_say_number_full_gr(chan, tm.tm_sec, ints, lang, -1, -1);
			if (!res)
				snprintf(nextmsg,sizeof(nextmsg), "digits/seconds");
			res = wait_file(chan,ints,nextmsg,lang);
			break;
		case 'T':
			res = ast_say_date_with_format_gr(chan, time, ints, lang, "HMS", timezone);
			break;
		case ' ':
		case '	':
			/* Just ignore spaces and tabs */
			break;
		default:
			/* Unknown character */
			ast_log(LOG_WARNING, "Unknown character in datetime format %s: %c at pos %d\n", format, format[offset], offset);
		}
		/* Jump out on DTMF */
		if (res) {
			break;
		}
	}
	return res;
}




/*********************************** Georgian Support ***************************************/


/*
	Convert a number into a semi-localized string. Only for Georgian.
	res must be of at least 256 bytes, preallocated.
	The output corresponds to Georgian spoken numbers, so
	it may be either converted to real words by applying a direct conversion
	table, or played just by substituting the entities with played files.

	Output may consist of the following tokens (separated by spaces):
	0, minus.
	1-9, 1_-9_. (erti, ori, sami, otxi, ... . erti, or, sam, otx, ...).
	10-19.
	20, 40, 60, 80, 20_, 40_, 60_, 80_. (oci, ormoci, ..., ocda, ormocda, ...).
	100, 100_, 200, 200_, ..., 900, 900_. (asi, as, orasi, oras, ...).
	1000, 1000_. (atasi, atas).
	1000000, 1000000_. (milioni, milion).
	1000000000, 1000000000_. (miliardi, miliard).

	To be able to play the sounds, each of the above tokens needs
	a corresponding sound file. (e.g. 200_.gsm).
*/
static char* ast_translate_number_ge(int num, char* res, int res_len)
{
	char buf[256];
	int digit = 0;
	int remainder = 0;


	if (num < 0) {
		strncat(res, "minus ", res_len - strlen(res) - 1);
		if ( num > INT_MIN ) {
			num = -num;
		} else {
			num = 0;
		}
	}


	/* directly read the numbers */
	if (num <= 20 || num == 40 || num == 60 || num == 80 || num == 100) {
		snprintf(buf, sizeof(buf), "%d", num);
		strncat(res, buf, res_len - strlen(res) - 1);
		return res;
	}


	if (num < 40) {  /* ocda... */
		strncat(res, "20_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(num - 20, res, res_len);
	}

	if (num < 60) {  /* ormocda... */
		strncat(res, "40_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(num - 40, res, res_len);
	}

	if (num < 80) {  /* samocda... */
		strncat(res, "60_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(num - 60, res, res_len);
	}

	if (num < 100) {  /* otxmocda... */
		strncat(res, "80_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(num - 80, res, res_len);
	}


	if (num < 1000) {  /*  as, oras, samas, ..., cxraas. asi, orasi, ..., cxraasi. */
		remainder = num % 100;
		digit = (num - remainder) / 100;

		if (remainder == 0) {
			snprintf(buf, sizeof(buf), "%d", num);
			strncat(res, buf, res_len - strlen(res) - 1);
			return res;
		} else {
			snprintf(buf, sizeof(buf), "%d_ ", digit*100);
			strncat(res, buf, res_len - strlen(res) - 1);
			return ast_translate_number_ge(remainder, res, res_len);
		}
	}


	if (num == 1000) {
		strncat(res, "1000", res_len - strlen(res) - 1);
		return res;
	}


	if (num < 1000000) {
		remainder = num % 1000;
		digit = (num - remainder) / 1000;

		if (remainder == 0) {
			ast_translate_number_ge(digit, res, res_len);
			strncat(res, " 1000", res_len - strlen(res) - 1);
			return res;
		}

		if (digit == 1) {
			strncat(res, "1000_ ", res_len - strlen(res) - 1);
			return ast_translate_number_ge(remainder, res, res_len);
		}

		ast_translate_number_ge(digit, res, res_len);
		strncat(res, " 1000_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(remainder, res, res_len);

	}


	if (num == 1000000) {
		strncat(res, "1 1000000", res_len - strlen(res) - 1);
		return res;
	}


	if (num < 1000000000) {
		remainder = num % 1000000;
		digit = (num - remainder) / 1000000;

		if (remainder == 0) {
			ast_translate_number_ge(digit, res, res_len);
			strncat(res, " 1000000", res_len - strlen(res) - 1);
			return res;
		}

		ast_translate_number_ge(digit, res, res_len);
		strncat(res, " 1000000_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(remainder, res, res_len);

	}


	if (num == 1000000000) {
		strncat(res, "1 1000000000", res_len - strlen(res) - 1);
		return res;
	}


	if (num > 1000000000) {
		remainder = num % 1000000000;
		digit = (num - remainder) / 1000000000;

		if (remainder == 0) {
			ast_translate_number_ge(digit, res, res_len);
			strncat(res, " 1000000000", res_len - strlen(res) - 1);
			return res;
		}

		ast_translate_number_ge(digit, res, res_len);
		strncat(res, " 1000000000_ ", res_len - strlen(res) - 1);
		return ast_translate_number_ge(remainder, res, res_len);

	}

	return res;

}



/*! \brief  ast_say_number_full_ge: Georgian syntax */
static int ast_say_number_full_ge(struct ast_channel *chan, int num, const char *ints, const char *language, const char *options, int audiofd, int ctrlfd)
{
	int res = 0;
	char fn[512] = "";
	char* s = 0;
	const char* remainder = fn;

	if (!num)
		return ast_say_digits_full(chan, 0, ints, language, audiofd, ctrlfd);


	ast_translate_number_ge(num, fn, 512);



	while (res == 0 && (s = strstr(remainder, " "))) {
		size_t len = s - remainder;
		char* new_string = malloc(len + 1 + strlen("digits/"));

		sprintf(new_string, "digits/");
		strncat(new_string, remainder, len);  /* we can't sprintf() it, it's not null-terminated. */
/* 		new_string[len + strlen("digits/")] = '\0'; */

		if (!ast_streamfile(chan, new_string, language)) {
			if ((audiofd  > -1) && (ctrlfd > -1))
				res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
			else
				res = ast_waitstream(chan, ints);
		}
		ast_stopstream(chan);

		free(new_string);

		remainder = s + 1;  /* position just after the found space char. */
		while(*remainder == ' ')  /* skip multiple spaces */
			remainder++;
	}


	/* the last chunk. */
	if (res == 0 && *remainder) {

		char* new_string = malloc(strlen(remainder) + 1 + strlen("digits/"));
		sprintf(new_string, "digits/%s", remainder);

		if (!ast_streamfile(chan, new_string, language)) {
			if ((audiofd  > -1) && (ctrlfd > -1))
				res = ast_waitstream_full(chan, ints, audiofd, ctrlfd);
			else
				res = ast_waitstream(chan, ints);
		}
		ast_stopstream(chan);

		free(new_string);

	}


	return res;

}



/*
Georgian support for date/time requires the following files (*.gsm):

mon-1, mon-2, ... (ianvari, tebervali, ...)
day-1, day-2, ... (orshabati, samshabati, ...)
saati_da
tsuti
tslis
*/



/* Georgian syntax. e.g. "oriatas xuti tslis 5 noemberi". */
static int ast_say_date_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	char fn[256];
	int res = 0;
	ast_localtime(&t,&tm,NULL);

	if (!res)
		res = ast_say_number(chan, tm.tm_year + 1900, ints, lang, (char *) NULL);

	if (!res) {
		snprintf(fn, sizeof(fn), "digits/tslis %d", tm.tm_wday);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}

	if (!res) {
		res = ast_say_number(chan, tm.tm_mday, ints, lang, (char * ) NULL);
/* 		if (!res)
 			res = ast_waitstream(chan, ints);
*/
	}

	if (!res) {
		snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
		res = ast_streamfile(chan, fn, lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}
	return res;

}





/* Georgian syntax. e.g. "otxi saati da eqvsi tsuti" */
static int ast_say_time_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);

	res = ast_say_number(chan, tm.tm_hour, ints, lang, (char*)NULL);
	if (!res) {
		res = ast_streamfile(chan, "digits/saati_da", lang);
		if (!res)
			res = ast_waitstream(chan, ints);
	}

	if (tm.tm_min) {
		if (!res) {
			res = ast_say_number(chan, tm.tm_min, ints, lang, (char*)NULL);

			if (!res) {
				res = ast_streamfile(chan, "digits/tsuti", lang);
				if (!res)
					res = ast_waitstream(chan, ints);
			}
		}
	}
	return res;
}



/* Georgian syntax. Say date, then say time. */
static int ast_say_datetime_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	struct tm tm;
	int res = 0;

	ast_localtime(&t, &tm, NULL);
	res = ast_say_date(chan, t, ints, lang);
	if (!res)
		ast_say_time(chan, t, ints, lang);
	return res;

}




/* Georgian syntax */
static int ast_say_datetime_from_now_ge(struct ast_channel *chan, time_t t, const char *ints, const char *lang)
{
	int res=0;
	time_t nowt;
	int daydiff;
	struct tm tm;
	struct tm now;
	char fn[256];

	time(&nowt);

	ast_localtime(&t, &tm, NULL);
	ast_localtime(&nowt, &now, NULL);
	daydiff = now.tm_yday - tm.tm_yday;
	if ((daydiff < 0) || (daydiff > 6)) {
		/* Day of month and month */
		if (!res)
			res = ast_say_number(chan, tm.tm_mday, ints, lang, (char *) NULL);
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/mon-%d", tm.tm_mon);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}

	} else if (daydiff) {
		/* Just what day of the week */
		if (!res) {
			snprintf(fn, sizeof(fn), "digits/day-%d", tm.tm_wday);
			res = ast_streamfile(chan, fn, lang);
			if (!res)
				res = ast_waitstream(chan, ints);
		}
	} /* Otherwise, it was today */
	if (!res)
		res = ast_say_time(chan, t, ints, lang);

	return res;
}



/*
 * remap the 'say' functions to use those in this file
 */
static void __attribute__((constructor)) __say_init(void)
{
	ast_say_number_full = say_number_full;
	ast_say_enumeration_full = say_enumeration_full;
	ast_say_digit_str_full = say_digit_str_full;
	ast_say_character_str_full = say_character_str_full;
	ast_say_phonetic_str_full = say_phonetic_str_full;
	ast_say_datetime = say_datetime;
	ast_say_time = say_time;
	ast_say_date = say_date;
	ast_say_datetime_from_now = say_datetime_from_now;
	ast_say_date_with_format = say_date_with_format;
}
