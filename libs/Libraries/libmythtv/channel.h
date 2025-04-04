#ifndef CHANNEL_H
#define CHANNEL_H

#include "channelbase.h"
#include "videodev_myth.h" // needed for v4l2_std_id type

using namespace std;

#define FAKE_VIDEO 0

class TVRec;

typedef QMap<int,int>         VidModV4L1;
typedef QMap<int,v4l2_std_id> VidModV4L2;

// Implements tuning for analog TV cards (both software and hardware encoding)
// using the V4L driver API
class Channel : public ChannelBase
{
 public:
    Channel(TVRec *parent, const QString &videodevice);
    virtual ~Channel(void);

    bool Open(void);
    void Close(void);

    // Sets
    void SetFd(int fd); 
    void SetFormat(const QString &format);
    int  SetDefaultFreqTable(const QString &name);
    bool SetChannelByString(const QString &chan); 
    bool SetChannelByDirection(ChannelChangeDirection);

    // Gets
    bool IsOpen(void)       const { return GetFd() >= 0; }
    int  GetFd(void)        const { return videofd; }
    QString GetDevice(void) const { return device; }

    // Commands
    bool SwitchToInput(int newcapchannel, bool setstarting);

    // Picture attributes.
    void SetBrightness(void);
    void SetContrast(void);
    void SetColour(void);
    void SetHue(void);
    int  ChangeBrightness(bool up);
    int  ChangeColour(bool up);
    int  ChangeContrast(bool up);
    int  ChangeHue(bool up);

    // PID caching
    void SaveCachedPids(const pid_cache_t&) const;
    void GetCachedPids(pid_cache_t&) const;

    // ATSC scanning stuff
    bool TuneMultiplex(uint mplexid);
    bool Tune(uint frequency, QString inputname="",
              QString modulation="analog");
    // V4L scanning stuff
    bool IsTuned(void) const;
  private:
    // Helper Sets
    void SetColourAttribute(int attrib, const char *name);
    void SetFreqTable(const int index);
    int  SetFreqTable(const QString &name);
    bool SetInputAndFormat(int newcapchannel, QString newFmt);

    // Helper Gets
    unsigned short *GetV4L1Field(int attrib, struct video_picture &vid_pic);
    int  GetChanID(void) const;
    int  GetCardID(void) const;
    int  GetCurrentChannelNum(const QString &channame);
    QString GetFormatForChannel(QString channum,
                                QString inputname);

    // Helper Commands
    int  ChangeColourAttribute(int attrib, const char *name, bool up);
    bool TuneTo(const QString &chan, int finetune);
    void InitializeInputs(void);

  private:
    // Data
    QString     device;
    int         videofd;

    struct CHANLIST *curList;  
    int         totalChannels;

    QString     currentFormat;
    bool        is_dtv;         ///< Set if 'currentFormat' is a DTV format
    bool        usingv4l2;      ///< Set to true if tuner accepts v4l2 commands
    VidModV4L1  videomode_v4l1; ///< Current video mode if 'usingv4l2' is false
    VidModV4L2  videomode_v4l2; ///< Current video mode if 'usingv4l2' is true

    int         defaultFreqTable;
};

#endif
