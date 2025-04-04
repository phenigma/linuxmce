/*
    EIBD eib bus access and management daemon
    Copyright (C) 2005-2008 Martin Koegler <mkoegler@auto.tuwien.ac.at>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef TRACE_H
#define TRACE_H

#include <stdarg.h>
#include "common.h"

#define TRACE_LEVEL_0 0x01
#define TRACE_LEVEL_1 0x02
#define TRACE_LEVEL_2 0x04
#define TRACE_LEVEL_3 0x08
#define TRACE_LEVEL_4 0x10
#define TRACE_LEVEL_5 0x20
#define TRACE_LEVEL_6 0x40
#define TRACE_LEVEL_7 0x80

/** implements debug output with different levels */
class Trace
{
  /** message levels to print */
  int layers;
public:
    Trace ()
  {
    layers = 0;
  }
  virtual ~ Trace ()
  {
  }

  /** sets trace level */
  virtual void SetTraceLevel (int l)
  {
    layers = l;
  }

  /** prints a message with a hex dump unconditional
   * @param layer level of the message
   * @param inst pointer to the source
   * @param msg Message
   * @param Len length of the data
   * @param data pointer to the data
   */
  virtual void TracePacketUncond (int layer, void *inst, const char *msg,
				  int Len, const uchar * data);
  /** prints a message with a hex dump
   * @param layer level of the message
   * @param inst pointer to the source
   * @param msg Message
   * @param Len length of the data
   * @param data pointer to the data
   */
  void TracePacket (int layer, void *inst, const char *msg, int Len,
		    const uchar * data)
  {
    if (!ShowPrint (layer))
      return;
    TracePacketUncond (layer, inst, msg, Len, data);
  }
  /** prints a message with a hex dump
   * @param layer level of the message
   * @param inst pointer to the source
   * @param msg Message
   * @param c array with the data
   */
  void TracePacket (int layer, void *inst, const char *msg, const CArray & c)
  {
    TracePacket (layer, inst, msg, c (), c.array ());
  }

  /** like printf for this trace
   * @param layer level of the message
   * @param inst pointer to the source
   * @param msg Message
   */
  virtual void TracePrintf (int layer, void *inst, const char *msg, ...);

  /** should trace message be written
   * @parm layer level of the message
   * @return bool
   */
  bool ShowPrint (int layer)
  {
    if (layers & (1 << layer))
      return 1;
    else
      return 0;
  }
};

#define TRACEPRINTF(trace, layer, msg, args...) do { if ((trace)->ShowPrint(layer)) (trace)->TracePrintf(layer, msg, ##args); } while (0)

#endif
