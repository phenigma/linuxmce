//<-dceag-d-b->
#ifndef SimplePhone_h
#define SimplePhone_h

//    DCE Implemenation for #1759 Orbiter Embedded Phone

#include "Gen_Devices/SimplePhoneBase.h"
//<-dceag-d-e->

#include "iaxclient.h"
//<-dceag-decl-b->
namespace DCE
{
    class SimplePhone : public SimplePhone_Command
    {
//<-dceag-decl-e->
private:
        // Private methods
        void PlayRingTone();
        void StopRingTone();
        // Private member variables
        iaxc_sound* ringTone;
        char* deviceExtension;
        char* devicePassword;
        char* asteriskHost;
        pthread_t iaxThread;
public:
        // Public member variables
        // Public methods
        void registerWithAsterisk();
        void unregisterWithAsterisk();
        void doProccess();
        virtual void CreateChildren();
//<-dceag-const-b->
public:
        // Constructors/Destructor
        SimplePhone(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
        virtual ~SimplePhone();
        virtual bool GetConfig();
        virtual bool Register();
        virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
        virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-const2-b->
        // The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router
        // You can delete this whole section and put an ! after dceag-const2-b tag if you don't want this constructor.  Do the same in the implementation file
        SimplePhone(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
//<-dceag-const2-e->

//<-dceag-h-b->
    /*
                AUTO-GENERATED SECTION
                Do not change the declarations
    */

    /*
            *****DATA***** accessors inherited from base class
    string DATA_Get_PhoneType();
    string DATA_Get_PhoneNumber();
    bool DATA_Get_Speak_in_the_House();

            *****EVENT***** accessors inherited from base class
    void EVENT_Incoming_Call();
            *****COMMANDS***** we need to implement
    */


    /** @brief COMMAND: #28 - Simulate Keypress */
    /** Send a DTMF code */
        /** @param #26 PK_Button */
            /** What key to simulate being pressed.  If 2 numbers are specified, separated by a comma, the second will be used if the Shift key is specified. */
        /** @param #50 Name */
            /** The application to send the keypress to. If not specified, it goes to the DCE device. */

    virtual void CMD_Simulate_Keypress(string sPK_Button,string sName) { string sCMD_Result; CMD_Simulate_Keypress(sPK_Button.c_str(),sName.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Simulate_Keypress(string sPK_Button,string sName,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #334 - Phone_Initiate */
    /** Initiates a call */
        /** @param #83 PhoneExtension */
            /** Extention to dial */

    virtual void CMD_Phone_Initiate(string sPhoneExtension) { string sCMD_Result; CMD_Phone_Initiate(sPhoneExtension.c_str(),sCMD_Result,NULL);};
    virtual void CMD_Phone_Initiate(string sPhoneExtension,string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #335 - Phone_Answer */
    /** Answer a call */

    virtual void CMD_Phone_Answer() { string sCMD_Result; CMD_Phone_Answer(sCMD_Result,NULL);};
    virtual void CMD_Phone_Answer(string &sCMD_Result,Message *pMessage);


    /** @brief COMMAND: #336 - Phone_Drop */
    /** Drop a call */

    virtual void CMD_Phone_Drop() { string sCMD_Result; CMD_Phone_Drop(sCMD_Result,NULL);};
    virtual void CMD_Phone_Drop(string &sCMD_Result,Message *pMessage);

//<-dceag-h-e->
    };

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
