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



#include "BusDirection.h"
#include "../componentManagers/NetworkManager.h"
#include "../device/Gateway.h"
//#include "../device/ENodeB.h"
#include "../device/Bus.h"
//#include "../device/UserEquipment.h"
#include "../load-parameters.h"

BusDirection::BusDirection()
{
  SetMobilityModel(Mobility::BUS_DIRECTION);
  SetSpeed (40);
  SetSpeedDirection ((0.1*2*3.1415926)/360);
  SetPositionLastUpdate (0.0);
  SetHandover (true);
  SetLastHandoverTime (0.0);
  SetAbsolutePosition (NULL);
}

BusDirection::~BusDirection()
{
  DeleteAbsolutePosition ();
}

void
BusDirection::UpdatePosition (double time)
{
#ifdef MOBILITY_DEBUG
	std::cout << "\t START Bus MOBILITY MODEL for "<< GetNodeID () << std::endl;
#endif

  if (GetSpeed () == 0)
    {
#ifdef MOBILITY_DEBUG
	std::cout << "\t\t speed = 0 --> position has not been updated!"<< std::endl;
#endif
      return;
    }

  double timeInterval = time - GetPositionLastUpdate ();

  Bus *thisNode = NetworkManager::Init ()->GetBusByID (GetNodeID ());
  Cell *thisCell = thisNode->GetCell ();


  NetworkNode *targetNode = thisNode->GetTargetNode ();

#ifdef MOBILITY_DEBUG
	std::cout << "MOBILITY_DEBUG: User ID: " << GetNodeID ()
	    << "\n\t Cell ID " <<
					NetworkManager::Init()->GetCellIDFromPosition (GetAbsolutePosition()->GetCoordinateX(),
																   GetAbsolutePosition()->GetCoordinateY())
		<< "\n\t Initial Position (X): " << GetAbsolutePosition()->GetCoordinateX()
		<< "\n\t Initial Position (Y): " << GetAbsolutePosition()->GetCoordinateY()
		<< "\n\t Speed: " << GetSpeed()
		<< "\n\t Speed Direction: " << GetSpeedDirection()
		<< "\n\t Time Last Update: " << GetPositionLastUpdate()
		<< "\n\t Time Interval: " << timeInterval
		<< std::endl;
#endif
#ifdef MOBILITY_DEBUG_TAB
	std::cout <<  GetNodeID () << ""
		<< GetAbsolutePosition()->GetCoordinateX() << " "
		<< GetAbsolutePosition()->GetCoordinateY() << " "
		<< GetSpeed() << " "
		<< GetSpeedDirection() << " "
		<< GetPositionLastUpdate() << " "
		<< timeInterval << " "
		<< std::endl;
#endif

  double shift = timeInterval * (GetSpeed()*(1000.0/3600.0));

  double shift_y =
    shift * sin (GetSpeedDirection());
  double shift_x =
      shift * cos (GetSpeedDirection());

  CartesianCoordinates *newPosition =
      new CartesianCoordinates(GetAbsolutePosition()->GetCoordinateX()+shift_x,
		  					   GetAbsolutePosition()->GetCoordinateY()+shift_y);


  CartesianCoordinates *ENodeBPosition = targetNode->GetMobilityModel ()->GetAbsolutePosition ();

  const double pi = 3.1415926;
  double azimut = newPosition->GetPolarAzimut (ENodeBPosition);
  double newDistanceFromTheENodeB = newPosition->GetDistance (ENodeBPosition);

  //std::cout<<"mouan dec dir"<<std::endl;
  //static int dirind=0;

  if(dirind==0){
	  if(GetAbsolutePosition()->GetCoordinateX()==700.0){
			double speedDirection;
			speedDirection = (90.0*2*pi)/360;
			SetSpeedDirection(speedDirection);
			dirind=1;
	  }
	  else if(GetAbsolutePosition()->GetCoordinateX()==-700.0){
			double speedDirection;
			speedDirection = (270.0*2*pi)/360;
			SetSpeedDirection(speedDirection);
			dirind=3;
	  }
	  else if(GetAbsolutePosition()->GetCoordinateY()==-700.0){
			double speedDirection;
			speedDirection = (0.0*2*pi)/360;
			SetSpeedDirection(speedDirection);
			dirind=4;
	  }
	  else if(GetAbsolutePosition()->GetCoordinateY()==700.0){
			double speedDirection;
			speedDirection = (180.0*2*pi)/360;
			SetSpeedDirection(speedDirection);
			dirind=2;
	  }
	  //std::cout<<"Init Bus Direction..."<<std::endl;
  }
  else{
	  if(dirind==1){
		  if(GetAbsolutePosition()->GetCoordinateY()>=700.0){
			  double speedDirection;
			  speedDirection = (180.0*2*pi)/360;
			  SetSpeedDirection(speedDirection);
			  dirind=2;
		  }
	  }
	  else if(dirind==2){
		  if(GetAbsolutePosition()->GetCoordinateX()<=-700.0){
			  double speedDirection;
			  speedDirection = (270.0*2*pi)/360;
			  SetSpeedDirection(speedDirection);
			  dirind=3;
		  }
	  }
	  else if(dirind==3){
		  if(GetAbsolutePosition()->GetCoordinateY()<=-700.0){
			  double speedDirection;
			  speedDirection = (0.0*2*pi)/360;
			  SetSpeedDirection(speedDirection);
			  dirind=4;
		  }
	  }
	  else if(dirind==4){
		  if(GetAbsolutePosition()->GetCoordinateX()>=700.0){
			  double speedDirection;
			  speedDirection = (90.0*2*pi)/360;
			  SetSpeedDirection(speedDirection);
			  dirind=1;
		  }
	  }
  }
	/*if (GetAbsolutePosition()->GetCoordinateX() >= 500.0 || GetAbsolutePosition()->GetCoordinateX() <= -1250.0)// old
	      {
			  std::cout<<"mouan change dir"<<std::endl;
	  	    CartesianCoordinates *Correction =
	  	        new CartesianCoordinates ((newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * cos(azimut),
	   		 			                  (newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * sin(azimut));

	  	    newPosition->SetCoordinateX(newPosition->GetCoordinateX() - Correction->GetCoordinateX());
	  	    newPosition->SetCoordinateY(newPosition->GetCoordinateY() - Correction->GetCoordinateY());

	  	  	//double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
			double speedDirection;
			if (GetAbsolutePosition()->GetCoordinateX() >= 500.0)
				speedDirection = (180.1*2*pi)/360;
			else
				speedDirection = (0.1*2*pi)/360;
	  	  	SetSpeedDirection(speedDirection);

	  	    delete Correction;
	      }
  else if (newDistanceFromTheENodeB <= (thisCell->GetMinDistance() * 1000))
	{
          /*
	  CartesianCoordinates *Correction =
	  new CartesianCoordinates (((thisCell->GetMinDistance()*1000) - newDistanceFromTheENodeB) * cos(azimut),
		  					    ((thisCell->GetMinDistance()*1000) - newDistanceFromTheENodeB) * sin(azimut));

	  newPosition->SetCoordinateX(newPosition->GetCoordinateX() + Correction->GetCoordinateX());
	  newPosition->SetCoordinateY(newPosition->GetCoordinateY() + Correction->GetCoordinateY());

	  if ((azimut > GetSpeedDirection ()-pi/2) && (azimut < GetSpeedDirection ()+pi/2))
		{
		  double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
		  SetSpeedDirection(speedDirection);
		}

	  delete Correction;
	  */
		/*if (GetAbsolutePosition()->GetCoordinateX() >= 500.0 || GetAbsolutePosition()->GetCoordinateX() <= -1250.0)
	      {
			  std::cout<<"mouan change dir"<<std::endl;
	  	    CartesianCoordinates *Correction =
	  	        new CartesianCoordinates ((newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * cos(azimut),
	   		 			                  (newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * sin(azimut));

	  	    newPosition->SetCoordinateX(newPosition->GetCoordinateX() - Correction->GetCoordinateX());
	  	    newPosition->SetCoordinateY(newPosition->GetCoordinateY() - Correction->GetCoordinateY());

	  	  	//double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
			double speedDirection;
			if(GetSpeedDirection() < 1.57)
				speedDirection = (180.1*2*pi)/360;
			else
				speedDirection = (0.1*2*pi)/360;
	  	  	SetSpeedDirection(speedDirection);

	  	    delete Correction;
	      }*//*
	}
  else if (newDistanceFromTheENodeB > (thisCell->GetRadius()*1000))
    {
	  if (GetHandover()== false)
		{
		  //the UE must remain into the same cell
		  CartesianCoordinates *Correction =
		      new CartesianCoordinates ((newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * cos(azimut),
		    		                    (newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * sin(azimut));

		  newPosition->SetCoordinateX(newPosition->GetCoordinateX() - Correction->GetCoordinateX());
		  newPosition->SetCoordinateY(newPosition->GetCoordinateY() - Correction->GetCoordinateY());

		  //double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
		  double speedDirection;
		  /*if(GetSpeedDirection() < 1.57)
				speedDirection = (180.1*2*pi)/360;
			else*//*
				speedDirection = (0.1*2*pi)/360;
		  SetSpeedDirection(speedDirection);

		  delete Correction;
		}
	  else if (newPosition->GetDistance(0.0, 0.0) >= (GetTopologyBorder ()))
	  //else if (GetAbsolutePosition()->GetCoordinateX() >= 500.0 || GetAbsolutePosition()->GetCoordinateX() <= -1250.0)
	      {
			  std::cout<<"mouan change dir"<<std::endl;
	  	    CartesianCoordinates *Correction =
	  	        new CartesianCoordinates ((newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * cos(azimut),
	   		 			                  (newDistanceFromTheENodeB - (thisCell->GetRadius()*1000)) * sin(azimut));

	  	    newPosition->SetCoordinateX(newPosition->GetCoordinateX() - Correction->GetCoordinateX());
	  	    newPosition->SetCoordinateY(newPosition->GetCoordinateY() - Correction->GetCoordinateY());

	  	  	//double speedDirection = (double)(rand() %360) * ((2*3.14)/360);
			double speedDirection;
			/*if(GetSpeedDirection() < 1.57)
				speedDirection = (180.1*2*pi)/360;
			else*//*
				speedDirection = (0.1*2*pi)/360;

	  	  	SetSpeedDirection(speedDirection);

	  	    delete Correction;
	      }
    }*/

  SetAbsolutePosition(newPosition);
  SetPositionLastUpdate (time);

#ifdef MOBILITY_DEBUG
  std::cout << "\n\t Final Position (X): " << GetAbsolutePosition()->GetCoordinateX()
			<< "\n\t Final Position (Y): " << GetAbsolutePosition()->GetCoordinateY()
			<< std::endl;
#endif
#ifdef MOBILITY_DEBUG_TAB
  std::cout << GetAbsolutePosition()->GetCoordinateX() << " "
			<< GetAbsolutePosition()->GetCoordinateY()
			<< std::endl;
#endif
  //if(timeInterval>1)
	  //std::cout<<"Bus "<<GetNodeID()<<" x "<<GetAbsolutePosition()->GetCoordinateX()<<" y "<<GetAbsolutePosition()->GetCoordinateY()<<std::endl;//test
  delete newPosition;
}
