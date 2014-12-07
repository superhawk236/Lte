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

#include "../device/Bus.h"   //struct Bus error

#include "../channel/LteChannel.h"
#include "../phy/enb-lte-phy.h"
#include "../phy/bus-lte-phy.h"
#include "../phy/ue-lte-phy.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../protocolStack/packet/packet-burst.h"
#include "../protocolStack/packet/Packet.h"
#include "../core/eventScheduler/simulator.h"
#include "../flows/application/InfiniteBuffer.h"
#include "../flows/application/VoIP.h"
#include "../flows/application/CBR.h"
#include "../flows/application/TraceBased.h"
#include "../device/IPClassifier/ClassifierParameters.h"
#include "../flows/QoS/QoSParameters.h"
#include "../flows/QoS/QoSForEXP.h"
#include "../flows/QoS/QoSForFLS.h"
#include "../flows/QoS/QoSForM_LWDF.h"
#include "../componentManagers/FrameManager.h"
#include "../utility/seed.h"
#include "../utility/RandomVariable.h"
#include "../utility/UsersDistribution.h"
#include "../channel/propagation-model/macrocell-urban-area-channel-realization.h"
#include "../load-parameters.h"

#include <iostream>
#include <queue>
#include <fstream>
#include <stdlib.h>
#include <cstring>

#define UELIMIT 60
class Bus;
//NetworkManager* nm;
double uecheck[UELIMIT];
double mouanth=0;
double ueth[UELIMIT];
double uecqith[UELIMIT];
double totaldelay;
double totaldelaynum;
double uepower;
double buspower;
//double mouancqith=0;
double cqitable[16]={
	0,
0.1523,
0.2344,
0.3770,
0.6016,
0.8770,
1.1758,
1.4766,
1.9141,
2.4063,
2.7305,
3.3223,
3.9023,
4.5234,
5.1152,
5.5547
};
double pctable[16]={
	200,
	200,
	183,
	177,
	159,
	146,
	135,
	127,
	108,
	95,
	82,
	69,
	56,
	53,
	49,
	43
};
double bpctable[16]={
	1000,
	1000,
	868,
	802,
	736,
	670,
	604,
	538,
	472,
	406,
	340,
	274,
	245,
	201,
	192,
	185
};


static void MultiCell (int nbCell, double radius,
                       int nbUE,
                       int nbVoIP, int nbVideo, int nbBE, int nbCBR,
                       int sched_type,
                       int frame_struct,
                       int speed,
		               double simtime, int videoBitRate,
                       int seed)
{
	std::cout<<"mouan sen"<<std::endl;
double maxDelay=1.0;
	for(int mm=0;mm<UELIMIT;mm++){
		ueth[mm]=0;
		uecqith[mm]=0;
		uecheck[mm]=0;
	}
	uepower=0;
	buspower=0;
	totaldelay=0;
	totaldelaynum=0;
int handover;
if(nbCell>1) handover = 1; //if multicell scenario -> activate HO mechanisms

  // define simulation times
  double duration = simtime;
  double flow_duration = simtime;

  int cluster = 3;//cluster = 3
  double bandwidth = 10;

  // CREATE COMPONENT MANAGER
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  //NetworkManager* nm = NetworkManager::Init();
  NetworkManager* nm = NetworkManager::Init();
  // CONFIGURE SEED
  if (seed >= 0)
	{
	  int commonSeed = GetCommonSeed (seed);
	  srand (commonSeed);
	}
  else
	{
	  srand (time(NULL));
	}
  std::cout << "Simulation with SEED = " << seed << std::endl;

  // SET SCHEDULING ALLOCATION SCHEME
  ENodeB::DLSchedulerType downlink_scheduler_type;
  switch (1)
    {
      case 1:
  	    downlink_scheduler_type = ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR;
  	    std::cout << "Scheduler PF "<< std::endl;
	    break;
      case 2:
  	    downlink_scheduler_type = ENodeB::DLScheduler_TYPE_MLWDF;
  	    std::cout << "Scheduler MLWDF "<< std::endl;
	    break;
      case 3:
  	    downlink_scheduler_type = ENodeB::DLScheduler_TYPE_EXP;
  	    std::cout << "Scheduler EXP "<< std::endl;
	    break;
      case 4:
  	    downlink_scheduler_type = ENodeB::DLScheduler_TYPE_FLS;
  	    std::cout << "Scheduler FLS "<< std::endl;
	    break;
      case 5:
  	    downlink_scheduler_type = ENodeB::DLScheduler_EXP_RULE;
  	    std::cout << "Scheduler EXP_RULE "<< std::endl;
	    break;
      case 6:
  	    downlink_scheduler_type = ENodeB::DLScheduler_LOG_RULE;
  	    std::cout << "Scheduler LOG RULE "<< std::endl;
	    break;
      default:
	    downlink_scheduler_type = ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR;
	    break;
    }

  ENodeB::ULSchedulerType uplink_scheduler_type;
  uplink_scheduler_type = ENodeB::ULScheduler_TYPE_FME;

  //mouan Bus sch
  Bus::DLSchedulerType downlink_scheduler_type_bus;
  switch (1)
    {
      case 1:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_TYPE_PROPORTIONAL_FAIR;
  	    std::cout << "Bus Scheduler PF "<< std::endl;
	    break;
      case 2:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_TYPE_MLWDF;
  	    std::cout << "Bus Scheduler MLWDF "<< std::endl;
	    break;
      case 3:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_TYPE_EXP;
  	    std::cout << "Bus Scheduler EXP "<< std::endl;
	    break;
      case 4:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_TYPE_FLS;
  	    std::cout << "Bus Scheduler FLS "<< std::endl;
	    break;
      case 5:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_EXP_RULE;
  	    std::cout << "Bus Scheduler EXP_RULE "<< std::endl;
	    break;
      case 6:
  	    downlink_scheduler_type_bus = Bus::DLScheduler_LOG_RULE;
  	    std::cout << "Bus Scheduler LOG RULE "<< std::endl;
	    break;
      default:
	    downlink_scheduler_type_bus = Bus::DLScheduler_TYPE_PROPORTIONAL_FAIR;
	    break;
    }

  Bus::ULSchedulerType uplink_scheduler_type_bus;
  uplink_scheduler_type_bus = Bus::ULScheduler_TYPE_FME;
  //uplink_scheduler_type_bus = Bus::ULScheduler_TYPE_MAXIMUM_THROUGHPUT;
  /*switch (1)//(ulsched_type)
    {
      case 1:
  	    uplink_scheduler_type = ENodeB::ULScheduler_TYPE_FME;
  	    std::cout << "Scheduler FME "<< std::endl;
	    break;
      case 2:
  	    uplink_scheduler_type = ENodeB::ULScheduler_TYPE_MAXIMUM_THROUGHPUT;
  	    std::cout << "Scheduler MT "<< std::endl;
	    break;
      default:
	    uplink_scheduler_type = ENodeB::ULScheduler_TYPE_FME;
	    break;
    }*/

  // SET FRAME STRUCTURE
  FrameManager::FrameStructure frame_structure;
  switch (frame_struct)
    {
	  case 1:
	    frame_structure = FrameManager::FRAME_STRUCTURE_FDD;
	    break;
	  case 2:
	    frame_structure = FrameManager::FRAME_STRUCTURE_TDD;
	    break;
	  default:
	    frame_structure = FrameManager::FRAME_STRUCTURE_FDD;
	    break;
    }
  frameManager->SetFrameStructure(FrameManager::FRAME_STRUCTURE_FDD);




  //create cells
  std::vector <Cell*> *cells = new std::vector <Cell*>;
  for (int i = 0; i < nbCell; i++)
    {
	  CartesianCoordinates center =
			  GetCartesianCoordinatesForCell(i, radius * 1000.);

	  Cell *c = new Cell (i, radius, 0.035, center.GetCoordinateX (), center.GetCoordinateY ());
	  cells->push_back (c);
	  nm->GetCellContainer ()->push_back (c);

	  std::cout << "Created Cell, id " << c->GetIdCell ()
			  <<", position: " << c->GetCellCenterPosition ()->GetCoordinateX ()
			  << " " << c->GetCellCenterPosition ()->GetCoordinateY () << std::endl;
    }


  std::vector <BandwidthManager*> spectrums = RunFrequencyReuseTechniques (nbCell, cluster, bandwidth);

  //Create a set of a couple of channels
  std::vector <LteChannel*> *dlChannels = new std::vector <LteChannel*>;
  std::vector <LteChannel*> *ulChannels = new std::vector <LteChannel*>;
  for (int i= 0; i < nbCell; i++)
    {
	  LteChannel *dlCh = new LteChannel ();
	  dlCh->SetChannelId (i);
	  dlChannels->push_back (dlCh);

	  LteChannel *ulCh = new LteChannel ();
	  ulCh->SetChannelId (i);
	  ulChannels->push_back (ulCh);
    }


  //create eNBs
  std::vector <ENodeB*> *eNBs = new std::vector <ENodeB*>;
  for (int i = 0; i < nbCell; i++)
    {
	  ENodeB* enb = new ENodeB (i, cells->at (i));

	  enb->GetPhy ()->SetDlChannel (dlChannels->at (i));
	  enb->GetPhy ()->SetUlChannel (ulChannels->at (i));

      //enb->SetDLScheduler (downlink_scheduler_type);
	  enb->SetULScheduler (uplink_scheduler_type);

	  enb->GetPhy ()->SetBandwidthManager (spectrums.at (i));

	  std::cout << "Created enb, id " << enb->GetIDNetworkNode()
			  << ", cell id " << enb->GetCell ()->GetIdCell ()
			  <<", position: " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << enb->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << ", channels id " << enb->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << enb->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	  spectrums.at (i)->Print ();


	  ulChannels->at (i)->AddDevice((NetworkNode*) enb);


	  nm->GetENodeBContainer ()->push_back (enb);
	  eNBs->push_back (enb);
    }




  //Define Application Container
  VoIP VoIPApplication[nbVoIP*nbCell*nbUE];
  TraceBased VideoApplication[nbVideo*nbCell*nbUE];
  InfiniteBuffer BEApplication[nbBE*nbCell*nbUE];
  CBR CBRApplication[nbCBR*nbCell*nbUE];
  int voipApplication = 0;
  int videoApplication = 0;
  int cbrApplication = 0;
  int beApplication = 0;
  int destinationPort = 101;
  int applicationID = 0;



  //Create GW
  Gateway *gw = new Gateway ();
  nm->GetGatewayContainer ()->push_back (gw);


  //nbUE is the number of users that are into each cell at the beginning of the simulation
  int idUE = nbCell;
  for (int j = 0; j < nbCell; j++)
  {

	  //users are distributed uniformly intio a cell
	  vector<CartesianCoordinates*> *positions = GetUniformUsersDistribution (j, nbUE);

      //Create UEs
      for (int i = 0; i < nbUE; i++)
        {
	      //ue's random position
    	  double posX = positions->at (i)->GetCoordinateX ();
    	  double posY = positions->at (i)->GetCoordinateY ();
    	  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);

		  double spe=speed;

		  if(i==0){
			  posX=-500;
			  posY=80;
			  spe=0;
		  }
		  if(i==1){
			  posX=-400;
			  posY=50;spe=0;
		  }
		  if(i==2){
			  posX=-450;
			  posY=0;spe=0;
		  }
		  if(i==3){
			  posX=-500;
			  posY=0;spe=0;
		  }
		  if(i==4){
			  posX=-500;
			  posY=-30;spe=0;
		  }
		  if(i==5){
			  posX=-500;
			  posY=-80;spe=0;
		  }

    	  UserEquipment* ue = new UserEquipment (idUE,
												 posX, posY, spe, speedDirection,
												 cells->at (j),
												 eNBs->at (j),
												 1, //HO activated!
												 //Mobility::RANDOM_DIRECTION
												 Mobility::MANHATTAN
												 );

		  std::cout << "Created UE - id " << idUE << " position " << posX << " " << posY
				  << ", cell " <<  ue->GetCell ()->GetIdCell ()
				  << ", target enb " << ue->GetTargetNode ()->GetIDNetworkNode () << std::endl;

		  ue->GetPhy ()->SetDlChannel (eNBs->at (j)->GetPhy ()->GetDlChannel ());
		  ue->GetPhy ()->SetUlChannel (eNBs->at (j)->GetPhy ()->GetUlChannel ());

		  FullbandCqiManager *cqiManager = new FullbandCqiManager ();
		  cqiManager->SetCqiReportingMode (CqiManager::PERIODIC);
		  cqiManager->SetReportingInterval (1);
		  cqiManager->SetDevice (ue);
		  ue->SetCqiManager (cqiManager);

		  nm->GetUserEquipmentContainer ()->push_back (ue);

		  // register ue to the enb
		  eNBs->at (j)->RegisterUserEquipment (ue);
		  // define the channel realization
		  MacroCellUrbanAreaChannelRealization* c = new MacroCellUrbanAreaChannelRealization (eNBs->at (j), ue);
		  c->SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_A);
		  eNBs->at (j)->GetPhy ()->GetDlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c);


		  // CREATE UPLINK APPLICATION FOR THIS UE
		  double start_time = 0.1; // + GetRandomVariable (5.);
		  double duration_time = start_time + flow_duration;

		  // *** voip application
		  for (int j = 0; j < nbVoIP; j++)
			{
			  // create application
			  VoIPApplication[voipApplication].SetSource (ue);
			  VoIPApplication[voipApplication].SetDestination (ue->GetTargetNode());
			  VoIPApplication[voipApplication].SetApplicationID (applicationID);
			  VoIPApplication[voipApplication].SetStartTime(start_time);
			  VoIPApplication[voipApplication].SetStopTime(duration_time);

			  /*// create qos parameters
			  if (uplink_scheduler_type == ENodeB::ULScheduler_TYPE_FME)
				{
					QoSParameters *qos = new QoSParameters ();
				  qos->SetMaxDelay (maxDelay);
				  if (maxDelay == 0.1)
					{
					  std::cout << "Target Delay = 0.1 s, M = 9" << std::endl;
					  qos->SetNbOfCoefficients (9);
					}
				  else if (maxDelay == 0.08)
					{
					  std::cout << "Target Delay = 0.08 s, M = 7" << std::endl;
					  qos->SetNbOfCoefficients (7);
					}
				  else if (maxDelay == 0.06)
					{
					  std::cout << "Target Delay = 0.06 s, M = 5" << std::endl;
					  qos->SetNbOfCoefficients (5);
					}
				  else if (maxDelay == 0.04)
					{
					  std::cout << "Target Delay = 0.04 s, M = 3" << std::endl;
					  qos->SetNbOfCoefficients (3);
					}
				  else
					{
					  std::cout << "ERROR: target delay is not available"<< std::endl;
					  return;
					}

				  VoIPApplication[voipApplication].SetQoSParameters (qos);
				}
			  else if (downlink_scheduler_type == ENodeB::DLScheduler_TYPE_EXP)
				{
				  QoSForEXP *qos = new QoSForEXP ();
				  qos->SetMaxDelay (maxDelay);
				  VoIPApplication[voipApplication].SetQoSParameters (qos);
				}
			  else if (downlink_scheduler_type == ENodeB::DLScheduler_TYPE_MLWDF)
				{
				  QoSForM_LWDF *qos = new QoSForM_LWDF ();
				  qos->SetMaxDelay (maxDelay);
				  VoIPApplication[voipApplication].SetQoSParameters (qos);
				}
			  else
				{
				  QoSParameters *qos = new QoSParameters ();
				  qos->SetMaxDelay (maxDelay);
				  VoIPApplication[voipApplication].SetQoSParameters (qos);
				}*/


			  //create classifier parameters
			  ClassifierParameters *cp = new ClassifierParameters (ue->GetIDNetworkNode(),
																   ue->GetTargetNode()->GetIDNetworkNode(),
																   0,
																   destinationPort,
																   TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
			  VoIPApplication[voipApplication].SetClassifierParameters (cp);

			  std::cout << "CREATED VOIP APPLICATION, ID " << applicationID << std::endl;

			  //update counter
			  destinationPort++;
			  applicationID++;
			  voipApplication++;
			}


		  // *** video application
		  for (int j = 0; j < nbVideo; j++)
			{
			  // create application
			  VideoApplication[videoApplication].SetSource (ue);
			  VideoApplication[videoApplication].SetDestination (ue->GetTargetNode());
			  VideoApplication[videoApplication].SetApplicationID (applicationID);
			  VideoApplication[videoApplication].SetStartTime(start_time);
			  VideoApplication[videoApplication].SetStopTime(duration_time);

			  string video_trace ("foreman_H264_");
			  //string video_trace ("highway_H264_");
			  //string video_trace ("mobile_H264_");

			  switch (videoBitRate)
				  {
					case 128:
					  {
					    string _file (path + "src/flows/application/Trace/" + video_trace + "128k.dat");
					    VideoApplication[videoApplication].SetTraceFile(_file);
					    std::cout << "		selected video @ 128k"<< std::endl;
					    break;
					  }
					case 242:
					  {
  				        string _file (path + "src/flows/application/Trace/" + video_trace + "242k.dat");
					    VideoApplication[videoApplication].SetTraceFile(_file);
					    std::cout << "		selected video @ 242k"<< std::endl;
					    break;
					  }
					case 440:
					  {
					    string _file (path + "src/flows/application/Trace/" + video_trace + "440k.dat");
     			        VideoApplication[videoApplication].SetTraceFile(_file);
					    std::cout << "		selected video @ 440k"<< std::endl;
					    break;
					  }
					default:
					  {
					    string _file (path + "src/flows/application/Trace/" + video_trace + "128k.dat");
    				    VideoApplication[videoApplication].SetTraceFile(_file);
					    std::cout << "		selected video @ 128k as default"<< std::endl;
					    break;
					  }
				  }

			  // create qos parameters
			  if (downlink_scheduler_type == ENodeB::DLScheduler_TYPE_FLS)
				{
				  QoSForFLS *qos = new QoSForFLS ();
				  qos->SetMaxDelay (maxDelay);
				  if (maxDelay == 0.1)
					{
					  std::cout << "Target Delay = 0.1 s, M = 9" << std::endl;
					  qos->SetNbOfCoefficients (9);
					}
				  else if (maxDelay == 0.08)
					{
					  std::cout << "Target Delay = 0.08 s, M = 7" << std::endl;
					  qos->SetNbOfCoefficients (7);
					}
				  else if (maxDelay == 0.06)
					{
					  std::cout << "Target Delay = 0.06 s, M = 5" << std::endl;
					  qos->SetNbOfCoefficients (5);
					}
				  else if (maxDelay == 0.04)
					{
					  std::cout << "Target Delay = 0.04 s, M = 3" << std::endl;
					  qos->SetNbOfCoefficients (3);
					}
				  else
					{
					  std::cout << "ERROR: target delay is not available"<< std::endl;
					  return;
					}

				  VideoApplication[videoApplication].SetQoSParameters (qos);
				}
			  else if (downlink_scheduler_type == ENodeB::DLScheduler_TYPE_EXP)
				{
				  QoSForEXP *qos = new QoSForEXP ();
				  qos->SetMaxDelay (maxDelay);
				  VideoApplication[videoApplication].SetQoSParameters (qos);
				}
			  else if (downlink_scheduler_type == ENodeB::DLScheduler_TYPE_MLWDF)
				{
				  QoSForM_LWDF *qos = new QoSForM_LWDF ();
				  qos->SetMaxDelay (maxDelay);
				  VideoApplication[videoApplication].SetQoSParameters (qos);
				}
			  else
				{
				  QoSParameters *qos = new QoSParameters ();
				  qos->SetMaxDelay (maxDelay);
				  VideoApplication[videoApplication].SetQoSParameters (qos);
				}


			  //create classifier parameters
			  ClassifierParameters *cp = new ClassifierParameters (ue->GetIDNetworkNode(),
																   ue->GetTargetNode()->GetIDNetworkNode(),
																   0,
																   destinationPort,
																   TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
			  VideoApplication[videoApplication].SetClassifierParameters (cp);

			  std::cout << "CREATED VIDEO APPLICATION, ID " << applicationID << std::endl;

			  //update counter
			  destinationPort++;
			  applicationID++;
			  videoApplication++;
			}

		  // *** be application
		  for (int j = 0; j < nbBE; j++)
			{
			  // create application
			  BEApplication[beApplication].SetSource (ue);
			  BEApplication[beApplication].SetDestination (ue->GetTargetNode());
			  BEApplication[beApplication].SetApplicationID (applicationID);
			  BEApplication[beApplication].SetStartTime(start_time);
			  BEApplication[beApplication].SetStopTime(duration_time);


			  // create qos parameters
			  QoSParameters *qosParameters = new QoSParameters ();
			  BEApplication[beApplication].SetQoSParameters (qosParameters);


			  //create classifier parameters
			  ClassifierParameters *cp = new ClassifierParameters (ue->GetIDNetworkNode(),
																   ue->GetTargetNode()->GetIDNetworkNode(),
																   0,
																   destinationPort,
																   TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
			  BEApplication[beApplication].SetClassifierParameters (cp);

			  std::cout << "CREATED BE APPLICATION, ID " << applicationID << std::endl;

			  //update counter
			  destinationPort++;
			  applicationID++;
			  beApplication++;
			}

		  // *** cbr application
		  for (int j = 0; j < nbCBR; j++)
			{
			  // create application
			  CBRApplication[cbrApplication].SetSource (ue);
			  CBRApplication[cbrApplication].SetDestination (ue->GetTargetNode());
			  CBRApplication[cbrApplication].SetApplicationID (applicationID);
			  CBRApplication[cbrApplication].SetStartTime(start_time);
			  CBRApplication[cbrApplication].SetStopTime(duration_time);
			  CBRApplication[cbrApplication].SetInterval (0.04);
			  CBRApplication[cbrApplication].SetSize (5);

			  // create qos parameters
			  QoSParameters *qosParameters = new QoSParameters ();
			  qosParameters->SetMaxDelay (maxDelay);

			  CBRApplication[cbrApplication].SetQoSParameters (qosParameters);


			  //create classifier parameters
			  ClassifierParameters *cp = new ClassifierParameters (ue->GetIDNetworkNode(),
																   ue->GetTargetNode()->GetIDNetworkNode(),
																   0,
																   destinationPort,
																   TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP);
			  CBRApplication[cbrApplication].SetClassifierParameters (cp);

			  std::cout << "CREATED CBR APPLICATION, ID " << applicationID << std::endl;

			  //update counter
			  destinationPort++;
			  applicationID++;
			  cbrApplication++;
			}

		  idUE++;

		}
    }

	//mouan
  //Create Bus
	int num_bus=sched_type,busnumstart=(nbCell*nbUE)+nbCell;
  double xx=0.0,yy=0.0;
  int bb=0;
  std::vector <Bus*> *buses = new std::vector <Bus*>;
  for (int i = busnumstart; i < (num_bus+busnumstart) ; i++,bb++)
    {
		if(bb==0){ xx=0.0; yy=700.0; }
		if(bb==1){ xx=700.0; yy=0.0; }
		if(bb==2){ xx=0.0; yy=-700.0; }

		Bus* bus = new Bus (i, cells->at (0), eNBs->at(0), xx, yy);

	  bus->GetPhy ()->SetDlChannel (dlChannels->at (0));
	  bus->GetPhy ()->SetUlChannel (ulChannels->at (0));

      //bus->SetDLScheduler (downlink_scheduler_type_bus);
	  bus->SetULScheduler (uplink_scheduler_type_bus);

	  bus->GetPhy ()->SetBandwidthManager (spectrums.at (0));

	  std::cout << "Created bus, id " << bus->GetIDNetworkNode()
			  //<< ", cell id " << bus->GetCell ()->GetIdCell ()
			  <<", position: " << bus->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateX ()
			  << " " << bus->GetMobilityModel ()->GetAbsolutePosition ()->GetCoordinateY ()
			  << ", channels id " << bus->GetPhy ()->GetDlChannel ()->GetChannelId ()
			  << bus->GetPhy ()->GetUlChannel ()->GetChannelId ()  << std::endl;

	  spectrums.at (0)->Print ();


	  ulChannels->at (0)->AddDevice((NetworkNode*) bus);


	  nm->GetBusContainer ()->push_back (bus);
	  buses->push_back (bus);

	  // register ue to the enb
	  eNBs->at (0)->RegisterBus (bus);//j=0;
	  // define the channel realization
	  MacroCellUrbanAreaChannelRealization* c = new MacroCellUrbanAreaChannelRealization (eNBs->at (0), bus);
	  c->SetChannelType (ChannelRealization::CHANNEL_TYPE_PED_A);
	  eNBs->at (0)->GetPhy ()->GetUlChannel ()->GetPropagationLossModel ()->AddChannelRealization (c);
    }

  simulator->SetStop(duration);
  simulator->Schedule(duration-10, &Simulator::PrintMemoryUsage, simulator);
  simulator->Run ();


  /*
  delete cell;
  delete dlCh;
  delete ulCh;
  delete spectrum;
  delete model;
  delete frameManager;
  delete networkManager;
  delete simulator;
  delete enb;
  */

  std::cout<<"MOUAN Th "<<mouanth<<std::endl;
  /*for(int pm=0;pm<UELIMIT;pm++){
	  std::cout<<"ID "<<pm<<" VALUE "<<ueth[pm]<<std::endl;
  }*/
  double tempforprint=0;
  for(int pm=0;pm<UELIMIT;pm++){
	  std::cout<<"ID "<<pm<<" CQIVALUE "<<uecqith[pm]<<std::endl;
	  if(uecqith[pm]>0){
		  tempforprint+=uecqith[pm];
	  }
  }
  std::cout<<"totalth "<<tempforprint<<std::endl;
  std::cout<<"avg totalth "<<tempforprint/nbUE<<std::endl;
  //std::cout<<"MOUAN CQITh "<<mouancqith<<std::endl;
  std::cout<<"avg delay "<<totaldelay/totaldelaynum<<std::endl;

  std::cout<<"total ue power "<<uepower<<std::endl;
  std::cout<<"total bus power "<<buspower<<std::endl;
}
