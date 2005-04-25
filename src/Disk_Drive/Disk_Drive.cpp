/*
 Disk_Drive

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty
 of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 See the GNU General Public License for more details.
*/

//<-dceag-d-b->
#include "Disk_Drive.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stropts.h>

#include <sstream>

#include <sys/wait.h>

extern "C"
{
    #include <linux/cdrom.h>
}
#include "pluto_main/Define_MediaType.h"

enum DiscTypes {
    DISCTYPE_NONE       = -1,
    DISCTYPE_CD_AUDIO   =  0,
    DISCTYPE_CD         =  1,
    DISCTYPE_DVD_VIDEO  =  2,
    DISCTYPE_DVD        =  3,
    DISCTYPE_BLANK      =  4,
    DISCTYPE_DATA       =  5,
    DISCTYPE_CD_MIXED   =  6,
    DISCTYPE_CD_VCD     =  7,
    DISCTYPE_CD_SVCD    =  8,
    DISCTYPE_DVD_AUDIO  =  9
};

#define SERVER_PORT 5150

//<-dceag-const-b->! custom
Disk_Drive::Disk_Drive(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
    : Disk_Drive_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter),
        m_monitorEnabled(true), m_mediaInserted(false), m_mediaDiskStatus(DISCTYPE_NONE), m_serverPid(-1), m_serverPort(SERVER_PORT)
//<-dceag-const-e->
{
}

//<-dceag-const2-b->!

//<-dceag-dest-b->
Disk_Drive::~Disk_Drive()
//<-dceag-dest-e->
{

}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool Disk_Drive::Register()
//<-dceag-reg-e->
{
    return Connect(PK_DeviceTemplate_get());
}

/*
	When you receive commands that are destined to one of your children,
	then if that child implements DCE then there will already be a separate class
	created for the child that will get the message.  If the child does not, then you will
	get all	commands for your children in ReceivedCommandForChild, where
	pDeviceData_Base is the child device.  If you handle the message, you
	should change the sCMD_Result to OK
*/
//<-dceag-cmdch-b->
void Disk_Drive::ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage)
//<-dceag-cmdch-e->
{
	sCMD_Result = "UNHANDLED CHILD";
}

/*
	When you received a valid command, but it wasn't for one of your children,
	then ReceivedUnknownCommand gets called.  If you handle the message, you
	should change the sCMD_Result to OK
*/
//<-dceag-cmduk-b->
void Disk_Drive::ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage)
//<-dceag-cmduk-e->
{
	sCMD_Result = "UNKNOWN DEVICE";
}


/*

    COMMANDS TO IMPLEMENT

*/

//<-dceag-c45-b->

	/** @brief COMMAND: #45 - Disk Drive Monitoring ON */
	/** Turn ON the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_ON(string &sCMD_Result,Message *pMessage)
//<-dceag-c45-e->
{
    g_pPlutoLogger->Write(LV_ACTION,"Turning ON the disk drive monitoring.");
    m_monitorEnabled = true;
    cdrom_lock(0);
}

//<-dceag-c46-b->

	/** @brief COMMAND: #46 - Disk Drive Monitoring OFF */
	/** Turn OFF the disk Monitoring. */

void Disk_Drive::CMD_Disk_Drive_Monitoring_OFF(string &sCMD_Result,Message *pMessage)
//<-dceag-c46-e->
{
    g_pPlutoLogger->Write(LV_STATUS,"Turning OFF the Disk Drive Monitoring.");
    m_monitorEnabled = false;
    cdrom_lock(1);
}

//<-dceag-c47-b->

	/** @brief COMMAND: #47 - Reset Disk Drive */
	/** Reset the disk drive. */

void Disk_Drive::CMD_Reset_Disk_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c47-e->
{
    m_mediaInserted = false;
    m_mediaDiskStatus = DISCTYPE_NONE;

    internal_reset_drive(true);
}

//<-dceag-c48-b->

	/** @brief COMMAND: #48 - Eject Disk */
	/** Eject the disk from the drive. */

void Disk_Drive::CMD_Eject_Disk(string &sCMD_Result,Message *pMessage)
//<-dceag-c48-e->
{
    cout << "Need to implement command #48 - Eject Disk" << endl;
}

//<-dceag-c49-b->

	/** @brief COMMAND: #49 - Start Burn Session */
	/** Initiates a new burning session. */

void Disk_Drive::CMD_Start_Burn_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c49-e->
{
    cout << "Need to implement command #49 - Start Burn Session" << endl;
}

//<-dceag-c50-b->

	/** @brief COMMAND: #50 - Start Ripping Session */
	/** Initiates a new ripping session. */

void Disk_Drive::CMD_Start_Ripping_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c50-e->
{
    cout << "Need to implement command #50 - Start Ripping Session" << endl;
}

//<-dceag-c51-b->

	/** @brief COMMAND: #51 - Add File To Burning Session */
	/** Add a new file to the initiated burning session. */

void Disk_Drive::CMD_Add_File_To_Burning_Session(string &sCMD_Result,Message *pMessage)
//<-dceag-c51-e->
{
    cout << "Need to implement command #51 - Add File To Burning Session" << endl;
}

//<-dceag-c52-b->

	/** @brief COMMAND: #52 - Start Burning */
	/** Starts burning. */

void Disk_Drive::CMD_Start_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c52-e->
{
    cout << "Need to implement command #52 - Start Burning" << endl;
}

//<-dceag-c53-b->

	/** @brief COMMAND: #53 - Abort Burning */
	/** Aborts the burning session. */

void Disk_Drive::CMD_Abort_Burning(string &sCMD_Result,Message *pMessage)
//<-dceag-c53-e->
{
    cout << "Need to implement command #53 - Abort Burning" << endl;
}

//<-dceag-c54-b->

	/** @brief COMMAND: #54 - Mount Disk Image */
	/** Will mount a disk image as a disk. */
		/** @param #13 Filename */
			/** What to mount. If it get's the Device name it will mount the actual disk in the drive. */
		/** @param #59 MediaURL */
			/** The URL which can be used to play the mounted media. */

void Disk_Drive::CMD_Mount_Disk_Image(string sFilename,string *sMediaURL,string &sCMD_Result,Message *pMessage)
//<-dceag-c54-e->
{
    g_pPlutoLogger->Write(LV_STATUS, "Got a mount media request %s", sFilename.c_str());
    string stringMRL;

	if ( ! mountDVD(sFilename, stringMRL) )
	{
		g_pPlutoLogger->Write(LV_WARNING, "Error executing the dvd mounting script (message: %s). Returning error.", stringMRL.c_str());
		sCMD_Result = "NOT_OK";
		*sMediaURL = stringMRL;
		return;
	}

	*sMediaURL += stringMRL;
	sCMD_Result = "OK";
	g_pPlutoLogger->Write(LV_STATUS, "Returning new media URL: %s", sMediaURL->c_str());
}

//<-dceag-c55-b->

	/** @brief COMMAND: #55 - Start Ripping DVD */
	/** Starts ripping a DVD. */

void Disk_Drive::CMD_Start_Ripping_DVD(string &sCMD_Result,Message *pMessage)
//<-dceag-c55-e->
{
    cout << "Need to implement command #55 - Start Ripping DVD" << endl;
}

//<-dceag-c56-b->

	/** @brief COMMAND: #56 - Format Drive */
	/** Formats a disk. */

void Disk_Drive::CMD_Format_Drive(string &sCMD_Result,Message *pMessage)
//<-dceag-c56-e->
{
    cout << "Need to implement command #56 - Format Drive" << endl;
}

//<-dceag-c57-b->

	/** @brief COMMAND: #57 - Close Tray */
	/** Closes the tray. */

void Disk_Drive::CMD_Close_Tray(string &sCMD_Result,Message *pMessage)
//<-dceag-c57-e->
{
    cout << "Need to implement command #57 - Close Tray" << endl;
}


void Disk_Drive::RunMonitorLoop()
{
    internal_monitor_step(false); // ignore any drive that is in the drive at the start.

    bool done = false;
    while ( ! done  && !m_bQuit )
    {
        done = ! internal_monitor_step(true);
        sleep(3); // Sleep 3 seconds
    }
}

bool Disk_Drive::internal_monitor_step(bool bFireEvent)
{
//  const int max_bytes = 1024;
//  static char buf[2][max_bytes];
//  static int offset[2]={0,0};
//  int i;

//  struct timeval tv;
//  fd_set rfds;

//  tv.tv_sec = 0;
//  tv.tv_usec = 200000;
    if ( ! internal_reset_drive(bFireEvent) )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Monitor drive returned false.");
        return false;
    }
/*
    if ( m_childOutFd == -1 || m_childErrFd == -1 )
    {
        struct timespec ts;
        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;
        nanosleep( &ts, NULL );
        return true;
    }

    FD_ZERO( &rfds );
    FD_SET( m_childOutFd, &rfds );
    FD_SET( m_childErrFd, &rfds );

    time_t timeout=time(NULL) + 3;
    while ( select( m_childErrFd+1, &rfds, NULL, NULL, &tv ) > 0 && time(NULL)<timeout )
    {
        for ( i = 0; i < m_childErrFd+1; ++i )
        {
            if ( FD_ISSET( i, &rfds ) )
            {
                int buf_idx = (i == m_childOutFd ? 0 : 1);
                char *buffer = buf[buf_idx];
                int o = offset[buf_idx];
                char *c;

                if ( read( i, buffer + o, 1 ) > 0 )
                {
                    buffer[++o] = '\0';
                    //printf("got from child: %s\n",buffer);
                    if ( ( c = strchr( buffer, '\r' ) ) != NULL )
                    {
                        *c = '\0';
                        parseChildStatus( buffer );
                        o = 0;
                    }
                    else if ( ( c = strchr( buffer, '\n' ) ) != NULL )
                    {
                        *c = '\0';
                        parseChildStatus( buffer );
                        o = 0;
                    }
                    offset[buf_idx] = o;
                }
            }
        }
    }*/
    return true;
}

/**
 *  Internal helper functions.
 */
int Disk_Drive::cdrom_lock( int lock )
{
  FILE *fp;

  if( (fp = fopen("/proc/sys/dev/cdrom/lock","w")) != NULL ) {
    fprintf(fp,"%d",lock);
    fclose(fp);
    return 1;
  } else {
    return 0;
  }
}

int Disk_Drive::cdrom_checkdrive (const char *filename, int *flag)
{
    int fd;
    int status;
    struct cdrom_tochdr th;

    // open the device
    fd = open (filename, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        g_pPlutoLogger->Write(LV_WARNING, "Error: couldn't open %s.", filename);
        return -1;
    }

//     g_pPlutoLogger->Write(LV_STATUS, "Disk Drive %s file handle was opened!", DATA_Get_Drive().c_str());
    // read the drive status info
    status = ioctl (fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
//     g_pPlutoLogger->Write(LV_STATUS, "Current disk status %d", status);

    switch (status)
    {
        // if there's a ok disc in there
    case CDS_DISC_OK:
//         g_pPlutoLogger->Write(LV_WARNING, "Media inserted value here: %d", m_mediaInserted);
//         g_pPlutoLogger->Write(LV_WARNING, "Disk type value here: %d", *flag);

        if (*flag != DISCTYPE_NONE || m_mediaInserted )
            break;

        g_pPlutoLogger->Write(LV_STATUS, "Got a disc. Sleep a sec, then reopen. One hack to allow the disk to spin I think.");
        close(fd);
        sleep(1);
        fd = open (filename, O_RDONLY | O_NONBLOCK);
        if (fd < 0)
        {
            g_pPlutoLogger->Write(LV_WARNING, "Error: couldn't open %s.", filename);
            return -1;
        }

        // read the drive status info
        status = ioctl (fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
        if( status!=CDS_DISC_OK )
        {
            g_pPlutoLogger->Write(LV_WARNING, "Disc was detected, but the status is no longer disc ok.");
            return 0;  // Don't change anything, we'll try again later
        }

        // see if we can read the disc's table of contents (TOC).
        status = ioctl (fd, CDROMREADTOCHDR, &th);
        if (status != 0)
        {
            g_pPlutoLogger->Write(LV_STATUS, "Can't read disc TOC. The disc is either a blank, or has a broken TOC.");
            *flag = DISCTYPE_BLANK;
            break;
        }

        // read disc status info
        status = ioctl (fd, CDROM_DISC_STATUS, CDSL_CURRENT);

        switch (status)
        {
        case CDS_AUDIO:
            // found a audio cd
            g_pPlutoLogger->Write(LV_STATUS, "Detected audio cd!");
            *flag = DISCTYPE_CD_AUDIO;
            break;

        case CDS_DATA_1:
        case CDS_DATA_2:
            {
                // Try up to 10 seconds to see what the directories are
                int result=-1;
                time_t timeout=time(NULL) + 10;
                while( result==-1 && timeout>time(NULL) )
                {
                    result = cdrom_has_dir (fd, "video_ts");
                    if( result==-1 )
                    {
                        g_pPlutoLogger->Write(LV_STATUS, "Not ready to read the directories yet.");
                        sleep(1);
                    }
                    g_pPlutoLogger->Write(LV_STATUS, "Result of video_ts search: %d.", result);
                }

                if ( result > 0 )
                {
                    g_pPlutoLogger->Write(LV_STATUS, "I think it's a DVD...");
                    *flag =DISCTYPE_DVD_VIDEO;
                }
                else if ( cdrom_has_dir (fd, "vcd") > 0 )
                {
                    g_pPlutoLogger->Write(LV_STATUS, "I think it's a VCD...");
                    *flag = DISCTYPE_CD_VCD;
                }
                else if ( cdrom_has_dir (fd, "svcd") > 0 )
                {
                    g_pPlutoLogger->Write(LV_STATUS, "I think it's a SVCD...");
                    *flag = DISCTYPE_CD_SVCD;
                }
                else
                {
                    g_pPlutoLogger->Write(LV_STATUS, "Doesn't have any directories -- must be data.");
                    *flag = DISCTYPE_DATA;
                }
            }
            break;

        case CDS_MIXED:
            // found a mixed cd
            g_pPlutoLogger->Write(LV_STATUS, "Detected mixed audio/data cd!");
            *flag = DISCTYPE_CD_MIXED;
            break;

        default:
            g_pPlutoLogger->Write(LV_WARNING, "Could not determine disc type: Doing nothing!");
            break;
        }
        break;

    case CDS_NO_INFO:
        // drive doesnt support querying, so this program will never work on that drive.
        g_pPlutoLogger->Write(LV_WARNING, "%s does not support status queries.", filename);

    default:
        // g_pPlutoLogger->Write(LV_STATUS, "Nothing interesting hapened");
        // release the device
        *flag = DISCTYPE_NONE;
    }
    close (fd);
    return 0;
}

string Disk_Drive::getTracks (string mrl, string &genre)
{
    g_pPlutoLogger->Write(LV_STATUS, "Finding CD tracks.");

//     time_t startTime=time(NULL);

    cddb_conn_t *conn = NULL;
    cddb_disc_t *disc = NULL;

    int fd = -1, status;
    string tracks = "";

    try {
        g_pPlutoLogger->Write(LV_STATUS, "Opening drive first time.");

        fd = open( DATA_Get_Drive().c_str(), O_RDONLY | O_NONBLOCK );
        if (fd < 0)
            throw string ("Failed to open CD device" + string (strerror (errno)));

        g_pPlutoLogger->Write(LV_STATUS, "Checking media with ioctl");
        status = ioctl( fd, CDROM_DISC_STATUS, CDSL_CURRENT );
        if ( status != CDS_AUDIO && status != CDS_MIXED )
            throw string ("Invalid media detected");

        conn = cddb_new();
        if (conn)
        {
            g_pPlutoLogger->Write(LV_STATUS, "Building cddb query parameters");
//          cddb_set_email_address( conn, "anonymous@localhost" );
//          cddb_set_server_name( conn, "freedb.freedb.org" );
//          cddb_set_server_port( conn, 8880 );
//          cddb_http_enable( conn, 0 );

            g_pPlutoLogger->Write(LV_STATUS, "Creating cddb disk structure!");
            disc = cddb_disc_new();
            if (disc)
            {
                g_pPlutoLogger->Write(LV_STATUS, "Reading disk toc");
                int result = read_toc( fd, conn, disc );
                bool found = false;
                if ( result < 0 )
                    throw string("Failed to read CDROM TOC.");

//              printf("before cddb_query(%ld) disc id: %d\n",time(NULL)-startTime,disc->discid);
//              result = cddb_query( conn, disc );
//              printf("cddb_query returned (%d)\n",(int) result);
//              if ( result > 0 )
//              {
/*                  printf("before cddb_read(%ld)\n",time(NULL)-startTime);
                    if ( cddb_read( conn, disc ) )
                    {
                        cddb_track_t *track = disc->tracks;
                        int i = 1;
                        ostringstream sb;

                        // make discid stateful, so it can be used when ripping to lookup track keys
                        m_discid = disc->discid;

                        Query query = m_mysql.create_Query();
                        query << "select item_dwID from DCL_item_attribute where s_attribute_type = 'FREEDB_ID' and attribute_val = '";
                        query << disc->discid << "'";
                        printf("executing query 1\n");
                        pthread_mutex_lock (&sqlLock);
                        Result_Store rows = query.store();
                        pthread_mutex_unlock (&sqlLock);
                        if (rows.size() > 0)
                        {
                            string itemID = rows[0][0];
                            printf("query 1 has rows: ID is %s\n",itemID.c_str());
                            ostringstream str;

                            str << "";
                            Query query = m_mysql.create_Query();
                            query << "select attribute_val FROM DCL_item_attribute";
                            query << " where item_dwID = " << itemID; //rows[0][0];
                            query << " and s_attribute_type = 'CDTRACK'";
                            query << " order by order_no";
                            pthread_mutex_lock (&sqlLock);
                            Result_Store rows = query.store();
                            pthread_mutex_unlock (&sqlLock);

                            for (Result_Store::iterator row = rows.begin(); row != rows.end(); ++row)
                            {
                                sb << mrl << i++ << "\t" << (*row)[0] << endl;
                            }
                            tracks = sb.str();
                            found = true;

                            printf("ready to execute query looking for genre\n");
                            query.str("");
                            query << "select attribute_val FROM DCL_item_attribute";
                            query << " where item_dwID = " << itemID;
                            query << " and s_attribute_type = 'CDDBGENRE'";

                            pthread_mutex_lock (&sqlLock);
                            Result_Store rows2 = query.store();
                            pthread_mutex_unlock (&sqlLock);

                            if( rows2.size() > 0 )
                            {
                                printf("Got it\n");
                                genre=rows2[0][0];
                                printf("Set genre to %s\n",genre.c_str());
                            }

                        }
                        else
                        {
                            printf("query 1 has 0 rows\n");

                            string title = string(disc->title);

                            query.str("");
                            query << "insert into DCL_item (title, s_item_type) values ('";
                            query << StringUtils::Replace(title, "'", "''") << "', 'CD')";
                            pthread_mutex_lock (&sqlLock);
                            Result_NoData queryResult = query.execute();
                            pthread_mutex_unlock (&sqlLock);
                            printf("executing query 2\n");
                            if (queryResult.get_succeeded())
                            {
        printf("executing rest of queries to add new disc properties\n");
        fflush(stdout);
                                result = queryResult.get_insert_id();
                                query.str("");
                                query << "insert into DCL_item_instance (item_dwID, instance_no, owner_id, s_status_type) values (";
                                query << result << ", 1, '1control', 'A')";
        printf("query 1 - a\n");
        fflush(stdout);
                                pthread_mutex_lock (&sqlLock);
        printf("query 1 - b\n");
        fflush(stdout);
                                query.execute();
        printf("query 1 - c\n");
        fflush(stdout);
                                pthread_mutex_unlock (&sqlLock);

                                query.str("");
                                query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                                query << result << ", 'FREEDB_ID', 0, '" << disc->discid << "')";
        printf("query 2 - a\n");
        fflush(stdout);
                                pthread_mutex_lock (&sqlLock);
                                query.execute();
                                pthread_mutex_unlock (&sqlLock);
        printf("query 2 - b\n");
        fflush(stdout);

                                string artist(disc->artist);
                                query.str("");
                                query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                                query << result << ", 'ARTIST', 10, '";
                                query << StringUtils::Replace(artist, string("'"), string("''")).c_str() << "')";
        printf("query 3 - a\n");
        fflush(stdout);
                                pthread_mutex_lock (&sqlLock);
                                query.execute();
                                pthread_mutex_unlock (&sqlLock);
        printf("query 3 - b\n");
        fflush(stdout);

                                query.str("");
                                genre = disc->genre;
                                printf("setting disc genre to %s",genre.c_str());
                                string s (disc->genre);
                                str_to_lwr (s);
                                query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                                query << result << ", 'CDDBGENRE', 30, '" << s << "')";
        printf("query 4 - a\n");
        fflush(stdout);
                                pthread_mutex_lock (&sqlLock);
                                query.execute();
                                pthread_mutex_unlock (&sqlLock);
        printf("query 4 - b\n");
        fflush(stdout);

                                query.str("");
                                int mins = disc->length / 60;
                                int secs = disc->length % 60;
                                query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                                query << result << ", 'CDTIME', 31, '" << mins << ":" << secs << "')";
        printf("query 5 - a\n");
        fflush(stdout);
                                pthread_mutex_lock (&sqlLock);
                                query.execute();
                                pthread_mutex_unlock (&sqlLock);
        printf("query 5 - b\n");
        fflush(stdout);

        //printf("track: %d\n",track);
        //fflush(stdout);
                                int seq = 50;
                                while (track)
                                {
                                    sb << mrl << i++ << "\t" << track->title << endl;

                                    string title(track->title);
                                    query.str("");
                                    query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                                    query << result << ", 'CDTRACK', " << seq++ <<", '";
                                    query << StringUtils::Replace(title, string("'"), string("''")).c_str() << "')";
        printf("query 6 track: %s\n",track->title);
        fflush(stdout);
                                    pthread_mutex_lock (&sqlLock);
                                    query.execute();
                                    pthread_mutex_unlock (&sqlLock);

                                    track = track->next;
                                }
                                tracks = sb.str();
        printf("found=true, tracks: %s\n",tracks.c_str());
        fflush(stdout);
                                found = true;
                            }
                        }
                    }*/
//                  }
//              else
//                  printf("cddb returned %d (%ld)\n",result,time(NULL)-startTime);

                if (!found)
                {
                    g_pPlutoLogger->Write(LV_STATUS, "CD was not found in CDDB archive. (This is currently disabled in the code)");
                    cddb_track_t *track = disc->tracks;
                    int i = 1;
                    ostringstream sb;

                    while (track)
                    {
                        sb << mrl << i << " Unknown Track " << i++ << endl;
                        track = track->next;
                    }

                    tracks = sb.str().c_str();
                }
            }
            else
                throw string ("unable to create libcddb disc structure" );
        }
        else
            throw string ("unable to initialize libcddb");
    }
    catch (string err)
    {
        g_pPlutoLogger->Write (LV_WARNING, "w1: %s",err.c_str());
    }
/*    catch (ex_base& e)
    {
        g_pPlutoLogger->Write (LV_WARNING, "w2: %s",e.what());
    }*/
    catch (...)
    {
        g_pPlutoLogger->Write (LV_WARNING, "Unknown error in getTracks()");
    }

    // printf("ready to destroy cddb info (%ld)\n",time(NULL)-startTime);
    if (conn != NULL)
        cddb_destroy( conn );
    if (disc != NULL)
        cddb_disc_destroy( disc );
    if (fd != -1)
        close( fd );

    // printf("Returning %d byte string as tracks\n",tracks.length());

    return tracks;
}

string Disk_Drive::cdrom_get_title (const int fd)
{
    cddb_conn_t *conn = NULL;
    cddb_disc_t *disc = cddb_disc_new();
    int result;
    ostringstream title;
    std::string fetched_name;

    conn = cddb_new();
    if (conn)
    {
        cddb_set_email_address( conn, "anonymous@localhost" );
        cddb_set_server_name( conn, "freedb.freedb.org" );
        cddb_set_server_port( conn, 8880 );
        cddb_http_enable( conn );
        disc = cddb_disc_new();
        if (disc)
        {
            result = cdrom_read_toc (fd, conn, disc);
            if (result < 0)
            {
                cddb_disc_destroy (disc);
                return "";
            }
            result = cddb_query (conn, disc);
            if (result <= 0)
            {
                //cddb_error_print (cddb_errno (conn) );
                return "Unrecognized Audio CD";
            }
            if (cddb_read (conn, disc))
            {
                title << disc->artist << " - " << disc->title << std::ends;
            }
            else
            {
                title << "Unrecognized Audio CD" << std::ends;
            }
            cddb_disc_destroy (disc);
        }
        else
        {
            throw ("unable to create disc structure" );
        }
        cddb_destroy( conn );
    }
    else
    {
        throw "could not create cddb connection";
    }
    return title.str();
}

#define DVD_VIDEO_LB_LEN 2048

string Disk_Drive::dvd_read_name(const int fd)
{
    int i;
    off_t off;
    uint8_t data[DVD_VIDEO_LB_LEN];
    ostringstream title, id;
    int result;


      off = lseek( fd, 32 * (int64_t) DVD_VIDEO_LB_LEN, SEEK_SET );
      if( off == ( 32 * (int64_t) DVD_VIDEO_LB_LEN ) ) {
        off = read( fd, data, DVD_VIDEO_LB_LEN );
        if (off == ( (int64_t) DVD_VIDEO_LB_LEN )) {
          for(i=25; i < 73; i++ ) {
            if((data[i] == 0)) break;
            if((data[i] > 32) && (data[i] < 127)) {
              title << (char) data[i];
            }
          }

          for(i=73; i < 89; i++ ) {
            if((data[i] == 0)) break;
            if((data[i] > 32) && (data[i] < 127)) {
                id << (char) data[i];
            }
          }

          // Update the mysql media library
/*          Query query = m_mysql.create_Query();
          query << "select item_dwID from DCL_item_attribute where s_attribute_type = 'FREEDB_ID' and attribute_val = '";
          query << id.str() << "'";
          pthread_mutex_lock (&sqlLock);
          Result_Store rows = query.store();
          pthread_mutex_unlock (&sqlLock);
          if (rows.size() == 0)
          {
              string strTitle(title.str());

              query.str("");
              query << "insert into DCL_item (title, s_item_type) values ('";
              query << StringUtils::Replace(strTitle, string("'"), string("''")).c_str() << "', 'DVD')";
              pthread_mutex_lock (&sqlLock);
              Result_NoData queryResult = query.execute();
              pthread_mutex_unlock (&sqlLock);
              if (queryResult.get_succeeded())
              {
                  result = queryResult.get_insert_id();
                  m_what_is_ripping=result; // Set this now so it will be correct if we decide to rip it
                  query.str("");
                  query << "insert into DCL_item_instance (item_dwID, instance_no, owner_id, s_status_type) values (";
                  query << result << ", 1, '1control', 'A')";
                  pthread_mutex_lock (&sqlLock);
                  query.execute();
                  pthread_mutex_unlock (&sqlLock);

                  query.str("");
                  query << "insert into DCL_item_attribute (item_dwID, s_attribute_type, order_no, attribute_val) values (";
                  query << result << ", 'FREEDB_ID', 0, '" << id.str() << "')";
                  pthread_mutex_lock (&sqlLock);
                  query.execute();
                  pthread_mutex_unlock (&sqlLock);
              }
              else
                  result = -1;*/
//         } else {
//             // We might rip a DVD that's already listed in the database
//             int what_might_rip;
//             what_might_rip=atoi(rows[0][0]);
//             printf("DVD already in that might rip is %d\n",what_might_rip);
//             m_what_is_ripping=what_might_rip;
//         }

        } else {
          //fprintf(stderr, "Can't read name block. Probably not a DVD-ROM device.\n");
            result = -1;
        }
      } else {
        //fprintf(stderr, "Can't seek to block %u\n", 32 );
          result = -1;
      }
      return title.str();
}

bool Disk_Drive::mountDVD(string fileName, string &strMediaUrl)
{
	// True if we're playing a physical dis, false if it's a file
	bool bDriveMount = StringUtils::StartsWith(fileName,"/dev/",true);

	string sDrive = bDriveMount ? fileName : DATA_Get_Drive();
	if( sDrive.length()==0 )
		sDrive = "/dev/cdrom";

	string cmd = "ln -sf " + sDrive + " /dev/dvd";
	g_pPlutoLogger->Write(LV_STATUS,"cmd drivemount: %d - %s",(int) bDriveMount,cmd.c_str());
	system(cmd.c_str());  // Don't care about the return.  Just making sure it's not loop 5 so we can delete it
	if( bDriveMount )
	{
g_pPlutoLogger->Write(LV_ACTION,"returning mounted drive");
		strMediaUrl = "dvd:/";
Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		return true;
	}

	cmd = "losetup -d /dev/loop5";
	g_pPlutoLogger->Write(LV_STATUS,"cmd: %s",cmd.c_str());
	system(cmd.c_str());  // Don't care about the return. 

    cmd = "losetup /dev/loop5 \"" + fileName + "\"";
	g_pPlutoLogger->Write(LV_STATUS,"cmd: %s",cmd.c_str());
    int iResult2=0,iResult = system(cmd.c_str());
	if( iResult==0 )
	{
Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		cmd = "ln -sf /dev/loop5 /dev/dvd";
		g_pPlutoLogger->Write(LV_STATUS,"cmd: %s",cmd.c_str());
		iResult2 = system(cmd.c_str());
	}

	if( iResult==0 && iResult2==0 )
	{
g_pPlutoLogger->Write(LV_ACTION,"returning mounted dvd");
		strMediaUrl = "dvd:/";
Sleep(500); // TODO: HACK  -- sometimes xine can't play dvd's.  Throw a small delay in to see if it has an effect
		return true;
	}

g_pPlutoLogger->Write(LV_CRITICAL,"Failed to mount %d %d",iResult,iResult2);
	cmd = "ln -sf " + sDrive + " /dev/dvd";
	g_pPlutoLogger->Write(LV_STATUS,"cmd: %s",cmd.c_str());
	system(cmd.c_str());  // Can't do anything if it fails

	return false;
}

int Disk_Drive::cdrom_has_dir (int fd, const char *directory)
{

    int ret = 0;        // return value
    unsigned short bs;  // the discs block size
    unsigned short ts;  // the path table size
    unsigned int tl;    // the path table location (in blocks)
    unsigned int i;
    unsigned int len_di = 0;    // length of the directory name in current path table entry
    unsigned int parent = 0;    // the number of the parent directory's path table entry
    char *dirname = NULL;   // filename for the current path table entry
    int pos = 0;        // our position into the path table
    int curr_record = 1;    // the path table record we're on

    // read the block size
    lseek (fd, 0x8080, SEEK_SET);
    int iFirstRead = read (fd, &bs, 2);

    g_pPlutoLogger->Write(LV_STATUS, "Looking for folder: %s.", directory);

    g_pPlutoLogger->Write(LV_STATUS, "FirstRead: %d fd: %d  directory: %s", iFirstRead, fd, directory);
    if( iFirstRead < 0 )
    {
        g_pPlutoLogger->Write(LV_STATUS, "Try again - Cannot read from drive.");
        return -1;
    }

    // read in size of path table
    lseek (fd, 2, SEEK_CUR);
    read (fd, &ts, 2);

    // read in which block path table is in
    lseek (fd, 6, SEEK_CUR);
    read (fd, &tl, 4);

    // seek to the path table
    lseek (fd, ((int) (bs) * tl), SEEK_SET);

    g_pPlutoLogger->Write(LV_STATUS, "Ready to loop through ts: %d.",ts);

    // loop through the path table entries
    while (pos < ts)
    {
        // get the length of the filename of the current entry
        int iRead = read (fd, &len_di, 1);

        // get the record number of this entry's parent
        // i'm pretty sure that the 1st entry is always the top directory
        int iSeek = lseek (fd, 5, SEEK_CUR);
        int iRead2 = read (fd, &parent, 2);

        // allocate and zero a string for the filename
        dirname = (char *) malloc (len_di + 1);
        for (i = 0; i < len_di + 1; i++)
            dirname[i] = '\0';

        // read the name
        int iRead3 = read (fd, dirname, len_di);

        g_pPlutoLogger->Write(LV_STATUS, "Directory: %s parent: %d (pos: %d  ts: %d read: %d read2: %d  read3: %d  seek: %d)",
                    dirname, parent,
                    pos, ts, iRead, iRead2, iRead3, iSeek);

        // if we found a folder that has the root as a parent, and the directory name matches
        // then return success
        if ((parent == 1) && (strcasecmp (dirname, directory) == 0))
        {
            g_pPlutoLogger->Write(LV_STATUS, "It's a match.");
            ret = 1;
            free (dirname);
            break;
        }

        free (dirname);

        // all path table entries are padded to be even, so if this is an odd-length table, seek a byte to fix it
        if (len_di % 2 == 1)
        {
            lseek (fd, 1, SEEK_CUR);
            pos++;
        }

        // update our position
        pos += 8 + len_di;
        curr_record++;
    }

    return ret;
}

#define CD_SECONDS_PER_MINUTE   60
#define CD_FRAMES_PER_SECOND    75
#define CD_LEADOUT_TRACK        0xAA
int Disk_Drive::read_toc( int fd, cddb_conn_t *c, cddb_disc_t *d )
{
    struct cdrom_tochdr tochdr;
    struct cdrom_tocentry tocentry;
    int i, total_tracks;
    cddb_track_t *t = NULL;

    /* fetch the table of contents */
    if (ioctl(fd, CDROMREADTOCHDR, &tochdr) == -1)
    {
        perror("CDROMREADTOCHDR");
        return errno;
    }
    total_tracks = tochdr.cdth_trk1 - tochdr.cdth_trk0 + 1;

    /* fetch each toc entry */
    for (i = tochdr.cdth_trk0; i <= tochdr.cdth_trk1; i++)
    {
        memset( &tocentry, 0, sizeof(tocentry) );

        tocentry.cdte_track = i;
        tocentry.cdte_format = CDROM_MSF;
        if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) == -1)
        {
            perror("CDROMREADTOCENTRY");
            return errno;
        }

        t = cddb_track_new();
        t->frame_offset =
            (tocentry.cdte_addr.msf.minute * CD_SECONDS_PER_MINUTE * CD_FRAMES_PER_SECOND) +
            (tocentry.cdte_addr.msf.second * CD_FRAMES_PER_SECOND) +
            tocentry.cdte_addr.msf.frame;
        cddb_disc_add_track( d, t );
    }

    /* fetch the leadout to get total duration */
    memset( &tocentry, 0, sizeof(tocentry) );
    tocentry.cdte_track = CD_LEADOUT_TRACK;
    tocentry.cdte_format = CDROM_MSF;
    if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) == -1)
    {
        perror("CDROMREADTOCENTRY");
        return errno;
    }
    d->length = tocentry.cdte_addr.msf.minute * CD_SECONDS_PER_MINUTE + tocentry.cdte_addr.msf.second;
    d->discid = cddb_disc_calc_discid( d );

    return 0;
}


/* subroutine from cdde */
#define CD_SECONDS_PER_MINUTE   60
#define CD_FRAMES_PER_SECOND    75
#define CD_LEADOUT_TRACK        0xAA
int Disk_Drive::cdrom_read_toc( int fd, cddb_conn_t *c, cddb_disc_t *d )
{
    struct cdrom_tochdr tochdr;
    struct cdrom_tocentry tocentry;
    int i, total_tracks;
    cddb_track_t *t = NULL;

    /* fetch the table of contents */
    if (ioctl(fd, CDROMREADTOCHDR, &tochdr) == -1)
    {
        perror("CDROMREADTOCHDR");
        return errno;
    }
    total_tracks = tochdr.cdth_trk1 - tochdr.cdth_trk0 + 1;

    /* fetch each toc entry */
    for (i = tochdr.cdth_trk0; i <= tochdr.cdth_trk1; i++)
    {
        memset( &tocentry, 0, sizeof(tocentry) );

        tocentry.cdte_track = i;
        tocentry.cdte_format = CDROM_MSF;
        if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) == -1)
        {
            perror("CDROMREADTOCENTRY");
            return errno;
        }

        t = cddb_track_new();
        t->frame_offset =
            (tocentry.cdte_addr.msf.minute * CD_SECONDS_PER_MINUTE * CD_FRAMES_PER_SECOND) +
            (tocentry.cdte_addr.msf.second * CD_FRAMES_PER_SECOND) +
            tocentry.cdte_addr.msf.frame;
        cddb_disc_add_track( d, t );
    }

    /* fetch the leadout to get total duration */
    memset( &tocentry, 0, sizeof(tocentry) );
    tocentry.cdte_track = CD_LEADOUT_TRACK;
    tocentry.cdte_format = CDROM_MSF;
    if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) == -1)
    {
        perror("CDROMREADTOCENTRY");
        return errno;
    }
    d->length = tocentry.cdte_addr.msf.minute * CD_SECONDS_PER_MINUTE + tocentry.cdte_addr.msf.second;
    d->discid = cddb_disc_calc_discid( d );

    return 0;
}

bool Disk_Drive::internal_reset_drive(bool bFireEvent)
{
    int status;
    string genre;
    string mrl = ""; //, serverMRL, title;

    int result = cdrom_checkdrive( DATA_Get_Drive().c_str(), &m_mediaDiskStatus);

    //     g_pPlutoLogger->Write(LV_STATUS, "Disc Reset: checkdrive status: %d  result: %d", m_mediaDiskStatus, result);

    // we only care if a new CD was inserted in the meantime.
    if (result >= 0 && m_mediaDiskStatus != DISCTYPE_NONE && m_mediaInserted == false)
    {
        int fd = open( DATA_Get_Drive().c_str(), O_RDONLY | O_NONBLOCK );
        if (fd < 0)
        {
            g_pPlutoLogger->Write(LV_WARNING, "Failed to open device: %s", DATA_Get_Drive().c_str());
            // throw ("failed to open cdrom device" );
            return false;
        }

        mrl = DATA_Get_Drive().c_str();
        switch (m_mediaDiskStatus)
        {
            case DISCTYPE_CD_MIXED: // treat a mixed CD as an audio CD for now.
            case DISCTYPE_CD_AUDIO:
                mrl = getTracks("cdda:/", genre).c_str();
                // mrl = getTracks ("cdda:/", genre);
//              serverMRL = getTracks ("cdda:/" + m_MyIPAddress + ":" + DVDCSS_SERVER_PORT + "/", genre);
//              title = cdrom_get_title(fd);
                status = MEDIATYPE_pluto_CD_CONST;
                break;

            case DISCTYPE_DVD_VIDEO:
                mrl = DATA_Get_Drive();
                // "dvd:/";
//              serverMRL = "dvd:/" + m_MyIPAddress + ": " + DVDCSS_SERVER_PORT + "/";
//              title = dvd_read_name (fd);
                status = MEDIATYPE_pluto_DVD_CONST;
                break;

            case DISCTYPE_BLANK:
                status = MEDIATYPE_misc_BlankMedia_CONST;
                break;

            case DISCTYPE_CD_VCD:
                status = MEDIATYPE_pluto_StoredVideo_CONST;
                break;

            default:
                status = MEDIATYPE_pluto_CD_CONST;
                break;
        }
        close (fd);

        g_pPlutoLogger->Write(LV_WARNING, "Disc of type %d was detected", status, mrl.c_str());

//         if ( ! serverStarted() && (status == MEDIATYPE_pluto_DVD_CONST || status == MEDIATYPE_pluto_StoredAudio_CONST))
//         {
//             startServer(result);
//
//         }

        if ( bFireEvent )
        {
            g_pPlutoLogger->Write(LV_WARNING, "One Media Inserted event fired (%s)", mrl.c_str());
            EVENT_Media_Inserted(status, mrl);
        }
        else
        {
            g_pPlutoLogger->Write(LV_WARNING, "Not firing the event");
        }

        m_mediaInserted = true;
//      GetEvents()->Media_Inserted(
//      mrl.c_str(), str_type, title.c_str(),genre.c_str());
/**
        Set_MRL_LOCAL (mrl.c_str());
        Set_MRL_EXTERNAL (serverMRL.c_str());
        Set_MEDIA_TYPE (str_type);
        Set_TITLE (title);
*/
    }

    // we mark media as not inserted on error or when the code tell us that no CD is in the unit.
    if (result < 0 || m_mediaDiskStatus == DISCTYPE_NONE )
    {
        if ( m_mediaInserted == true )
        {
            m_mediaInserted = false;
            g_pPlutoLogger->Write(LV_STATUS, "Disk is not in the drive at the moment");
        }
    }

    return true;
//  cout << "Need to implement command #47 - Reset Disk Drive" << endl;
}

//<-dceag-sample-b->! no sample
//<-dceag-createinst-b->!
//<-dceag-c337-b->

	/** @brief COMMAND: #337 - Rip Disk */
	/** This will try to RIP a DVD to the HDD. */
		/** @param #17 PK_Users */
			/** The user who needs this rip in his private area. */
		/** @param #50 Name */
			/** The target disk name. */

void Disk_Drive::CMD_Rip_Disk(int iPK_Users,string sName,string &sCMD_Result,Message *pMessage)
//<-dceag-c337-e->
{
	if ( m_isRipping )
	{
		EVENT_Ripping_Completed(RIP_RESULT_ALREADY_RIPPING, sName);
		return;
	}

	if ( ! m_mediaInserted )
	{
		EVENT_Ripping_Completed(RIP_RESULT_NO_DISC, sName);
		return;
	}

	if ( m_mediaDiskStatus != DISCTYPE_CD_AUDIO && m_mediaDiskStatus != DISCTYPE_DVD_VIDEO && m_mediaDiskStatus != DISCTYPE_CD_MIXED && m_mediaDiskStatus != DISCTYPE_CD_VCD )
	{
		EVENT_Ripping_Completed(RIP_RESULT_INVALID_DISC_TYPE, sName);
		return;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Launching ripping job with name \"%s\" for disk with type \"%d\"", sName.c_str(), m_mediaDiskStatus );

	string strParameters, strCommOnFailure, strCommOnSuccess;

	// use temp variables since the Replace function changes the input string
	string quotedDeviceName = DATA_Get_Drive();
	string quotedJobName = sName;

	strParameters = StringUtils::Format("%d %d %s %s %d %d",
			m_dwPK_Device,
			pMessage->m_dwPK_Device_From,
			StringUtils::Replace(quotedJobName, " ", "\\ ").c_str(),
			StringUtils::Replace(quotedDeviceName, " ", "\\ ").c_str(),
			m_mediaDiskStatus, iPK_Users);

    DCE::CMD_Spawn_Application_DT
		spawnApplication(m_dwPK_Device,
						DEVICETEMPLATE_App_Server_CONST,
						BL_SameComputer,
						"/usr/pluto/bin/ripDiskWrapper.sh", sName, strParameters,
						"", "", "0");

    spawnApplication.m_pMessage->m_bRelativeToSender = true;
    SendCommand(spawnApplication);
}
