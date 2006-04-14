#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "id3info.h"

#include <id3/tag.h>
#include <id3/utils.h>
#include <id3/misc_support.h>
#include <id3/readers.h>
//#include <id3/io_helpers.h>
#include <id3/globals.h>

#include "../../pluto_media/Define_AttributeType.h"

using namespace dami;
using std::cout;
using std::endl;

#include <map>
using namespace std;

void GetInformation(const ID3_Tag &myTag, map<int,string>& mapAttributes)
{
  ID3_Tag::ConstIterator* iter = myTag.CreateIterator();
  const ID3_Frame* frame = NULL;
  while (NULL != (frame = iter->GetNext()))
  {
    const char* desc = frame->GetDescription();
	string id = frame->GetTextID();
	
    if (!desc) desc = "";

	cout << "=== " << id << " (" << desc << "): ";

	int PK_Attr = 0;
	if(id == "TALB")
		PK_Attr = ATTRIBUTETYPE_Album_CONST;
	else if(id == "TIT2")
		PK_Attr = ATTRIBUTETYPE_Title_CONST;
	else if(id == "TPE1")
		PK_Attr = ATTRIBUTETYPE_Performer_CONST;
	else if(id == "TCON")
		PK_Attr = ATTRIBUTETYPE_Genre_CONST;
	else if(id == "TRCK")
		PK_Attr = ATTRIBUTETYPE_Track_CONST;
	else if(id == "TYER")
		PK_Attr = ATTRIBUTETYPE_Release_Date_CONST;
	else if(id == "COMM") //comments
		PK_Attr = -2; // todo ask
	else if(id == "TMED") //media
		PK_Attr = -3; // todo ask
	else if(id == "TLEN") //length
		PK_Attr = -4; // todo ask;
    else if(id == "WXXX")
	    PK_Attr = ATTRIBUTETYPE_Website_CONST; 
	else if(id == "TCOM")
	    PK_Attr = ATTRIBUTETYPE_Composer_CONST;
	else if(id == "TENC") //encoded by
        PK_Attr = -5; // todo ask
    else if(id == "TCOP") //copyright
        PK_Attr = -6; // todo ask
    else if(id == "TOPE") //Original artist(s)/performer(s)
        PK_Attr = -7; // todo ask;
	else if(id == "TXXX") //used defined text
		PK_Attr = Internal_UserDefinedText_CONST; 
	else
		PK_Attr = -1000;

	string value;
	
    ID3_FrameID eFrameID = frame->GetID();
    switch (eFrameID)
    {
      case ID3FID_ALBUM:
      case ID3FID_BPM:
      case ID3FID_COMPOSER:
      case ID3FID_CONTENTTYPE:
      case ID3FID_COPYRIGHT:
      case ID3FID_DATE:
      case ID3FID_PLAYLISTDELAY:
      case ID3FID_ENCODEDBY:
      case ID3FID_LYRICIST:
      case ID3FID_FILETYPE:
      case ID3FID_TIME:
      case ID3FID_CONTENTGROUP:
      case ID3FID_TITLE:
      case ID3FID_SUBTITLE:
      case ID3FID_INITIALKEY:
      case ID3FID_LANGUAGE:
      case ID3FID_SONGLEN:
      case ID3FID_MEDIATYPE:
      case ID3FID_ORIGALBUM:
      case ID3FID_ORIGFILENAME:
      case ID3FID_ORIGLYRICIST:
      case ID3FID_ORIGARTIST:
      case ID3FID_ORIGYEAR:
      case ID3FID_FILEOWNER:
      case ID3FID_LEADARTIST:
      case ID3FID_BAND:
      case ID3FID_CONDUCTOR:
      case ID3FID_MIXARTIST:
      case ID3FID_PARTINSET:
      case ID3FID_PUBLISHER:
      case ID3FID_TRACKNUM:
      case ID3FID_RECORDINGDATES:
      case ID3FID_NETRADIOSTATION:
      case ID3FID_NETRADIOOWNER:
      case ID3FID_SIZE:
      case ID3FID_ISRC:
      case ID3FID_ENCODERSETTINGS:
      case ID3FID_YEAR:
      {
        char *sText = ID3_GetString(frame, ID3FN_TEXT);
        cout << sText << endl;
		value = sText;
        delete [] sText;
        break;
      }
      case ID3FID_USERTEXT:
      {
        char 
        *sText = ID3_GetString(frame, ID3FN_TEXT), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
        cout << "(" << sDesc << "): " << sText << endl;
		value = sText; //ignoring Description
        delete [] sText;
        delete [] sDesc;
        break;
      }
      case ID3FID_COMMENT:
      case ID3FID_UNSYNCEDLYRICS:
      {
        char 
        *sText = ID3_GetString(frame, ID3FN_TEXT), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION), 
        *sLang = ID3_GetString(frame, ID3FN_LANGUAGE);
        cout << "(" << sDesc << ")[" << sLang << "]: "
             << sText << endl;

		value = string("(") + sDesc + ")[" + sLang + "]: " + sText;
		
        delete [] sText;
        delete [] sDesc;
        delete [] sLang;
        break;
      }
      case ID3FID_WWWAUDIOFILE:
      case ID3FID_WWWARTIST:
      case ID3FID_WWWAUDIOSOURCE:
      case ID3FID_WWWCOMMERCIALINFO:
      case ID3FID_WWWCOPYRIGHT:
      case ID3FID_WWWPUBLISHER:
      case ID3FID_WWWPAYMENT:
      case ID3FID_WWWRADIOPAGE:
      {
        char *sURL = ID3_GetString(frame, ID3FN_URL);
        cout << sURL << endl;
		value = sURL;
        delete [] sURL;
        break;
      }
      case ID3FID_WWWUSER:
      {
        char 
        *sURL = ID3_GetString(frame, ID3FN_URL),
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION);
        cout << "(" << sDesc << "): " << sURL << endl;
		value = sURL;
        delete [] sURL;
        delete [] sDesc;
        break;
      }
      case ID3FID_INVOLVEDPEOPLE:
      {
        size_t nItems = frame->GetField(ID3FN_TEXT)->GetNumTextItems();
        for (size_t nIndex = 0; nIndex < nItems; nIndex++)
        {
          char *sPeople = ID3_GetString(frame, ID3FN_TEXT, nIndex);
          cout << sPeople;
		  value += sPeople;
          delete [] sPeople;
          if (nIndex + 1 < nItems)
          {
            cout << ", ";
			value += ", ";
          }
        }
        cout << endl;
        break;
      }
      case ID3FID_PICTURE:
      {
        char
        *sMimeType = ID3_GetString(frame, ID3FN_MIMETYPE),
        *sDesc     = ID3_GetString(frame, ID3FN_DESCRIPTION),
        *sFormat   = ID3_GetString(frame, ID3FN_IMAGEFORMAT);
        size_t
        nPicType   = frame->GetField(ID3FN_PICTURETYPE)->Get(),
        nDataSize  = frame->GetField(ID3FN_DATA)->Size();
        cout << "(" << sDesc << ")[" << sFormat << ", "
             << int(nPicType) << "]: " << sMimeType << ", " << int(nDataSize)
             << " bytes" << endl;

		value = string("(") + sDesc + ")[" + sFormat + "]: " + sMimeType;
        delete [] sMimeType;
        delete [] sDesc;
        delete [] sFormat;
        break;
      }
      case ID3FID_GENERALOBJECT:
      {
        char 
        *sMimeType = ID3_GetString(frame, ID3FN_MIMETYPE), 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION), 
        *sFileName = ID3_GetString(frame, ID3FN_FILENAME);
        size_t 
        nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << "(" << sDesc << ")[" 
             << sFileName << "]: " << sMimeType << ", " << int(nDataSize)
             << " bytes" << endl;
		value = string("(") + sDesc + ")[" + sFileName + "]: " + sMimeType;
        delete [] sMimeType;
        delete [] sDesc;
        delete [] sFileName;
        break;
      }
      case ID3FID_UNIQUEFILEID:
      {
        char *sOwner = ID3_GetString(frame, ID3FN_OWNER);
        size_t nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << sOwner << ", " << int(nDataSize)
             << " bytes" << endl;
		value = sOwner;
        delete [] sOwner;
        break;
      }
      case ID3FID_PLAYCOUNTER:
      {
        size_t nCounter = frame->GetField(ID3FN_COUNTER)->Get();
        cout << int(nCounter) << endl;
        break;
      }
      case ID3FID_POPULARIMETER:
      {
        char *sEmail = ID3_GetString(frame, ID3FN_EMAIL);
        size_t
        nCounter = frame->GetField(ID3FN_COUNTER)->Get(),
        nRating = frame->GetField(ID3FN_RATING)->Get();
        cout << sEmail << ", counter=" 
             << int(nCounter) << " rating=" << int(nRating) << endl;
		value = sEmail;
        delete [] sEmail;
        break;
      }
      case ID3FID_CRYPTOREG:
      case ID3FID_GROUPINGREG:
      {
        char *sOwner = ID3_GetString(frame, ID3FN_OWNER);
        size_t 
        nSymbol = frame->GetField(ID3FN_ID)->Get(),
        nDataSize = frame->GetField(ID3FN_DATA)->Size();
        cout << "(" << int(nSymbol) << "): " << sOwner
             << ", " << int(nDataSize) << " bytes" << endl;
        break;
      }
      case ID3FID_SYNCEDLYRICS:
      {
		  /*
        char 
        *sDesc = ID3_GetString(frame, ID3FN_DESCRIPTION), 
        *sLang = ID3_GetString(frame, ID3FN_LANGUAGE);
        size_t
        nTimestamp = frame->GetField(ID3FN_TIMESTAMPFORMAT)->Get(),
        nRating = frame->GetField(ID3FN_CONTENTTYPE)->Get();
        const char* format = (2 == nTimestamp) ? "ms" : "frames";
        cout << "(" << sDesc << ")[" << sLang << "]: ";
        switch (nRating)
        {
          case ID3CT_OTHER:    cout << "Other"; break;
          case ID3CT_LYRICS:   cout << "Lyrics"; break;
          case ID3CT_TEXTTRANSCRIPTION:     cout << "Text transcription"; break;
          case ID3CT_MOVEMENT: cout << "Movement/part name"; break;
          case ID3CT_EVENTS:   cout << "Events"; break;
          case ID3CT_CHORD:    cout << "Chord"; break;
          case ID3CT_TRIVIA:   cout << "Trivia/'pop up' information"; break;
        }
        cout << endl;
        ID3_Field* fld = frame->GetField(ID3FN_DATA);
        if (fld)
        {
          ID3_MemoryReader mr(fld->GetRawBinary(), ID3_Reader::size_type(fld->BinSize()));
          while (!mr.atEnd())
          {
            //cout << io::readString(mr).c_str();
            //cout << " [" << io::readBENumber(mr, sizeof(uint32)) << " " 
            //     << format << "] ";
          }
        }
        cout << endl;
        delete [] sDesc;
        delete [] sLang;
		*/
        break;
      }
      case ID3FID_AUDIOCRYPTO:
      case ID3FID_EQUALIZATION:
      case ID3FID_EVENTTIMING:
      case ID3FID_CDID:
      case ID3FID_MPEGLOOKUP:
      case ID3FID_OWNERSHIP:
      case ID3FID_PRIVATE:
      case ID3FID_POSITIONSYNC:
      case ID3FID_BUFFERSIZE:
      case ID3FID_VOLUMEADJ:
      case ID3FID_REVERB:
      case ID3FID_SYNCEDTEMPO:
      case ID3FID_METACRYPTO:
      {
        cout << " (unimplemented)" << endl;
        break;
      }
      default:
      {
        cout << " frame" << endl;
        break;
      }
    }

	mapAttributes[PK_Attr] = value;
  }
  delete iter;
}

void GetId3Info(string sFilename, map<int,string>& mapAttributes)
{
	ID3_Tag myTag; 
	myTag.Link(sFilename.c_str(), ID3TT_ALL);
	myTag.GetMp3HeaderInfo();
    GetInformation(myTag, mapAttributes);
}

size_t ID3_RemoveComposers(ID3_Tag *tag)
{
    size_t num_removed = 0;
    ID3_Frame *frame = NULL;

    if (NULL == tag)
    {
        return num_removed;
    }

    while ((frame = tag->Find(ID3FID_COMPOSER)))
    {
        frame = tag->RemoveFrame(frame);
        delete frame;
        num_removed++;
    }

    return num_removed;
} 

ID3_Frame* ID3_AddComposer(ID3_Tag *tag, const char *text, bool replace = true)
{
    ID3_Frame* frame = NULL;
    if (NULL != tag && NULL != text && strlen(text) > 0)
    {
		cout << "replace: " << replace << endl;
        if (replace)
        {
            ID3_RemoveComposers(tag);
        }
        if (replace || tag->Find(ID3FID_COMPOSER) == NULL)
        {
            frame = new ID3_Frame(ID3FID_COMPOSER);
            if (frame)
            {
                frame->GetField(ID3FN_TEXT)->Set(text);
                tag->AttachFrame(frame);
            }
        }
    }

    return frame;
} 

size_t ID3_RemoveUserDefinedText(ID3_Tag *tag)
{
	size_t num_removed = 0;
	ID3_Frame *frame = NULL;

	if (NULL == tag)
	{
		return num_removed;
	}

	while ((frame = tag->Find(ID3FID_USERTEXT)))
	{
		frame = tag->RemoveFrame(frame);
		delete frame;
		num_removed++;
	}

	return num_removed;
} 

ID3_Frame* ID3_AddUserDefinedText(ID3_Tag *tag, const char *text, bool replace = true)
{
	ID3_Frame* frame = NULL;
	if (NULL != tag && NULL != text && strlen(text) > 0)
	{
		cout << "replace: " << replace << endl;
		if (replace)
		{
			ID3_RemoveUserDefinedText(tag);
		}
		if (replace || tag->Find(ID3FID_USERTEXT) == NULL)
		{
			frame = new ID3_Frame(ID3FID_USERTEXT);
			if (frame)
			{
				frame->GetField(ID3FN_TEXT)->Set(text);
				tag->AttachFrame(frame);
			}
		}
	}

	return frame;
} 

void SetId3Info(string sFilename, const map<int,string>& mapAttributes)
{
    ID3_Tag myTag; 
    myTag.Link(sFilename.c_str());

    for(map<int,string>::const_iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
    {
        cout << "Writing: PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

        int PK_Attr = (*it).first;
        string sValue = (*it).second;

        switch(PK_Attr)
        {
            case ATTRIBUTETYPE_Performer_CONST:
                ID3_AddArtist(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Album_CONST:
                ID3_AddAlbum(&myTag, sValue.c_str(), true);
                break;

            case ATTRIBUTETYPE_Song_CONST:
            case ATTRIBUTETYPE_Title_CONST:
                ID3_AddTitle(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Release_Date_CONST:
                ID3_AddYear(&myTag, sValue.c_str(), true);
                break;

            /*
            case ATTRIBUTETYPE_Comment_CONST: //todo
                ID3_AddComment(&myTag, sValue.c_str(), "", true); 
                break;
            */

            case ATTRIBUTETYPE_Genre_CONST:
                ID3_AddGenre(&myTag, sValue.c_str(), true); 
                break;

            case ATTRIBUTETYPE_Track_CONST:
                ID3_AddTrack(&myTag, atoi(sValue.c_str()), 0, true); 
                break;

            case ATTRIBUTETYPE_Composer_CONST:
                ID3_AddComposer(&myTag, sValue.c_str(), true);
				break;
			
			case Internal_UserDefinedText_CONST:
				ID3_AddUserDefinedText(&myTag, sValue.c_str(), true);
				break;

            default:
                cout << "Don't know yet how to save tag with PK_Attr = " << PK_Attr << " and value " << sValue << endl;
        }
    }
    
    myTag.Update(ID3TT_ID3); 
}

#ifdef ENABLE_TEST_UNIT
int main( unsigned int argc, char * const argv[])
{
    if(argc != 2)
    {
	    cout << "Usage: id3info file" << endl;
	    return 1;
    }

    map<int,string> mapAttributes;

    //reading attr
    GetId3Info(argv[1], mapAttributes);

    //display attr
    cout << endl << "Size: " << int(mapAttributes.size()) << endl;
    for(map<int,string>::iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
	    cout << "PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

    //write attr
    mapAttributes.clear();
    mapAttributes[ATTRIBUTETYPE_Composer_CONST] = "bubu";
    SetId3Info(argv[1], mapAttributes);

    //reading attr
    mapAttributes.clear();
    GetId3Info(argv[1], mapAttributes);

    //display attr
    cout << endl << "Size: " << int(mapAttributes.size()) << endl;
    for(map<int,string>::iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
        cout << "PK_Attr = " << (*it).first << "\t\t" << (*it).second << endl;

    return 0;
}
#endif

