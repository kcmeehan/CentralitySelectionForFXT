//This is just a simple example of how to read a davis DST and create a new file with some stuff

//C++ includes
#include <iostream>

//ROOT includes
#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TClonesArray.h>
#include <TCanvas.h>

//DataCollectorReaderLibs Sub-Module Includes
#include "TrackInfo.h"
#include "PrimaryVertexInfo.h"
#include "EventInfo.h"

void reader(TString inputDataFile, Int_t maxEvents=-1, TString outputFile=""){

  //Open the inputDataFile for reading and get the data tree
  TFile *inFile = new TFile(inputDataFile,"READ");
  TTree *tree = (TTree *)inFile->Get("DataTree");

  //If no outputFile was specified then we won't produce one
  //otherwise we will
  TFile *outFile = NULL;
  if (outputFile.CompareTo(""))
    outFile = new TFile(outputFile,"RECREATE");

  //Create some class pointers we will use to access the data in the tree
  TrackInfo *track = NULL;
  PrimaryVertexInfo *primaryVertex = NULL;
  EventInfo *event = NULL;

  //Set the address of the EventInfo object in the tree to the event pointer
  tree->FindBranch("EventInfo")->SetAddress(&event);

  //As a demonstration lets fill a few histograms...
  //An event level one...
  TH1D *nPrimaryVertexHisto = new TH1D("nPrimaryVertexHisto",";Number of Primary Vertices;",20,0,20);
  //A vertex level one
  TH1D *zVertexHisto = new TH1D("zVertexHisto",";Z-Vertex Position (cm);dN/dZ",460,-230,230);
  //And a track level one
  TH2D *dEdxHisto = new TH2D("dEdxHisto",";|p|q;dE/dx (KeV/cm)",500,.09,5,500,.9,50);


  //Set the number of events to run over
  Long64_t nEvents(0);
  //If the user has specified a number then use that
  if (maxEvents > 0)
    nEvents = maxEvents;
  //Otherwise run over all the events in the tree
  else 
    nEvents = tree->GetEntries();
  
  //Loop Over the Events in the tree. For each event loop over its primary
  //vertices, and for each primary vertex loop over its associated primary tracks
  for (Long64_t iEvent=0; iEvent< nEvents; iEvent++){

    //Get the Event Entry
    tree->GetEntry(iEvent);

    //Fill the event level histogram
    nPrimaryVertexHisto->Fill(event->nPrimaryVertices);
    
    //Loop Over the Primary Vertices
    Int_t nPrimaryVertices = event->primaryVertexArray->GetEntries(); 
    for (Int_t iPrimaryVertex=0; iPrimaryVertex<nPrimaryVertices; iPrimaryVertex++){

      //Get the ith primary vertex.
      //It is saved as a TObject so we need to cast it to the correct type
      primaryVertex = (PrimaryVertexInfo *)event->primaryVertexArray->At(iPrimaryVertex);

      //Fill the vertex level histogram
      zVertexHisto->Fill(primaryVertex->zVertex);

      //Loop Over the Primary Tracks associated with this primary vertex
      Int_t nPrimaryTracks = primaryVertex->trackArray->GetEntries();
      for (Int_t iPrimaryTrack=0; iPrimaryTrack<nPrimaryTracks; iPrimaryTrack++){

	//Get the ith primary Track.
	//It is saved as a TObject so we need to cast it to the correct type
	track = (TrackInfo *)primaryVertex->trackArray->At(iPrimaryTrack);

	//Compute the Track's Total Momentum
	Double_t totalMomentum = sqrt(pow(track->pT,2) + pow(track->pZ,2));

	//Fill the track level histo
	dEdxHisto->Fill(totalMomentum,1000000*track->dEdxTruncated);	

      }//End Loop Over Primary Vertex

    }//End Loop Over Primary Vertices

  }//End Loop Over Events
    

  //Create a Canvas to Draw the histograms
  TCanvas *canvas = new TCanvas("canvas","canvas",20,20,1200,400);
  canvas->Divide(3,1);

  //Draw the Histograms
  canvas->cd(1);
  nPrimaryVertexHisto->Draw();
  canvas->cd(2);
  zVertexHisto->Draw();
  canvas->cd(3);
  gPad->SetLogz();
  gPad->SetLogy();
  gPad->SetLogx();
  dEdxHisto->Draw("COLZ");


  //If we are creating an output file then save these histograms
  if (outFile){
    nPrimaryVertexHisto->Write();
    zVertexHisto->Write();
    dEdxHisto->Write();
  }
  
  return;

}
