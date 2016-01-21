/**************************************************************
 **  This is an example file showing how you will implement your event, 
 **  vertex, and track cuts in each of the following functions.
 **
 **  IMPORTANT!: 
 **  You should modify the cuts within the functions below to
 **  make the cuts whatever you need/want them to be.
 **************************************************************/

#include <iostream>
#include <vector>

#include <TClonesArray.h>

#include <TrackInfo.h>
#include <PrimaryVertexInfo.h>
#include <EventInfo.h>
#include <ParticleInfo.h>
#include <UserCuts.h>

//_______________________________________________________________
Bool_t IsGoodEvent(EventInfo *event){

  //Returns true if this event passes all your cuts
  if (event->GetNPrimaryVertices() <= 0)
    return false;

  return true;

}

//_______________________________________________________________
Bool_t IsGoodVertex(PrimaryVertexInfo *vertex){

  //Returns true if this vertex passes all your cuts
  if (vertex->GetZVertex() < -200 || vertex->GetZVertex() >= -150)
    return false;

  if (vertex->GetRVertex() < 2 || vertex->GetRVertex() > 5)
    return false;

  return true;
}

//_______________________________________________________________
Bool_t IsGoodTrack(TrackInfo *track){

  //Returns true if this track passes all your cuts
  
  if ( track->GetNFrac() < 0.52)
    return false;
  
  return true;
}

//_______________________________________________________________
std::vector<double> GetCentralityPercents(){

  //Returns a vector containing the matching percent bins of
  //the centrality cuts in GetCentralityCuts() below

  std::vector<double> centralityPercents;
  //Most Central
  centralityPercents.push_back(10); //Top 10%
  centralityPercents.push_back(20); //Top 20%
  centralityPercents.push_back(30); //Top 30%
  //Most Peripheral

  return centralityPercents;

}

//_______________________________________________________________
std::vector<int> GetCentralityCuts(){

  //Returns a the vector of centrality Cuts

  //Set your centrality cuts here order them from most central
  //to most peripheral.
  std::vector<int> centralityCuts;
  //Most Central
  centralityCuts.push_back(200);
  centralityCuts.push_back(154);
  centralityCuts.push_back(100);
  //Most Peripheral

  return centralityCuts;

}

//_______________________________________________________________
Int_t GetCentralityVariable(PrimaryVertexInfo *vertex){

  //Returns the centrality variable you would like to cut on

  //Compute the Centrality Variable you want to cut on if
  //you are not using a value already stored in PrimaryVertexInfo
  Int_t totalMultiplicity(0);
  TrackInfo *track;

  for (Int_t iTrack=0; iTrack<vertex->GetPrimaryTrackArray()->GetEntries(); iTrack++){
    
    track = (TrackInfo *)vertex->GetPrimaryTrackArray()->At(iTrack);
    
    if (IsGoodTrack(track))
      totalMultiplicity++;

  }

  return totalMultiplicity;
}


//______________________________________________________________
Int_t GetCentralityBin(Int_t centralityVariable){

  //Returns the centrality Bin of a primary vertex with
  //quantity centralityVariable from GetCentralityVariable

  //Get the Centrality Cuts
  std::vector<int> centralityCuts = GetCentralityCuts();

  //Determine which centrality bin this primary Vertex belongs to

  //Check the high bin
  if (centralityVariable >= centralityCuts.at(0))
    return 0;

  //Loop Over the remaining bins and find where this vertex should be binned
  for (int iCentBin=1; iCentBin<(int)centralityCuts.size(); iCentBin++){

    if (centralityVariable >= centralityCuts.at(iCentBin) &&
	centralityVariable < centralityCuts.at(iCentBin-1))
      return iCentBin;

  }

  //If no centrality bin was found then return -1. In this case
  //the vertex will always be skipped;
  return -1;

}
