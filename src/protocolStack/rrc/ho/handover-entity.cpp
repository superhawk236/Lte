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


#include "handover-entity.h"
#include "../../../device/NetworkNode.h"
#include "../../../device/UserEquipment.h"
#include "../../../device/Bus.h"
#include "ho-manager.h"
#include "position-based-ho-manager.h"
#include "power-based-ho-manager.h"

//#include "../../../core/spectrum/transmitted-signal.h"

HandoverEntity::HandoverEntity()
{
  m_device = NULL;
  //m_hoManager = new PowerBasedHoManager ();
  m_hoManager = new PositionBasedHoManager ();
}

HandoverEntity::~HandoverEntity()
{
  m_device = 0;
  delete m_hoManager;
}

void
HandoverEntity::SetDevice (NetworkNode* d)
{
  m_device = d;
}

NetworkNode*
HandoverEntity::GetDevice ()
{
  return m_device;
}

void
HandoverEntity::SetHoManager (HoManager *h)
{
  m_hoManager = h;
}

HoManager*
HandoverEntity::GetHoManager (void)
{
  return m_hoManager;
}


bool
HandoverEntity::CheckHandoverNeed (UserEquipment* ue)
{
  return GetHoManager ()->CheckHandoverNeed (ue);
}

//mouan
bool
HandoverEntity::CheckBusHandoverNeed (Bus* bus)
{
  return GetHoManager ()->CheckBusHandoverNeed (bus);
}

bool
HandoverEntity::MouanCheckHandoverNeed (UserEquipment* ue)//,TransmittedSignal *signalvalue)
{
  return GetHoManager ()->MouanCheckHandoverNeed (ue);//,signalvalue);
}
