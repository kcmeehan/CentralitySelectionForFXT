//Applies cuts to skim the data at the event, vertex, and track level. Then bins the
//tracks in centrality, rapidity, and transverse mass for various particle species.

#include <iostream>
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TMath.h>

#include "TrackInfo.h"
#include "PrimaryVertexInfo.h"
#include "EventInfo.h"
#include "ParticleInfo.h"
#include "UserCuts.h"

//___MAIN___________________________
void skimmerAndBinner(TString inputDataFile,TString starLibrary, 
		      Long64_t nEvents=-1, TString outputFile=""){

  const double rapidityMin = -2.05;
  const double rapidityMax = 2.05;
  const double rapidityBinWidth = 0.1;
  const int nRapidityBins = TMath::Nint( fabs(rapidityMax-rapidityMin) / rapidityBinWidth );

  const double mTm0Min = 0.0;
  const double mTm0Max = 2.0;
  const double mTm0BinWidth = 0.025;
  const int nmTm0Bins = TMath::Nint( fabs(mTm0Min-mTm0Max) / mTm0BinWidth );

  //Create Pointers needed for reading the tree
  TrackInfo *track = NULL;
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

  //Create an instance of the Particle Info Class
  ParticleInfo *particleInfo = new ParticleInfo(starLibrary,true,TRUNCATED);
  //const int nParticles = particleInfo->GetNumberOfDefinedParticles();

  //************************************************************************
  //Temporary solution since nSigma there is not an nSigma 
  //value for deteruon,triton,helio,alpha,muon
  const int nParticles = 3;
  //************************************************************************

  //Get the centrality vector from the User's function
  std::vector<int> centralityBins = GetCentralityCuts();
  const int nCentralityBins = (int)centralityBins.size();
  
  //Declare Histogram to hold the binned Particles and the Number of Events

  //Contains the number of events in each centrality bin
  TH1D *nEventsHisto = new TH1D("nEvents","Events in Each Centrality Bin",
				nCentralityBins,0,nCentralityBins);
  std::vector<TH1D *> centVarHisto(nCentralityBins,(TH1D *)NULL);

  //The Yield Histograms for the difference centrality bins and particle species
  std::vector<std::vector<TH3D *> > ZTPCHistoPlus(nParticles,vector<TH3D *>(nCentralityBins,(TH3D *)NULL));
  std::vector<std::vector<TH3D *> > ZTOFHistoPlus(nParticles,vector<TH3D *>(nCentralityBins,(TH3D *)NULL));
  std::vector<std::vector<TH3D *> > ZTPCHistoMinus(nParticles,vector<TH3D *>(nCentralityBins,(TH3D *)NULL));
  std::vector<std::vector<TH3D *> > ZTOFHistoMinus(nParticles,vector<TH3D *>(nCentralityBins,(TH3D *)NULL));

  //Number of tracks seen in the TPC and TOF - Will be used for TOF Matching Efficiency
  std::vector<std::vector<TH2D *> > TPCTracksPlus(nParticles,vector<TH2D *>(nCentralityBins,(TH2D *)NULL));
  std::vector<std::vector<TH2D *> > TOFTracksPlus(nParticles,vector<TH2D *>(nCentralityBins,(TH2D *)NULL));
  std::vector<std::vector<TH2D *> > TPCTracksMinus(nParticles,vector<TH2D *>(nCentralityBins,(TH2D *)NULL));
  std::vector<std::vector<TH2D *> > TOFTracksMinus(nParticles,vector<TH2D *>(nCentralityBins,(TH2D *)NULL));

  //Loop Over the Particle Species and centrality bins and define the histograms
  for (Int_t iParticle=0; iParticle<nParticles; iParticle++){
    
    //Skip Electrons and Muons
    if (iParticle == ELECTRON || iParticle == MUON)
      continue;

    for (Int_t iCentBin=0; iCentBin<nCentralityBins; iCentBin++){
      
      if (!centVarHisto.at(iCentBin)){
	centVarHisto.at(iCentBin) = 
	  new TH1D(Form("centralityVariable_Cent%d",iCentBin),
		   Form("CentralityVariable Distribution in Centrality Bin %d",iCentBin)
		   ,1000,0,(int)centralityBins.at(0)+50);
      }

      //Define Yield Histograms
      ZTPCHistoPlus.at(iParticle).at(iCentBin) = 
	new TH3D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max,1000,-5,10);
      ZTOFHistoPlus.at(iParticle).at(iCentBin) = 
	new TH3D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max,1000,-1,3);

      ZTPCHistoMinus.at(iParticle).at(iCentBin) =
        new TH3D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max,1000,-5,10);
      ZTOFHistoMinus.at(iParticle).at(iCentBin) =
        new TH3D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max,1000,-1,3);

      TPCTracksPlus.at(iParticle).at(iCentBin) =
	new TH2D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max);
      TOFTracksPlus.at(iParticle).at(iCentBin) = 
	new TH2D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max);
      
      TPCTracksMinus.at(iParticle).at(iCentBin) =
	new TH2D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max);
      TOFTracksMinus.at(iParticle).at(iCentBin) =
	new TH2D("","",nRapidityBins,rapidityMin,rapidityMax,nmTm0Bins,mTm0Min,mTm0Max);

      //Set the Names and Titles of the Yield Histograms
      const char *particleSymbolPlus = particleInfo->GetParticleSymbol(iParticle,1).Data();
      const char *particleSymbolMinus = particleInfo->GetParticleSymbol(iParticle,-1).Data();

      ZTPCHistoPlus.at(iParticle).at(iCentBin)->
	SetName(Form("zTPC_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      ZTPCHistoPlus.at(iParticle).at(iCentBin)->
        SetTitle(Form("Z_{TPC} of %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2});Z_{TPC,%s}",
		      particleSymbolPlus,iCentBin,particleSymbolPlus,
		      particleSymbolPlus,particleSymbolPlus));

      ZTOFHistoPlus.at(iParticle).at(iCentBin)->
        SetName(Form("zTOF_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      ZTOFHistoPlus.at(iParticle).at(iCentBin)->
	SetTitle(Form("Z_{TOF} of %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2});Z_{TOF,%s}",
		      particleSymbolPlus,iCentBin,particleSymbolPlus,
		      particleSymbolPlus,particleSymbolPlus));

      ZTPCHistoMinus.at(iParticle).at(iCentBin)->
        SetName(Form("zTPC_%s_Cent%d",particleInfo->GetParticleName(iParticle,-1).Data(),iCentBin));
      ZTPCHistoMinus.at(iParticle).at(iCentBin)->
	SetTitle(Form("Z_{TPC} of %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2});Z_{TPC,%s}",
		      particleSymbolMinus,iCentBin,particleSymbolMinus,
		      particleSymbolMinus,particleSymbolMinus));

      ZTOFHistoMinus.at(iParticle).at(iCentBin)->
	SetName(Form("zTOF_%s_Cent%d",particleInfo->GetParticleName(iParticle,-1).Data(),iCentBin));
      ZTOFHistoMinus.at(iParticle).at(iCentBin)->
	SetTitle(Form("Z_{TOF} of %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2});Z_{TOF,%s}",
                      particleSymbolMinus,iCentBin,particleSymbolMinus,
                      particleSymbolMinus,particleSymbolMinus));

      TPCTracksPlus.at(iParticle).at(iCentBin)->
	SetName(Form("nTPCTracks_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      TPCTracksPlus.at(iParticle).at(iCentBin)->
	SetTitle(Form("n TPC Tracks for %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2})",
		      particleSymbolPlus,iCentBin,particleSymbolPlus,particleSymbolPlus));

      TOFTracksPlus.at(iParticle).at(iCentBin)->
	SetName(Form("nTOFTracks_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      TOFTracksPlus.at(iParticle).at(iCentBin)->
	SetTitle(Form("n TOF Tracks for %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2})",
                      particleSymbolPlus,iCentBin,particleSymbolPlus,particleSymbolPlus));

      TPCTracksMinus.at(iParticle).at(iCentBin)->
	SetName(Form("nTPCTracks_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      TPCTracksMinus.at(iParticle).at(iCentBin)->
	SetTitle(Form("n TPC Tracks for %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2})",
                      particleSymbolMinus,iCentBin,particleSymbolMinus,particleSymbolMinus));

      TOFTracksMinus.at(iParticle).at(iCentBin)->
        SetName(Form("nTOFTracks_%s_Cent%d",particleInfo->GetParticleName(iParticle,1).Data(),iCentBin));
      TOFTracksMinus.at(iParticle).at(iCentBin)->
        SetTitle(Form("n TOF Tracks for %s Centrality Bin: %d;y_{%s};m_{T}-m_{%s} (GeV/c^{2})",
                      particleSymbolMinus,iCentBin,particleSymbolMinus,particleSymbolMinus));


    }//End Loop Over Centrality Bins

  }//End Loop Over particle species

  //If the user has passed a number of events to process then use it,
  //otherwise use all the entries in the tree
  Long64_t nEntries(0);
  if (nEvents > 0)
    nEntries = nEvents;
  else
    nEntries = tree->GetEntries();

  //Loop Over the entries of the Tree. Make sure that each event, vertex, and track
  //pass the cuts. For Tracks bin them in centrality, rapidity, and transverse mass
  for (Int_t iEntry=0; iEntry<nEntries; iEntry++){

    //Get the ith entry and check if it passes the cuts
    tree->GetEntry(iEntry);
    if (!IsGoodEvent(event))
      continue;

    //Loop Over the Primary Vertex Array of this event
    Int_t nPrimaryVertices = event->GetPrimaryVertexArray()->GetEntries();
    for (Int_t iPrimaryVertex=0; iPrimaryVertex<nPrimaryVertices; iPrimaryVertex++){

      //Get the ith primary vertex and check if it passes the cuts
      primaryVertex = (PrimaryVertexInfo *)event->GetPrimaryVertexArray()->At(iPrimaryVertex);
      if (!IsGoodVertex(primaryVertex))
	continue;

      //Figure out the centrality bin here
      Int_t centralityVariable = GetCentralityVariable(primaryVertex);
      Int_t centralityBin = GetCentralityBin(centralityVariable);

      //Skip this vertex if it has a bad centrality bin determination
      if (centralityBin < 0)
	continue;

      //Fill the Centrality Histograms
      nEventsHisto->Fill(centralityBin);
      centVarHisto.at(centralityBin)->Fill((double)centralityVariable);

      //Loop Over the Primary Tracks of this primary vertex
      Int_t nPrimaryTracks = primaryVertex->GetPrimaryTrackArray()->GetEntries();
      for (Int_t iPrimaryTrack=0; iPrimaryTrack<nPrimaryTracks; iPrimaryTrack++){

	//Get the ith primary track and check if it passes the cuts
	track = (TrackInfo *)primaryVertex->GetPrimaryTrackArray()->At(iPrimaryTrack);
	if (!IsGoodTrack(track))
	  continue;

	//Loop over Each of the Particle Species and Fill the Yield Histograms
	for (Int_t iParticle=0; iParticle<nParticles; iParticle++){

	  //Skip Electrons and Muons
	  if (iParticle == ELECTRON || iParticle == MUON)
	    continue;

	  Double_t rapidity = track->GetRapidity(particleInfo->GetParticleMass(iParticle));
	  Double_t mTm0 = track->GetmTm0(particleInfo->GetParticleMass(iParticle));
	  Double_t nSigmaTPC = track->GetNSigmaOfSpecies(iParticle);
	  
	  //Compute the zTPC variable and fill the yield histogram
	  Double_t zTPC = particleInfo->ComputeZTPC(track->GetdEdx(TRUNCATED),track->pT,track->pZ,iParticle);
	  
	  if (track->GetCharge() > 0){
	    ZTPCHistoPlus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0,zTPC);
	    if (fabs(nSigmaTPC) < 2.0)
	      TPCTracksPlus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0);
	  }
	  else if (track->GetCharge() < 0){
	    ZTPCHistoMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0,zTPC);
	    if (fabs(nSigmaTPC) < 2.0)
	      TPCTracksMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0);
	  }

	  //Check to make sure this track has a TOF hit and a physical 1/Beta
	  if (track->GetTofMatchFlag() < 1 || 1.0/track->GetTofBeta() < 1.0)
	    continue;
	  
	  //Compute the zTOF variable and fill the yield Histogram
	  Double_t zTOF = particleInfo->ComputeZTOF(1.0/track->tofBeta,track->pT,track->pZ,iParticle);

	  if (track->GetCharge() > 0){
	    ZTOFHistoMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0,zTOF);
	    if (fabs(nSigmaTPC) < 2.0)
	      TOFTracksMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0);
	  }
	  else if (track->GetCharge() < 0){
	    ZTOFHistoMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0,zTOF);
	    if (fabs(nSigmaTPC) < 2.0)
	      TOFTracksMinus.at(iParticle).at(centralityBin)->Fill(rapidity,mTm0);
	  }

	}//End Loop Over Particle Species

      }//End Loop Over Primary Tracks

    }//End Loop Over Primary Vertices
    
  }//End Loop Over Events/Entry


  //Write the Histograms to File
  nEventsHisto->Write();

  outFile->mkdir("CentralityVariableHistograms");
  outFile->cd("CentralityVariableHistograms");
  for (Int_t iCentBin=0; iCentBin<nCentralityBins; iCentBin++){
    centVarHisto.at(iCentBin)->Write();
  }
  outFile->cd();

  for (Int_t iParticle=0; iParticle<nParticles; iParticle++){

    //Skip Electrons and Muons
    if (iParticle == ELECTRON || iParticle == MUON)
      continue;

    outFile->mkdir(Form("%s",particleInfo->GetParticleName(iParticle,-1).Data()));
    outFile->mkdir(Form("%s",particleInfo->GetParticleName(iParticle,1).Data()));

    for (Int_t iCentBin=0; iCentBin<nCentralityBins; iCentBin++){

      outFile->cd(Form("%s",particleInfo->GetParticleName(iParticle,1).Data()));
      ZTPCHistoPlus.at(iParticle).at(iCentBin)->Write();
      ZTOFHistoPlus.at(iParticle).at(iCentBin)->Write();
      TPCTracksPlus.at(iParticle).at(iCentBin)->Write();
      TOFTracksPlus.at(iParticle).at(iCentBin)->Write();

      outFile->cd();

      outFile->cd(Form("%s",particleInfo->GetParticleName(iParticle,-1).Data()));
      ZTPCHistoMinus.at(iParticle).at(iCentBin)->Write();
      ZTOFHistoMinus.at(iParticle).at(iCentBin)->Write();
      TPCTracksMinus.at(iParticle).at(iCentBin)->Write();
      TOFTracksMinus.at(iParticle).at(iCentBin)->Write();
      
      outFile->cd();
    }

  }//End Loop Over particle species

  outFile->Close();

}
