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
 * Author: Francesco Capozzi <f.capozzi@poliba.it>
 */


#include "power-based-ho-manager.h"
#include "../../../device/NetworkNode.h"
#include "../../../device/UserEquipment.h"
#include "../../../device/ENodeB.h"
#include "../../../device/Bus.h"
#include "../../../device/HeNodeB.h"
#include "../../../componentManagers/NetworkManager.h"
#include "../../../core/spectrum/bandwidth-manager.h"
#include "../../../phy/lte-phy.h"
#include "../../../utility/ComputePathLoss.h"

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
//#include "../../../scenarios/multi-cell.h"

extern NetworkManager* nm;

PowerBasedHoManager::PowerBasedHoManager()
{
  m_target = NULL;
}

PowerBasedHoManager::~PowerBasedHoManager()
{
  m_target = NULL;
}

bool
PowerBasedHoManager::CheckBusHandoverNeed (Bus* bus)
{
	std::cout<<"ERROR: not using this"<<std::endl;
}

bool
PowerBasedHoManager::CheckHandoverNeed (UserEquipment* ue)
{
	/*//test RX cqi success?
	  std::cout<<"PBHM: UE "<<ue->GetIDNetworkNode();
		ENodeB *testenb=nm->GetENodeBByID(ue->GetTargetNode()->GetIDNetworkNode());
		std::vector<int> testtemp=testenb->GetUserEquipmentRecord(ue->GetIDNetworkNode())->GetCQI();
		int testj=0;
		for(std::vector<int>::iterator testit=testtemp.begin();testit!=testtemp.end();testit++,testj++){
			std::cout<<"cqi="<<testtemp.at(testj)<<std::endl;
		}*/
	return false;
	//std::cout<<"power based ho"<<std::endl;//test
	/*
  NetworkNode *targetNode = ue->GetTargetNode ();

  double TXpower = 10 * log10 (
		  pow (10., (targetNode->GetPhy()->GetTxPower() - 30)/10)
		  /
		  targetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );
  double pathLoss = ComputePathLossForInterference(targetNode, ue);

  double targetRXpower = TXpower - pathLoss;
  double RXpower;


  std::vector<ENodeB*> *listOfNodes = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;
  for (it = listOfNodes->begin (); it != listOfNodes->end (); it++)
    {
	  if ((*it)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it);

	      TXpower = 10 * log10 (
				  pow (10., (probableNewTargetNode->GetPhy()->GetTxPower() - 30)/10)
				  /
				  probableNewTargetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );
	      pathLoss = ComputePathLossForInterference(probableNewTargetNode, ue);

	      RXpower = TXpower - pathLoss;

	      if (RXpower > targetRXpower)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
		    	  targetRXpower = RXpower;
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

	      TXpower =   10 * log10 (
				  pow (10., (probableNewTargetNode->GetPhy()->GetTxPower() - 30)/10)
				  /
				  probableNewTargetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );

	      pathLoss = ComputePathLossForInterference(probableNewTargetNode, ue);

	      RXpower = TXpower - pathLoss;

	      if (RXpower > targetRXpower)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
		    	  targetRXpower = RXpower;
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
    }*/
	//mouan
	//NetworkNode *targetNode = ue->GetTargetNode ();
	NetworkNode *targetNode;

	//if(targetNode->GetNodeType()==NetworkNode::TYPE_ENODEB){
	std::cout<<"enb vs ue"<<std::endl;//test
	ENodeB *targetenb;
		int compare[2];
		int selected_bus_id=0;
		{
			std::vector<ENodeB *> *enbs=nm->GetENodeBContainer();
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
				targetNode=nm->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
			}
		//ENodeB *targetenb=nm->GetENodeBByID(targetNode->GetIDNetworkNode());
		//std::vector<int> uecqi=targetenb->GetUserEquipmentRecord(ue->GetIDNetworkNode())->GetCQI();
			std::vector<double> mouan_bstoueSinr;
			  std::vector<double> rxSignalValues;
			  std::vector<double>::iterator it;

			  rxSignalValues = ue->GetPhy()->GetTxSignal()->Getvalues();

			  //compute noise + interference
			  double interference;
			  if (ue->GetPhy()->GetInterference () != NULL)
				{
					ENodeB *node;
					interference = 0;

					std::vector<ENodeB*> *eNBs = NetworkManager::Init ()->GetENodeBContainer ();
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

						  /*
						  std::cout << "\t add interference from eNB " << node->GetIDNetworkNode ()
								  << " " << powerTXForSubBandwidth << " "  << ComputePathLossForInterference (node, ue)
								  << " " << nodeInterference_db << " " << nodeInterference
								  << " --> tot: " << interference
								  << std::endl;
						  */
						}
					}
				}
			  else
				{
				  interference = 0;
				}

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
			  
		/*std::vector<double> spectralEfficiency;
		//std::vector<int> cqiFeedbacks = ueRecord->GetCQI ();
		int numberOfCqi = uecqi.size ();
		for (int i = 0; i < numberOfCqi; i++)
		{
		  double sEff = GetMacEntity ()->GetAmcModule ()->GetEfficiencyFromCQI (uecqi.at (i));
		  spectralEfficiency.push_back (sEff);//i didn't do * 180000.0,because just for compare
		}*/

		//copy start (from enhance-uplink-scheduler.cpp)

		UplinkPacketScheduler *ulps=targetenb->GetULScheduler();

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
			return false;
		}
		bool Allocated[nbOfRBs];
		bool allocationMade;
		double metrics[nbOfRBs][users->size ()];
		int requiredPRBs[users->size ()];
		//std::vector<int> m_mouan;
		int m_mouan[users->size()];
		int ueat=-1;

		//test
		int tt=0;
		for(UplinkPacketScheduler::UsersToSchedule::iterator test=users->begin();test!=users->end();test++){
			tt++;
		}
		std::cout<<"3:"<<users->size()<<"="<<tt<<std::endl;//test

		for(int i=0 ; i < users->size() ; i++){
			std::cout<<"A ";//test
			m_mouan[i]=0;
		}
std::cout<<"3end"<<std::endl;//test

std::cout<<"4"<<std::endl;//test

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
std::cout<<"5"<<std::endl;//test
		//create number of required PRB's per scheduled users
		for(int j=0; j < users->size(); j++)
		{
			std::cout<<"1 "<<std::endl;//test
			scheduledUser = users->at(j);
std::cout<<"2 "<<std::endl;//test
			std::vector<double> sinrs;
			for(int c=0;c<scheduledUser->m_channelContition.size();c++)
			//for (std::vector<int>::iterator c = scheduledUser->m_channelContition.begin ();
			//		c != scheduledUser->m_channelContition.end (); c++)
			{
				//cout << *c <<" ";
				std::cout<<"a="<<c<<"="<<scheduledUser->m_channelContition.at(c)<<std::endl;//test
				std::cout<<"size="<<scheduledUser->m_channelContition.size()<<std::endl;//test
				std::cout<<"id="<<scheduledUser->m_userToSchedule->GetIDNetworkNode()<<std::endl;//test

				//if(scheduledUser->m_channelContition.at(c)>14)scheduledUser->m_channelContition.at(c)=14;
				//else if(scheduledUser->m_channelContition.at(c)<0)scheduledUser->m_channelContition.at(c)=0;
				if(scheduledUser->m_channelContition.at(c)>14||scheduledUser->m_channelContition.at(c)<0){
					return false;
				}

				sinrs.push_back (ulps->GetMacEntity ()->GetAmcModule ()->GetSinrFromCQI (scheduledUser->m_channelContition.at(c)));
				
				std::cout<<"b "<<std::endl;//test
			}
std::cout<<"3 "<<std::endl;//test

			double effectiveSinr =  GetEesmEffectiveSinr (sinrs);
std::cout<<"4 "<<std::endl;//test
			int mcs = ulps->GetMacEntity ()->GetAmcModule ()->GetMCSFromCQI (
				  ulps->GetMacEntity ()->GetAmcModule ()->GetCQIFromSinr (effectiveSinr));
			//scheduledUser->m_selectedMCS = mcs;
std::cout<<"5 "<<std::endl;//test
			requiredPRBs[j] = (floor) (scheduledUser->m_dataToTransmit /
					  (ulps->GetMacEntity ()->GetAmcModule ()->GetTBSizeFromMCS (mcs, 1) / 8));
			std::cout<<"6end"<<std::endl;//test
		}
std::cout<<"6"<<std::endl;//test
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
std::cout<<"7"<<std::endl;//test
			//copy end
if(ueat==-1)return false;//temp
			compare[1]=m_mouan[ueat];//m_mouan[ueat];//bs vs ue result
		  }
			std::cout<<"enb vs ue end"<<std::endl;//test

			//bus route
		  
		  std::vector<Bus *> *buses=nm->GetBusContainer();
		  std::vector<Bus *>::iterator select_bus;

		  for(select_bus=buses->begin();select_bus != buses->end();select_bus++)
		  {

			  Bus *selectedbus=(*select_bus);

			  std::vector<double> mouan_bustoueSinr;
			  std::vector<double> rxSignalValues;
			  std::vector<double>::iterator it;

			  rxSignalValues = ue->GetPhy()->GetTxSignal()->Getvalues();

			  //compute noise + interference
			  double interference;
			  if (ue->GetPhy()->GetInterference () != NULL)
				{
					ENodeB *node;
					interference = 0;

					std::vector<ENodeB*> *eNBs = NetworkManager::Init ()->GetENodeBContainer ();
					std::vector<ENodeB*>::iterator it;

					//std::cout << "Compute interference for UE " << ue->GetIDNetworkNode () << " ,target node " << ue->GetTargetNode ()->GetIDNetworkNode ()<< std::endl;

					for (it = eNBs->begin (); it != eNBs->end (); it++)
					{
					  node = (*it);
					  if (//node->GetIDNetworkNode () != ue->GetTargetNode ()->GetIDNetworkNode () &&
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

						  /*
						  std::cout << "\t add interference from eNB " << node->GetIDNetworkNode ()
								  << " " << powerTXForSubBandwidth << " "  << ComputePathLossForInterference (node, ue)
								  << " " << nodeInterference_db << " " << nodeInterference
								  << " --> tot: " << interference
								  << std::endl;
						  */
						}
					}
				}
			  else
				{
				  interference = 0;
				}

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

			    UplinkPacketScheduler *ulps=selectedbus->GetULScheduler();

				UplinkPacketScheduler::UsersToSchedule *users = ulps->GetUsersToSchedule();
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
				int ueat;
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
						  
						  if(scheduledUser->m_userToSchedule->GetIDNetworkNode()==ue->GetIDNetworkNode()){
							ueat=selectedUser;
							printf("selected user %d is ueat\n",ueat);//test
						  }
					  } else { // nothing to do exit the allocation loop
						  break;
					  }
				  }//end while
				  if(m_mouan[ueat]>compare[0]){
					compare[0]=m_mouan[ueat];//bus vs ue
					selected_bus_id=selectedbus->GetIDNetworkNode();
				  }
		  }//end for
std::cout<<"bus vs ue end"<<std::endl;//test
		  //bus vs bs

		  {
			//ENodeB *targetenb=nm->GetENodeBByID(targetNode->GetIDNetworkNode());
			std::vector<int> buscqi=targetenb->GetUserEquipmentRecord(selected_bus_id)->GetCQI();
			/*std::vector<double> spectralEfficiency;
			//std::vector<int> cqiFeedbacks = ueRecord->GetCQI ();
			int numberOfCqi = uecqi.size ();
			for (int i = 0; i < numberOfCqi; i++)
			{
			  double sEff = GetMacEntity ()->GetAmcModule ()->GetEfficiencyFromCQI (uecqi.at (i));
			  spectralEfficiency.push_back (sEff);//i didn't do * 180000.0,because just for compare
			}*/

			//copy start (from enhance-uplink-scheduler.cpp)
			UplinkPacketScheduler *ulps=targetenb->GetULScheduler();

			UplinkPacketScheduler::UsersToSchedule *users = ulps->GetUsersToSchedule();
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
			  }
				//compare[1]=m_mouan[ueat];//m_mouan[ueat];//bs vs ue result
		  }
		  if(compare[0]>compare[1]){//using bs
			  if(ue->GetTargetNode()->GetIDNetworkNode()==targetenb->GetIDNetworkNode()){
				  return false;
			  }
			  else{
				  m_target=nm->GetNetworkNodeByID(targetenb->GetIDNetworkNode());
				  //std::cout<<"HO TRUE"<<std::endl;//test

				  NetworkNode* newTagertNode = m_target;

		#ifdef HANDOVER_DEBUG
				  std::cout << "** HO ** \t time: " << time << " user " <<  ue->GetIDNetworkNode () <<
        				  " old eNB " << targetNode->GetIDNetworkNode () <<
        				  " new eNB " << newTagertNode->GetIDNetworkNode () << std::endl;
		#endif
				  double time=0;
				  nm->HandoverProcedure(time, ue, targetNode, newTagertNode);

				  return true;
			  }
		  }
		  else{//using bus
			  if(ue->GetTargetNode()->GetIDNetworkNode()==selected_bus_id){
				  return false;
			  }
			  else{
				  m_target=nm->GetNetworkNodeByID(selected_bus_id);
				  //std::cout<<"HO TRUE"<<std::endl;//test
				  
				  NetworkNode* newTagertNode = m_target;

		#ifdef HANDOVER_DEBUG
				  std::cout << "** HO ** \t time: " << time << " user " <<  ue->GetIDNetworkNode () <<
        				  " old eNB " << targetNode->GetIDNetworkNode () <<
        				  " new eNB " << newTagertNode->GetIDNetworkNode () << std::endl;
		#endif
				  double time=0;
				  nm->HandoverProcedure(time, ue, targetNode, newTagertNode);

				  return true;
			  }
		  }
		  //std::cout<<"enb vs bus end"<<std::endl;//test
	/*}
	else{//default bus route
		;
	}

  double TXpower = 10 * log10 (
		  pow (10., (targetNode->GetPhy()->GetTxPower() - 30)/10)
		  /
		  targetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );
  double pathLoss = ComputePathLossForInterference(targetNode, ue);

  double targetRXpower = TXpower - pathLoss;
  double RXpower;


  std::vector<ENodeB*> *listOfNodes = NetworkManager::Init ()->GetENodeBContainer ();
  std::vector<ENodeB*>::iterator it;
  for (it = listOfNodes->begin (); it != listOfNodes->end (); it++)
    {
	  if ((*it)->GetIDNetworkNode () != targetNode->GetIDNetworkNode () )
	    {

	      NetworkNode *probableNewTargetNode = (*it);

	      TXpower = 10 * log10 (
				  pow (10., (probableNewTargetNode->GetPhy()->GetTxPower() - 30)/10)
				  /
				  probableNewTargetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );
	      pathLoss = ComputePathLossForInterference(probableNewTargetNode, ue);

	      RXpower = TXpower - pathLoss;

	      if (RXpower > targetRXpower)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
		    	  targetRXpower = RXpower;
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

	      TXpower =   10 * log10 (
				  pow (10., (probableNewTargetNode->GetPhy()->GetTxPower() - 30)/10)
				  /
				  probableNewTargetNode->GetPhy()->GetBandwidthManager()->GetDlSubChannels().size () );

	      pathLoss = ComputePathLossForInterference(probableNewTargetNode, ue);

	      RXpower = TXpower - pathLoss;

	      if (RXpower > targetRXpower)
	        {
	    	  if (NetworkManager::Init()->CheckHandoverPermissions(probableNewTargetNode,ue))
	    	  {
		    	  targetRXpower = RXpower;
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
    }*/
}
