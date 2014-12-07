/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010,2011,2012 TELEMATICS LAB, Politecnico di Bari
 *
 * This file is part of LTE-Sim
 *
 * LTE-Sim is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation;
 *
 * LTE-Sim is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LTE-Sim; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Giuseppe Piro <g.piro@poliba.it>
 */


#ifndef BUS_MAC_ENTITY_H
#define BUS_MAC_ENTITY_H

#include <list>
#include "mac-entity.h"
#include "packet-scheduler/uplink-packet-scheduler.h"

/*
 * This class implements the MAC layer of the eNodeB device
 */

class PacketScheduler;
class CqiIdealControlMessage;
class PdcchMapIdealControlMessage;
class SchedulingRequestIdealControlMessage;

class BusMacEntity : public MacEntity
{
public:

  BusMacEntity (void);
  virtual ~BusMacEntity (void);


  void SetUplinkPacketScheduler (UplinkPacketScheduler* s);
  void SetDownlinkPacketScheduler (PacketScheduler* s);
  UplinkPacketScheduler* GetUplinkPacketScheduler (void);
  PacketScheduler* GetDownlinkPacketScheduler (void);

  void ReceiveCqiIdealControlMessage  (CqiIdealControlMessage* msg);
  void SendPdcchMapIdealControlMessage  (PdcchMapIdealControlMessage* msg);
  void ReceiveSchedulingRequestIdealControlMessage (SchedulingRequestIdealControlMessage *msg);
private:

	UplinkPacketScheduler* m_uplinkScheduler;
  PacketScheduler* m_downlinkScheduler;
};


#endif /* BUS_MAC_ENTITY_H */
