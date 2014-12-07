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


#include "NetworkNode.h"
#include "UserEquipment.h"
//#include "ENodeB.h"
#include "Gateway.h"
#include "Bus.h"
#include "../protocolStack/mac/packet-scheduler/packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/mt-uplink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-pf-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-mlwdf-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-exp-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/dl-fls-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/exp-rule-downlink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/log-rule-downlink-packet-scheduler.h"
#include "../protocolStack/mac/packet-scheduler/enhanced-uplink-packet-scheduler.h"
#include "../phy/bus-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"

Bus::Bus ()
{}

Bus::Bus (int idElement,
			    Cell *cell)
{
	//shouldn't come here
	std::cout<<"mouan Warning: Bus won't move!"<<std::endl;
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_BUS);
  SetCell (cell);

  CartesianCoordinates *position = new CartesianCoordinates(cell->GetCellCenterPosition ()->GetCoordinateX (),
		                                                    cell->GetCellCenterPosition ()->GetCoordinateY ());
  //std::cout<<"CC_Bus"<<std::endl;//memorytest
  Mobility* m = new ConstantPosition ();
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);
  delete position;

  m_userEquipmentRecords = new UserEquipmentRecords;

  BusLtePhy *phy = new BusLtePhy ();
  //std::cout<<"BLP"<<std::endl;//memorytest
  phy->SetDevice (this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  //std::cout<<"ProtocolStack_bus"<<std::endl;//memorytest
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
 // std::cout<<"classifer_bus"<<std::endl;//memorytest
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

Bus::Bus (int idElement,
				Cell *cell,
				NetworkNode* target,
				double posx,
				double posy)
{

  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_BUS);

  SetCell (cell);

  m_targetNode = target;

  CartesianCoordinates *position = new CartesianCoordinates(posx, posy);
  //std::cout<<"CC_bus"<<std::endl;//memorytest

  Mobility* m = new BusDirection ();
  //std::cout<<"BD"<<std::endl;//memorytest

  m->SetHandover (true);//always handover
  m->SetAbsolutePosition (position);
  m->SetNodeID (idElement);

  static int num=0;
  if(num==0)
	  m->SetSpeedDirection ((0.1*2*3.1415926)/360);
  if(num==1)
	  m->SetSpeedDirection ((180.1*2*3.1415926)/360);
  if(num==2)
	  m->SetSpeedDirection ((270.1*2*3.1415926)/360);
  num++;

  SetMobilityModel (m);


  m_userEquipmentRecords = new UserEquipmentRecords;
  //std::cout<<"UERs_bus"<<std::endl;//memorytest

  BusLtePhy *phy = new BusLtePhy ();
  //std::cout<<"BLP"<<std::endl;//memorytest
  phy->SetDevice(this);
  SetPhy (phy);

  m_cqiManager = NULL;

  ProtocolStack *stack = new ProtocolStack (this);
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

Bus::~Bus()
{
  Destroy ();
  m_userEquipmentRecords->clear();
  delete m_userEquipmentRecords;
}

void
Bus::RegisterUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecord *record = new UserEquipmentRecord (UE);
  //std::cout<<"UER_bus"<<std::endl;//memorytest
  GetUserEquipmentRecords ()->push_back(record);
}

void
Bus::DeleteUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecords *records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  UserEquipmentRecords::iterator iter;

  UserEquipmentRecords *new_records = new UserEquipmentRecords ();
  //std::cout<<"UERs_bus"<<std::endl;//memorytest

  for (iter = records->begin (); iter != records->end (); iter++)
    {
	  record = *iter;
	  if (record->GetUE ()->GetIDNetworkNode () != UE->GetIDNetworkNode ())
	    {
          //records->erase(iter);
          //break;
		  new_records->push_back (record);
	    }
	  else
	    {
		  delete record;
	    }
    }

  m_userEquipmentRecords->clear ();
  delete m_userEquipmentRecords;
  m_userEquipmentRecords = new_records;
}

int
Bus::GetNbOfUserEquipmentRecords (void)
{
  return GetUserEquipmentRecords ()->size();
}

void
Bus::CreateUserEquipmentRecords (void)
{
  m_userEquipmentRecords = new UserEquipmentRecords ();
  //std::cout<<"UERS_Bus2"<<std::endl;//memorytest
}

void
Bus::DeleteUserEquipmentRecords (void)
{
  m_userEquipmentRecords->clear ();
  delete m_userEquipmentRecords;
}

Bus::UserEquipmentRecords*
Bus::GetUserEquipmentRecords (void)
{
  return m_userEquipmentRecords;
}

Bus::UserEquipmentRecord*
Bus::GetUserEquipmentRecord (int idUE)
{
  UserEquipmentRecords *records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  UserEquipmentRecords::iterator iter;

  for (iter = records->begin (); iter != records->end (); iter++)
	{
	  record = *iter;
	  if (record->GetUE ()->
			  GetIDNetworkNode () == idUE)
		{
		  return record;
		}
	}
  return false;
}


Bus::UserEquipmentRecord::UserEquipmentRecord ()
{
  m_UE = NULL;
  //Create initial CQI values:
  m_cqiFeedback.clear ();
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

Bus::UserEquipmentRecord::~UserEquipmentRecord ()
{
  m_cqiFeedback.clear ();
}

Bus::UserEquipmentRecord::UserEquipmentRecord (UserEquipment *UE)
{
  m_UE = UE;
  m_cqiFeedback.clear ();
  BandwidthManager *s = m_UE->GetPhy ()->GetBandwidthManager ();
  int nbRbs = s->GetDlSubChannels ().size ();

  for (int i = 0; i < nbRbs; i++ )
    {
	  m_cqiFeedback.push_back (10);
    }
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

void
Bus::UserEquipmentRecord::SetUE (UserEquipment *UE)
{
  m_UE = UE;
}

UserEquipment*
Bus::UserEquipmentRecord::GetUE (void) const
{
  return m_UE;
}

void
Bus::UserEquipmentRecord::SetCQI (std::vector<int> cqi)
{
  m_cqiFeedback = cqi;
}

/*mouan*/
void
Bus::UserEquipmentRecord::SetCQIone (std::vector<int> cqi)
{
  m_cqiFeedback_oneago = cqi;
}
void
Bus::UserEquipmentRecord::SetCQItwo (std::vector<int> cqi)
{
  m_cqiFeedback_twoago = cqi;
}
std::vector<int>
Bus::UserEquipmentRecord::GetCQIone (void) const
{
  return m_cqiFeedback_oneago;
}
std::vector<int>
Bus::UserEquipmentRecord::GetCQItwo (void) const
{
  return m_cqiFeedback_twoago;
}

std::vector<int>
Bus::UserEquipmentRecord::GetCQI (void) const
{
 return m_cqiFeedback;
}

int
Bus::UserEquipmentRecord::GetSchedulingRequest (void)
{
  return m_schedulingRequest;
}

void
Bus::UserEquipmentRecord::SetSchedulingRequest (int r)
{
  m_schedulingRequest = r;
}

void
Bus::UserEquipmentRecord::UpdateSchedulingGrants (int b)
{
  m_averageSchedulingGrants = (0.9 * m_averageSchedulingGrants)	+ (0.1 * b);
}

int
Bus::UserEquipmentRecord::GetSchedulingGrants (void)
{
  return m_averageSchedulingGrants;
}

void
Bus::UserEquipmentRecord::SetUlMcs (int mcs)
{
  m_ulMcs = mcs;
}

int
Bus::UserEquipmentRecord::GetUlMcs (void)
{
  return m_ulMcs;
}



void
Bus::SetDLScheduler (Bus::DLSchedulerType type)
{
  BusMacEntity *mac = (BusMacEntity*) GetProtocolStack ()->GetMacEntity ();
  PacketScheduler *scheduler;
  switch (type)
    {
      case Bus::DLScheduler_TYPE_PROPORTIONAL_FAIR:
    	scheduler = new  DL_PF_PacketScheduler ();
    	scheduler->SetMacEntity (mac);
    	mac->SetDownlinkPacketScheduler (scheduler);
	    break;

      case Bus::DLScheduler_TYPE_FLS:
      	scheduler = new  DL_FLS_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
        break;

      case Bus::DLScheduler_TYPE_EXP:
      	scheduler = new  DL_EXP_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case Bus::DLScheduler_TYPE_MLWDF:
      	scheduler = new  DL_MLWDF_PacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case Bus::DLScheduler_EXP_RULE:
      	scheduler = new  ExpRuleDownlinkPacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case Bus::DLScheduler_LOG_RULE:
      	scheduler = new  LogRuleDownlinkPacketScheduler ();
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

	  default:
	    std::cout << "ERROR: invalid scheduler type" << std::endl;
    	scheduler = new  DL_PF_PacketScheduler ();
    	scheduler->SetMacEntity (mac);
    	mac->SetDownlinkPacketScheduler (scheduler);
	    break;
    }
}

PacketScheduler*
Bus::GetDLScheduler (void) const
{
  BusMacEntity *mac = (BusMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetDownlinkPacketScheduler ();
}

void
Bus::SetULScheduler (ULSchedulerType type)
{
  BusMacEntity *mac = (BusMacEntity*) GetProtocolStack ()->GetMacEntity ();
  //PacketScheduler *scheduler;
  UplinkPacketScheduler *scheduler;
  switch (type)
	{
	  case Bus::ULScheduler_TYPE_MAXIMUM_THROUGHPUT:
		scheduler = new MaximumThroughputUplinkPacketScheduler ();
		scheduler->SetMacEntity (mac);
		mac->SetUplinkPacketScheduler (scheduler);
		break;
	  case Bus::ULScheduler_TYPE_FME:
	    scheduler = new EnhancedUplinkPacketScheduler();
	    scheduler->SetMacEntity (mac);
	    mac->SetUplinkPacketScheduler (scheduler);
	    break;

	  default:
		std::cout << "ERROR: invalid scheduler type" << std::endl;
		scheduler = new EnhancedUplinkPacketScheduler();
	    scheduler->SetMacEntity (mac);
	    mac->SetUplinkPacketScheduler (scheduler);
	    break;
	}
}

UplinkPacketScheduler*
Bus::GetULScheduler (void) const
{
  BusMacEntity *mac = (BusMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetUplinkPacketScheduler ();
}

void
Bus::ResourceBlocksAllocation (void)
{
  DownlinkResourceBlokAllocation ();
  UplinkResourceBlockAllocation ();
}

void
Bus::UplinkResourceBlockAllocation (void)
{
  if (GetULScheduler () != NULL && GetNbOfUserEquipmentRecords () > 0)
   {
	  GetULScheduler ()->Schedule();
   }
}

void
Bus::DownlinkResourceBlokAllocation (void)
{
  if (GetDLScheduler () != NULL && GetNbOfUserEquipmentRecords () > 0)
   {
	  GetDLScheduler ()->Schedule();
   }
  else
    {
	  //send only reference symbols
	  //PacketBurst *pb = new PacketBurst ();
	  //SendPacketBurst (pb);
    }
}

//Debug
void
Bus::Print (void)
{
  std::cout << " Bus object:"
      "\n\t m_idNetworkNode = " << GetIDNetworkNode () <<
	  "\n\t m_idCell = " << GetCell ()->GetIdCell () <<
	  "\n\t Served Users: " <<
  std::endl;

  vector<UserEquipmentRecord*>* records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  vector<UserEquipmentRecord*>::iterator iter;
  for (iter = records->begin (); iter != records->end (); iter++)
    {
  	  record = *iter;
	  std::cout << "\t\t idUE = " << record->GetUE ()->
			  GetIDNetworkNode () << std::endl;
    }
}

//like ue
void
Bus::SetTargetNode (NetworkNode* n)
{
  m_targetNode = n;
  SetCell (n->GetCell ());
}

NetworkNode*
Bus::GetTargetNode (void)
{
  return m_targetNode;
}


void
Bus::UpdateUserPosition (double time)
{
  GetMobilityModel ()->UpdatePosition (time);
}


void
Bus::SetCqiManager (CqiManager *cm)
{
  m_cqiManager = cm;
}

CqiManager*
Bus::GetCqiManager (void)
{
  return m_cqiManager;
}