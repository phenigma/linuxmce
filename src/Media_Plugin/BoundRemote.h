#ifndef BoundRemote_h
#define BoundRemote_h

namespace DCE
{
	class BoundRemote
    {
    public:
        class OH_Orbiter *m_pOH_Orbiter;
        class EntertainArea *m_pEntertainArea;
        class Media_Plugin *m_pMedia_Plugin;
        string m_sPK_DesignObj_Remote,m_sPK_DesignObj_GraphicImage,m_sOptions;
        int m_iPK_Text_Description,m_iPK_Text_Section,m_iPK_Text_Synopsis,m_iPK_Text_Timecode;

        BoundRemote(class Media_Plugin *pMedia_Plugin) { m_iPK_Text_Description=m_iPK_Text_Timecode=0; m_pMedia_Plugin=pMedia_Plugin;}
        void UpdateOrbiter(class MediaStream *pMediaStream,bool bRefreshScreen);
    };

    typedef map<int,BoundRemote *> MapBoundRemote;
}

#endif
