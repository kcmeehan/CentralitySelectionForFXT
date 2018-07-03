/**************************************************************
 **  This file includes all the analysis cuts used and is called 
 **  by the centrality selection code when making the centrality 
 **  variable histogram.
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
	//Requires events to have at least one primary vertex.
  if (event->GetNPrimaryVertices() <= 0)
    return false;

	//Rejects events that come from the laser trigger instead of the fxt trigger.
  if (event->GetTofMultiplicity() < 50)
    return false;
  
  return true;

}

//_______________________________________________________________
Bool_t IsGoodVertex(PrimaryVertexInfo *vertex){

	//Requires the primary vertex to be a zero-index vertex. 
  if (vertex->GetVertexIndex() != 0)  
	    return false;

  //Requires the vertex of interest to have a z-coordinate of 210 cm <= Vz <= 212 cm. (The target is located at 211 cm).
  if (vertex->GetZVertex() < 210 || vertex->GetZVertex() > 212)
    return false;

  return true;
}

//_______________________________________________________________
Bool_t IsGoodTrack(TrackInfo *track){

	//Requires nHitsFit/nHitsPoss >= 0.52 to protect against split tracks
  if (track->GetNFrac() < 0.52)
    return false;

	//Rejects y_lab ~0 tracks that go behind the gating grid due to the position of the target
  if (track->GetdEdxHits() <= 0)
    return false;

  return true;
}

//_______________________________________________________________
std::vector<double> GetCentralityPercents(){

  //Returns a vector containing the matching percent bins of
  //the centrality cuts in GetCentralityCuts() below

  std::vector<double> centralityPercents;
  //Most Central
  centralityPercents.push_back(5);  //Top 5%
  centralityPercents.push_back(10); 
  centralityPercents.push_back(15); 
  centralityPercents.push_back(20); 
  centralityPercents.push_back(25); 
  centralityPercents.push_back(30); 
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
  centralityCuts.push_back(153);
  centralityCuts.push_back(121);
  centralityCuts.push_back(97);
  centralityCuts.push_back(77);
  centralityCuts.push_back(61);
  centralityCuts.push_back(48);
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
  if (centralityVariable >= centralityCuts.at(0) && centralityVariable < 240)
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
