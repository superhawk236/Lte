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


#include "position-based-ho-manager.h"
#include "../../../device/NetworkNode.h"
#include "../../../device/UserEquipment.h"
#include "../../../device/ENodeB.h"
#include "../../../device/Bus.h"
#include "../../../device/HeNodeB.h"
#include "../../../componentManagers/NetworkManager.h"

#include "../../../core/spectrum/bandwidth-manager.h"
#include "../../../phy/lte-phy.h"
//#include "../../../utility/ComputePathLoss.h"
#include "../../../channel/LteChannel.h"
#include "../../../channel/propagation-model/propagation-loss-model.h"
#include "../../../core/eventScheduler/simulator.h"

#include "../../mac/packet-scheduler/uplink-packet-scheduler.h"
#include "../../mac/mac-entity.h"
#include "../../packet/Packet.h"
#include "../../packet/packet-burst.h"
#include "../../../flows/radio-bearer.h"
#include "../../rrc/rrc-entity.h"
#include "../../../flows/application/Application.h"
#include "../../mac/AMCModule.h"
#include "../../../flows/MacQueue.h"
#include "../../../utility/eesm-effective-sinr.h"
#include "../../../core/spectrum/transmitted-signal.h"

//extern NetworkManager *nm;
extern double uecheck[60];

PositionBasedHoManager::PositionBasedHoManager()
{
  m_target = NULL;
}

PositionBasedHoManager::~PositionBasedHoManager()
{
  m_target = NULL;
}

bool
PositionBasedHoManager::CheckHandoverNeed (UserEquipment* ue)
{
//	std::cout<<"position based ho"<<std::endl;//test
	std::cout<<"ERROR no need"<<std::endl;//test
  NetworkNode *targetNode = ue->GetTargetNode ();

  CartesianCoordinates *uePosition = ue->GetMobilityModel ()->GetAbsolutePosition ();
  CartesianCoordinates *targetPosition;

  targetPosition = targetNode->GetMobilityModel ()->GetAbsolutePosition ();
  double targetDistance = uePosition->GetDistance (targetPosition);

  /*
  if (targetDistance <= (ue->GetCell ()->GetRadius () * 0.8))
    {
	  return false;
    }
  */

  std::vector<ENodeB*> *listOfNodes = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;
  for (it = listOfNodes->begin (); it != listOfNodes->end (); it++)
    {
	  if ((*it)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (uePosition);

	      if (distance < targetDistance)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }
  std::vector<HeNodeB*> *listOfNodes2 = NetworkManager::Init ()->GetHomeENodeBContainer();
  std::vector<HeNodeB*>::iterator it2;
  for (it2 = listOfNodes2->begin (); it2 != listOfNodes2->end (); it2++)
    {
	  if ((*it2)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it2);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (uePosition);

	      if (distance < targetDistance)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }

  if (ue->GetTargetNode ()->GetIDNetworkNode () != targetNode->GetIDNetworkNode ())
    {
	  m_target = targetNode;
	  return true;
    }
  else
    {
	  return false;
    }
}

//mouan
bool
PositionBasedHoManager::CheckBusHandoverNeed (Bus* bus)
{
	//std::cout<<"position based ho bus"<<std::endl;//test
  NetworkNode *targetNode = bus->GetTargetNode ();

  CartesianCoordinates *busPosition = bus->GetMobilityModel ()->GetAbsolutePosition ();
  CartesianCoordinates *targetPosition;

  targetPosition = targetNode->GetMobilityModel ()->GetAbsolutePosition ();
  double targetDistance = busPosition->GetDistance (targetPosition);

  /*
  if (targetDistance <= (ue->GetCell ()->GetRadius () * 0.8))
    {
	  return false;
    }
  */

  std::vector<ENodeB*> *listOfNodes = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;
  for (it = listOfNodes->begin (); it != listOfNodes->end (); it++)
    {
	  if ((*it)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (busPosition);

	      if (distance < targetDistance)
	        {
	    	  //if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,bus))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }
  /*std::vector<HeNodeB*> *listOfNodes2 = NetworkManager::Init ()->GetHomeENodeBContainer();
  std::vector<HeNodeB*>::iterator it2;
  for (it2 = listOfNodes2->begin (); it2 != listOfNodes2->end (); it2++)
    {
	  if ((*it2)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it2);


	      double distance = probableNewTargetNode->GetMobilityModel ()->
	    		  GetAbsolutePosition ()->GetDistance (uePosition);

	      if (distance < targetDistance)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
			      targetDistance = distance;
			      targetNode = probableNewTargetNode;
	    	  }
	        }
	    }
    }*/

  if (bus->GetTargetNode ()->GetIDNetworkNode () != targetNode->GetIDNetworkNode ())
    {
		//std::cout<<"bus HOing"<<std::endl;//test
	  m_target = targetNode;
	  return true;
    }
  else
    {
	  return false;
    }
}




bool
PositionBasedHoManager::MouanCheckHandoverNeed (UserEquipment* ue)//,TransmittedSignal *signalvalue)
{
	//std::cout<<ue->GetIDNetworkNode()<<" "<<(Simulator::Init()->Now () - uecheck[ue->GetIDNetworkNode()])<<std::endl;//test
	double hointerval=0.2;//HO interval 0.1s
	if(ue->GetTargetNode()->GetNodeType()==NetworkNode::TYPE_BUS)hointerval=1.0;
	if((Simulator::Init()->Now () - uecheck[ue->GetIDNetworkNode()])<hointerval)return false;
	uecheck[ue->GetIDNetworkNode()]=Simulator::Init()->Now ();
	//std::cout<<"mouan check ho"<<std::endl;//test
  //NetworkNode *targetNode = ue->GetTargetNode ();
  int compare[2]={0,0};
  int selected_bus_id=0;
  ENodeB *targetenb=NULL;
  //enb vs ue
  {	  
	  int needreg=0;
	  std::vector<ENodeB *> *enbs=NetworkManager::Init()->GetENodeBContainer();
		double mindis=9999999.0;
		for(std::vector<ENodeB *>::iterator ie=enbs->begin();ie!=enbs->end();ie++){
			CartesianCoordinates *dis=(*ie)->GetCell()->GetCellCenterPosition();
			double temp=dis->GetDistance(ue->GetMobilityModel()->GetAbsolutePosition());
			if(mindis>temp){
				mindis=temp;
				targetenb=(*ie);
			}
		}
		if(targetenb==NULL){
			std::cout<<"ERROR! targetenb==NULL"<<std::endl;
		}
		else{
			//targetNode=NetworkManager::Init()->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
		}
	//ENodeB *targetenb=NetworkManager::Init()->GetENodeBByID(targetNode->GetIDNetworkNode());
	//std::vector<int> uecqi=targetenb->GetUserEquipmentRecord(ue->GetIDNetworkNode())->GetCQI();

		if(targetenb->GetUserEquipmentRecord(ue->GetIDNetworkNode())==false){
			needreg=1;
			targetenb->RegisterUserEquipment(ue);
		}

		std::vector<double> mouan_bstoueSinr;
		  std::vector<double> rxSignalValues;
		  std::vector<double>::iterator it;

		  //rxSignalValues = ue->GetPhy()->GetTxSignal()->Getvalues();
		  //rxSignalValues=signalvalue->Getvalues();
		  rxSignalValues=targetenb->GetPhy()->GetUlChannel()->GetPropagationLossModel()
			  ->AddLossModel(ue,targetenb,ue->GetPhy()->GetTxSignal())->Getvalues();
		  //std::cout<<"mouantest"<<std::endl;//test

		  //compute noise + interference
		  double interference;
		  if (ue->GetPhy()->GetInterference () != NULL)
			{
				//ENodeB *node;
				interference = 0;

				/*std::vector<ENodeB*> *eNBs = NetworkManager::Init()->GetENodeBContainer ();//fixed
				std::vector<ENodeB*>::iterator it;

				//std::cout << "Compute interference for UE " << ue->GetIDNetworkNode () << " ,target node " << ue->GetTargetNode ()->GetIDNetworkNode ()<< std::endl;

				for (it = eNBs->begin (); it != eNBs->end (); it++)
				{
				  node = (*it);
				  if (node->GetIDNetworkNode () != targetenb->GetIDNetworkNode () &&
					  node->GetPhy ()->GetBandwidthManager ()->GetUlOffsetBw () ==
					  ue->GetTargetNode ()->GetPhy ()->GetBandwidthManager ()->GetUlOffsetBw ())
					{
					  double powerTXForSubBandwidth = 10 * log10 (
							  pow (10., (node->GetPhy()->GetTxPower() - 30)/10)
							  /
							  node->GetPhy()->GetBandwidthManager ()->GetUlSubChannels().size ());


					  double nodeInterference_db = powerTXForSubBandwidth - 10 - ComputePathLossForInterference (node, ue); // in dB
					  double nodeInterference = pow(10, nodeInterference_db/10);

					  interference += nodeInterference;

					}
				}*/
			}
		  else
			{
			  interference = 0;
			}
		  //interference+=pow(10,ComputePathLossForInterference(ue,targetenb)/10);
		  //interference+=pow(10,(ComputePathLossForInterference(ue,targetenb))/10);

		  double noise_interference = 10. * log10 (pow(10., -148.95/10) + interference); // dB


		  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
			{
			  double power; // power transmission for the current sub channel [dB]
			  if ((*it) != 0.)
				{
				  power = (*it);
				}
			  else
				{
				  power = 0.;
				}
			  mouan_bstoueSinr.push_back (power - noise_interference);
			}
		  AMCModule *amc = ue->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ();
		  std::vector<int> uecqi = amc->CreateCqiFeedbacks (mouan_bstoueSinr);
		  // sche part
		  /*
		  UplinkPacketScheduler *ulps=targetenb->GetULScheduler();

		  ulps->SelectUsersToSchedule();

		UplinkPacketScheduler::UsersToSchedule *users = ulps->GetUsersToSchedule();
		UplinkPacketScheduler::UserToSchedule* scheduledUser;
		int nbOfRBs = ulps->GetMacEntity()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetUlSubChannels ().size ();

		int availableRBs;     // No of RB's not allocated
		int unallocatedUsers; // No of users who remain unallocated
		int selectedUser;     // user to be selected for allocation
		int selectedPRB;      // PRB to be selected for allocation
		double bestMetric;    // best metric to identify user/RB combination
		int left, right;      // index of left and left PRB's to check
		if(nbOfRBs>1000||users->size()>1000){
			std::cout<<"ERROR! some value wrong."<<std::endl;
			std::cout<<"RBs "<<nbOfRBs<<" user size "<<users->size()<<std::endl;
			return false;
		}
		bool Allocated[nbOfRBs];
		bool allocationMade;
		double metrics[nbOfRBs][users->size ()];
		int requiredPRBs[users->size ()];
		//std::vector<int> m_mouan;
		int m_mouan[users->size()];
		int ueat=-1;

		for(int i=0 ; i < users->size() ; i++){
			m_mouan[i]=0;
		}

		//Some initialization
		availableRBs = nbOfRBs;
		unallocatedUsers = users->size ();
		for(int i=0; i < nbOfRBs; i++)
			Allocated[i] = false;

		//create a matrix of flow metrics
		for (int i = 0; i < nbOfRBs; i++)
		{
			for (int j = 0; j < users->size (); j++)
			{
				//metrics[i][j] = ComputeSchedulingMetric (users->at (j), i);
				double spectralEfficiency = ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (uecqi.at(i));
				metrics[i][j] = spectralEfficiency * 180000;
			}
		}

		//create number of required PRB's per scheduled users
		for(int j=0; j < users->size(); j++)
		{
			scheduledUser = users->at(j);

			std::vector<double> sinrs;
			for(int c=0;c<scheduledUser->m_channelContition.size();c++)
			//for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
			//		c != scheduledUser->m_channelContition.end (); c++)
			{
				if(scheduledUser->m_channelContition.at(c)>15||scheduledUser->m_channelContition.at(c)<0){
					std::cout<<"ERROR! m_channelContition = "<<scheduledUser->m_channelContition.at(c)<<std::endl;
					return false;
				}
				sinrs.push_back (ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (scheduledUser->m_channelContition.at(c)));
			}

			double effectiveSinr =  GetEesmEffectiveSinr (sinrs);

			int mcs = ulps->GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
				  ulps->GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
			//scheduledUser->m_selectedMCS = mcs;

			requiredPRBs[j] = (floor) (scheduledUser->m_dataToTransmit /
					  (ulps->GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
		}

		  //RBs allocation

		  while(availableRBs > 0 && unallocatedUsers > 0) //
		  {
			  // First step: find the best user-RB combo
			  selectedPRB = -1;
			  selectedUser = -1;
			  bestMetric = (double) (-(1<<30));

			  for(int i=0; i < nbOfRBs; i++)
			  {
				  if (!Allocated[i]){ // check only unallocated PRB's
					  for(int j=0; j < users->size (); j++)
					  {
						  if ( users->at (j)->m_listOfAllocatedRBs.size() == 0
								  && requiredPRBs[j] > 0) //only unallocated users requesting some RB's
							  if (bestMetric < metrics[i][j]){
								  selectedPRB = i;
								  selectedUser = j;
								  bestMetric = metrics[i][j];
							  }
					  }

				  }
			  }
			  // Now start allocating for the selected user at the selected PRB the required blocks
			  // using how many PRB's are needed for the user
			  if (selectedUser != -1)
			  {
				  scheduledUser = users->at(selectedUser);
				  //scheduledUser->m_listOfAllocatedRBs.push_back (selectedPRB);
				  m_mouan[selectedUser]++;
				  Allocated[selectedPRB] = true;
				  left =  selectedPRB - 1;
				  right = selectedPRB + 1;
				  availableRBs--;
				  unallocatedUsers--;

				  allocationMade = true;
				  for(int i = 1; i < requiredPRBs[selectedUser] && availableRBs > 0 && allocationMade; i++ )
				  { // search right and left of initial allocation
					  allocationMade = false;
					  if (left >=0 && Allocated[left] && right < nbOfRBs && Allocated[right])
						  break; // nothing is available, since we need to have contiguous allocation

					  if (    (right < nbOfRBs) && (! Allocated[right]) &&
							  (
									  ((left >=0) &&
									  (metrics[right][selectedUser] >= metrics[left][selectedUser])) // right is better than left
									  || (left < 0) || Allocated[left]// OR no more left
							  )
						)
					  {
						  //Allocate PRB at right to the user
						  Allocated[right] = true;
						  //scheduledUser->m_listOfAllocatedRBs.push_back (right);
						  m_mouan[selectedUser]++;
						  right++;
						  allocationMade = true;
						  availableRBs--;
					  } else if ( (left >=0) && (! Allocated[left]) &&
								  (
									  ((right < nbOfRBs) &&
									  (metrics[left][selectedUser] > metrics[right][selectedUser])) //left better than right
									  || (right >= nbOfRBs) || Allocated[right]// OR no more right
								   )
								)
					  {
						  //Allocate PRB at left to the user
						  Allocated[left] = true;
						  //scheduledUser->m_listOfAllocatedRBs.push_back (left);
						  m_mouan[selectedUser]++;
						  left--;
						  allocationMade = true;
						  availableRBs--;
					  }
				  } // end of for
				  
				  if(scheduledUser->m_userToSchedule->GetIDNetworkNode()==ue->GetIDNetworkNode()){
					ueat=selectedUser;
					printf("selected user %d is ueat\n",ueat);//test
				  }
			  } else { // nothing to do exit the allocation loop
				  break;
			  }
		  } //while
			//copy end
			if(ueat<0){
				std::cout<<"ERROR! ueat="<<ueat<<std::endl;
				return false;//temp
			}
			compare[1]=m_mouan[ueat];//m_mouan[ueat];//bs vs ue result
			
			ulps->DeleteUsersToSchedule();*/
			if(needreg==1){
				targetenb->DeleteUserEquipment(ue);
			}

			//no sche(SCFDMA)
			for(int no=0;no<uecqi.size();no++)
				compare[1]+=uecqi.at(no);
  }



  //bus vs ue
  {
	  std::vector<Bus *> *buses=NetworkManager::Init()->GetBusContainer();
	  std::vector<Bus *>::iterator select_bus;

	  for(select_bus=buses->begin();select_bus != buses->end();select_bus++)
	  {

		  int needreg=0;

		  Bus *selectedbus=(*select_bus);

		  if(selectedbus->GetUserEquipmentRecord(ue->GetIDNetworkNode())==false){
			  needreg=1;
			  selectedbus->RegisterUserEquipment(ue);
		  }

		  std::vector<double> mouan_bustoueSinr;
		  std::vector<double> rxSignalValues;
		  std::vector<double>::iterator it;

		  //rxSignalValues = ue->GetPhy()->GetTxSignal()->Getvalues();
		  //rxSignalValues=signalvalue->Getvalues();
		  rxSignalValues=selectedbus->GetPhy()->GetUlChannel()->GetPropagationLossModel()
			  ->AddLossModel(ue,selectedbus,ue->GetPhy()->GetTxSignal())->Getvalues();
		  //std::cout<<"mouantest2"<<std::endl;//test

		  //compute noise + interference
		  double interference;
		  if (ue->GetPhy()->GetInterference () != NULL)
			{
				ENodeB *node;
				interference = 0;

				/*std::vector<ENodeB*> *eNBs = NetworkManager::Init()->GetENodeBContainer ();
				std::vector<ENodeB*>::iterator it;

				//std::cout << "Compute interference for UE " << ue->GetIDNetworkNode () << " ,target node " << ue->GetTargetNode ()->GetIDNetworkNode ()<< std::endl;

				for (it = eNBs->begin (); it != eNBs->end (); it++)
				{
				  node = (*it);
				  if (node->GetIDNetworkNode () != ue->GetTargetNode ()->GetIDNetworkNode () &&
					  node->GetPhy ()->GetBandwidthManager ()->GetUlOffsetBw () ==
					  ue->GetTargetNode ()->GetPhy ()->GetBandwidthManager ()->GetUlOffsetBw ())
					{
					  double powerTXForSubBandwidth = 10 * log10 (
							  pow (10., (node->GetPhy()->GetTxPower() - 30)/10)
							  /
							  node->GetPhy()->GetBandwidthManager ()->GetUlSubChannels().size ());


					  double nodeInterference_db = powerTXForSubBandwidth - 10 - ComputePathLossForInterference (node, ue); // in dB
					  double nodeInterference = pow(10, nodeInterference_db/10);

					  interference += nodeInterference;
					}
				}*/
			}
		  else
			{
			  interference = 0;
			}

		  //interference+=pow(10,(ComputePathLossForInterference(ue,selectedbus))/10);


		  double noise_interference = 10. * log10 (pow(10., -148.95/10) + interference); // dB

		  for (it = rxSignalValues.begin(); it != rxSignalValues.end(); it++)
			{
			  double power; // power transmission for the current sub channel [dB]
			  if ((*it) != 0.)
				{
				  power = (*it);
				}
			  else
				{
				  power = 0.;
				}
			  mouan_bustoueSinr.push_back (power - noise_interference);
			}

		  AMCModule *amc = ue->GetProtocolStack ()->GetMacEntity ()->GetAmcModule ();
		  std::vector<int> mouan_bustouecqi = amc->CreateCqiFeedbacks (mouan_bustoueSinr);

		  //run the bus scheduler
/*
			UplinkPacketScheduler *ulps=selectedbus->GetULScheduler();
			ulps->SelectUsersToSchedule();

			UplinkPacketScheduler::UsersToSchedule *users = ulps->GetUsersToSchedule();
			UplinkPacketScheduler::UserToSchedule* scheduledUser;
			int nbOfRBs = ulps->GetMacEntity()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetUlSubChannels ().size ();

			int availableRBs;     // No of RB's not allocated
			int unallocatedUsers; // No of users who remain unallocated
			int selectedUser;     // user to be selected for allocation
			int selectedPRB;      // PRB to be selected for allocation
			double bestMetric;    // best metric to identify user/RB combination
			int left, right;      // index of left and left PRB's to check
			if(nbOfRBs>1000||users->size()>1000){
				std::cout<<"ERROR! some value wrong."<<std::endl;
				std::cout<<"RBs "<<nbOfRBs<<" user size "<<users->size()<<std::endl;
				return false;
			}
			bool Allocated[nbOfRBs];
			bool allocationMade;
			double metrics[nbOfRBs][users->size ()];
			int requiredPRBs[users->size ()];
			//std::vector<int> m_mouan;
			int m_mouan[users->size()];
			int ueat=-1;
			for(int i=0;i<users->size();i++)m_mouan[i]=0;

			//Some initialization
			availableRBs = nbOfRBs;
			unallocatedUsers = users->size ();
			for(int i=0; i < nbOfRBs; i++)
				Allocated[i] = false;

			//create a matrix of flow metrics
			for (int i = 0; i < nbOfRBs; i++)
			{
				for (int j = 0; j < users->size (); j++)
				{
					//metrics[i][j] = ComputeSchedulingMetric (users->at (j), i);
					double spectralEfficiency = ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (mouan_bustouecqi.at(i));
					metrics[i][j] = spectralEfficiency * 180000;
				}
			}

			//create number of required PRB's per scheduled users
			for(int j=0; j < users->size(); j++)
			{
				scheduledUser = users->at(j);

				std::vector<double> sinrs;
				for(int c=0;c<scheduledUser->m_channelContition.size();c++)
				//for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
				//		c != scheduledUser->m_channelContition.end (); c++)
				{
					if(scheduledUser->m_channelContition.at(c)>15||scheduledUser->m_channelContition.at(c)<0){
						std::cout<<"ERROR! m_channelContition = "<<scheduledUser->m_channelContition.at(c)<<std::endl;
						return false;
					}
					sinrs.push_back (ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (scheduledUser->m_channelContition.at(c)));
				}
				double effectiveSinr =  GetEesmEffectiveSinr (sinrs);

				int mcs = ulps->GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
					  ulps->GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
				//scheduledUser->m_selectedMCS = mcs;
				requiredPRBs[j] = (floor) (scheduledUser->m_dataToTransmit /
						  (ulps->GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
			}

			  //RBs allocation

			  while(availableRBs > 0 && unallocatedUsers > 0) //
			  {
				  // First step: find the best user-RB combo
				  selectedPRB = -1;
				  selectedUser = -1;
				  bestMetric = (double) (-(1<<30));

				  for(int i=0; i < nbOfRBs; i++)
				  {
					  if (!Allocated[i]){ // check only unallocated PRB's
						  for(int j=0; j < users->size (); j++)
						  {
							  if ( users->at (j)->m_listOfAllocatedRBs.size() == 0
									  && requiredPRBs[j] > 0) //only unallocated users requesting some RB's
								  if (bestMetric < metrics[i][j]){
									  selectedPRB = i;
									  selectedUser = j;
									  bestMetric = metrics[i][j];
								  }
						  }

					  }
				  }
				  // Now start allocating for the selected user at the selected PRB the required blocks
				  // using how many PRB's are needed for the user
				  if (selectedUser != -1)
				  {
					  scheduledUser = users->at(selectedUser);
					  //scheduledUser->m_listOfAllocatedRBs.push_back (selectedPRB);
					  m_mouan[selectedUser]++;
					  Allocated[selectedPRB] = true;
					  left =  selectedPRB - 1;
					  right = selectedPRB + 1;
					  availableRBs--;
					  unallocatedUsers--;

					  allocationMade = true;
					  for(int i = 1; i < requiredPRBs[selectedUser] && availableRBs > 0 && allocationMade; i++ )
					  { // search right and left of initial allocation
						  allocationMade = false;
						  if (left >=0 && Allocated[left] && right < nbOfRBs && Allocated[right])
							  break; // nothing is available, since we need to have contiguous allocation

						  if (    (right < nbOfRBs) && (! Allocated[right]) &&
								  (
										  ((left >=0) &&
										  (metrics[right][selectedUser] >= metrics[left][selectedUser])) // right is better than left
										  || (left < 0) || Allocated[left]// OR no more left
								  )
							)
						  {
							  //Allocate PRB at right to the user
							  Allocated[right] = true;
							  //scheduledUser->m_listOfAllocatedRBs.push_back (right);
							  m_mouan[selectedUser]++;
							  right++;
							  allocationMade = true;
							  availableRBs--;
						  } else if ( (left >=0) && (! Allocated[left]) &&
									  (
										  ((right < nbOfRBs) &&
										  (metrics[left][selectedUser] > metrics[right][selectedUser])) //left better than right
										  || (right >= nbOfRBs) || Allocated[right]// OR no more right
									   )
									)
						  {
							  //Allocate PRB at left to the user
							  Allocated[left] = true;
							  //scheduledUser->m_listOfAllocatedRBs.push_back (left);
							  m_mouan[selectedUser]++;
							  left--;
							  allocationMade = true;
							  availableRBs--;
						  }
					  } // end of for
					  
					  if(scheduledUser->m_userToSchedule->GetIDNetworkNode()==ue->GetIDNetworkNode()){
						ueat=selectedUser;
						printf("selected user %d is ueat\n",ueat);//test
					  }
				  } else { // nothing to do exit the allocation loop
					  break;
				  }
			  }//end while
			  if(ueat<0){
				  std::cout<<"ERROR! ueat="<<ueat<<std::endl;
				  return false;
			  }
			  if(m_mouan[ueat]>compare[0]){
				compare[0]=m_mouan[ueat];//bus vs ue
				selected_bus_id=selectedbus->GetIDNetworkNode();
			  }
			  ulps->DeleteUsersToSchedule();*/
			  if(needreg==1){
				  selectedbus->DeleteUserEquipment(ue);
			  }

			  //no sche
			  int bustemp=0;
				for(int no=0;no<mouan_bustouecqi.size();no++)
					bustemp+=mouan_bustouecqi.at(no);
				if(bustemp>compare[0]){
					compare[0]=bustemp;
					selected_bus_id=selectedbus->GetIDNetworkNode();
				}
			  
	  }//end for
  }


  //enb vs bus
  {
	  /*std::vector<int> buscqi=targetenb->GetUserEquipmentRecord(selected_bus_id)->GetCQI();
	  UplinkPacketScheduler *ulps=targetenb->GetULScheduler();

		UplinkPacketScheduler::UsersToSchedule *users = ulps->GetUsersToSchedule();
		ulps->SelectUsersToSchedule ();
		UplinkPacketScheduler::UserToSchedule* scheduledUser;
		int nbOfRBs = ulps->GetMacEntity()->GetDevice ()->GetPhy ()->GetBandwidthManager ()->GetUlSubChannels ().size ();

		int availableRBs;     // No of RB's not allocated
		int unallocatedUsers; // No of users who remain unallocated
		int selectedUser;     // user to be selected for allocation
		int selectedPRB;      // PRB to be selected for allocation
		double bestMetric;    // best metric to identify user/RB combination
		int left, right;      // index of left and left PRB's to check
		bool Allocated[nbOfRBs];
		bool allocationMade;
		double metrics[nbOfRBs][users->size ()];
		int requiredPRBs[users->size ()];
		//std::vector<int> m_mouan;
		int m_mouan[users->size()];
		int busat;
		for(int i=0;i<users->size();i++)m_mouan[i]=0;


		//Some initialization
		availableRBs = nbOfRBs;
		unallocatedUsers = users->size ();
		for(int i=0; i < nbOfRBs; i++)
			Allocated[i] = false;

		//create a matrix of flow metrics
		for (int i = 0; i < nbOfRBs; i++)
		{
			for (int j = 0; j < users->size (); j++)
			{
				//metrics[i][j] = ComputeSchedulingMetric (users->at (j), i);
				double spectralEfficiency = ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (buscqi.at(i));
				metrics[i][j] = spectralEfficiency * 180000;
			}
		}

		//create number of required PRB's per scheduled users
		for(int j=0; j < users->size(); j++)
		{
			scheduledUser = users->at(j);

			std::vector<double> sinrs;
			for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
					c != scheduledUser->m_channelContition.end (); c++)
			{
				//cout << *c <<" ";
				sinrs.push_back (ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (*c));
			}


			double effectiveSinr =  GetEesmEffectiveSinr (sinrs);

			int mcs = ulps->GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
				  ulps->GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
			//scheduledUser->m_selectedMCS = mcs;
			requiredPRBs[j] = (floor) (scheduledUser->m_dataToTransmit /
					  (ulps->GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
		}

	  //RBs allocation

	  while(availableRBs > 0 && unallocatedUsers > 0) //
	  {
		  // First step: find the best user-RB combo
		  selectedPRB = -1;
		  selectedUser = -1;
		  bestMetric = (double) (-(1<<30));

		  for(int i=0; i < nbOfRBs; i++)
		  {
			  if (!Allocated[i]){ // check only unallocated PRB's
				  for(int j=0; j < users->size (); j++)
				  {
					  if ( users->at (j)->m_listOfAllocatedRBs.size() == 0
							  && requiredPRBs[j] > 0) //only unallocated users requesting some RB's
						  if (bestMetric < metrics[i][j]){
							  selectedPRB = i;
							  selectedUser = j;
							  bestMetric = metrics[i][j];
						  }
				  }

			  }
		  }
		  // Now start allocating for the selected user at the selected PRB the required blocks
		  // using how many PRB's are needed for the user
		  if (selectedUser != -1)
		  {
			  scheduledUser = users->at(selectedUser);
			  //scheduledUser->m_listOfAllocatedRBs.push_back (selectedPRB);
			  m_mouan[selectedUser]++;
			  Allocated[selectedPRB] = true;
			  left =  selectedPRB - 1;
			  right = selectedPRB + 1;
			  availableRBs--;
			  unallocatedUsers--;

			  allocationMade = true;
			  for(int i = 1; i < requiredPRBs[selectedUser] && availableRBs > 0 && allocationMade; i++ )
			  { // search right and left of initial allocation
				  allocationMade = false;
				  if (left >=0 && Allocated[left] && right < nbOfRBs && Allocated[right])
					  break; // nothing is available, since we need to have contiguous allocation

				  if (    (right < nbOfRBs) && (! Allocated[right]) &&
						  (
								  ((left >=0) &&
								  (metrics[right][selectedUser] >= metrics[left][selectedUser])) // right is better than left
								  || (left < 0) || Allocated[left]// OR no more left
						  )
					)
				  {
					  //Allocate PRB at right to the user
					  Allocated[right] = true;
					  //scheduledUser->m_listOfAllocatedRBs.push_back (right);
					  m_mouan[selectedUser]++;
					  right++;
					  allocationMade = true;
					  availableRBs--;
				  } else if ( (left >=0) && (! Allocated[left]) &&
							  (
								  ((right < nbOfRBs) &&
								  (metrics[left][selectedUser] > metrics[right][selectedUser])) //left better than right
								  || (right >= nbOfRBs) || Allocated[right]// OR no more right
							   )
							)
				  {
					  //Allocate PRB at left to the user
					  Allocated[left] = true;
					  //scheduledUser->m_listOfAllocatedRBs.push_back (left);
					  m_mouan[selectedUser]++;
					  left--;
					  allocationMade = true;
					  availableRBs--;
				  }
			  } // end of for
			  
			  if(scheduledUser->m_userToSchedule->GetIDNetworkNode()==selected_bus_id){
				busat=selectedUser;
				printf("selected bus %d is busat\n",busat);//test
			  }
		  } else { // nothing to do exit the allocation loop
			  break;
		  }
	  } //while
		//copy end
	  if(compare[0]>m_mouan[busat]){
		  compare[0]=m_mouan[busat];
	  }*/
  }
  //std::cout<<"mouan check result BUS "<<compare[0]<<" ENB "<<compare[1]<<std::endl;
  //std::cout<<compare[0]<<" : "<<compare[1]<<std::endl;
  if(abs(compare[0]-compare[1])<=(0.1*compare[0])){//better 10 %
	  //std::cout<<"false"<<std::endl;
	  return false;
  }
  if(compare[0]<=compare[1]){//using bs
	  if(ue->GetTargetNode()->GetIDNetworkNode()==targetenb->GetIDNetworkNode()){
		  return false;
	  }
	  else{
		  m_target=NetworkManager::Init()->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
		  //std::cout<<"HO TRUE"<<std::endl;//test

		  NetworkNode* newTagertNode = m_target;

#ifdef HANDOVER_DEBUG
		  std::cout << "** HO ** \t time: " << time << " user " <<  ue->GetIDNetworkNode () <<
				  " old eNB " << ue->GetTargetNode()->GetIDNetworkNode () <<
				  " new eNB " << newTagertNode->GetIDNetworkNode () << std::endl;
#endif
		  double time=0;
		  //NetworkManager::Init()->HandoverProcedure(time, ue, ue->GetTargetNode() , newTagertNode);

		  return true;
	  }
  }
  else{//using bus
	  if(ue->GetTargetNode()->GetIDNetworkNode()==selected_bus_id){
		  return false;
	  }
	  else{
		  m_target=NetworkManager::Init()->GetNetworkNodeByID(selected_bus_id);
		 // std::cout<<"HO TRUE"<<std::endl;//test
		  
		  NetworkNode* newTagertNode = m_target;

#ifdef HANDOVER_DEBUG
		  std::cout << "** HO ** \t time: " << time << " user " <<  ue->GetIDNetworkNode () <<
				  " old eNB " << ue->GetTargetNode()->GetIDNetworkNode () <<
				  " new eNB " << newTagertNode->GetIDNetworkNode () << std::endl;
#endif
		  double time=0;
		  //NetworkManager::Init()->HandoverProcedure(time, ue, ue->GetTargetNode(), newTagertNode);

		  return true;
	  }
  }
  return false;//test temp
}


/*
bool
PositionBasedHoManager::MouanCheckHandoverNeed (UserEquipment* ue)//,TransmittedSignal *signalvalue)
{
	double hointerval=0.2;//HO interval 0.1s
	if(ue->GetTargetNode()->GetNodeType()==NetworkNode::TYPE_BUS)hointerval=1.0;
	if((Simulator::Init()->Now () - uecheck[ue->GetIDNetworkNode()])<hointerval)return false;
	uecheck[ue->GetIDNetworkNode()]=Simulator::Init()->Now ();
	
  //int compare[2]={0,0};
  //int selected_bus_id=0;
  ENodeB *targetenb=NULL;
	  std::vector<ENodeB *> *enbs=NetworkManager::Init()->GetENodeBContainer();
		double mindis=9999999.0,tempenb=0,tempbus=0,mindisbus=9999999.0;
		for(std::vector<ENodeB *>::iterator ie=enbs->begin();ie!=enbs->end();ie++){
			CartesianCoordinates *dis=(*ie)->GetCell()->GetCellCenterPosition();
			tempenb=dis->GetDistance(ue->GetMobilityModel()->GetAbsolutePosition());
			if(mindis>tempenb){
				mindis=tempenb;
				targetenb=(*ie);
			}
		}
		if(targetenb==NULL){
			std::cout<<"ERROR! targetenb==NULL"<<std::endl;
		}
		else{
			//targetNode=NetworkManager::Init()->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
		}

		Bus *targetbus=NULL;
		std::vector<Bus *> *buses=NetworkManager::Init()->GetBusContainer();

		for(std::vector<Bus *>::iterator ib=buses->begin();ib!=buses->end();ib++){
			CartesianCoordinates *dis=(*ib)->GetMobilityModel()->GetAbsolutePosition();
			tempbus=dis->GetDistance(ue->GetMobilityModel()->GetAbsolutePosition());
			if(mindisbus>tempbus){
				mindisbus=tempbus;
				targetbus=(*ib);
			}
		}

		if(tempbus<tempenb){//using bus
			if(targetbus->GetIDNetworkNode()==ue->GetTargetNode()->GetIDNetworkNode()){
				return false;
			}
			else{
				if(ue->GetTargetNode()->GetNodeType()==NetworkNode::TYPE_BUS){
					Bus *curbus=NetworkManager::Init()->GetBusByID(ue->GetTargetNode()->GetIDNetworkNode());
					curbus->DeleteUserEquipment(ue);
					targetbus->RegisterUserEquipment(ue);
				}
				else{
					ENodeB *curenb=NetworkManager::Init()->GetENodeBByID(ue->GetTargetNode()->GetIDNetworkNode());
					curenb->DeleteUserEquipment(ue);
					targetbus->RegisterUserEquipment(ue);
				}
				m_target=NetworkManager::Init()->GetNetworkNodeByID(targetbus->GetIDNetworkNode());
				return true;
			}
		}
		else{//using enb
			if(targetenb->GetIDNetworkNode()==ue->GetTargetNode()->GetIDNetworkNode()){
				return false;
			}
			else{
				if(ue->GetTargetNode()->GetNodeType()==NetworkNode::TYPE_BUS){
					Bus *curbus=NetworkManager::Init()->GetBusByID(ue->GetTargetNode()->GetIDNetworkNode());
					curbus->DeleteUserEquipment(ue);
					targetenb->RegisterUserEquipment(ue);
				}
				else{
					ENodeB *curenb=NetworkManager::Init()->GetENodeBByID(ue->GetTargetNode()->GetIDNetworkNode());
					curenb->DeleteUserEquipment(ue);
					targetenb->RegisterUserEquipment(ue);
				}
				m_target=NetworkManager::Init()->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
				return true;
			}
		}
	

  return false;//test temp
}*/