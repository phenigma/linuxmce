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

#ifndef BUSMONITOR_H
#define BUSMONITOR_H

#include "layer3.h"
#include "client.h"

/** implements busmonitor functions for a client */
class A_Busmonitor:public L_Busmonitor_CallBack, private Thread
{
  /** semaphore for the input queue */
  pth_sem_t sem;
  /** input queue */
    Queue < L_Busmonitor_PDU * >data;
    /** is virtual busmonitor */
  bool v;

  void Run (pth_sem_t * stop);
protected:
  /** Layer 3 Interface*/
    Layer3 * l3;
    /** client connection */
  ClientConnection *con;
  /** debug output */
  Trace *t;
  /** turns a busmonitor LPDU into a eibd packet and sends it */
  virtual int sendResponse (L_Busmonitor_PDU * p, pth_event_t stop);
public:
  /** initializes busmonitor
   * @param c client connection
   * @param tr debug output
   * @param l3 Layer 3
   * @param virt is virtual busmonitor
   */
    A_Busmonitor (ClientConnection * c, Layer3 * l3, Trace * tr, bool virt =
		  0);
    virtual ~ A_Busmonitor ();
  void Get_L_Busmonitor (L_Busmonitor_PDU * l);

  /** start processing */
  void Do (pth_event_t stop);
};

/** implements text busmonitor functions for a client */
class A_Text_Busmonitor:public A_Busmonitor
{
protected:
  int sendResponse (L_Busmonitor_PDU * p, pth_event_t stop);
public:
  /** initializes busmonitor
   * @param c client connection
   * @param tr debug output
   * @param l3 Layer 3
   * @param virt is virtual busmonitor
   */
  A_Text_Busmonitor (ClientConnection * c, Layer3 * l3, Trace * tr, bool virt = 0):A_Busmonitor (c, l3, tr,
		virt)
  {
  }
};

#endif
