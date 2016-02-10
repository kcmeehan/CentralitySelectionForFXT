//Creates the centrality variable distributions that will be used in GlauberMCModel for
//centrality bin determination

#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TH1D.h>
#include <TMath.h>

#include "TrackInfo.h"
#include "PrimaryVertexInfo.h"
#include "EventInfo.h"
#include "DavisDstReader.h"
#include "ParticleInfo.h"
#include "UserCuts.h"


//____MAIN___________________________________________________________________________
void centralityVariableDistributions(TString inputDataFile, Long64_t nEvents=-1, TString outputFile=""){

  //Read the DavisDst
  DavisDstReader davisDst(inputDataFile);

  //Create Pointers needed for reading the tree
  TrackInfo *track = NULL;
  PrimaryVertexInfo *primaryVertex = NULL;
  EventInfo *event = NULL;

  //If no output file was specified then we won't produce one
  TFile *outFile = NULL;
  if (outputFile.CompareTo(""))
    outFile = new TFile(outputFile,"RECREATE");

  //Create Histograms for the centrality variables
  TH1D *nPionsHisto = new TH1D("nPionHisto","Number of Pions",1000,0,1000);
  TH1D *nGoodHisto = new TH1D("nGoodTracks","Number of Tracks that Pass Track Quality Cuts",1000,0,1000);
  TH1D *nPrimaryHisto = new TH1D("nPrimaryHisto","Number of Primary Tracks",1000,0,1000);

  //If the user has passed a number of events to process then use it,
  //otherwise use all the entries in the tree
  Long64_t nEntries(0);
  if (nEvents > 0)
    nEntries = nEvents;
  else
    nEntries = davisDst.GetEntries();

  for (Int_t iEntry=0; iEntry<nEntries; iEntry++){

    //Get the ith entry and check if it passes the cuts
    event = davisDst.GetEntry(iEntry);
    if (!IsGoodEvent(event))
      continue;
    
    //Loop over the primary vertex array of this event                                                           
    Int_t nPrimaryVertices = event->GetNPrimaryVertices();
    for (Int_t iPrimaryVertex=0; iPrimaryVertex<nPrimaryVertices; iPrimaryVertex++){

      //Get the ith primary vertex and check if it passes the cuts                                               
      primaryVertex = event->GetPrimaryVertex(iPrimaryVertex);
      if (!IsGoodVertex(primaryVertex))
        continue;

      Double_t pionMult(0), totalMult(0);
      
      //Loop Over the Primary Tracks associated with this primary vertex
      Int_t nPrimaryTracks = primaryVertex->GetNPrimaryTracks();
      for (Int_t iPrimaryTrack=0; iPrimaryTrack<nPrimaryTracks; iPrimaryTrack++){

	//Get the ith primary track and check if it is good
	track = primaryVertex->GetPrimaryTrack(iPrimaryTrack);
	if (!IsGoodTrack(track))
	  continue;

	//Increment count of total multiplicity
	totalMult++;

	//Increment count of pion mutltiplicity
	if (fabs(track->GetNSigmaPion()) < 2.0){
	  
	  if (track->GetCharge() < 0)
	    pionMult++;
	  else if (track->GetNSigmaProton() < -1.0){
	    pionMult++;
	  }
	  
	}	
	
      }//End Loop Over iPrimary Track
      
      //Fill the Centrality Variable Histograms
      nPionsHisto->Fill(pionMult);
      nGoodHisto->Fill(totalMult);
      nPrimaryHisto->Fill(primaryVertex->GetNPrimaryTracks());
      
    }//End Loop Over primary Vertices      

  }//End Loop Over tree Entries

  nPionsHisto->Write();
  nGoodHisto->Write();
  nPrimaryHisto->Write();
  outFile->Close();

}
