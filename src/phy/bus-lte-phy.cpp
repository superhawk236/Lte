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


#include "bus-lte-phy.h"
#include "../device/NetworkNode.h"
#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../core/spectrum/transmitted-signal.h"
#include "../core/idealMessages/ideal-control-messages.h"
//#include "../device/ENodeB.h"
#include "../device/Bus.h"
#include "../device/UserEquipment.h"
#include "interference.h"
#include "error-model.h"

#include "../protocolStack/rrc/ho/handover-entity.h"
#include "../utility/ComputePathLoss.h"
#include "../core/eventScheduler/simulator.h"

#include "../componentManagers/NetworkManager.h"
#include "../protocolStack/mac/AMCModule.h"

#include "../channel/propagation-model/propagation-loss-model.h"

//extern NetworkManager* nm;
extern double mouanth;
extern double ueth[60];
extern double uecqith[60];
//extern double mouancqith;
extern double cqitable[16];
extern double pctable[16];
extern double bpctable[16];
extern double totaldelay;
extern double totaldelaynum;
extern double uepower;
extern double buspower;
/*
 * Noise is computed as follows:
 *  - noise figure = 2.5
 *  - n0 = -174 dBm
 *  - sub channel bandwidth = 180 kHz
 *
 *  noise_db = noise figure + n0 + 10log10 (180000) - 30 = -148.95
 */
#define NOISE -148.95

BusLtePhy::BusLtePhy()
{
  SetDevice(NULL);
  SetDlChannel(NULL);
  SetUlChannel(NULL);
  SetBandwidthManager(NULL);
  SetTxSignal (NULL);
  SetErrorModel (NULL);
  SetInterference (NULL);
  SetTxPower(43); //dBm
}

BusLtePhy::~BusLtePhy()
{
  Destroy ();
}

void
BusLtePhy::DoSetBandwidthManager (void)
{
  BandwidthManager* s = GetBandwidthManager ();
  std::vector<double> channels = s->GetDlSubChannels ();

  TransmittedSignal* txSignal = new TransmittedSignal ();

  std::vector<double> values;
  std::vector<double>::iterator it;

  double powerTx = pow (10., (GetTxPower () - 30) / 10); // in natural unit

  double txPower = 10 * log10 (powerTx / channels.size ()); //in dB

  for (it = channels.begin (); it != channels.end (); it++ )
    {
      values.push_back(txPower);
    }

  txSignal->SetValues (values);
  //txSignal->SetBandwidthManager (s->Copy());

  SetTxSignal (txSignal);
}

void
BusLtePhy::StartTx (PacketBurst* p)
{
  //std::cout << "Node " << GetDevice()->GetIDNetworkNode () << " starts phy tx" << std::endl;
  GetUlChannel ()->StartTx (p, GetTxSignal (), GetDevice ());
}

void
BusLtePhy::StartRx (PacketBurst* p, TransmittedSignal* txSignal)
{
	//std::cout<<"bus rx"<<std::endl;//test
#ifdef TEST_DEVICE_ON_CHANNEL
  std::cout << "Bus Node " << GetDevice()->GetIDNetworkNode () << " starts phy rx" << std::endl;
#endif
  if(p->GetNPackets()>0){
	  //COMPUTE THE SINR
	  std::vector<double> measuredSinr;
	  std::vector<int> channelsForRx;
	  std::vector<double> rxSignalValues;
	  std::vector<double>::iterator it;
	  //rxSignalValues = txSignal->Getvalues();

	  double interference = 0;
	  //interference+=GetInterference()->ComputeInterference();
	  /*std::list<Packet* > packetsforinterference = p->GetPackets ();
	  std::list<Packet* >::iterator itpi=packetsforinterference.begin();
	  interference+=pow(10,(ComputePathLossForInterference(NetworkManager::Init()->GetNetworkNodeByID((*itpi)->GetSourceMAC()),GetDevice()))/10);*/

	  double noise_interference = 10. * log10 (pow(10., NOISE/10) + interference); // dB

	  /*int chId = 0;
	  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
		{
		  double power; // power transmission for the current sub channel [dB]
		  if ((*it) != 0.)
			{
			  power = (*it);
			  channelsForRx.push_back (chId);
			}
		  else
			{
			  power = 0.;
			}
		  chId++;
		  measuredSinr.push_back (power - noise_interference);
		}*/


	  //mouan
	  std::list<Packet* > packets = p->GetPackets ();
	  std::list<Packet* >::iterator itp;

	  Bus *thisbus=NetworkManager::Init()->GetBusByID(GetDevice()->GetIDNetworkNode());
	  AMCModule *amc=thisbus->GetProtocolStack()->GetMacEntity()->GetAmcModule();

	  std::vector<int>uecqi=amc->CreateCqiFeedbacks(measuredSinr);

	  int already=0,once=0,ueid;

	  UserEquipment *ue;

	  for (itp = packets.begin (); itp != packets.end (); itp++)
	  {
		  Packet* packet = (*itp);
		  ueid=packet->GetSourceMAC();
		  ue=NetworkManager::Init()->GetUserEquipmentByID(ueid);

		  if(NetworkManager::Init()->GetNetworkNodeByID(packet->GetSourceMAC())->GetNodeType() == NetworkNode::TYPE_BUS){
			  delete txSignal;
			  delete p;
			  return;
		  }
		  
		  Bus::UserEquipmentRecord *buer=thisbus->GetUserEquipmentRecord(packet->GetSourceMAC ());
		  if(buer==NULL){//this ue is not belongs to this bus
			  delete txSignal;
			  delete p;
			  return;
		  }

		  if(once==0){
			  AMCModule *amc=ue->GetProtocolStack()->GetMacEntity()->GetAmcModule();

			  rxSignalValues=ue->GetTargetNode()->GetPhy()->GetUlChannel()->GetPropagationLossModel()
				  ->AddLossModel(ue,ue->GetTargetNode(),ue->GetPhy()->GetTxSignal())->Getvalues();

			  int chId = 0;
			  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
				{
				  double power; // power transmission for the current sub channel [dB]
				  if ((*it) != 0.)
					{
					  power = (*it);
					  channelsForRx.push_back (chId);
					}
				  else
					{
					  power = 0.;
					}
				  chId++;
				  measuredSinr.push_back (power - noise_interference);
				}
			  uecqi=amc->CreateCqiFeedbacks(measuredSinr);
		  }

				  
				  
		  //add throughtputs delay
		  mouanth+=packet->GetSize();
		  //std::cout<<"MOUAN ["<<Simulator::Init()->Now ()<<"] BUSThroughtput "<<mouanth<<std::endl;//result
		  double avgcqi=0;
		  for(int mtest=0;mtest<uecqi.size();mtest++){
			  avgcqi+=uecqi.at(mtest);
		  }
		  if(uecqi.size()!=0){
			  avgcqi=avgcqi/uecqi.size();
			  if(avgcqi<16&&avgcqi>0){
				  int abc=int (avgcqi);
				  uecqith[ueid]+=cqitable[abc]*2;//*2??
				  std::cout<<"M ["<<Simulator::Init()->Now ()<<"] UE "<<ueid<<" CT "<<uecqith[ueid]<<" A "<<cqitable[abc]*2;//result
				  std::cout<<" BB "<<GetDevice()->GetIDNetworkNode();
				  std::cout<<std::endl;
				  totaldelay+=Simulator::Init()->Now () - packet->GetTimeStamp();
				  totaldelaynum++;

				  uepower+=pctable[int (avgcqi)];
				  //std::cout<<"M uepower A "<<temppower<<std::endl;
				  //compute bus power
				  std::vector<double> bussv=GetUlChannel()->GetPropagationLossModel()
					  ->AddLossModel(GetDevice(),NetworkManager::Init()->GetNetworkNodeByID(0),GetDevice()->GetPhy()->GetTxSignal())->Getvalues();
				  std::vector<double>::iterator busit;
				  std::vector<double> bsinr;

				  AMCModule *bamc=GetDevice()->GetProtocolStack()->GetMacEntity()->GetAmcModule();

				  int bchId = 0;
				  for (busit = bussv.begin(); busit != bussv.end(); busit++)
					{
					  double bpower; // power transmission for the current sub channel [dB]
					  if ((*busit) != 0.)
						{
						  bpower = (*busit);
						  channelsForRx.push_back (bchId);
						}
					  else
						{
						  bpower = 0.;
						}
					  bchId++;
					  bsinr.push_back (bpower - noise_interference);
					}
				  std::vector<int>bcqi=bamc->CreateCqiFeedbacks(bsinr);
				  double bavgcqi=0;
				  for(int bmtest=0;bmtest<bcqi.size();bmtest++){
					  bavgcqi+=bcqi.at(bmtest);
				  }
				  bavgcqi=bavgcqi/bcqi.size();
				  buspower+=bpctable[int (bavgcqi)];
			  }
		  }
		  /*double mouandelay=Simulator::Init()->Now () - packet->GetTimeStamp();
		  std::cout<<"MOUAN ["<<Simulator::Init()->Now ()<<"] BUSDelay "<<mouandelay<<std::endl;//result*/
		  

		  if(once==0){
			  buer->SetCQItwo(buer->GetCQIone());
			  buer->SetCQIone(buer->GetCQI());
			  buer->SetCQI(uecqi);
			  
			  /*//relaying
			  if(packet->GetDestinationMAC()!=GetDevice()->GetIDNetworkNode()){
				  StartTx(p);
				  delete txSignal;
				  delete p;
				  return;
			  }*/

			  //test Rx cqi success?
			  
			  /*std::cout<<"Bus: UE "<<ue->GetIDNetworkNode();
			  Bus *testbus=NetworkManager::Init()->GetBusByID(ue->GetTargetNode()->GetIDNetworkNode());
				std::vector<int> testtemp=testbus->GetUserEquipmentRecord(ue->GetIDNetworkNode())->GetCQI();
				int testj=0;
				for(std::vector<int>::iterator testit=testtemp.begin();testit!=testtemp.end();testit++,testj++){
					std::cout<<"cqi="<<testtemp.at(testj)<<std::endl;
				}*/

				//go	  

			  int compute=0,compute2=0,compute3=0;
			  std::vector<int> temp1=buer->GetCQI();
			  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
				  compute += (*i);
			  }
			  temp1=buer->GetCQIone();
			  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
				  compute2 += (*i);
			  }
			  temp1=buer->GetCQItwo();
			  for(std::vector<int>::iterator i=temp1.begin();i != temp1.end();i++){
				  compute3 += (*i);
			  }
			  //std::cout<<"comparing "<<compute<<" "<<compute2<<" "<<compute3<<std::endl;
			  if(compute<=compute2&&compute2<=compute3&&already==0){
				  already=1;
				  thisbus->GetProtocolStack()->GetRrcEntity()->GetHandoverEntity()->MouanCheckHandoverNeed(ue);//,txSignal);//run check ho
			  }
		  }
		  once=1;
	  }
	  //std::cout<<"bus's ue : "<<thisbus->GetUserEquipmentRecords()->size()<<std::endl;//test
  }

  //CHECK FOR PHY ERROR
  bool phyError = false;
  /*
  if (GetErrorModel() != NULL)
    {
	  std::vector<int> cqi; //compute the CQI
	  phyError = GetErrorModel ()->CheckForPhysicalError (channelsForRx, cqi, measuredSinr);
	  if (_PHY_TRACING_)
	    {
	      if (phyError)
	        {
		      std::cout << "**** YES PHY ERROR (node " << GetDevice ()->GetIDNetworkNode () << ") ****" << std::endl;
	        }
	      else
	        {
		      std::cout << "**** NO PHY ERROR (node " << GetDevice ()->GetIDNetworkNode () << ") ****" << std::endl;
	        }
	    }
    }
    */

  if (!phyError && p->GetNPackets() > 0)
    {
	  //FORWARD RECEIVED PACKETS TO THE DEVICE
	  GetDevice()->ReceivePacketBurst(p);
    }
  //std::cout<<"bus rx fin"<<std::endl;//test
  delete txSignal;
  delete p;
}

void
BusLtePhy::SendIdealControlMessage (IdealControlMessage *msg)
{
  if (msg->GetMessageType () == IdealControlMessage::ALLOCATION_MAP)
	{
	  Bus *bus = (Bus*) GetDevice ();
	  Bus::UserEquipmentRecords* registeredUe = bus->GetUserEquipmentRecords ();
	  Bus::UserEquipmentRecords::iterator it;

	  for (it = registeredUe->begin (); it != registeredUe->end (); it++)
	    {
		  //std::cout << "SendIdealControlMessage to " << (*it)->GetUE ()->GetIDNetworkNode() << std::endl;
		  (*it)->GetUE ()->GetPhy ()->ReceiveIdealControlMessage (msg);
	    }
	}
}

void
BusLtePhy::ReceiveIdealControlMessage (IdealControlMessage *msg)
{
#ifdef TEST_CQI_FEEDBACKS
  std::cout << "ReceiveIdealControlMessage (PHY) from  " << msg->GetSourceDevice ()->GetIDNetworkNode ()
		  << " to " << msg->GetDestinationDevice ()->GetIDNetworkNode () << std::endl;
#endif

  //RECEIVE CQI FEEDBACKS
  if (msg->GetMessageType () == IdealControlMessage::CQI_FEEDBACKS)
    {
	  CqiIdealControlMessage *cqiMsg = (CqiIdealControlMessage*) msg;
	  BusMacEntity *mac = (BusMacEntity*) GetDevice ()->GetProtocolStack ()->GetMacEntity ();
      mac->ReceiveCqiIdealControlMessage (cqiMsg);
    }
  if (msg->GetMessageType () == IdealControlMessage::SCHEDULING_REQUEST)
    {
	  SchedulingRequestIdealControlMessage *srMsg = (SchedulingRequestIdealControlMessage*) msg;
	  BusMacEntity *mac = (BusMacEntity*) GetDevice ()->GetProtocolStack ()->GetMacEntity ();
	  mac->ReceiveSchedulingRequestIdealControlMessage (srMsg);
    }
}
