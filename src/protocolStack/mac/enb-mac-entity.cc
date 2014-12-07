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



#include "enb-mac-entity.h"
#include "../packet/Packet.h"
#include "../packet/packet-burst.h"
#include "AMCModule.h"
#include "../../core/idealMessages/ideal-control-messages.h"
#include "../../device/NetworkNode.h"
#include "packet-scheduler/packet-scheduler.h"

#include "packet-scheduler/uplink-packet-scheduler.h"
#include "../../protocolStack/rrc/ho/handover-entity.h"

#include "../../device/UserEquipment.h"
#include "../../device/ENodeB.h"
//#include "../../device/Bus.h"
#include "../../load-parameters.h"


EnbMacEntity::EnbMacEntity ()
{
  SetAmcModule (new AMCModule ());
  //std::cout<<"EnbAMCM"<<std::endl;//memorytest
  SetDevice (NULL);
  m_downlinkScheduler = NULL;
  m_uplinkScheduler = NULL;
}


EnbMacEntity::~EnbMacEntity ()
{
  delete m_downlinkScheduler;
  delete m_uplinkScheduler;
  Destroy ();
}


void
EnbMacEntity::SetUplinkPacketScheduler (UplinkPacketScheduler* s)
{
  m_uplinkScheduler = s;
}


void
EnbMacEntity::SetDownlinkPacketScheduler (PacketScheduler* s)
{
  m_downlinkScheduler = s;
}


UplinkPacketScheduler*
EnbMacEntity::GetUplinkPacketScheduler (void)
{
  return m_uplinkScheduler;
}


PacketScheduler*
EnbMacEntity::GetDownlinkPacketScheduler (void)
{
  return m_downlinkScheduler;
}


void
EnbMacEntity::ReceiveCqiIdealControlMessage  (CqiIdealControlMessage* msg)
{
#ifdef TEST_CQI_FEEDBACKS
  std::cout << "ReceiveIdealControlMessage (MAC) from  " << msg->GetSourceDevice ()->GetIDNetworkNode ()
		  << " to " << msg->GetDestinationDevice ()->GetIDNetworkNode () << std::endl;
#endif

  CqiIdealControlMessage::CqiFeedbacks *cqi = msg->GetMessage ();

  UserEquipment* ue = (UserEquipment*) msg->GetSourceDevice ();
  ENodeB* enb = (ENodeB*) GetDevice ();
  ENodeB::UserEquipmentRecord* record = enb->GetUserEquipmentRecord (ue->GetIDNetworkNode ());

  if (record != NULL)
    {
      std::vector<int> cqiFeedback;
      for (CqiIdealControlMessage::CqiFeedbacks::iterator it = cqi->begin (); it != cqi->end (); it++)
        {
	      cqiFeedback.push_back ((*it).m_cqi);
        }

#ifdef TEST_CQI_FEEDBACKS
      std::cout << "\t CQI: ";
      for (int i = 0; i < cqiFeedback.size (); i++)
        {
	      std::cout << cqiFeedback.at (i) << " ";
        }
      std::cout << std::endl;
#endif

#ifdef AMC_MAPPING
      std::cout << "\t CQI: ";
      for (int i = 0; i < cqiFeedback.size (); i++)
        {
	      std::cout << cqiFeedback.at (i) << " ";
        }
      std::cout << std::endl;

      std::cout << "\t MCS: ";
      for (int i = 0; i < cqiFeedback.size (); i++)
        {
	      std::cout << GetAmcModule ()->GetMCSFromCQI (cqiFeedback.at (i)) << " ";
        }
      std::cout << std::endl;

      std::cout << "\t TB: ";
      for (int i = 0; i < cqiFeedback.size (); i++)
        {
	      std::cout << GetAmcModule ()->GetTBSizeFromMCS(
	    		  GetAmcModule ()->GetMCSFromCQI (cqiFeedback.at (i))) << " ";
        }
      std::cout << std::endl;
#endif

	  record->SetCQItwo(record->GetCQIone());
	  record->SetCQIone(record->GetCQI());
      record->SetCQI (cqiFeedback);

	  /*int compute=0,compute2=0,compute3=0;
	  std::vector<int> temp1=record->GetCQI();
	  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
		  compute += (*i);
	  }
	  temp1=record->GetCQIone();
	  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
		  compute2 += (*i);
	  }
	  temp1=record->GetCQItwo();
	  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
		  compute3 += (*i);
	  }
	  std::cout<<"comparing "<<compute<<" "<<compute2<<" "<<compute3<<std::endl;
	  if(compute<=compute2&&compute2<=compute3){
		  enb->GetProtocolStack()->GetRrcEntity()->GetHandoverEntity()->MouanCheckHandoverNeed(ue);//run check ho
	  }*/

    }
  else
    {
      std::cout << "ERROR: received cqi from unknow ue!"<< std::endl;
	  std::cout <<"this ue "<<ue->GetIDNetworkNode()<<" target "
		  <<ue->GetTargetNode()->GetIDNetworkNode()<<std::endl;
    }

  //delete msg;
}


void
EnbMacEntity::SendPdcchMapIdealControlMessage  (PdcchMapIdealControlMessage* msg)
{
}


void
EnbMacEntity::ReceiveSchedulingRequestIdealControlMessage (SchedulingRequestIdealControlMessage* msg)
{
	//std::cout<<"Recv SRICM"<<std::endl;//test
  UserEquipment* ue = (UserEquipment*) msg->GetSourceDevice ();
  ENodeB* enb = (ENodeB*) GetDevice ();
  ENodeB::UserEquipmentRecord* record = enb->GetUserEquipmentRecord (ue->GetIDNetworkNode ());

  int bufferStatusReport = msg->GetBufferStatusReport ();

  if (record != NULL)
	{
	  record->SetSchedulingRequest (bufferStatusReport);
	}
  else
    {
      std::cout << "ERROR: received cqi from unknow ue!"<< std::endl;
	  std::cout <<"this ue "<<ue->GetIDNetworkNode()<<" target "
		  <<ue->GetTargetNode()->GetIDNetworkNode()<<std::endl;
    }
}
