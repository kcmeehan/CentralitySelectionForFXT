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

using namespace std;

//_____MAIN____________________
void vertexQAmaker(TString inputDataFile, Bool_t vertexCuts, Bool_t centCuts){
//This function takes your input data file and produces an output file with vertex qa plots 
//with no vertex cuts, with vertex-level cuts, and centrality selection, or both. 
//Note that no track qa plots are made you must use trackQAmaker.cxx for track qa. 
//The purpose of this function is to allow the user to optimize vertex cuts.

//setting output file name
TString outFile = "vertexQA";
TString none    = "_noVertexCuts";
TString vertex  = "_vertexCuts";
TString cent    = "_centralitySelected";






}//end of function
