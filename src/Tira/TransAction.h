// $Header: /cygdrive/e/CVS/Software/APIBase/TransAction.h,v 1.15 2004/07/01 00:32:40 szilber Exp $
//=============================================================================
//  Copyright (C) 2004, Home Electronics ( http://www.home-electro.com )
//  support@home-electro.com
//
// This software is a part of Tira API. Permission is granted to use and modify
// as long as this notice is preserved, and the software remains a part of Tira API
// 
// No warranty express or implied.
//=============================================================================

#ifndef TransActionH
#define TransActionH

#include <vector>


struct TransmitAction {
   int PLA[8];
   int FrequencySelect; // 0..7
   int RepeatSelect;    // 1..10
   int RepeatDelay;

   int SequenceLength;  // Repeat capable sequences are stored
                        // in the form of START {REPEAT},

   int RepeatBegin;  // -1 if no repeat is defined
};

const int MaxPLA = 16;
struct TransmitActionNew {
   int StructureVersion;
   int TimeBase; // microseconds
   int PLA[MaxPLA];
   int PLA_Size;
   int PulsePeriod; // nanoseconds;
   int RepeatSelect;    // 1..10

   int SequenceLength;  // Repeat capable sequences are stored
                        // in the form of START {REPEAT},

   int RepeatBegin;  // -1 if no repeat is defined
   TransmitActionNew( TransmitAction& ta);
   TransmitActionNew() {
      char* cp = (char*)this;
      for (int i = 0; i < sizeof(*this); i++)
         *cp = 0;

      StructureVersion = 0;
   };
};
// Typical use these functions:

// Whenever you get timing data call TryProcessData with Force =false, DetectEnd = true
// If it fails, it means not there is not enough data yet. Normally it needs 350mS of an incoming
// IR Data. Wait for more timing data from Tira, and call this function again

// After TryProcessData succeded you need call FindFrequencyWord. Again, it may fail if frequency word is not yet
// present. Wait for more data, and call it again.

// Then call CalcSequenceLength. It will tell you the size of a buffer required to store a complete "IX" string.

// Then call GenTransmitSequence. It will build a transmit sequence, that can be sent to Tira as a
// part of IX command.

int TryProcessData(                                 // returns 0 if success
      const std::vector<unsigned char>* timingArray, // Input. array containing timing data
      TransmitActionNew* ta,								// Output. a structure containg some info about IR data
      unsigned char* destData,                  // Output. Processed data

      const int destDataSize,                   // Input. Set it to the size of available space in
                                                // destination buffer
      bool Force,                               // Forces successful conversion, even if not enough
                                                // data is supplied
      bool DetectEnd = true,                    // If set repeat pattern will be detected
      bool UseTira2 = false);

int FindFrequencyWord(
   const std::vector<unsigned char>* timingArray,
   TransmitActionNew* ta,
   int*  Position);        // I/O Set it to zero on a first call, then leave it as is.


int   CalcSequenceLength(const TransmitActionNew* ta,
                        unsigned int Parameters = 0);
int   CalcSequenceLength(const TransmitAction* pa,
                        unsigned int Parameters = 0);

const   unsigned int tpTIRA2 = 0x1;
int  GenTransmitSequence(unsigned char* Seq,
                     const TransmitActionNew& ta,
                     const unsigned char* taData,
                     unsigned int Parameters = 0);



#endif
// $Log: TransAction.h,v $
// Revision 1.15  2004/07/01 00:32:40  szilber
// Copyright notice added
//
// Revision 1.14  2004/06/22 20:12:15  szilber
// initial
//
// Revision 1.13  2004/06/05 15:33:33  Administrator
// moved to APIBase
//
// Revision 1.6  2004/06/05 15:23:11  Administrator
// Minor changes, apparently for compatibility reasons
// Also some comments in H file
//
// Revision 1.5  2003/08/19 16:35:56  Administrator
// fixed some captur problem with Tira-2
//
// Revision 1.4  2003/07/24 09:13:01  Administrator
// Major update for Tira-2
// So far Tira2.dll seems to be working
//
// Revision 1.3  2003/06/06 00:38:10  Administrator
// no message
//
// Revision 1.2  2003/05/11 13:18:21  Administrator
// First new release
//
// Revision 1.1  2003/03/18 13:07:48  Administrator
// The first more or less working version.
//
