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
/*
#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../core/eventScheduler/simulator.h"
#include "../flows/application/InfiniteBuffer.h"
#include "../flows/QoS/QoSParameters.h"
#include "../componentManagers/FrameManager.h"
#include "../componentManagers/FlowsManager.h"
#include "../device/ENodeB.h"


static void Simple ()
{

  // CREATE COMPONENT MANAGERS
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* networkManager = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();


  //Create CHANNELS
  LteChannel *dlCh = new LteChannel ();
  LteChannel *ulCh = new LteChannel ();


  // CREATE SPECTRUM
  BandwidthManager* spectrum = new BandwidthManager (5, 5, 0, 0);


  // CREATE CELL
  int idCell = 0;
  int radius = 1; //km
  int minDistance = 0.0035; //km
  int posX = 0;
  int posY = 0;
  Cell* cell = networkManager->CreateCell (idCell, radius, minDistance, posX, posY);


  //Create ENodeB
  int idEnb = 1;
  ENodeB* enb = networkManager->CreateEnodeb (idEnb, cell, posX, posY, dlCh, ulCh, spectrum);
  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);


  //Create GW
  Gateway *gw = networkManager->CreateGateway ();


  //Create UE
  int idUe = 2;
  int posX_ue = 40; //m
  int posY_ue = 0;  //m
  int speed = 3;    //km/h
  double speeDirection = 0;
  UserEquipment* ue = networkManager->CreateUserEquipment (idUe, posX_ue, posY_ue, speed, speeDirection, cell, enb);

  //Create an Application
  QoSParameters *qos = new QoSParameters ();
  int applicationID = 0;
  int srcPort = 0;
  int dstPort = 100;
  double startTime = 0.1; //s
  double stopTime = 0.12;  //s
  Application* be = flowsManager->CreateApplication (applicationID,
						  gw, ue,
						  srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
						  Application::APPLICATION_TYPE_INFINITE_BUFFER,
						  qos,
						  startTime, stopTime);


  simulator->SetStop(0.13);
  simulator->Run ();

}*/

#include "../channel/LteChannel.h"
#include "../core/spectrum/bandwidth-manager.h"
#include "../networkTopology/Cell.h"
#include "../core/eventScheduler/simulator.h"
#include "../flows/application/InfiniteBuffer.h"
#include "../flows/QoS/QoSParameters.h"
#include "../componentManagers/FrameManager.h"
#include "../componentManagers/FlowsManager.h"
#include "../device/ENodeB.h"


static void Simple ()
{

  // CREATE COMPONENT MANAGERS
  Simulator *simulator = Simulator::Init();
  FrameManager *frameManager = FrameManager::Init();
  NetworkManager* networkManager = NetworkManager::Init();
  FlowsManager* flowsManager = FlowsManager::Init ();


  //Create CHANNELS
  LteChannel *dlCh = new LteChannel ();
  LteChannel *ulCh = new LteChannel ();


  // CREATE SPECTRUM
  BandwidthManager* spectrum = new BandwidthManager (5, 5, 0, 0);


  // CREATE CELL
  int idCell = 0;
  int radius = 1; //km
  int minDistance = 0.0035; //km
  int posX = 0;
  int posY = 0;
  Cell* cell = networkManager->CreateCell (idCell, radius, minDistance, posX, posY);


  //Create ENodeB
  int idEnb = 1;
  ENodeB* enb = networkManager->CreateEnodeb (idEnb, cell, posX, posY, dlCh, ulCh, spectrum);
  enb->SetDLScheduler (ENodeB::DLScheduler_TYPE_PROPORTIONAL_FAIR);
  enb->SetULScheduler (ENodeB::ULScheduler_TYPE_FME);


  //Create GW
  Gateway *gw = networkManager->CreateGateway ();


  //Create UE
  int idUe = 2;
  int posX_ue = 60; //m
  int posY_ue = 0;  //m
  int speed = 3;    //km/h
  double speeDirection = 0;
  UserEquipment* ue = networkManager->CreateUserEquipment (idUe, posX_ue, posY_ue, speed, speeDirection, cell, enb);

  //Create UE
  int idUe3 = 3;
  int posX_ue3 = 10; //m
  int posY_ue3 = 0;  //m
  int speed3 = 3;    //km/h
  double speeDirection3 = 0;
  UserEquipment* ue3 = networkManager->CreateUserEquipment (idUe3, posX_ue3, posY_ue3, speed3, speeDirection3, cell, enb);

  //Create an Application
  QoSParameters *qos = new QoSParameters ();
  int applicationID = 1;
  int srcPort = 0;
  int dstPort = 100;
  double startTime = 0.1; //s
  double stopTime = 1;  //s
  Application* be = flowsManager->CreateApplication (applicationID,
						  ue, enb,
						  srcPort, dstPort, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
						  Application::APPLICATION_TYPE_INFINITE_BUFFER,
						  qos,
						  startTime, 0.5);

QoSParameters *qos3 = new QoSParameters ();
  Application* be3 = flowsManager->CreateApplication (2,
						  ue3, enb,
						  srcPort, dstPort+1, TransportProtocol::TRANSPORT_PROTOCOL_TYPE_UDP ,
						  Application::APPLICATION_TYPE_INFINITE_BUFFER,
						  qos3,
						  0.6, stopTime);


  simulator->SetStop(1.1);
  simulator->Run ();

}
