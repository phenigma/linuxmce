/**
 *
 * @file RA_Action.h
 * @brief header file for the RA_Action class
 * @author @todo addauthor
 * @todo notcommented
 *
 */

#ifndef RA_ACTION_H
#define RA_ACTION_H

#include "SerializeClass/SerializeClass.h"

// Request Types
#define ACTION_FORWARDREQUEST_TOPHONE   1
#define ACTION_UPDATETRANSACTION        2
#define ACTION_SENDFILE_TOPHONE         3

class RA_Action : public SerializeClass
{
protected:

    unsigned long m_dwActionSize; /** <the size of the action */
    unsigned long m_dwActionChecksum; /** <the checksum of the action */
    char *m_pcAction; /** <the action data */

public:

    class RA_Request *m_pcRequest; /** <the action that triggered the request */

    /**
     * @brief basic constructor
     * the one initiating the action calls this constructor, and will call ConvertActionToBinary and Send it
     */
    RA_Action();

    /**
     * @brief constructor
     * the one receiving the action calls this one
     * received a binary action, call ProcessAction
     * @param dwSize the size of the binary data
     * @param pcData the data
     */
    RA_Action( unsigned long dwSize,const char *pcData); // Received a binary action, call ProcessAction

    /**
     * @brief virual destructor
     */
    virtual ~RA_Action()=0;

    /**
     * @brief virtual convertor to binary
     */
    virtual void ConvertActionToBinary();

    /**
     * @brief pure virtual that, in the inheriting classes, will process the action
     */
    virtual void ProcessAction()=0;

    /**
     * @return private member
     */
    long ActionSize() { return m_dwActionSize; }

    /**
     * @return private member
     */
    long ActionChecksum() { return m_dwActionChecksum; }

    /**
     * @return constant private member
     */
    const char *Action() { return m_pcAction; }

    /**
     * @brief pure virtual method... should return the ID of the action in inheriting classes
     */
    virtual unsigned long ID()=0;
};

#endif
