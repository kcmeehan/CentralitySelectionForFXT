#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TSystem.h>
#include <TLatex.h>
#include <THistPainter.h>
#include <TAttLine.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TVector3.h>
#include <TMath.h>

//after- verify that you need all these classes
#include "../submodules/datacollectorreaderlibs/TrackInfo/TrackInfo.h"
#include "../submodules/datacollectorreaderlibs/PrimaryVertexInfo/PrimaryVertexInfo.h"
#include "../submodules/datacollectorreaderlibs/EventInfo/EventInfo.h"
#include "../submodules/datacollectorreaderlibs/DavisDstReader/DavisDstReader.h"
#include "ParticleInfo.h"
#include "UserCuts.h"

using namespace std;

//_____MAIN____________________
void vertexQAmaker(TString inputDataFile, TString outputFile, Bool_t vertexCuts = false, Int_t nEvents = -1){
//This function takes your input data file and produces an output file with vertex qa plots
//with no vertex cuts or with vertex-level cuts. Note that no track qa plots are made 
//you must use trackQAmaker.cxx for track qa. The purpose of this function is to allow 
//the user to optimize vertex cuts.

DavisDstReader davisDst(inputDataFile);
if(vertexCuts == false){
  davisDst.SetBranchStatus("TrackInfo",0);
}

TFile *outFile  = new TFile(outputFile,"RECREATE");

TrackInfo *track = NULL;
PrimaryVertexInfo *primaryVertex = NULL;
EventInfo *event = NULL;

//initializing pre-cut vertex histograms
TH1D *FullVzHistoNoCuts = new TH1D("FullVzHistoNoCuts","V_{z} Distribution",520,-260,260);
TH1D *VzHistoNoCuts = new TH1D("VzHistoNoCuts","V_{z} Distribution",500,200,225);
TH2D *VyVzHistoNoCuts = new TH2D("VyVzHistoNoCuts","V_{y} vs. V_{z} Distribution",500,200,225,500,-5,5);
TH2D *VxVyHistoNoCuts = new TH2D("VxVyHistoNoCuts","V_{y} vs. V_{x} Distribution",500,-5,5,500,-5,5);
TH1D *multHistoNoCuts = new TH1D("multHistoNoCuts","Multiplicity",500,0,500);
TH1D *ntofMatchHistNoCuts = new TH1D("ntofMatchHistNoCuts","Number of TOF Matches",500,0,500);
TH2D *TOFvsMultHistNoCuts = new TH2D("TOFvsMultHistNoCuts","Number of TOF Matches vs. Multiplicity",100,0,100,50,0,50);
TH1D *VzHisto, *multHisto, *ntofMatchHist, *piMult;
TH2D *VyVzHisto, *VxVyHisto, *TOFvsMultHist;

if (vertexCuts){
  VzHisto = new TH1D("VzHisto","V_{z} Distribution",140,208,215);
  VyVzHisto = new TH2D("VyVzHisto","V_{y} vs. V_{z} Distribution",500,200,225,500,-5,5)    ;
  VxVyHisto = new TH2D("VxVyHisto","V_{y} vs. V_{x} Distribution",500,-5,5,500,-5,5);
  multHisto = new TH1D("multHisto","Multiplicity",500,0,500);
  ntofMatchHist = new TH1D("ntofMatchHist","Number of TOF Matches",500,0,500);
  piMult = new TH1D("piMult","Pion Multiplicity",200,0,200);
  TOFvsMultHist = new TH2D("TOFvsMultHist","Number of TOF Matches vs. Multiplicity",100,0,100,50,0,50);
}

Double_t x(0), y(0), z(0), refMultUser(0);
Int_t pvEntries;
Double_t entries;
if(nEvents > 0) entries = nEvents;
else entries = davisDst.GetEntries();
for(Int_t i=0; i<entries; i++){//loop over triggers
  event = davisDst.GetEntry(i);
	if (!IsGoodEvent(event)) continue;
	pvEntries = event->GetNPrimaryVertices();
  for (Int_t j=0; j<pvEntries; j++){//event->primaryVertexArray->GetEntries() 
	  primaryVertex = event->GetPrimaryVertex(j);
	
	  x = primaryVertex->xVertex;
	  y = primaryVertex->yVertex;
	  z = primaryVertex->zVertex;
	  refMultUser = primaryVertex->refMultUser;
	
	  //fill no cuts histograms
	  FullVzHistoNoCuts->Fill(z);
	  VzHistoNoCuts->Fill(z);
	  VyVzHistoNoCuts->Fill(z,y);
	  VxVyHistoNoCuts->Fill(x,y);
	  multHistoNoCuts->Fill(refMultUser);

    //track loop to calculate variables for event cuts
    Int_t nTOFmatches = 0;
		Int_t nPions=0;
		Int_t nTracks = 0;
		Int_t nPrimaryTracks = primaryVertex->GetNPrimaryTracks();
		for(Int_t k = 0; k<nPrimaryTracks;k++){
		  track = primaryVertex->GetPrimaryTrack(k);
			if (!IsGoodTrack(track)) continue;
			nTracks++;
		  if (track->tofMatchFlag > 0) nTOFmatches = nTOFmatches + 1;
		  Double_t q = track->charge;
		  Double_t nSigmaPi = track->nSigmaPion;
		  Double_t nSigmaPro = track->nSigmaProton;
		  if ((q < 0 && abs(nSigmaPi) < 2) || (q > 0 && abs(nSigmaPi) < 2 && nSigmaPro < -1)) nPions++;
		}
		ntofMatchHistNoCuts->Fill(nTOFmatches);
		TOFvsMultHistNoCuts->Fill(nTracks,nTOFmatches);
	  if (!vertexCuts) continue;
    if (!IsGoodVertex(primaryVertex)) continue;
    VzHisto->Fill(z); 
    VyVzHisto->Fill(z,y);
    VxVyHisto->Fill(x,y);
    multHisto->Fill(refMultUser);
    ntofMatchHist->Fill(nTOFmatches);
    TOFvsMultHist->Fill(nTracks,nTOFmatches);
    piMult->Fill(nPions);
 }//end of loop over vertices
}//end of loop over triggers

outFile->Write();

}//end of function
