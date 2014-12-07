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
#include "ENodeB.h"
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
#include "../phy/enb-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"

ENodeB::ENodeB ()
{}

ENodeB::ENodeB (int idElement,
			    Cell *cell)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_ENODEB);
  SetCell (cell);

  CartesianCoordinates *position = new CartesianCoordinates(cell->GetCellCenterPosition ()->GetCoordinateX (),
		                                                    cell->GetCellCenterPosition ()->GetCoordinateY ());
  //std::cout<<"CC"<<std::endl;//memorytest
  Mobility* m = new ConstantPosition ();
  //std::cout<<"CP"<<std::endl;//memorytest
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);
  delete position;

  m_userEquipmentRecords = new UserEquipmentRecords;
  //std::cout<<"UER"<<std::endl;//memorytest
  m_busRecords = new BusRecords;
  //std::cout<<"BR"<<std::endl;//memorytest

  EnbLtePhy *phy = new EnbLtePhy ();
  //std::cout<<"ENBLP"<<std::endl;//memorytest
  phy->SetDevice (this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  //std::cout<<"ProtocolStack"<<std::endl;//memorytest
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  //std::cout<<"Classifier"<<std::endl;//memorytest
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

ENodeB::ENodeB (int idElement,
				Cell *cell,
				double posx,
				double posy)
{
  SetIDNetworkNode (idElement);
  SetNodeType(NetworkNode::TYPE_ENODEB);
  SetCell (cell);

  CartesianCoordinates *position = new CartesianCoordinates(posx, posy);
  //std::cout<<"CC2"<<std::endl;//memorytest
  Mobility* m = new ConstantPosition ();
  //std::cout<<"CP2"<<std::endl;//memorytest
  m->SetAbsolutePosition (position);
  SetMobilityModel (m);


  m_userEquipmentRecords = new UserEquipmentRecords;
  //std::cout<<"UER2"<<std::endl;//memorytest
  m_busRecords = new BusRecords;
  //std::cout<<"BR2"<<std::endl;//memorytest

  EnbLtePhy *phy = new EnbLtePhy ();
  //std::cout<<"ENBLP2"<<std::endl;//memorytest
  phy->SetDevice(this);
  SetPhy (phy);

  ProtocolStack *stack = new ProtocolStack (this);
  //std::cout<<"ProtocolStack2"<<std::endl;//memorytest
  SetProtocolStack (stack);

  Classifier *classifier = new Classifier ();
  //std::cout<<"Classifier2"<<std::endl;//memorytest
  classifier->SetDevice (this);
  SetClassifier (classifier);
}

ENodeB::~ENodeB()
{
  Destroy ();
  m_userEquipmentRecords->clear();
  delete m_userEquipmentRecords;
  m_busRecords->clear();
  delete m_busRecords;
}

void
ENodeB::RegisterUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecord *record = new UserEquipmentRecord (UE);
  //std::cout<<"UER(UE)"<<std::endl;//memorytest
  GetUserEquipmentRecords ()->push_back(record);
}

void
ENodeB::RegisterBus (Bus *bus)
{
  BusRecord *record = new BusRecord (bus);
  //std::cout<<"BR(bus)"<<std::endl;//memorytest
  GetBusRecords ()->push_back(record);
}

void
ENodeB::DeleteUserEquipment (UserEquipment *UE)
{
  UserEquipmentRecords *records = GetUserEquipmentRecords ();
  UserEquipmentRecord *record;
  UserEquipmentRecords::iterator iter;

  UserEquipmentRecords *new_records = new UserEquipmentRecords ();
  //std::cout<<"UERs"<<std::endl;//memorytest

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

void
ENodeB::DeleteBus (Bus *bus)
{
  BusRecords *records = GetBusRecords ();
  BusRecord *record;
  BusRecords::iterator iter;

  BusRecords *new_records = new BusRecords ();
  //std::cout<<"BRs"<<std::endl;//memorytest

  for (iter = records->begin (); iter != records->end (); iter++)
    {
	  record = *iter;
	  if (record->Getbus ()->GetIDNetworkNode () != bus->GetIDNetworkNode ())
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

  m_busRecords->clear ();
  delete m_busRecords;
  m_busRecords = new_records;
}

int
ENodeB::GetNbOfUserEquipmentRecords (void)
{
  return GetUserEquipmentRecords ()->size();
}

int
ENodeB::GetNbOfBusRecords (void)
{
  return GetBusRecords ()->size();
}

void
ENodeB::CreateUserEquipmentRecords (void)
{
  m_userEquipmentRecords = new UserEquipmentRecords ();
 // std::cout<<"UERs()"<<std::endl;//memorytest
}

void
ENodeB::CreateBusRecords (void)
{
  m_busRecords = new BusRecords ();
  //std::cout<<"bRs()"<<std::endl;//memorytest
}

void
ENodeB::DeleteUserEquipmentRecords (void)
{
  m_userEquipmentRecords->clear ();
  delete m_userEquipmentRecords;
}

void
ENodeB::DeleteBusRecords (void)
{
  m_busRecords->clear ();
  delete m_busRecords;
}

ENodeB::UserEquipmentRecords*
ENodeB::GetUserEquipmentRecords (void)
{
  return m_userEquipmentRecords;
}

ENodeB::BusRecords*
ENodeB::GetBusRecords (void)
{
  return m_busRecords;
}

ENodeB::UserEquipmentRecord*
ENodeB::GetUserEquipmentRecord (int idUE)
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

ENodeB::BusRecord*
ENodeB::GetBusRecord (int idbus)
{
  BusRecords *records = GetBusRecords ();
  BusRecord *record;
  BusRecords::iterator iter;

  for (iter = records->begin (); iter != records->end (); iter++)
	{
	  record = *iter;
	  if (record->Getbus ()->
			  GetIDNetworkNode () == idbus)
		{
		  return record;
		}
	}
  return false;
}


ENodeB::UserEquipmentRecord::UserEquipmentRecord ()
{
  m_UE = NULL;
  //Create initial CQI values:
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear();
  m_cqiFeedback_twoago.clear();
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

ENodeB::BusRecord::BusRecord ()
{
  m_bus = NULL;
  //Create initial CQI values:
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear();
  m_cqiFeedback_twoago.clear();
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

ENodeB::UserEquipmentRecord::~UserEquipmentRecord ()
{
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear ();
  m_cqiFeedback_twoago.clear ();
}

ENodeB::BusRecord::~BusRecord ()
{
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear ();
  m_cqiFeedback_twoago.clear ();
}

ENodeB::UserEquipmentRecord::UserEquipmentRecord (UserEquipment *UE)
{
  m_UE = UE;
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear ();
  m_cqiFeedback_twoago.clear ();
  BandwidthManager *s = m_UE->GetPhy ()->GetBandwidthManager ();
  int nbRbs = s->GetUlSubChannels ().size ();//old is Dl

  for (int i = 0; i < nbRbs; i++ )
    {
	  m_cqiFeedback.push_back (10);
	  m_cqiFeedback_oneago.push_back (10);
	  m_cqiFeedback_twoago.push_back (10);
    }
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

ENodeB::BusRecord::BusRecord (Bus *bus)
{
  m_bus = bus;
  m_cqiFeedback.clear ();
  m_cqiFeedback_oneago.clear ();
  m_cqiFeedback_twoago.clear ();
  BandwidthManager *s = m_bus->GetPhy ()->GetBandwidthManager ();
  int nbRbs = s->GetUlSubChannels ().size ();//old is Dl

  for (int i = 0; i < nbRbs; i++ )
    {
	  m_cqiFeedback.push_back (10);
	  m_cqiFeedback_oneago.push_back (10);
	  m_cqiFeedback_twoago.push_back (10);
    }
  m_schedulingRequest = 0;
  m_averageSchedulingGrants = 1;
}

void
ENodeB::UserEquipmentRecord::SetUE (UserEquipment *UE)
{
  m_UE = UE;
}

void
ENodeB::BusRecord::Setbus (Bus *bus)
{
  m_bus = bus;
}

UserEquipment*
ENodeB::UserEquipmentRecord::GetUE (void) const
{
  return m_UE;
}

Bus*
ENodeB::BusRecord::Getbus (void) const
{
  return m_bus;
}

void
ENodeB::UserEquipmentRecord::SetCQI (std::vector<int> cqi)
{
  m_cqiFeedback = cqi;
}

void
ENodeB::BusRecord::SetCQI (std::vector<int> cqi)
{
  m_cqiFeedback = cqi;
}

/*mouan*/
void
ENodeB::UserEquipmentRecord::SetCQIone (std::vector<int> cqi)
{
  m_cqiFeedback_oneago = cqi;
}
void
ENodeB::BusRecord::SetCQIone (std::vector<int> cqi)
{
  m_cqiFeedback_oneago = cqi;
}
void
ENodeB::UserEquipmentRecord::SetCQItwo (std::vector<int> cqi)
{
  m_cqiFeedback_twoago = cqi;
}
void
ENodeB::BusRecord::SetCQItwo (std::vector<int> cqi)
{
  m_cqiFeedback_twoago = cqi;
}
std::vector<int>
ENodeB::UserEquipmentRecord::GetCQIone (void) const
{
  return m_cqiFeedback_oneago;
}
std::vector<int>
ENodeB::BusRecord::GetCQIone (void) const
{
  return m_cqiFeedback_oneago;
}
std::vector<int>
ENodeB::UserEquipmentRecord::GetCQItwo (void) const
{
  return m_cqiFeedback_twoago;
}
std::vector<int>
ENodeB::BusRecord::GetCQItwo (void) const
{
  return m_cqiFeedback_twoago;
}

std::vector<int>
ENodeB::UserEquipmentRecord::GetCQI (void) const
{
 return m_cqiFeedback;
}

std::vector<int>
ENodeB::BusRecord::GetCQI (void) const
{
 return m_cqiFeedback;
}

int
ENodeB::UserEquipmentRecord::GetSchedulingRequest (void)
{
  return m_schedulingRequest;
}

int
ENodeB::BusRecord::GetSchedulingRequest (void)
{
  return m_schedulingRequest;
}

void
ENodeB::UserEquipmentRecord::SetSchedulingRequest (int r)
{
  m_schedulingRequest = r;
}

void
ENodeB::BusRecord::SetSchedulingRequest (int r)
{
  m_schedulingRequest = r;
}

void
ENodeB::UserEquipmentRecord::UpdateSchedulingGrants (int b)
{
  m_averageSchedulingGrants = (0.9 * m_averageSchedulingGrants)	+ (0.1 * b);
}

void
ENodeB::BusRecord::UpdateSchedulingGrants (int b)
{
  m_averageSchedulingGrants = (0.9 * m_averageSchedulingGrants)	+ (0.1 * b);
}

int
ENodeB::UserEquipmentRecord::GetSchedulingGrants (void)
{
  return m_averageSchedulingGrants;
}

int
ENodeB::BusRecord::GetSchedulingGrants (void)
{
  return m_averageSchedulingGrants;
}

void
ENodeB::UserEquipmentRecord::SetUlMcs (int mcs)
{
  m_ulMcs = mcs;
}

void
ENodeB::BusRecord::SetUlMcs (int mcs)
{
  m_ulMcs = mcs;
}

int
ENodeB::UserEquipmentRecord::GetUlMcs (void)
{
  return m_ulMcs;
}

int
ENodeB::BusRecord::GetUlMcs (void)
{
  return m_ulMcs;
}



void
ENodeB::SetDLScheduler (ENodeB::DLSchedulerType type)
{

  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();

  PacketScheduler *scheduler;
  switch (type)
    {
      case ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR:
    	scheduler = new  DL_PF_PacketScheduler ();
		//std::cout<<"DLPFPS"<<std::endl;//memorytest
    	scheduler->SetMacEntity (mac);
    	mac->SetDownlinkPacketScheduler (scheduler);
	    break;

      case ENodeB::DLScheduler_TYPE_FLS:
      	scheduler = new  DL_FLS_PacketScheduler ();
		//std::cout<<"DLFLSPS"<<std::endl;//memorytest
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
        break;

      case ENodeB::DLScheduler_TYPE_EXP:
      	scheduler = new  DL_EXP_PacketScheduler ();
		//std::cout<<"DLEXPPS"<<std::endl;//memorytest
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_TYPE_MLWDF:
      	scheduler = new  DL_MLWDF_PacketScheduler ();
		//std::cout<<"DLMLWDFPS"<<std::endl;//memorytest
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_EXP_RULE:
      	scheduler = new  ExpRuleDownlinkPacketScheduler ();
		//std::cout<<"ERDPS"<<std::endl;//memorytest
      	scheduler->SetMacEntity (mac);
      	mac->SetDownlinkPacketScheduler (scheduler);
		  break;

      case ENodeB::DLScheduler_LOG_RULE:
      	scheduler = new  LogRuleDownlinkPacketScheduler ();
		//std::cout<<"LRDPS"<<std::endl;//memorytest
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
ENodeB::GetDLScheduler (void) const
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetDownlinkPacketScheduler ();
}

void
ENodeB::SetULScheduler (ULSchedulerType type)
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  UplinkPacketScheduler *scheduler;
  switch (type)
	{
	  case ENodeB::ULScheduler_TYPE_MAXIMUM_THROUGHPUT:
		scheduler = new MaximumThroughputUplinkPacketScheduler ();
		//std::cout<<"MTUPS"<<std::endl;//memorytest
		scheduler->SetMacEntity (mac);
		mac->SetUplinkPacketScheduler (scheduler);
		break;
	  case ENodeB::ULScheduler_TYPE_FME:
	    scheduler = new EnhancedUplinkPacketScheduler();
		//std::cout<<"EUPS"<<std::endl;//memorytest
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
ENodeB::GetULScheduler (void) const
{
  EnbMacEntity *mac = (EnbMacEntity*) GetProtocolStack ()->GetMacEntity ();
  return mac->GetUplinkPacketScheduler ();
}

void
ENodeB::ResourceBlocksAllocation (void)
{
  DownlinkResourceBlokAllocation ();
  UplinkResourceBlockAllocation ();
}

void
ENodeB::UplinkResourceBlockAllocation (void)
{
  if (GetULScheduler () != NULL && GetNbOfUserEquipmentRecords () > 0)
   {
	  GetULScheduler ()->Schedule();
   }
}

void
ENodeB::DownlinkResourceBlokAllocation (void)
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
ENodeB::Print (void)
{
  std::cout << " ENodeB object:"
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

  //mouan
  vector<BusRecord*>* busrecords = GetBusRecords ();
  BusRecord *busrecord;
  vector<BusRecord*>::iterator busiter;
  for (busiter = busrecords->begin (); busiter != busrecords->end (); busiter++)
    {
  	  busrecord = *busiter;
	  std::cout << "\t\t idbus = " << busrecord->Getbus ()->
			  GetIDNetworkNode () << std::endl;
    }
}
