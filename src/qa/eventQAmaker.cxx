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
#include "TrackInfo/TrackInfo.h"
#include "PrimaryVertexInfo/PrimaryVertexInfo.h"
#include "EventInfo/EventInfo.h"
#include "ParticleInfo.h"
#include "UserCuts.h"

using nampespace std;

//_____MAIN____________________
void eventQAmaker(TString inputDataFile, Bool_t eventCuts, Bool_t centCuts){

//setting output file name
TString outFile = "eventQA";
TString none    = "_noCuts";
TString event   = "_eventCuts";
TString cent    = "_centSelected";

if((eventCuts && centCuts) == false) outputFile+=none;
if(eventCuts == true) outputFile+=event;
if(centCuts  == true) outputFile+=cent;

//obtaining data tree
TFile *file     = new TFile(inputDataFile,"READ");
TTree *tree     = (TTree *)file->Get("DataTree");
 
TrackInfo *track = NULL;
PrimaryVertexInfo *primaryVertex = NULL;
EventInfo *event = NULL;

tree->FindBranch("EventInfo")->SetAddress(&event);
  //initializing trigger level histograms
  TH1D *htrig = new TH1D("htrig","Triggers",500,0,500000);
  TH1D *tofMultHist = new TH1D("tofMultHist","TOF Multiplicity",500,0,500);
  TH1I *hnPrimaryVertices = new TH1I("hnPrimaryVertices","Primary Vertex Distribution",20,0,20);

if((eventCuts && centCuts) == false){//initialize in no cuts case
  TH1D *FullVzHistoNoCuts = new TH1D("FullVzHistoNoCuts","V_{z} Distribution",520,-260,260);
  TH1D *VzHistoNoCuts = new TH1D("VzHistoNoCuts","V_{z} Distribution",500,200,225);
  TH2D *VyVzHistoNoCuts = new TH2D("VyVzHistoNoCuts","V_{y} vs. V_{z} Distribution",500,200,225,500,-5,5);
  TH2D *VxVyHistoNoCuts = new TH2D("VxVyHistoNoCuts","V_{y} vs. V_{x} Distribution",500,-5,5,500,-5,5);
  TH1D *multHistoNoCuts = new TH1D("multHistoNoCuts","Multiplicity",500,0,500);
  TH1D *ntofMatchHist = new TH1D("ntofMatchHist","Number of TOF Matches",500,0,500);
  TH2D *TOFvsPiMultHistNoCuts = new TH2D("TOFvsPiMultHistNoCuts","Number of TOF Matches vs. Pion Multiplicity",100,0,100,50,0,50);
} 

else{//initialize in case of any cuts
TH1D *VzHisto = new TH1D("VzHisto","V_{z} Distribution",140,208,215);
TH2D *VyVzHisto = new TH2D("VyVzHisto","V_{y} vs. V_{z} Distribution",500,200,225,500,-5,5);
TH2D *VxVyHisto = new TH2D("VxVyHisto","V_{y} vs. V_{x} Distribution",500,-5,5,500,-5,5);
TH1D *multHisto = new TH1D("multHisto","Multiplicity",500,0,500);
TH1D *piMult = new TH1D("piMult","Pion Multiplicity",200,0,200);
}

}//end of function



