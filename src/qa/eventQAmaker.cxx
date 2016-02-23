#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
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

#include "../submodules/datacollectorreaderlibs/TrackInfo/TrackInfo.h"
#include "../submodules/datacollectorreaderlibs/PrimaryVertexInfo/PrimaryVertexInfo.h"
#include "../submodules/datacollectorreaderlibs/EventInfo/EventInfo.h"
#include "../submodules/datacollectorreaderlibs/DavisDstReader/DavisDstReader.h"
#include "ParticleInfo.h"
#include "UserCuts.h"

using namespace std;

//_____MAIN____________________
void eventQAmaker(TString inputDataFile, TString outFileName, Bool_t eventCuts = false, Int_t nEvents = -1){
//This function takes your input data file and produces an output file with event qa plots 
//with no cuts or with the trigger-level cuts in your user file. Note that no vertex or
//track qa plots are made- you must use vertexQAmaker.cxx or trackQAmaker.cxx for those.
//The purpose of this function is to allow the user to optimize triggered-event cuts.

//Use the DavisDstReader to open and read the file
DavisDstReader davisDst(inputDataFile);
if(eventCuts == false){
  davisDst.SetBranchStatus("VertexInfo",0);
  davisDst.SetBranchStatus("TrackInfo",0);
}

TFile *outFile  = new TFile(outFileName,"RECREATE");

TrackInfo *track = NULL;
PrimaryVertexInfo *primaryVertex = NULL;
EventInfo *event = NULL;


//initializing trigger level histograms
TH1D *htrigNoCuts = new TH1D("htrigNoCuts","Triggers",100,0,100);
TH1D *tofMultHistNoCuts = new TH1D("tofMultHistNoCuts","TOF Multiplicity",500,0,500);
TH1I *hnPrimaryVerticesNoCuts = new TH1I("hnPrimaryVerticesNoCuts","Primary Vertex Distribution",20,0,20);
TH1I *hrunNumNoCuts = new TH1I("hrunNumNoCuts","Run Number",100,16140000,16140100);
TH1D *htrig, *tofMultHist;
TH1I *hnPrimaryVertices, *hrunNum;

if (eventCuts){
  htrig = new TH1D("htrig","Triggers",100,0,100);
  tofMultHist = new TH1D("tofMultHist","TOF Multiplicity",500,0,500);
  hnPrimaryVertices = new TH1I("hnPrimaryVertices","Primary Vertex Distribution",20,0,20);
  hrunNum = new TH1I("hrunNum","Run Number",100,16140000,16140100);
}

Int_t nTrig;
unsigned short tofMult;
Double_t entries;
//start loop over triggered-events
if(nEvents > 0) entries = nEvents;
else entries = davisDst.GetEntries(); 
for(Int_t i=0;i<entries;i++){
  //access data and fill trigger level histograms
	event = davisDst.GetEntry(i);
  vector <unsigned int> trigIDs = event->triggerIDs;
	nTrig = trigIDs.size();
	tofMult = event->tofMultiplicity;
	for(int itrig=0;itrig<nTrig;itrig++){
		htrigNoCuts->Fill(trigIDs.at(itrig));
	}//currently we cannot access trigger ids, when this changes this code might need to be updated
  tofMultHistNoCuts->Fill(tofMult);
	hnPrimaryVerticesNoCuts->Fill(event->nPrimaryVertices);
	hrunNumNoCuts->Fill(event->runNumber);
  if(!eventCuts) continue; //if there are no event cuts we are done filling histograms
	if(!IsGoodEvent(event)) continue; //an event passes this only if there are cuts and that event passes those cuts
	for(int jtrig=0;jtrig<nTrig;jtrig++){
		htrig->Fill(trigIDs.at(jtrig));
	}
  tofMultHist->Fill(tofMult);
  hnPrimaryVertices->Fill(event->nPrimaryVertices);
	hrunNum->Fill(event->runNumber);
}//end loop over triggers

outFile->Write();

}//end of function



