/**
 *
 * @file Other.h
 * @brief header file for the other utilities class
 * @author
 * @todo notcommented
 *
 */

#ifndef OTHERH
#define OTHERH

#ifndef SYMBIAN

#include <string>
#include <vector>
using namespace ::std;

#else

#include "../VIPShared/MyString.h"

#define abs(x) Abs(x)
#define atoi(x) SymbianAtoi(x, 0)
#define itos(x) SymbianItoa(x)
#define strchr(x, y) (x)

#endif

#ifndef SYMBIAN

/**
 * @brief used to pharse data at hi speed
 */
class FastParser
{
    char m_cDelimiter; /** < the delimiter used by the parser */
    char *m_pcData; /** < the pointer to the data to be parsed */
    char *m_pcEnd; /** < pointer to show the end of the data */

public:

    inline bool IsAtEnd() { return ( m_pcData > m_pcEnd ); }; /** < checks to see if the end of the data has been reached */
    inline const char *GetCurrentPtr() { return m_pcData; }; /** < returns current position of the pointer to the data */

    /**
     * @brief sets the member variables with initial values
     */
    void Setup(char *pcData, const char cDelimiter) { m_pcEnd = pcData + strlen(pcData); m_pcData = pcData; m_cDelimiter = cDelimiter; };

    /**
     * @brief sets the member variables with initial values
     * @param iLength the length of the data to be considered
     */
    void Setup(char *pcData, const char cDelimiter, int iLength) { m_pcEnd = pcData + iLength - 1; m_pcData = pcData; m_cDelimiter = cDelimiter; };

    /**
     * @brief returns the next numeric parameter
     */
    int FastParser::GetNextNumParm();

    /**
     * @brief returns the next parameter
     */
    const char *FastParser::GetNextParm();
};

#ifndef WIN32


/**
 * @brief putting it here for a lack of a better place
 */
extern long GetTickCount();

#endif //#ifndef WIN32

#endif //#ifndef SYMBIAN

#endif

