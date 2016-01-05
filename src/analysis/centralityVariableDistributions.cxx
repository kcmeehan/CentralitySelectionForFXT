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
#include "ParticleInfo.h"
#include "UserCuts.h"


//____MAIN___________________________________________________________________________
void centralityVariableDistributions(TString inputDataFile, Long64_t nEvents=-1, TString outputFile=""){


  //Create Pointers needed for reading the tree
  //  TrackInfo *track = NULL;
  PrimaryVertexInfo *primaryVertex = NULL;
  EventInfo *event = NULL;

  //Open the inputFile for reading and get the tree
  TFile *inFile = new TFile(inputDataFile,"READ");
  TTree *tree = (TTree *)inFile->Get("DataTree");
  tree->FindBranch("EventInfo")->SetAddress(&event);

  //If no output file was specified then we won't produce one
  TFile *outFile = NULL;
  if (outputFile.CompareTo(""))
    outFile = new TFile(outputFile,"RECREATE");

  //Create Histograms for the centrality variables
  TH1D *nPionsHisto = new TH1D("nPionHisto","Number of Pions",1000,0,1000);
  TH1D *nPrimaryHisto = new TH1D("nPrimaryHisto","Number of Primary Tracks",1000,0,1000);

  //If the user has passed a number of events to process then use it,
  //otherwise use all the entries in the tree
  Long64_t nEntries(0);
  if (nEvents > 0)
    nEntries = nEvents;
  else
    nEntries = tree->GetEntries();

  tree->SetBranchStatus("primaryVertexArray.trackArray",0);
  for (Int_t iEntry=0; iEntry<nEntries; iEntry++){

    //Get the ith entry and check if it passes the cuts
    tree->GetEntry(iEntry);
    if (!IsGoodEvent(event))
      continue;
    
    //Loop over the primary vertex array of this event                                                           
    Int_t nPrimaryVertices = event->GetPrimaryVertexArray()->GetEntries();
    for (Int_t iPrimaryVertex=0; iPrimaryVertex<nPrimaryVertices; iPrimaryVertex++){

      //Get the ith primary vertex and check if it passes the cuts                                               
      primaryVertex = (PrimaryVertexInfo *)event->GetPrimaryVertexArray()->At(iPrimaryVertex);
      if (!IsGoodVertex(primaryVertex))
        continue;

      //Fill the Centrality Variable Histograms
      nPionsHisto->Fill(primaryVertex->GetNPions());
      nPrimaryHisto->Fill(primaryVertex->GetNPrimaryTracks());

    }//End Loop Over primary Vertices      

  }//End Loop Over tree Entries

  nPionsHisto->Write();
  nPrimaryHisto->Write();
  outFile->Close();

}
