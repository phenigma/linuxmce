/**
 *
 * @file sqlCVSrequests.h
 * @brief header file  @todo ask
 * @author
 *
 */
 
#ifndef sqlCVSrequests_h
#define sqlCVSrequests_h

/**
 * @brief Implement the mandatory BuildRequestFromData for use with PlutoVIP
 */

#define R_COMMIT_CHANGES		1
#define R_COMMIT_TABLE			2
#define R_COMMIT_ROW			3
#define R_GET_ALL_PSC_ID		4
#define R_UPDATE_REPOSITORY		5
#define R_UPDATE_TABLE			6
#define R_CLOSE_TRANSACTION		7
#define R_APPROVE_BATCH			8

#define A_UPDATE_ROW			1
#define A_UPDATE_SCHEMA			2

#endif
