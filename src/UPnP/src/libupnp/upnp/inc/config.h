/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000-2003 Intel Corporation 
// All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
//
// * Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer. 
// * Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution. 
// * Neither name of Intel Corporation nor the names of its contributors 
// may be used to endorse or promote products derived from this software 
// without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_H
#define CONFIG_H 

#include <stdio.h>
#define _strdup strdup
#include "ithread.h"

/** @name Compile time configuration options
 *  The Linux SDK for UPnP Devices contains some compile-time parameters 
 *  that effect the behavior of the SDK.  All configuration options are 
 *  located in {\tt inc/config.h}.
 */
 
//@{

/** @name THREAD_IDLE_TIME
 *  The {\tt THREAD_IDLE_TIME} constant determines when a thread will be
 *  removed from the thread pool and returned to the operating system. When 
 *  a thread in the thread pool has been idle for this number of milliseconds
 *  the thread will be released from the thread pool.  The default value is
 *  5000 milliseconds (5 seconds).
 */

//@{
#define THREAD_IDLE_TIME 5000
//@}

/** @name JOBS_PER_THREAD
 *  The {\tt JOBS_PER_THREAD} constant determines when a new thread will be
 *  allocated to the thread pool inside the  SDK. The thread pool will
 *  try and maintain this jobs/thread ratio. When the jobs/thread ratio 
 *  becomes greater than this, then a new thread (up to the max) will be 
 *  allocated to the thread pool.  The default ratio is 10 jobs/thread.
 */

//@{
#define JOBS_PER_THREAD 10
//@}

/** @name MIN_THREADS
 *  The {\tt MIN_THREADS} constant defines the minimum number of threads the
 *  thread pool inside the SDK will create.  The thread pool will
 *  always have this number of threads. These threads are used
 *  for both callbacks into applications built on top of the SDK and also
 *  for making connections to other control points and devices. This number
 *  includes persistent threads.  The default value is two threads.
 */

//@{
#define MIN_THREADS 2 
//@}

/** @name MAX_THREADS
 *  The {\tt MAX_THREADS} constant defines the maximum number of threads the
 *  thread pool inside the SDK will create.  These threads are used
 *  for both callbacks into applications built on top of the library and also 
 *  for making connections to other control points and devices.  It is not 
 *  recommended that this value be below 10, since the threads are 
 *  necessary for correct operation.  This value can be increased for greater
 *  performance in operation at the expense of greater memory overhead.  The 
 *  default value is 12.
 */

//@{
#define MAX_THREADS 12 
//@}

/** @name HTTP_READ_BYTES
 * HTTP responses will read at most {\tt HTTP_READ_BYTES}.  This prevents 
 * devices that have a misbehaving web server to send a large amount of data to
 * the control point causing it to crash.  A value of -1 means there is no 
 * max.  The default is -1.
 */
//@{
#define HTTP_READ_BYTES       -1
//@}

/** @name NUM_SSDP_COPY
 * This configuration parameter determines how many copies of each SSDP 
 * advertisement and search packets will be sent. By default it will send two 
 * copies of every packet.  
 */
//@{
#define NUM_SSDP_COPY  2
//@}

/** @name SSDP_PAUSE
 * This configuration parameter determines the pause between identical SSDP 
 * advertisement and search packets. The pause is measured in milliseconds
 * and defaults to 100.
 */
//@{
#define SSDP_PAUSE  100
//@}

/** @name WEB_SERVER_BUF_SIZE 
 * This configuration parameter sets the maximum buffer size for the 
 * webserver.  The default value is 1MB.
 */
//@{
#define WEB_SERVER_BUF_SIZE  1024*1024
//@}

/** @name AUTO_RENEW_TIME
 * The {\tt AUTO_RENEW_TIME} is the time, in seconds, before a subscription
 * expires that the SDK automatically resubscribes.  The default 
 * value is 10 seconds.  Setting this value too low can result in the 
 * subscription renewal not making it to the device in time, causing the 
 * subscription to timeout. In order to avoid continually resubscribing
 * the minimum subscription time is five seconds more than the auto renew
 * time.
 */

//@{
#define AUTO_RENEW_TIME 10
//@}

/** @name CP_MINIMUM_SUBSCRIPTION_TIME 
 * The {\tt CP_MINIMUM_SUBSCRIPTION_TIME} is the minimum subscription time
 * allowed for a control point using the SDK. Subscribing for less than
 * this time automatically results in a subscription for this amount.  The 
 * default value is 5 seconds more than the {\tt AUTO_RENEW_TIME}, or 15
 * seconds.
 */

//@{
#define CP_MINIMUM_SUBSCRIPTION_TIME AUTO_RENEW_TIME + 5
//@}

/** @name MAX_SEARCH_TIME 
 * The {\tt MAX_SEARCH_TIME} is the maximum time
 * allowed for an SSDP search by a control point. Searching for greater than
 * this time automatically results in a search for this amount.  The default 
 * value is 80 seconds.
 */

//@{
#define MAX_SEARCH_TIME 80
//@}

/** @name MIN_SEARCH_TIME 
 * The {\tt MIN_SEARCH_TIME} is the minimumm time
 * allowed for an SSDP search by a control point. Searching for less than
 * this time automatically results in a search for this amount.  The default 
 * value is 2 seconds.
 */

//@{
#define MIN_SEARCH_TIME 2
//@}


/** @name AUTO_ADVERTISEMENT_TIME
 *  The {\tt AUTO_ADVERTISEMENT_TIME} is the time, in seconds, before an
 *  device advertisements expires before a renewed advertisement is sent.
 *  The default time is 30 seconds.
 */

//@{
#define AUTO_ADVERTISEMENT_TIME 30
//@}

/** @name SSDP_PACKET_DISTRIBUTE
 *  The {\tt SSDP_PACKET_DISTRIBUTE} enables the SSDP packets to be sent
 *  at an interval equal to half of the expiration time of SSDP packets
 *  minus the AUTO_ADVERTISEMENT_TIME. This is used to increase
 *  the probability of SSDP packets reaching to control points.
 *  It is recommended that this flag be turned on for embedded wireless 
 *  devices.
 */

//@{
#define SSDP_PACKET_DISTRIBUTE 1
//@}

/** @name Module Exclusion
 *  Depending on the requirements, the user can selectively discard any of 
 *  the major modules like SOAP, GENA, SSDP or the Internal web server. By 
 *  default everything is included inside the SDK.  By setting any of
 *  the values below to 0, that component will not be included in the final
 *  SDK.
 *  \begin{itemize}
 *    \item {\tt EXCLUDE_SOAP[0,1]}
 *    \item {\tt EXCLUDE_GENA[0,1]}
 *    \item {\tt EXCLUDE_SSDP[0,1]}
 *    \item {\tt EXCLUDE_DOM [0,1]}
 *    \item {\tt EXCLUDE_WEB_SERVER[0,1]}
 *    \item {\tt EXCLUDE_JNI[0,1]}
 *  \end{itemize}
 *
 */

//@{
#define EXCLUDE_SSDP 0
#define EXCLUDE_SOAP 0
#define EXCLUDE_GENA 0
#define EXCLUDE_DOM  0
#define EXCLUDE_MINISERVER 0
#define EXCLUDE_WEB_SERVER 0
#ifdef USE_JNI
#define EXCLUDE_JNI 0
#else
#define EXCLUDE_JNI 1
#endif
//@}

/** @name DEBUG_LEVEL
 *  The user has the option to select 4 different types of debugging levels. 
 *  The critical level (3) will show only those messages which can halt the 
 *  normal processing of the library, like memory allocation errors. The 
 *  remaining three levels are just for debugging purposes.  Packet level 
 *  will display all the incoming and outgoing packets that are flowing 
 *  between the control point and the device. Info Level displays the 
 *  other important operational information regarding the working of 
 *  the library. If the user selects All, then the library displays all the 
 *  debugging information that it has.
 *  \begin{itemize}
 *    \item {\tt Critical [0]}
 *    \item {\tt Packet Level [1]}
 *    \item {\tt Info Level [2]}
 *    \item {\tt All [3]}
 *  \end{itemize}
 */

//@{
 #define DEBUG_LEVEL	     3
//@}
    
/** @name DEBUG_TARGET
 *  The user has the option to redirect the library output debug messages 
 *  to either the screen or to a log file.  All the output messages with 
 *  debug level 0 will go to {\tt upnp.err} and messages with debug level 
 *  greater than zero will be redirected to {\tt upnp.out}.
 */

//@{
#define DEBUG_TARGET	    1   
//@}


//@} // Compile time configuration options

#define DEBUG_ALL		0   
#define DEBUG_SSDP		0    
#define DEBUG_SOAP		0    
#define DEBUG_GENA		0    
#define DEBUG_TPOOL		0     
#define DEBUG_MSERV		0
#define DEBUG_DOM		0
#define DEBUG_HTTP		0
#define DEBUG_API		0    

/** @name Other debugging features
          The UPnP SDK contains other features to aid in debugging.
 */

//@{

// Do not change, Internal purpose only!!! 

// normally, this should be 0; adds extra seconds to delay
#define MINIMUM_DELAY 10

/** @name DBGONLY
          The {\bf DBGONLY} macro allows code to be marked so that it 
          is only included in the DEBUG build and not the release.  To
          use this macro, put the code inside of the parentheses:

          {\tt DBGONLY(int i;)}

          This will cause a declaration of the integer {\tt i} only
          in the debug build.
  */

//@{
#ifdef DEBUG
#define DBGONLY(x) x
#else
#define DBGONLY(x)  
#endif
//@}

typedef enum Upnp_Module {SSDP,SOAP,GENA,TPOOL,MSERV,DOM,API, HTTP} Dbg_Module;
typedef enum DBG_LVL {UPNP_CRITICAL,UPNP_PACKET,UPNP_INFO,UPNP_ALL} Dbg_Level;
DBGONLY(extern ithread_mutex_t GlobalDebugMutex;)


#ifdef __cplusplus
extern "C" {
#endif
  DBGONLY(
	  // Function declarations only for debug 
/************************************************************************
* Function : InitLog											
*																	
* Parameters:	void													
*																	
* Description:														
*	This functions initializes the log files
* Returns: int
*	-1 : If fails
*	UPNP_E_SUCCESS : if success
***************************************************************************/
  int InitLog();

/************************************************************************
* Function : CloseLog											
*																	
* Parameters:	void													
*																	
* Description:														
*	This functions closes the log files
* Returns: void
***************************************************************************/
  void CloseLog();

/************************************************************************
* Function : SetLogFileNames											
*																	
* Parameters:														
*	IN const char* ErrFileName: name of the error file
*	IN const char *InfoFileName: name of the information file
*	IN int size: Size of the buffer
*	IN int starLength: This parameter provides the width of the banner
*																	
* Description:														
*	This functions takes the buffer and writes the buffer in the file as 
*	per the requested banner											
* Returns: void
***************************************************************************/
  void SetLogFileNames(const char *ErrFileName,const char *InfoFileName);


/**************************************************************************
* Function : GetDebugFile											
*																	
* Parameters:														
*	IN Dbg_Level DLevel: The level of the debug logging. It will decide 
*		whether debug statement will go to standard output, or any of the 
*		log files.
*	IN Dbg_Module Module: debug will go in the name of this module
*																	
* Description:
*	This function checks if the module is turned on for debug 
*	and returns the file descriptor corresponding to the debug level
* Returns: FILE *
*	NULL : if the module is turn off for debug 
*	else returns the right file descriptor
***************************************************************************/
  FILE * GetDebugFile(Dbg_Level level, Dbg_Module module);

/**************************************************************************
* Function : UpnpPrintf											
*																	
* Parameters:														
*	IN Dbg_Level DLevel: The level of the debug logging. It will decide 
*		whether debug statement will go to standard output, or any of the 
*		log files.
*	IN Dbg_Module Module: debug will go in the name of this module
*	IN char *DbgFileName: Name of the file from where debug statement is
*							coming
*	IN int DbgLineNo : Line number of the file from where debug statement is
*							coming
*	IN char * FmtStr, ...: Variable number of arguments that will go in the 
*			debug statement
*																	
* Description:														
*	This functions prints the debug statement either on the startdard output
*	or log file along with the information from where this debug statement 
*	is coming
* Returns: void
***************************************************************************/ 
  void UpnpPrintf(Dbg_Level DLevel, Dbg_Module Module,char
			*DbgFileName, int DbgLineNo,char * FmtStr,
			...);

/************************************************************************
* Function : UpnpDisplayBanner											
*																	
* Parameters:														
*	IN FILE *fd: file descriptor where the banner will be written
*	IN char **lines: The buffer that will be written
*	IN int size: Size of the buffer
*	IN int starLength: This parameter provides the width of the banner
*																	
* Description:														
*	This functions takes the buffer and writes the buffer in the file as 
*	per the requested banner											
* Returns: void
***************************************************************************/
  void UpnpDisplayBanner(FILE *fd,
			 char **lines, int size, int starlength);

/**************************************************************************
* Function : UpnpDisplayFileAndLine											
*																	
* Parameters:														
*	IN FILE *fd: File descriptor where line number and file name will be 
*			written 
*	IN char *DbgFileName: Name of the file  
*	IN int DbgLineNo : Line number of the file
*																	
* Description:
*	This function writes the file name and file number from where
*		debug statement is coming to the log file
* Returns: void
***************************************************************************/
  void UpnpDisplayFileAndLine(FILE *fd,char *DbgFileName, int DbgLineNo);)

       //End of function declarations only for debug
#ifdef __cplusplus
}
#endif



#ifdef  INTERNAL_WEB_SERVER
#undef  EXCLUDE_WEB_SERVER 
#undef  EXCLUDE_MINISERVER 
#define EXCLUDE_WEB_SERVER 0
#define EXCLUDE_MINISERVER 0
#endif

#if EXCLUDE_GENA == 1 && EXCLUDE_SOAP == 1 && EXCLUDE_WEB_SERVER == 1
#undef  EXCLUDE_MINISERVER 
#undef  INTERNAL_WEB_SERVER
#define EXCLUDE_MINISERVER 1
#endif

#if EXCLUDE_GENA == 0 || EXCLUDE_SOAP == 0 || EXCLUDE_WEB_SERVER == 0
#undef  EXCLUDE_MINISERVER 
#define EXCLUDE_MINISERVER 0
#endif

#ifndef INTERNAL_WEB_SERVER
#if EXCLUDE_WEB_SERVER == 0
#define INTERNAL_WEB_SERVER
#endif
#endif


#ifdef INCLUDE_CLIENT_APIS
#define CLIENTONLY(x) x
#else 
#define CLIENTONLY(x)
#endif

#ifdef INCLUDE_DEVICE_APIS
#define DEVICEONLY(x) x
#else 
#define DEVICEONLY(x) 
#endif

//@}
#endif
