/*!
 * \file        ast106.h
 * \brief       SCCP PBX Asterisk Header
 * \author      Marcello Ceshia
 * \author      Diederik de Groot <ddegroot [at] users.sourceforge.net>
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 *
 * $Date: 2015-10-18 08:34:39 +0000 (Sun, 18 Oct 2015) $
 * $Revision: 6264 $  
 */
#pragma once

#include <config.h>

#ifdef CS_SCCP_CONFERENCE
#include "asterisk/bridging.h"
#include "asterisk/bridging_features.h"
#ifdef HAVE_PBX_BRIDGING_ROLES_H
#include "asterisk/bridging_roles.h"
#endif
#endif

#define sccp_sched_context_destroy sched_context_destroy
#if ASTERISK_VERSION_NUMBER >= 10601
#define pbx_channel_unref(c) ({ ao2_ref(c, -1); (PBX_CHANNEL_TYPE *) (NULL); })
#define pbx_channel_ref(c) ({ ao2_ref(c, 1); (PBX_CHANNEL_TYPE *) c; })
#else
#define pbx_channel_unref(c) NULL
#define pbx_channel_ref(c) ({(PBX_CHANNEL_TYPE *) c;})
#endif
#define NEWCONST const												// old functions used without const
#define OLDCONST												// new function used with const

#define PBX_ENDPOINT_TYPE void
#define PBX_EVENT_SUBSCRIPTION struct ast_event_sub

#define pbx_manager_register ast_manager_register2

#undef pbx_channel_get_by_name
#define pbx_channel_get_by_name(_x) ast_get_channel_by_name_locked(_x)

#ifndef CONFIG_STATUS_FILEMISSING
#define CONFIG_STATUS_FILEMISSING (void *)-2
#endif

#ifndef CONFIG_STATUS_FILEINVALID
#define CONFIG_STATUS_FILEINVALID (void *)-2
#endif

#undef pbx_check_hangup_locked
#define pbx_check_hangup_locked(_x) ({int res; ast_channel_lock(_x); res = ast_check_hangup(_x); ast_channel_unlock(_x); (res);})
#undef pbx_bridge_lock
#undef pbx_bridge_unlock
#define pbx_bridge_lock(_x) NULL
#define pbx_bridge_unlock(_x) NULL

enum AST_CONNECTED_LINE_UPDATE_SOURCE {
	/*! Update for unknown reason (May be interpreted to mean from answer) */
	AST_CONNECTED_LINE_UPDATE_SOURCE_UNKNOWN,
	/*! Update from normal call answering */
	AST_CONNECTED_LINE_UPDATE_SOURCE_ANSWER,
	/*! Update from call diversion (Deprecated, use REDIRECTING updates instead.) */
	AST_CONNECTED_LINE_UPDATE_SOURCE_DIVERSION,
	/*! Update from call transfer(active) (Party has already answered) */
	AST_CONNECTED_LINE_UPDATE_SOURCE_TRANSFER,
	/*! Update from call transfer(alerting) (Party has not answered yet) */
	AST_CONNECTED_LINE_UPDATE_SOURCE_TRANSFER_ALERTING
};

#include "ast.h"
typedef int ast_format_t;
typedef int64_t format_t;
int skinny_codecs2pbx_codec_pref(skinny_codec_t * skinny_codecs, struct ast_codec_pref *astCodecPref);
int sccp_wrapper_asterisk_set_rtp_peer(PBX_CHANNEL_TYPE * ast, PBX_RTP_TYPE * rtp, PBX_RTP_TYPE * vrtp, PBX_RTP_TYPE * trtp, int codecs, int nat_active);

//void *sccp_do_monitor(void *data);
//int sccp_restart_monitor(void);
char *pbx_getformatname(format_t format);
char *pbx_getformatname_multiple(char *buf, size_t size, format_t format);

#define pbx_channel_name(x) x->name

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define CLI_AMI_OUTPUT(fd, s, ...) 										\
	if (NULL != s) {											\
		astman_append(s, __VA_ARGS__);									\
		local_line_total++;										\
	} else {												\
		ast_cli(fd, __VA_ARGS__);									\
	}

#define CLI_AMI_CAMEL_PARAM(param, camelParam)									\
	char *current = param;											\
	char *ptr = camelParam;											\
	int CapsNext = 0;											\
	while (*current) {											\
		if ((*current >= 48 && *current <= 57 /*num*/) || (*current >= 65 && *current <= 90 /*A-Z*/) || (*current >= 97 && *current <= 122 /*a-z*/)) {	\
			if (CapsNext) 	*ptr++ = toupper(*current++);						\
			else 		*ptr++ = *current++;							\
			CapsNext = 0;										\
		} else {											\
			CapsNext = 1;										\
			current++;										\
		}												\
	}													\
	*ptr='\0';												

#define CLI_AMI_OUTPUT_PARAM(param, width, fmt, ...) 								\
	if (NULL != s) {											\
		char *camelParam = ast_strdupa(param);								\
		CLI_AMI_CAMEL_PARAM(param, camelParam);								\
		astman_append(s, "%s: " fmt "\r\n", camelParam, __VA_ARGS__);					\
		local_line_total++;										\
	} else {												\
		ast_cli(fd, "%-*.*s %s " fmt "\n", width, width, param, ":", __VA_ARGS__);			\
	}

#define CLI_AMI_OUTPUT_BOOL(param, width, value) 								\
	if (NULL != s) {											\
		char *camelParam = ast_strdupa(param);								\
		CLI_AMI_CAMEL_PARAM(param, camelParam);								\
		astman_append(s, "%s: %s\r\n", camelParam, ((value) ? "on" : "off"));				\
		local_line_total++;										\
	} else {												\
		ast_cli(fd, "%-*.*s %s %s\n", width, width, param, ":", ((value) ? "on" : "off")); 		\
	}

#define CLI_AMI_OUTPUT_YES_NO(param, width, value) 								\
	if (NULL != s) {											\
		char *camelParam = ast_strdupa(param);								\
		CLI_AMI_CAMEL_PARAM(param, camelParam);								\
		astman_append(s, "%s: %s\r\n", camelParam, ((value) ? "yes" : "no"));				\
		local_line_total++;										\
	} else {												\
		ast_cli(fd, "%-*.*s %s %s\n", width, width, param, ":", ((value) ? "yes" : "no")); 		\
	}

#define _CLI_AMI_RETURN_ERROR(fd, s, m, line, fmt, ...) 							\
        /*pbx_log(LOG_WARNING, "SCCP CLI ERROR: " fmt, __VA_ARGS__);*/						\
	if (NULL != s) {											\
                char tmp_ ## line[100];										\
	        snprintf(tmp_ ## line,sizeof(tmp_ ## line),fmt,__VA_ARGS__);					\
	        astman_send_error(s, m, tmp_ ## line);								\
		local_line_total++;										\
	} else {												\
		ast_cli(fd, "SCCP CLI ERROR: " fmt, __VA_ARGS__);						\
	}													\
	return RESULT_FAILURE;
#define CLI_AMI_RETURN_ERROR(fd, s, m, fmt, ...) _CLI_AMI_RETURN_ERROR(fd, s, m, __LINE__, fmt, __VA_ARGS__)

// CLI_ENTRY
//   param1=registration_name
//   param2=function to execute when called
//   param3=cli string to be types as array of strings
//   param4=registration description
//   param5=usage string
#define CLI_AMI_ENTRY(_FUNCTION_NAME,_CALLED_FUNCTION,_DESCR,_USAGE, _COMPLETER_REPEAT, _EVENTLIST)		\
	static int manager_ ## _FUNCTION_NAME(struct mansession *s, const struct message *m)			\
	{													\
		const char *id = astman_get_header(m, "ActionID");						\
		static char *cli_ami_params[] = { CLI_COMMAND, CLI_AMI_PARAMS };				\
		static char *arguments[ARRAY_LEN(cli_ami_params)];						\
		uint8_t x = 0, i = 0; 										\
		for (x=0; x<ARRAY_LEN(cli_ami_params); x++) {							\
			if(NULL != cli_ami_params[x] && strlen(cli_ami_params[x]) > 0){ 			\
				arguments[i++]=(char *)astman_get_header(m, cli_ami_params[x]);		 	\
			} 											\
		}												\
		char idtext[256] = "";										\
		sccp_cli_totals_t totals = {0};									\
		if (!pbx_strlen_zero(id)) {									\
			snprintf(idtext, sizeof(idtext), "ActionID: %s\r\n", id);				\
		}												\
		if (_EVENTLIST == TRUE) {									\
			astman_send_listack(s, m, AMI_COMMAND " list will follow", "start");			\
		}												\
		if (RESULT_SUCCESS==_CALLED_FUNCTION(-1, &totals, s, m, ARRAY_LEN(arguments), arguments)) {	\
			if (_EVENTLIST == TRUE) {								\
				astman_append(s,								\
				"Event: " AMI_COMMAND "Complete\r\n"						\
				"EventList: Complete\r\n"							\
				"ListItems: %d\r\n"								\
				"ListTableItems: %d\r\n"							\
				"%s"										\
				"\r\n", totals.lines, totals.tables, idtext);  				\
			} else {										\
				astman_append(s, "\r\n");							\
			}											\
                } else {											\
                         astman_send_error(s, m, "Execution Failed\n");						\
                }												\
		return 0;											\
	}													\
														\
	static char * cli_ ## _FUNCTION_NAME(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a) {	\
		char *cli_command[] = { CLI_COMMAND, NULL };							\
		static sccp_cli_completer_t cli_complete[] = { CLI_COMPLETE };					\
		static char command[80]="";									\
		if (cmd == CLI_INIT) {										\
		 	ast_join(command, sizeof(command), cli_command);					\
			e->command = command;									\
			e->usage = _USAGE;									\
			return NULL;										\
		} else if (cmd == CLI_GENERATE) {								\
        		uint8_t completer;									\
			for (completer=0; completer<ARRAY_LEN(cli_complete); completer++) {			\
				if ((unsigned)a->pos == (completer + ARRAY_LEN(cli_command) - 1) || _COMPLETER_REPEAT ) {\
					return sccp_exec_completer(cli_complete[completer], (char *)a->line, (char *)a->word, a->pos, a->n);\
				}										\
			}											\
			return NULL;										\
		}												\
		if (a->argc < (int)(ARRAY_LEN(cli_command)-1)) 							\
			return CLI_SHOWUSAGE;									\
														\
		static char *cli_ami_params[] = { CLI_COMMAND, CLI_AMI_PARAMS };				\
		struct message m = { 0 };									\
		size_t hdrlen; 											\
		uint8_t x;											\
                for (x = 0; x < ARRAY_LEN(cli_ami_params) && x < a->argc; x++) {				\
                        hdrlen = strlen(cli_ami_params[x]) + 2 + strlen(a->argv[x]) + 1;			\
                        m.headers[m.hdrcount] = alloca(hdrlen);							\
                        snprintf((char *) m.headers[m.hdrcount], hdrlen, "%s: %s", cli_ami_params[x], a->argv[x]);	\
                        m.hdrcount++;                                        					\
                }												\
		switch (_CALLED_FUNCTION(a->fd, NULL, NULL, NULL, a->argc, (char **) a->argv)) {		\
			case RESULT_SUCCESS: return CLI_SUCCESS;						\
			case RESULT_FAILURE: return CLI_FAILURE;						\
			case RESULT_SHOWUSAGE: return CLI_SHOWUSAGE;						\
			default: return CLI_FAILURE;								\
		}												\
	};
#define CLI_ENTRY(_FUNCTION_NAME,_CALLED_FUNCTION,_DESCR,_USAGE, _COMPLETER_REPEAT)				\
	static char *_FUNCTION_NAME(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a) {			\
		char *cli_command[] = { CLI_COMMAND, NULL };							\
		static sccp_cli_completer_t cli_complete[] = { CLI_COMPLETE };					\
		static char command[80]="";									\
		if (cmd == CLI_INIT) {										\
		 	ast_join(command, sizeof(command), cli_command);					\
			e->command = command;									\
			e->usage = _USAGE;									\
			return NULL;										\
		} else if (cmd == CLI_GENERATE) {								\
                        uint8_t completer;									\
			for (completer=0; completer<ARRAY_LEN(cli_complete); completer++) {			\
				if ((unsigned)a->pos == (completer + ARRAY_LEN(cli_command) -1) || _COMPLETER_REPEAT ) {\
					return sccp_exec_completer(cli_complete[completer], (char *)a->line, (char *)a->word, a->pos, a->n);\
				}										\
			}											\
			return NULL;										\
		}												\
		if (a->argc < (int)(ARRAY_LEN(cli_command)-1)) 							\
			return CLI_SHOWUSAGE;									\
														\
		switch (_CALLED_FUNCTION(a->fd, a->argc, (char **) a->argv)) {					\
			case RESULT_SUCCESS: return CLI_SUCCESS;						\
			case RESULT_FAILURE: return CLI_FAILURE;						\
			case RESULT_SHOWUSAGE: return CLI_SHOWUSAGE;						\
			default: return CLI_FAILURE;								\
		}												\
	};
#endif														/* DOXYGEN_SHOULD_SKIP_THIS */
// kate: indent-width 8; replace-tabs off; indent-mode cstyle; auto-insert-doxygen on; line-numbers on; tab-indents on; keep-extra-spaces off; auto-brackets off;
